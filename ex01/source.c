#include <linux/module.h> /* Needed by all modules */
#include <linux/printk.h> /* Needed for pr_info() */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>

static int num = 10;
static char *dev_name = "dummy";
static int arg[2] = {1, 2};
static int ac = 2; 

module_param(num, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(num, "Dummy param");

module_param(dev_name, charp, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(dev_name, "Dev device name");

module_param_array(arg, int, &ac, 0000);
MODULE_PARM_DESC(arg, "Dummy array");

static int __init mod_init(void)
{
    pr_info("Hello world 1. deviec name : %s\n", dev_name);
    return 0;
}

static void __exit mod_exit(void)
{
    pr_info("Goodbye world 1.\n");
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");