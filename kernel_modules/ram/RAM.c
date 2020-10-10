#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mm.h>

static const char *filename = "memory_info";
struct sysinfo i;

static int show_memory_info(struct seq_file *f, void *v)
{
    si_meminfo(&i);
    seq_printf(f, "TotalRAM: %8lu\n", (i.totalram));
    seq_printf(f, "FreeRAM: %8lu\n", (i.freeram));
    return 0;
}

static int meminfo_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, show_memory_info, NULL);
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = meminfo_proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

static int __init ram_init(void)
{
    printk(KERN_INFO "Module loaded...\n");
    proc_create(filename, 0, NULL, &fops);
    printk(KERN_INFO "Device file created: /proc/%s.\n", filename);
    return 0;
}

static void __exit ram_exit(void)
{
    remove_proc_entry(filename, NULL);
    printk(KERN_INFO "Module removed...\n");
}

module_init(ram_init);
module_exit(ram_exit);

MODULE_AUTHOR("Pablo Gerardo García Perusina");
MODULE_DESCRIPTION("Kernel module to show total and free RAM. OS1");
MODULE_LICENSE("GPL");
