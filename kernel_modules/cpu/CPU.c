
//Librerías
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

/*Librerías necesarias para la lectura del CPU
/linux/proc/fs/stat --->  Archivo Utilizado como referencia
*/
#include <linux/cpumask.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/kernel_stat.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/irqnr.h>
#include <asm/cputime.h>
#include <linux/tick.h>

//Nombre del Archivo
static const char *filename = "cpu_info";

//Carga de las arquitecturas de Hardware
#ifndef arch_irq_stat_cpu
#define arch_irq_stat_cpu(cpu) 0
#endif
#ifndef arch_irq_stat
#define arch_irq_stat() 0
#endif

#ifdef arch_idle_time

static cputime64_t get_idle_time(int cpu)
{
	cputime64_t idle;

	idle = kcpustat_cpu(cpu).cpustat[CPUTIME_IDLE];
	if (cpu_online(cpu) && !nr_iowait_cpu(cpu))
		idle += arch_idle_time(cpu);
	return idle;
}

static cputime64_t get_iowait_time(int cpu)
{
	cputime64_t iowait;

	iowait = kcpustat_cpu(cpu).cpustat[CPUTIME_IOWAIT];
	if (cpu_online(cpu) && nr_iowait_cpu(cpu))
		iowait += arch_idle_time(cpu);
	return iowait;
}

#else
static u64 get_idle_time(int cpu)
{
	u64 idle, idle_time = -1ULL;

	if (cpu_online(cpu))
		idle_time = get_cpu_idle_time_us(cpu, NULL);

	if (idle_time == -1ULL)
		/* !NO_HZ or cpu offline so we can rely on cpustat.idle */
		idle = kcpustat_cpu(cpu).cpustat[CPUTIME_IDLE];
	else
		idle = usecs_to_cputime64(idle_time);

	return idle;
}

static u64 get_iowait_time(int cpu)
{
	u64 iowait, iowait_time = -1ULL;

	if (cpu_online(cpu))
		iowait_time = get_cpu_iowait_time_us(cpu, NULL);

	if (iowait_time == -1ULL)
		/* !NO_HZ or cpu offline so we can rely on cpustat.iowait */
		iowait = kcpustat_cpu(cpu).cpustat[CPUTIME_IOWAIT];
	else
		iowait = usecs_to_cputime64(iowait_time);

	return iowait;
}
#endif

static int show_stat(struct seq_file *p, void *v)
{
	int i, j;
	unsigned long jif;
	u64 Total = 0;
	u64 user, nice, system, idle, iowait, irq, softirq, steal;
	u64 guest, guest_nice;
	u64 sum = 0;
	u64 sum_softirq = 0;
	unsigned int per_softirq_sums[NR_SOFTIRQS] = {0};
	struct timespec boottime;

	user = nice = system = idle = iowait = irq = softirq = steal = 0;
	guest = guest_nice = 0;
	getboottime(&boottime);
	jif = boottime.tv_sec;

	for_each_possible_cpu(i)
	{
		user += kcpustat_cpu(i).cpustat[CPUTIME_USER];
		nice += kcpustat_cpu(i).cpustat[CPUTIME_NICE];
		system += kcpustat_cpu(i).cpustat[CPUTIME_SYSTEM];
		idle += get_idle_time(i);
		iowait += get_iowait_time(i);
		irq += kcpustat_cpu(i).cpustat[CPUTIME_IRQ];
		softirq += kcpustat_cpu(i).cpustat[CPUTIME_SOFTIRQ];
		steal += kcpustat_cpu(i).cpustat[CPUTIME_STEAL];
		guest += kcpustat_cpu(i).cpustat[CPUTIME_GUEST];
		guest_nice += kcpustat_cpu(i).cpustat[CPUTIME_GUEST_NICE];
		sum += kstat_cpu_irqs_sum(i);
		for (j = 0; j < NR_SOFTIRQS; j++)
		{
			unsigned int softirq_stat = kstat_softirqs_cpu(j, i);
			per_softirq_sums[j] += softirq_stat;
			sum_softirq += softirq_stat;
		}
	}

	Total = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
	//seq_printf(p, "CPU \t %%usr \t %%sys \t %%iowait \t %%idle \t Total\n");
	seq_printf(p, "All \t");
	seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(user));
	seq_printf(p, "\t ");
	seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(system));
	seq_printf(p, "\t");
	seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(iowait));
	seq_printf(p, "\t");
	seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(idle));
	seq_printf(p, "\t");
	seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(Total));
	seq_printf(p, "\n");

	for_each_online_cpu(i)
	{
		user = kcpustat_cpu(i).cpustat[CPUTIME_USER];
		nice = kcpustat_cpu(i).cpustat[CPUTIME_NICE];
		system = kcpustat_cpu(i).cpustat[CPUTIME_SYSTEM];
		idle = get_idle_time(i);
		iowait = get_iowait_time(i);
		irq = kcpustat_cpu(i).cpustat[CPUTIME_IRQ];
		softirq = kcpustat_cpu(i).cpustat[CPUTIME_SOFTIRQ];
		steal = kcpustat_cpu(i).cpustat[CPUTIME_STEAL];
		guest = kcpustat_cpu(i).cpustat[CPUTIME_GUEST];
		guest_nice = kcpustat_cpu(i).cpustat[CPUTIME_GUEST_NICE];
		Total = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;

		seq_printf(p, "cpu%d \t", i);
		seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(user));
		seq_printf(p, "\t ");
		seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(system));
		seq_printf(p, "\t");
		seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(iowait));
		seq_printf(p, "\t");
		seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(idle));
		seq_printf(p, "\t");
		seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(Total));
		seq_printf(p, "\n");
	}

	return 0;
}

static int stat_open(struct inode *inode, struct file *file)
{
	size_t size = 1024 + 128 * num_online_cpus();

	/* minimum size to display an interrupt count : 2 bytes */
	size += 2 * nr_irqs;
	return single_open_size(file, show_stat, NULL, size);
}

static const struct file_operations proc_stat_operations = {
	.open = stat_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int __init start_function(void)
{
	printk(KERN_INFO "Modulo Cargado .\n");
	proc_create(filename, 0, NULL, &proc_stat_operations);
	printk(KERN_INFO "Archivo Creado: /proc/%s.\n", filename);
	return 0;
}

static void __exit clean_function(void)
{
	remove_proc_entry(filename, NULL);
	printk(KERN_INFO "Modulo Eliminado.\n");
}

module_init(start_function);
module_exit(clean_function);

MODULE_AUTHOR("Alan Giovanni Guzman Toledo");
MODULE_DESCRIPTION("Modulo de kernel que escribe en un archivo llamado 'CPU_stat' informacion del CPU");
MODULE_LICENSE("GPL");
