#include <linux/module.h>

static int __init drv_init(void)
{
	printk("%s is invoked\n", __func__);

	return 0;
}

static void __exit drv_exit(void)
{
	printk("%s is invoked\n", __func__);
}

module_init(drv_init);
module_exit(drv_exit);

MODULE_LICENSE("GPL");

