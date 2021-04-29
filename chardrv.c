#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/mutex.h>

#include "define.h"

static int major;

static int simple_char_open(struct inode *ip, struct file *fp)
{
	struct simple_char_private_data *p;

	p = (struct simple_char_private_data*)kmalloc(sizeof(struct simple_char_private_data), GFP_KERNEL);
	if (!p) {
		goto fail_get_private_data;
	}

	fp->private_data = (void*)p;

	p->buffer = (char*)kmalloc(BUF_SIZE, GFP_KERNEL);
	if(!fp->private_data) {
		goto fail_get_buffer;
	}

	mutex_init(&p->buffer_lock);

	return 0;

fail_get_buffer:
	kfree(p);
fail_get_private_data:
	return -ENOMEM;
}

static int simple_char_release(struct inode *ip, struct file *fp)
{
	struct simple_char_private_data *p = fp->private_data;

	mutex_destroy(&p->buffer_lock);
	kfree(p->buffer);
	kfree(p);

	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = simple_char_open,
	.release = simple_char_release,
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
