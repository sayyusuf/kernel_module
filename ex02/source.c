#include <linux/module.h> /* Needed by all modules */
#include <linux/printk.h> /* Needed for pr_info() */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/atmioc.h>
#include <linux/uaccess.h>

#define DEVICE_CLASS "test_device"
#define BUFFER_SIZE 100

static char msg[BUFFER_SIZE] = "hello world\n";
static int num = 10;
static char *d_name = "dummy";
static int arg[2] = {1, 2};
static int ac = 2;

module_param(num, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(num, "Dummy param");

module_param(d_name, charp, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(d_name, "Dev device name");

module_param_array(arg, int, &ac, 0000);
MODULE_PARM_DESC(arg, "Dummy array");

enum {
    CDEV_NOT_USED = 0,
    CDEV_EXCLUSIVE_OPEN = 1,
};


static int mod_open(struct inode *inode, struct file *file);
static int mod_release(struct inode *inode, struct file *file);
static ssize_t mod_read(struct file *file, char __user *buff, size_t lenght, loff_t *offset);

static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);
static int major;
static struct class *cls;
static struct file_operations fops = {
    .open = mod_open,
    .release = mod_release,
    .read = mod_read,
};



static int open_count = 0;
static int mod_open(struct inode *inode, struct file *file)
{
    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN) == -EBUSY)
        return (-EBUSY);
    try_module_get(THIS_MODULE);
    ++open_count;
    return (0);
}

static int mod_release(struct inode *inode, struct file *file)
{
    --open_count;
    if (open_count < 1){
        atomic_set(&already_open, CDEV_NOT_USED);
    }
    module_put(THIS_MODULE);
    return (0);
}

static ssize_t mod_read(struct file *file, char __user *buff, size_t lenght, loff_t *offset)
{
    int rd_bytes = 0;
    const char *msg_ptr = msg;
    if (strlen(msg_ptr) <= *offset) { /* we are at the end of message */
        offset = 0; /* reset the offset */
        return 0; /* signify end of file */
    }
    msg_ptr += *offset;
    while (msg_ptr[rd_bytes] && lenght--)
    {
        put_user(msg_ptr[rd_bytes], &buff[rd_bytes]);
        rd_bytes++;
    }
    *offset += rd_bytes;
    return (rd_bytes);
}


static int __init mod_init(void)
{
    major = register_chrdev(0, d_name, &fops);
    if (major < 0)
    {
        pr_alert("Registering char device failed with %d\n", major);
        return (major);
    }

    cls = class_create(THIS_MODULE, DEVICE_CLASS);
    device_create(cls, NULL, MKDEV(major, 0), NULL, d_name);
    pr_info("Hello world 1. device name : %s\n", d_name);
    return 0;
}

static void __exit mod_exit(void)
{
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
    unregister_chrdev(major, d_name);
    pr_info("Goodbye world 1.\n");
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");