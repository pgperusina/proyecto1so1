#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>

#include <linux/mman.h>
#include <linux/mmzone.h>
#include <linux/proc_fs.h>
#include <linux/percpu.h>
#include <linux/quicklist.h>
#include <linux/seq_file.h>
#include <linux/swap.h>
#include <linux/vmstat.h>
#include <linux/atomic.h>
#include <linux/vmalloc.h>

#include <asm/page.h>
#include <asm/pgtable.h>
#include "internal.h"

static const char *filename = "memory_info";
struct sysinfo i;

static void show_val_kb(struct seq_file *m, const char *s, unsigned long num)
{
    seq_put_decimal_ull_width(m, s, num << (PAGE_SHIFT - 10), 8);
    seq_write(m, " kB\n", 4);
}

static int show_memory_info(struct seq_file *f, void *v)
{
    si_meminfo(&i);
    show_val_kb(f, "TotalRAM:  ", i.totalram);
    show_val_kb(f, "FreeRAM:   ", i.freeram);
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
