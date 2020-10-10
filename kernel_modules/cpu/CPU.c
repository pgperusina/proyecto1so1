// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/cpumask.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/kernel_stat.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/sched/stat.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/irqnr.h>
#include <linux/sched/cputime.h>
#include <linux/tick.h>
#include <linux/delay.h>

#ifndef arch_irq_stat_cpu
#define arch_irq_stat_cpu(cpu) 0
#endif
#ifndef arch_irq_stat
#define arch_irq_stat() 0
#endif

#ifdef arch_idle_time

static u64 get_idle_time(int cpu)
{
	u64 idle;

	idle = kcpustat_cpu(cpu).cpustat[CPUTIME_IDLE];
	if (cpu_online(cpu) && !nr_iowait_cpu(cpu))
		idle += arch_idle_time(cpu);
	return idle;
}

static u64 get_iowait_time(int cpu)
{
	u64 iowait;

	iowait = kcpustat_cpu(cpu).cpustat[CPUTIME_IOWAIT];
	if (cpu_online(cpu) && nr_iowait_cpu(cpu))
		iowait += arch_idle_time(cpu);
	return iowait;
}

#else

static u64 get_idle_time(int cpu)
{
	u64 idle, idle_usecs = -1ULL;

	if (cpu_online(cpu))
		idle_usecs = get_cpu_idle_time_us(cpu, NULL);

	if (idle_usecs == -1ULL)
		/* !NO_HZ or cpu offline so we can rely on cpustat.idle */
		idle = kcpustat_cpu(cpu).cpustat[CPUTIME_IDLE];
	else
		idle = idle_usecs * NSEC_PER_USEC;

	return idle;
}

static u64 get_iowait_time(int cpu)
{
	u64 iowait, iowait_usecs = -1ULL;

	if (cpu_online(cpu))
		iowait_usecs = get_cpu_iowait_time_us(cpu, NULL);

	if (iowait_usecs == -1ULL)
		/* !NO_HZ or cpu offline so we can rely on cpustat.iowait */
		iowait = kcpustat_cpu(cpu).cpustat[CPUTIME_IOWAIT];
	else
		iowait = iowait_usecs * NSEC_PER_USEC;

	return iowait;
}

#endif

static u64 nano_to_clock_t(u64 x)
{
	if (NSEC_PER_SEC % USER_HZ == 0)
	{
		return div_u64(x, NSEC_PER_SEC / USER_HZ);
	}
	else if (USER_HZ % 512 == 0)
	{
		return div_u64(x * USER_HZ / 512, NSEC_PER_SEC / 512);
	}
	else
	{
		/*
					* max relative error 5.7e-8 (1.8s per year) for USER_HZ <= 1024,
					* overflow after 64.99 years.
					* exact for HZ=60, 72, 90, 120, 144, 180, 300, 600, 900, ...
					*/
		return div_u64(x * 9, (9ull * NSEC_PER_SEC + (USER_HZ / 2)) / USER_HZ);
	}
}

static int show_stat(struct seq_file *p, void *v)
{
	int i;
	u64 user, nice, system, idle, iowait, irq, softirq, steal;
	u64 guest, guest_nice;
	u64 sum, last_sum = 0;
	u64 last_cpu_idle = 0;
	int32_t cpu_delta, cpu_used = 0;
	int32_t cpu_usage, cpu_idle = 0;
	struct timespec64 boottime;

	user = nice = system = idle = iowait =
		irq = softirq = steal = 0;
	guest = guest_nice = 0;
	getboottime64(&boottime);

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
	}

	sum = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
	cpu_idle = idle;

	seq_printf(p, "First \t");
	seq_printf(p, "%llu", nano_to_clock_t(user));
	seq_printf(p, "\t ");
	seq_printf(p, "%llu", nano_to_clock_t(system));
	seq_printf(p, "\t");
	seq_printf(p, "%llu", nano_to_clock_t(iowait));
	seq_printf(p, "\t");
	seq_printf(p, "%llu", nano_to_clock_t(idle));
	seq_printf(p, "\t");
	seq_printf(p, "%llu", nano_to_clock_t(sum));
	seq_printf(p, "\n");

	cpu_usage = 100 - (100 * cpu_idle) / sum;

	// msleep(2000);

	// i = 0;
	// user = 0;
	// nice = 0;
	// system = 0;
	// idle = 0;
	// iowait = 0;
	// irq = 0;
	// softirq = 0;
	// steal = 0;
	// guest = 0;
	// guest_nice = 0;

	// for_each_possible_cpu(i)
	// {
	// 	user += kcpustat_cpu(i).cpustat[CPUTIME_USER];
	// 	nice += kcpustat_cpu(i).cpustat[CPUTIME_NICE];
	// 	system += kcpustat_cpu(i).cpustat[CPUTIME_SYSTEM];
	// 	idle += get_idle_time(i);
	// 	iowait += get_iowait_time(i);
	// 	irq += kcpustat_cpu(i).cpustat[CPUTIME_IRQ];
	// 	softirq += kcpustat_cpu(i).cpustat[CPUTIME_SOFTIRQ];
	// 	steal += kcpustat_cpu(i).cpustat[CPUTIME_STEAL];
	// 	guest += kcpustat_cpu(i).cpustat[CPUTIME_GUEST];
	// 	guest_nice += kcpustat_cpu(i).cpustat[CPUTIME_GUEST_NICE];
	// }

	// last_sum = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
	// last_cpu_idle = idle;

	// cpu_delta = sum - last_sum;
	// cpu_idle = cpu_idle - last_cpu_idle;
	// seq_printf(p, "delta \t");
	// seq_printf(p, "%d", abs(cpu_delta));
	// seq_printf(p, "\n");
	// seq_printf(p, "idle_delta \t");
	// seq_printf(p, "%d", abs(cpu_idle));
	// seq_printf(p, "\n");

	// cpu_used = abs(cpu_delta) - abs(cpu_idle);

	// cpu_usage = 100 - (100 * cpu_idle) / cpu_delta;

	// seq_printf(p, "Last \t");
	// seq_printf(p, "%llu", nano_to_clock_t(user));
	// seq_printf(p, "\t ");
	// seq_printf(p, "%llu", nano_to_clock_t(system));
	// seq_printf(p, "\t");
	// seq_printf(p, "%llu", nano_to_clock_t(iowait));
	// seq_printf(p, "\t");
	// seq_printf(p, "%llu", nano_to_clock_t(idle));
	// seq_printf(p, "\t");
	// seq_printf(p, "%llu", nano_to_clock_t(last_sum));
	// seq_printf(p, "\n");

	// seq_printf(p, "CPU used \t");
	// seq_printf(p, "%d", cpu_used);
	// seq_printf(p, "\n");

	seq_printf(p, "CPU usage \t");
	seq_printf(p, "%d", cpu_usage);
	seq_printf(p, "\n");

	return 0;
}

static int stat_open(struct inode *inode, struct file *file)
{
	unsigned int size = 1024 + 128 * num_online_cpus();

	/* minimum size to display an interrupt count : 2 bytes */
	size += 2 * nr_irqs;
	return single_open_size(file, show_stat, NULL, size);
}

static const struct file_operations proc_stat_operations = {
	.owner = THIS_MODULE,
	.open = stat_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int __init proc_stat_init(void)
{
	proc_create("cpu_info", 0, NULL, &proc_stat_operations);
	return 0;
}

// static int __init start_function(void)
// {
// 	printk(KERN_INFO "Modulo Cargado .\n");
// 	proc_create("cpu_info", 0, NULL, &stat_proc_ops);
// 	printk(KERN_INFO "Archivo Creado: /proc/%s.\n", "cpu_info");
// 	return 0;
// }

static void __exit clean_function(void)
{
	remove_proc_entry("cpu_info", NULL);
	printk(KERN_INFO "Modulo Eliminado.\n");
}

module_init(proc_stat_init);
module_exit(clean_function);

MODULE_AUTHOR("PGGP");
MODULE_DESCRIPTION("Modulo de kernel CPU");
MODULE_LICENSE("GPL");