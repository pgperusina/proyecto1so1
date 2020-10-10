// #include <linux/module.h>
// #include <linux/kernel.h>
// #include <linux/init.h>

// #include <linux/fs.h>
// #include <linux/mm.h>

// #include <linux/proc_fs.h>
// #include <linux/seq_file.h>

// #include <asm/page.h>

// static const char *filename = "memory_info";
// struct sysinfo i;

// // static void show_val_kb(struct seq_file *m, const char *s, unsigned long num)
// // {
// //     seq_put_decimal_ull_width(m, s, num << (PAGE_SHIFT - 10), 8);
// //     seq_write(m, " kB\n", 4);
// // }

// static int show_memory_info(struct seq_file *f, void *v)
// {
//     si_meminfo(&i);
//     seq_printf(f, "%8lu\n", i.totalram);
//     seq_printf(f, "%8lu\n", i.freeram);
//     return 0;
// }

// static int meminfo_proc_open(struct inode *inode, struct file *file)
// {
//     return single_open(file, show_memory_info, NULL);
// }

// static const struct file_operations fops = {
//     .owner = THIS_MODULE,
//     .open = meminfo_proc_open,
//     .read = seq_read,
//     .llseek = seq_lseek,
//     .release = single_release,
// };

// static int __init ram_init(void)
// {
//     printk(KERN_INFO "Module loaded...\n");
//     proc_create(filename, 0, NULL, &fops);
//     printk(KERN_INFO "Device file created: /proc/%s.\n", filename);
//     return 0;
// }

// static void __exit ram_exit(void)
// {
//     remove_proc_entry(filename, NULL);
//     printk(KERN_INFO "Module removed...\n");
// }

// module_init(ram_init);
// module_exit(ram_exit);

// MODULE_AUTHOR("Pablo Gerardo García Perusina");
// MODULE_DESCRIPTION("Kernel module to show total and free RAM. OS1");
// MODULE_LICENSE("GPL");

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/cpufreq.h>

void __attribute__((weak)) arch_report_meminfo(struct seq_file *m) {}

static int my_proc_show(struct seq_file *m, void *v)
{
    struct sysinfo i;
    struct cpufreq_policy *cp;
    unsigned long uc_temp, tc_temp, um_temp, tm_temp;

    si_meminfo(&i);                //gets memory info
    used = i.totalram - i.freeram; //used memory
    total = i.totalram;            //total memory

    //cp = cpufreq_cpu_get(0); //returning 0
    //uc_temp = cp->cur - cp->min; //usec cpu freq
    //tc_temp = cp->max - cp->min; //total cpu freq

    seq_printf(m, "{\n\t\"used\":%lu,\n\t\"total\":%lu,\n}\n", used, total, ); //writing in JSON format in /proc/modulo file

    arch_report_meminfo(m);

    return 0;
}

static int my_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, my_proc_show, NULL);
}

static ssize_t my_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *f_pos)
{
    return 0;
}

static struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = my_proc_open,
    .release = single_release,
    .read = seq_read,
    .llseek = seq_lseek,
    .write = my_proc_write};

static int __init modulo_init(void)
{
    struct proc_dir_entry *entry;
    entry = proc_create("modulo", 0, NULL, &my_fops);
    if (!entry)
    {
        return -1;
    }
    else
    {
        printk(KERN_INFO "Start\n");
    }
    return 0;
}

static void __exit modulo_exit(void)
{
    remove_proc_entry("modulo", NULL);
    printk(KERN_INFO "End\n");
}

module_init(modulo_init);
module_exit(modulo_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Memory and CPU usage");
MODULE_AUTHOR("Some College Student");