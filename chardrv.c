#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>

#include "define.h"

static int major;
/* open and close context cant't process as parallel cause of private_data cause of private_data exclusion */
static DEFINE_MUTEX(private_data_lock);

static int simple_char_open(struct inode *ip, struct file *fp)
{
	struct simple_char_private_data *p;
	int ret = 0;

	ret = mutex_lock_interruptible(&private_data_lock);
	if (ret < 0) {
		return ret;
	}

	if (fp->private_data) {
		goto exit;
	}

	p = fp->private_data = kmalloc(sizeof(struct simple_char_private_data), GFP_KERNEL);
	if (!p) {
		ret = -ENOMEM;
		goto exit;
	}

	mutex_init(&p->buffer_lock);

	p->buffer = kmalloc(BUF_SIZE, GFP_KERNEL);
	if(!p->buffer) {
		ret = -ENOMEM;
		goto fail_get_buffer;
	}

	goto exit;

fail_get_buffer:
	kfree(p);
exit:
	mutex_unlock(&private_data_lock);
	if (!ret) {
		p->ref_count++;
	}

	return ret;
}

static int simple_char_release(struct inode *ip, struct file *fp)
{
	struct simple_char_private_data *p = fp->private_data;
	int ret;

	ret = mutex_lock_interruptible(&private_data_lock);
	if (ret < 0) {
		return ret;
	}

	p->ref_count--;
	if (!p->ref_count) {
		mutex_destroy(&p->buffer_lock);
		kfree(p->buffer);
		kfree(p);
	}

	mutex_unlock(&private_data_lock);

	return 0;
}

static ssize_t simple_char_read(struct file *fp, char *buf, size_t count, loff_t *offp)
{
	size_t offset = (size_t)*offp;
	ssize_t len;
	int ret;
	struct simple_char_private_data *p = fp->private_data;
	char *kernel_buffer = p->buffer;

	ret = mutex_lock_interruptible(&p->buffer_lock);
	if (ret < 0) {
		return ret;
	}

	if (offset >= BUF_SIZE) {
		printk("offset is invalid in %s", __func__);
		BUG();
	}

	len = min(count, BUF_SIZE - offset);
	if (copy_to_user(buf, &kernel_buffer[offset], len)) {
		return -EFAULT;
	}

	mutex_unlock(&p->buffer_lock);

	*offp += len;

	return len;
}

static ssize_t simple_char_write(struct file *fp, const char *buf, size_t count, loff_t *offp)
{
	size_t offset = (size_t)*offp;
	ssize_t len;
	int ret;
	struct simple_char_private_data *p = fp->private_data;
	char *kernel_buffer = p->buffer;

	ret = mutex_lock_interruptible(&p->buffer_lock);
	if (ret < 0) {
		return ret;
	}

	if (offset >= BUF_SIZE) {
		printk("offset is invalid in %s", __func__);
		BUG();
	}

	len = min(count, BUF_SIZE - offset);
	if (copy_from_user(&kernel_buffer[offset], buf, len)) {
		return -EFAULT;
	}

	mutex_unlock(&p->buffer_lock);

	*offp += len;

	return len;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = simple_char_open,
	.release = simple_char_release,
	.read = simple_char_read,
	.write = simple_char_write,
	.llseek = default_llseek,
};

static int __init simple_char_init(void)
{
	int ret;

	ret = register_chrdev(0, DRIVERNAME, &fops);
	if (ret < 0) {
		return ret;
	}

	major = ret;

	return 0;
}

static void __exit simple_char_exit(void)
{
	unregister_chrdev(major, DRIVERNAME);
}

module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_LICENSE("GPL");
