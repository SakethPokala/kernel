/** Headers for driver */

#include<linux/cdev.h>
#include<linux/errno.h>
#include<linux/fs.h>
#include<linux/init.h>
#include<linux/ioport.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/poll.h>


/** MODULE INFORMATION */

MODULE_AUTHOR("Saketh");
MODULE_LICENSE("GPL");

/** structure to create class and device */
struct class *cls;
struct device *dev;

dev_t devno;

/** Function declarations */

static int my_open(struct inode *inode, struct file *file);
static int my_close(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *file, char *buf, size_t len, loff_t *offset);
static ssize_t my_write(struct file *file, const char *buf, size_t len, loff_t *offset);

/** File operations and device info structures*/

static struct file_operations my_routines = {
	.owner = THIS_MODULE,
	.read  = my_read,
	.write = my_write,
	.open = my_open,
	.release = my_close,
};

struct d_info {
	char buf[1024];
	int no_char;
	void *addr;
	int irq;
	struct cdev c_dev;
} dinfo;

/**
 * MODULE INIT FUNCTION
 */

static int my_init(void)
{
	int ret = 0;
	
	ret = alloc_chrdev_region(&devno, 0, 1, "Basic_Char_Driver");
	if (ret < 0) { 
		pr_info("Basic_char_Driver is not loaded\n");
		return -1;
	} else {
		pr_info("Basic_Char_Driver loaded\n");
	}

	cls = class_create(THIS_MODULE, "mycharbasic");
	if (cls == NULL) {
		pr_info("can not create the class\n");
		goto r_class;
	}
	
	dev = device_create(cls, NULL, devno, 0, "mydevss");
	if (dev == NULL) {
		pr_info("cannot create device\n");
		goto r_device;
	}

	cdev_init(&dinfo.c_dev, &my_routines);

	dinfo.c_dev.owner = THIS_MODULE;

	ret = cdev_add(&dinfo.c_dev, devno, 1);
	if (ret < 0) {
		pr_info("cdev_object is not loaded\n");
		goto r_class;
	} else {
		pr_info("Cdev obj is loaded\n");
	}

	dinfo.no_char = 0;
	
	return ret;
	
	r_device:
		class_destroy(cls);
	r_class:
		unregister_chrdev_region(devno, 1);
	
		return -1;
}

/**
 * MODULE EXIT FUNCTION
 */
static void my_exit(void)
{
	cdev_del(&dinfo.c_dev);
	unregister_chrdev_region(devno, 1);
	device_destroy(cls, devno);
	class_destroy(cls);
	pr_info("Basic_Char_Driver unloaded\n");
}

/**
 * This function is implemented when the device file is opened
 */

static int my_open(struct inode *inode, struct file *file)
{
	try_module_get(THIS_MODULE);

	file->private_data = &dinfo;
	
	if (file->f_mode & FMODE_READ)
		pr_info("OPEN FOR READ\n");
	if (file->f_mode & FMODE_WRITE)
		pr_info("OPEN FOR WRITE\n");
	
	return 0;
}

/**
 * This function is implemented when device file is closed
 */

static int my_close(struct inode *inode, struct file *file)
{
	module_put(THIS_MODULE);

	pr_info("Device file closed\n");

	return 0;
}

/**
 * This function is implemented when write operation is performed on device file
 */

static ssize_t my_write(struct file *file, const char *buf, size_t len, loff_t *offset)
{
	struct d_info *tdev;
	unsigned long res;

	tdev = file->private_data;

	if (len > 1024 - tdev->no_char)
		len = 1024 - tdev->no_char;
	
	res = copy_from_user(tdev->buf, buf, len);
	if (res != 0)
		pr_info("Unsuccessful write\n");

	tdev->no_char += len;

	pr_info("DATA WRITTEN\n");

	return (ssize_t)len;
}

/**
 * This function is implemented when read operation is performed on device file
 */

static ssize_t my_read(struct file *file, char *buf, size_t len, loff_t *offset)
{
	struct d_info *tdev;
	unsigned long res;

	tdev = file->private_data;

	if (len > tdev->no_char)
		len = tdev->no_char;
	
	res = copy_to_user(buf, tdev->buf, len);
	if (res != 0)
		pr_info("UNSUCCESSFUL READ\n");

	tdev->no_char = 0;

	pr_info("DATA READ\n");

	return (ssize_t)len;
}

/** To identify init and exit functions */
module_init(my_init);
module_exit(my_exit);
