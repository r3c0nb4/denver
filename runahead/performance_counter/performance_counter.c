#include <linux/kernel.h>
#include <linux/module.h>

static void enable(void *in)
{
u64 val;
	/* Disable cycle counter overflow interrupt */
	asm volatile("msr pmintenclr_el1, %0" : : "r" ((u64)(1 << 31)));
	/* Enable cycle counter */
	asm volatile("msr pmcntenset_el0, %0" :: "r" BIT(31));
	/* Enable user-mode access to cycle counters. */
	asm volatile("msr pmuserenr_el0, %0" : : "r"(BIT(0) | BIT(2)));
	/* Clear cycle counter and start */
	asm volatile("mrs %0, pmcr_el0" : "=r" (val));
	val |= (BIT(0) | BIT(2));
	isb();
	asm volatile("msr pmcr_el0, %0" : : "r" (val));
	val = BIT(27);
	asm volatile("msr pmccfiltr_el0, %0" : : "r" (val));
}

static void disable(void *in)
{
	asm volatile("msr pmcntenset_el0, %0" :: "r" (0 << 31));
	asm volatile("msr pmuserenr_el0, %0" : : "r"((u64)0));

}

static int __init init(void)
{
	on_each_cpu(enable, NULL, 1);
	return 0;

}

static void __exit fini(void)
{
	on_each_cpu(disable, NULL, 1);
}

module_init(init);
module_exit(fini);
