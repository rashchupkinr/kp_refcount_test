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

refcount_t count;

int refcount_test_iter(void)
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

int refcount_test(void)
{
	int i;
	refcount_set(&count, 1);
	for (i=0; i<1000; i++)
		if (refcount_test_iter())
			return -1;
	if (refcount_read(&count) == 1)
		return 0;
	return -1;
}
EXPORT_SYMBOL(refcount_test);

void start_refcount_test(void)
{
	if (refcount_test())
		printk(KERN_ERR "refcount_test: error\n");
	else
		printk(KERN_ERR "refcount_test: success\n");
}

struct delayed_work kp_test_work;
void kp_refcount_test_work(struct work_struct *work)
{
	start_refcount_test();
	schedule_delayed_work(&kp_test_work, msecs_to_jiffies(1000));
}

static __init int kp_refcount_test_init(void)
{
	INIT_DELAYED_WORK(&kp_test_work, kp_refcount_test_work);
	schedule_delayed_work(&kp_test_work, msecs_to_jiffies(1000));
	return 0;
}

static __exit void kp_refcount_test_exit(void)
{
	cancel_delayed_work_sync(&kp_test_work);
}

module_init(kp_refcount_test_init);
module_exit(kp_refcount_test_exit);
