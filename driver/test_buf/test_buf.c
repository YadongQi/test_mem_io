#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/err.h>
#include <linux/set_memory.h>
#include <linux/mm.h>
#include <asm/io.h>

#include "test_buf.h"

MODULE_DESCRIPTION("TBUF module");
MODULE_AUTHOR("Qi, Yadong");
MODULE_LICENSE("Dual BSD/GPL");

#define KB *(1024U)
#define MB *(1024U KB)

#define BUFF_SIZE (2048 MB)
#define BUFF_CNT64 (BUFF_SIZE/8)

static int tbuf_open(struct inode* inode, struct file * file) {
	pr_info("test buf opened!\n");
	return 0;
}

static uint32_t loops = 1;
static uint32_t mem_ro = 0;

static void tbuf_start(void *arg) {
	uint32_t i = 0, j = 0;
	int corrupted = 0;
	unsigned long pfn = 0;
	uint64_t *buf = vmalloc(BUFF_SIZE);
	if (!buf) {
		pr_err("vmalloc buffer failed!\n");
		return;
	}
	pfn = vmalloc_to_pfn(buf);
	pr_info("va=0x%lx, pfn=0x%lx, pfn2ka=0x%lx, pfn2ka_off=0x%lx\n", (unsigned long)buf, pfn, (unsigned long)pfn_to_kaddr(pfn), __pa(pfn_to_kaddr(pfn)) + offset_in_page(buf));
	for (i = 0; i < BUFF_CNT64; i++) {
		buf[i] = -1ULL;
	}
#if 0
	if (mem_ro)
		set_memory_ro((uint64_t)buf, BUFF_SIZE >> PAGE_SHIFT);
#endif

	pr_info("Buffer:[%lx(0x%llx), 0x%x], Loop counts:%d, mr=%d\n", (unsigned long)buf, virt_to_phys(buf), BUFF_SIZE, loops, mem_ro);
	for (i = 0; i < loops; i++) {
		for (j = 0; j < BUFF_CNT64; j++) {
			if (buf[j] != -1ULL) {
				pr_err("Loop[%u],Buf[%p]=0x%llx\n", i, &buf[j], buf[j]);
				corrupted = 1;
				break;
			}
		}
		if (corrupted)
			break;
	}
	if (buf)
		vfree(buf);
	return;
}

static long tbuf_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
	uint32_t lps = 0, mro = 0;
	pr_info("tbuf ioctl!\n");
	switch (cmd) {
	case TBUF_SETLOOPS:
		if (copy_from_user(&lps, (uint32_t *)arg, sizeof(loops))) {
			pr_err("Failed to read loops!\n");
			break;
		}
		loops = (lps == 0) ? 100 : lps;
		break;
	case TBUF_SETMEMRO:
		if (copy_from_user(&mro, (uint32_t *)arg, sizeof(mro))) {
			pr_err("Failed to read mro!\n");
			break;
		}
		mem_ro = mro;
		break;
	case TBUF_START:
		smp_call_function_single(0, tbuf_start, NULL, 1);
		break;
	default:
		pr_err("Unknown IOCTL cmd!\n");
		break;
	}
	return 0;
}

static const struct file_operations tbuf_fops = {
	.open		= tbuf_open,
	.unlocked_ioctl = tbuf_ioctl,
};

dev_t dev = 0;
static struct class *tbuf_class;
static struct cdev tbuf_cdev;

static int __init chr_dev_init(void)
{
	if((alloc_chrdev_region(&dev, 0, 1, "tbuf")) <0){
		pr_err("Cannot allocate major number\n");
		return -1;
	}
	pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
	cdev_init(&tbuf_cdev, &tbuf_fops);
 
	if((cdev_add(&tbuf_cdev, dev, 1)) < 0){
		pr_err("Cannot add the device to the system\n");
		goto r_class;
	}
 
	if(IS_ERR(tbuf_class = class_create(THIS_MODULE, "tbuf_class"))){
		pr_err("Cannot create the struct class\n");
		goto r_class;
	}
 
	/*Creating device*/
	if(IS_ERR(device_create(tbuf_class, NULL, dev, NULL, "tbuf"))){
		pr_err("Cannot create the Device 1\n");
		goto r_device;
	}
	pr_info("TBUF Device Insert...Done!!!\n");
	return 0;
 
r_device:
	class_destroy(tbuf_class);
r_class:
	unregister_chrdev_region(dev,1);
	return -1;

}

static void __exit chr_dev_clean(void) {
		device_destroy(tbuf_class, dev);
		class_destroy(tbuf_class);
		cdev_del(&tbuf_cdev);
		unregister_chrdev_region(dev, 1);
}

static int mod_init(void)
{
	pr_err("%s: INIT\n", __func__);
	chr_dev_init();
	return 0;
}

static void mod_exit(void)
{
	pr_err("%s: Exit\n", __func__);
	chr_dev_clean();
}

module_init(mod_init);
module_exit(mod_exit);
