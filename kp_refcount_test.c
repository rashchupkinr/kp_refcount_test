#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/printk.h>
#include <linux/refcount.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("kp_refcount_test");
MODULE_VERSION("0.8");

#define NREF 100
struct ref_holder
{
	int v;
} ref_holders[NREF] = {0};

refcount_t count;

static int refcount_test_iter(void)
{
	int i;
	for (i=0; i<NREF; i++) {
		refcount_inc(&count);
	}
	for (i=0; i<NREF; i++) {
		if (refcount_dec_and_test(&count))
			return -1;
	}
	return 0;
}

static int refcount_test(void)
{
	int i;
	for (i=0; i<1000; i++)
		if (refcount_test_iter())
			return -1;
	return 0;
}

static __init int refcount_test_init(void)
{
	refcount_set(&count, 1);
	if (refcount_test())
		printk(KERN_ERR "refcount_test: error\n");
	else
		printk(KERN_ERR "refcount_test: success\n");
	return 0;
}

module_init(refcount_test_init);
