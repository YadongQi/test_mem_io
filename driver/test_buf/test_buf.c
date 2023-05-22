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
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/kallsyms.h>
#include <asm/io.h>

#include "test_buf.h"

MODULE_DESCRIPTION("TBUF module");
MODULE_AUTHOR("Qi, Yadong");
MODULE_LICENSE("Dual BSD/GPL");

static int (*k_set_memory_ro)(unsigned long addr, int numpages) = NULL;
static int (*k_set_memory_rw)(unsigned long addr, int numpages) = NULL;

#define KB *(1024U)
#define MB *(1024U KB)

#define BUFF_SIZE (2648 MB)
#define BUFF_CNT64 (BUFF_SIZE/8)

static int tbuf_open(struct inode* inode, struct file * file) {
	pr_info("test buf opened!\n");
	return 0;
}

static uint32_t mem_ro = 0;

static unsigned long vmalloc_va_to_pa(void *va) {
	unsigned long pfn = 0;
	pfn = vmalloc_to_pfn(va);
	return __pa(pfn_to_kaddr(pfn)) + offset_in_page(va);
}

static uint64_t *g_buf = NULL;

static int alloc_buf(void) {
	unsigned long pfn = 0;
	int i = 0;
	if (g_buf)
		return 0;
	g_buf = vmalloc(BUFF_SIZE);
	if (!g_buf) {
		pr_err("vmalloc buffer failed!\n");
		return -1;
	}
	pfn = vmalloc_to_pfn(g_buf);
	pr_info("va=0x%lx, pfn=0x%lx, pfn2ka=0x%lx, pfn2ka_off=0x%lx\n",
			(unsigned long)g_buf,
			pfn,
			(unsigned long)pfn_to_kaddr(pfn),
			__pa(pfn_to_kaddr(pfn)) + offset_in_page(g_buf));

	for (i = 0; i < BUFF_CNT64; i++) {
		g_buf[i] = -1ULL;
	}

	if (mem_ro && k_set_memory_ro)
		k_set_memory_ro((uint64_t)g_buf, BUFF_SIZE >> PAGE_SHIFT);

	return 0;
}

static int free_buf(void) {
	if (mem_ro && k_set_memory_rw && g_buf)
		k_set_memory_rw((uint64_t)g_buf, BUFF_SIZE >> PAGE_SHIFT);
	if(g_buf) {
		vfree(g_buf);
		g_buf = NULL;
	}
	return 0;
}

static struct task_struct *buf_scan_thread = NULL;

static int tbuf_scan(void *data) {
	uint32_t i = 0, j = 0;
	int corrupted = 0;
	unsigned long pa =  0;

	pa = vmalloc_va_to_pa(g_buf);
	pr_info("Buffer:[%lx(0x%lx, 0x%x)], mr=%d\n", (unsigned long)g_buf, pa, BUFF_SIZE, mem_ro);
	while(!kthread_should_stop()) {
		for (j = 0; j < BUFF_CNT64; j++) {
			if (g_buf[j] != -1ULL) {
				pa = vmalloc_va_to_pa(&g_buf[j]);
				pr_err("Loop[%u],Buf[%lx(0x%lx)]=0x%llx\n", i, (unsigned long)&g_buf[j], pa, g_buf[j]);
				corrupted = 1;
				break;
			}
		}
		if (corrupted) {
			while(!kthread_should_stop()) {
				msleep_interruptible(1);
			}
			break;
		}
		i++;
		msleep_interruptible(1);;
	}

	free_buf();

	return 0;
}

static int tbuf_kthread_start(void) {
	if (buf_scan_thread)
		return -1;

	if (alloc_buf() != 0)
		return -1;

	buf_scan_thread = kthread_run(&tbuf_scan, NULL, "tbuf-scan-thread");
	if (!buf_scan_thread) {
		pr_err("Failed to run kthread: tbuf_scan()\n");
		return -ECHILD;
	}

	return 0;
}

static int tbuf_kthread_stop(void) {
	if (!buf_scan_thread)
		return 0;

	kthread_stop(buf_scan_thread);
	buf_scan_thread = NULL;
	return 0;
}


static long tbuf_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
	uint32_t mro = 0;
	uint64_t smro = 0, smrw = 0;
	pr_info("tbuf ioctl!\n");
	switch (cmd) {
	case TBUF_SETMEMRO:
		if (copy_from_user(&mro, (uint32_t *)arg, sizeof(mro))) {
			pr_err("Failed to read mro!\n");
			break;
		}
		mem_ro = mro;
		break;
	case TBUF_SET_KSETMRO:
		if (copy_from_user(&smro, (uint64_t *)arg, sizeof(smro))) {
			pr_err("Failed to read smro!\n");
			break;
		}
		pr_info("address of set_memory_ro:0x%llx\n", smro);
		k_set_memory_ro = (int (*)(unsigned long, int))smro;
		break;
	case TBUF_SET_KSETMRW:
		if (copy_from_user(&smrw, (uint64_t *)arg, sizeof(smrw))) {
			pr_err("Failed to read smro!\n");
			break;
		}
		pr_info("address of set_memory_rw:0x%llx\n", smrw);
		k_set_memory_rw = (int (*)(unsigned long, int))smrw;
		break;
	case TBUF_START:
		tbuf_kthread_start();
		break;
	case TBUF_STOP:
		tbuf_kthread_stop();
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
	tbuf_kthread_stop();
	pr_err("%s: Exit\n", __func__);
	chr_dev_clean();
}

module_init(mod_init);
module_exit(mod_exit);
