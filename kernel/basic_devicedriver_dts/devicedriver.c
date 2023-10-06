/** Headers for driver */

#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/errno.h>
#include<linux/fs.h>
#include<linux/init.h>
#include<linux/ioport.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/of.h>
#include<linux/platform_device.h>
#include<linux/poll.h>
#include<linux/version.h>


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

static int my_init(struct platform_device *pdev)
{
	int ret = 0;
	
	ret = alloc_chrdev_region(&devno, 0, 1, "Basic_Char_Driver");
	if (ret < 0) 
		pr_info("Basic_char_Driver is not loaded\n");
	else
		pr_info("Basic_Char_Driver loaded\n");

	cls = class_create(THIS_MODULE, "mycharbasic");
	dev = device_create(cls, NULL, devno, 0, "mydevss");

	cdev_init(&dinfo.c_dev, &my_routines);

	dinfo.c_dev.owner = THIS_MODULE;

	ret = cdev_add(&dinfo.c_dev, devno, 1);

	if (ret < 0) {
		pr_info("cdev_object is not loaded\n");
		unregister_chrdev_region(devno, 1);
	}
	else {
		pr_info("Cdev obj is loaded\n");
	}

	dinfo.no_char = 0;
	
	return ret;
}

/**
 * MODULE EXIT FUNCTION
 */
static int my_exit(struct platform_device *pdev)
{
	cdev_del(&dinfo.c_dev);
	unregister_chrdev_region(devno, 1);
	device_destroy(cls, devno);
	class_destroy(cls);
	pr_info("Basic_Char_Driver unloaded\n");

	return 0;
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

static const struct of_device_id sample_dt[] = {
	{ .compatible = "SampleDT", },
	{ },
};

MODULE_DEVICE_TABLE(of, sample_dt);

static struct platform_driver sample_pldriver = {
	.probe = my_init,
	.remove = my_exit,
	.driver = {
		.name = "Sample_pldrv",
		.of_match_table = of_match_ptr(sample_dt),
	},
};


static int sample_init(void)
{
	pr_info("Platform driver\n");

	platform_driver_register(&sample_pldriver);

	return 0;
}

void sample_exit(void)
{
	pr_info("Platform device exit\n");

	platform_driver_unregister(&sample_pldriver);

}

/** To identify init and exit functions */
module_init(sample_init);
module_exit(sample_exit);
