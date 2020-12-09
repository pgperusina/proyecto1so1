#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h> 
#include <linux/hugetlb.h>
#include <linux/sched/signal.h>
#include <linux/sched.h>
#include <unistd.h>
 


struct task_struct *task;
struct task_struct *task_child;
struct list_head *list;
static const long pagesize sysconf(_SC_PAGE_SIZE) / 1024;  // in Kb
static const long pagesize2 = sysconf(_SC_PAGESIZE) / 1024;  // in Kb

static const char *filename = "m_grupo4";

static int show_cpu_info(struct seq_file *f, void *v) {
        seq_printf(f, "pagesize: %ld\n\t", pagesize);
        seq_printf(f, "pagesize 2: %ld\n\t", pagesize2);
        long ram;
        //Procesos Padre
        seq_printf(f, "{\n\t[\n\t")
	for_each_process(task){
                ram = (task->mm->total_vm * pagesize) / 1024;  // number of pages times pagesize in Mb
                
                seq_printf(f, "{\n\t\"PID\":\"%d\",\n\t\"nombre\":\"%d\",\n\t\"usuario\":\"%d\",\n\t\"estado\":\"%d\",\n\t\"RAM\":\"%dd\"\n,\n\t\"children\":\n", task->pid, task->comm, task->uid, task->state, ram);
                long child_ram;
                //Procesos Hijos
                seq_printf(f, "[\n\t");
                list_for_each(list, &task->children){
                        task_child = list_entry(list, struct task_struct, sibling);
                        ram = (task_child->mm->total_vm * pagesize) / 1024;  // number of pages times pagesize in Mb
                        seq_printf(f, "{\n\t\"PID\":\"%d\",\n\t\"nombre\":\"%d\",\n\t\"usuario\":\"%d\",\n\t\"estado\":\"%d\",\n\t\"RAM\":\"%ld\"\n},\n", task_child->pid, task_child->comm, task_child->uid, task_child->state, child_ram);
                }
                seq_printf(f, "]\n")
                seq_printf(f, "},\n")
	}
        seq_printf(f, "]\n}\n")
        return 0;
}

static int open_file_function(struct inode *inode, struct  file *file) {
        return single_open(file, show_cpu_info, NULL);
}

static struct file_operations fops =
{
        .owner = THIS_MODULE,
        .open = open_file_function,
        .read = seq_read,
        .llseek = seq_lseek,
        .release = single_release,
};


/**
*	Defino que es lo que se va a hacer al cargar el modulo
*/
static int __init iniciar(void)
{
        printk(KERN_INFO "Module loaded...\n");
        proc_create(filename, 0, NULL, &fops);
        printk(KERN_INFO "Device file created: /proc/%s.\n", filename);
        printk(KERN_INFO "Buenas, att: Grupo 4, monitor de procesos")
        return 0;
}

/**
*	Defino que es lo que se va a hacer al terminar el modulo
*/
static void __exit terminar(void)
{
	remove_proc_entry(filename,NULL);
  	printk(KERN_INFO "Module removed...\n");
        printk(KERN_INFO "Bai, att: Grupo 4 y este fue el monitor de procesos.")
}

module_init(iniciar);
module_exit(terminar);

MODULE_AUTHOR("Sistemas Operativos 2 - USAC - Grupo No. 4");
MODULE_DESCRIPTION("Kernel module to show CPU processes.");
MODULE_LICENSE("GPL");