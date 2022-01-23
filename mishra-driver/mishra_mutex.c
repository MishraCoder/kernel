#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>			//kmalloc, kzalloc, krealloc
#include <linux/uaccess.h>		//copy_from/to_user()
#include <linux/kthread.h>		//kernel thred
#include <linux/sched.h>		//task_struct
#include <linux/delay.h>
#include <linux/mutex.h>

struct mutex chr_mutex;
unsigned long chr_global_var;

dev_t dev = 0;
static struct class *dev_class;
static struct cdev chr_cdev;

static int __init chr_driver_init(void);
static void __exit chr_driver_exit(void);

static struct task_struct *chr_thread1;
static struct task_struct *chr_thread2;

/*********** Driver Functions Decalarations ************/
static int chr_open(struct inode *inode, struct file *file);
static int chr_release(struct inode *inode, struct file *file);
static ssize_t chr_read(struct file *file,
		char __user *buf, size_t len, loff_t *off);
static ssize_t chr_write(struct file *file,
		const char *buf, size_t len, loff_t *off);
/******************************************************/

int thread_function1(void *pv);
int thread_function2(void *pv);

int thread_function1(void *pv) {
	while(!kthread_should_stop()) {
		mutex_lock(&chr_mutex);
		chr_global_var++;
		printk(KERN_INFO "In chr driver Thread Function1 %lu\n", chr_global_var);
		mutex_unlock(&chr_mutex);
		msleep(1000);
	}
	return 0;
}

int thread_function2(void *pv) {
	while(!kthread_should_stop()) {
		mutex_lock(&chr_mutex);
		chr_global_var++;
		printk(KERN_INFO "In chr driver Thread Function2 %lu\n", chr_global_var);
		mutex_unlock(&chr_mutex);
		msleep(1000);
	}
	return 0;
}

static struct file_operations fops =
{
	.owner	= 	THIS_MODULE,
	.read	=	chr_read,
	.write	=	chr_write,
	.open	=	chr_open,
	.release =	chr_release
};

static int chr_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Device File Opened...!!!\n");
	return 0;
}

static int chr_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Device File Closed...!!!\n");
	return 0;
}

static ssize_t chr_read(struct file *file,
	char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "Read function\n");
	return 0;
}

static ssize_t chr_write(struct file *file,
	const char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "Write Function\n");
	return len;
}

static int __init chr_driver_init(void)
{
	/*Allocating Major number*/
	if((alloc_chrdev_region(&dev, 0, 1, "chr_dev")) < 0) {
		printk(KERN_INFO "Cannt allocate major number\n");
		return -1;
	}
	printk(KERN_INFO "Major = %d Minor = %d\n", MAJOR(dev), MINOR(dev));

	/*Creating cdev structure */
	cdev_init(&chr_cdev, &fops);

	/*Adding character device to the system*/
	if((cdev_add(&chr_cdev, dev, 1)) < 0) {
		printk(KERN_INFO "Cannot add device to the system\n");
		goto r_class;
	}

	/*Creating struct class*/
	if((dev_class = class_create(THIS_MODULE,"chr_class")) == NULL){
		printk(KERN_INFO "Cannot create the struct class\n");
		goto r_class;
	}

	/*Creating device*/
	if((device_create(dev_class,NULL,dev,NULL,"chr_device")) == NULL){
		printk(KERN_INFO "Cannot create the Device \n");
		goto r_device;
	}

	mutex_init(&chr_mutex);

	/* Creating Thread 1 */
	chr_thread1 = kthread_run(thread_function1,NULL,"chr Thread1");

	if(chr_thread1) {
		printk(KERN_ERR "Kthread1 Created Successfully...\n");

	} else {
		printk(KERN_ERR "Cannot create kthread1\n");
		goto r_device;
	}

	/* Creating Thread 2 */
	chr_thread2 = kthread_run(thread_function2,NULL,"chr Thread2");

	if(chr_thread2) {
		printk(KERN_ERR "Kthread2 Created Successfully...\n");

	} else {
		printk(KERN_ERR "Cannot create kthread2\n");
		goto r_device;
	}

	printk(KERN_INFO "Device Driver Insert...Done!!!\n");
	return 0;

r_device:
	class_destroy(dev_class);

r_class:
	unregister_chrdev_region(dev,1);
	cdev_del(&chr_cdev);

	return -1;	
}

void __exit chr_driver_exit(void)
{
	kthread_stop(chr_thread1);
	kthread_stop(chr_thread2);
	device_destroy(dev_class,dev);
	class_destroy(dev_class);
	cdev_del(&chr_cdev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Device Driver Remove...Done!!\n");
}

module_init(chr_driver_init);
module_exit(chr_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SM <********@gmail.com>");
MODULE_DESCRIPTION("A simple char device driver - Mutex");
MODULE_VERSION("1.17");
