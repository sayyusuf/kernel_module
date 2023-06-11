#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/version.h>


#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
# define HAVE_PROC_OPS
#endif

static char *d_name = "dummy";
module_param(d_name, charp, S_IRUSR | S_IWUSR | S_IRGRP | S_IRGRP);
MODULE_PARM_DESC(d_mame, "proc file device name");

static struct proc_dir_entry *proc_file;

#ifdef HAVE_PROC_OPS
    static const struct proc_ops proc_file_ops = {};
#else
    static const struct file_operations proc_file_ops = {}; 
#endif


static int __init init_proc_dev(void)
{
    proc_file = proc_create(d_name, 0644, NULL, &proc_file_ops);
    if (!proc_file)
        return (-ENOMEM);
    pr_info("/proc/%s proc file added\n", d_name);
    return (0);
}


static void __exit exit_proc_dev(void)
{
    proc_remove(proc_file);

}

module_init(init_proc_dev);
module_exit(exit_proc_dev);

MODULE_LICENSE("GPL");