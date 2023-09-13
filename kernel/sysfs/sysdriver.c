
/** Headers */

#include<linux/cdev.h>
#include<linux/errno.h>
#include<linux/fs.h>
#include<linux/init.h>
#include<linux/ioport.h>
#include<linux/kernel.h>
#include<linux/kobject.h>
#include<linux/module.h>
#include<linux/sysfs.h>

/**
 * IOCTL MACROS
 */

#define WR_VALUE _IOW('a', 'a', int32_t *)
#define RD_VALUE _IOR('a', 'b', int32_t *)

int sysfs_value;
dev_t devno;


/** Kobject structure */
struct kobject *kobj_ref;

/**
 * Function prototypes
 */

static int myinit(void);
static void myexit(void);

static int myopen(struct inode *inode, struct file *file);
static int myclose(struct inode *inode, struct file *file);
static ssize_t myread(struct file *file, char *buff, size_t len, loff_t *offset);
static ssize_t mywrite(struct file *file, const char *buff, size_t len, loff_t *offset);
static long ioctl(struct file *file, unsigned int cmd, unsigned long arg);


/**
 * Functions to read and write in sysfs
 */

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buff);
static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count);


/** Structure to create attribute to kobject */
struct kobj_attribute etx_attr = __ATTR(sysfs_value, 0664, sysfs_show, sysfs_store);

/**
 * File operations and device info structure
 */

static struct file_operations my_routines = {
	.owner = THIS_MODULE,
	.open = myopen,
	.release = myclose,
	.read = myread,
	.write = mywrite,
	.unlocked_ioctl = ioctl,
};

struct d_info {
	char buff[1024];
	int len;
	int irq;
	void *addr;
	struct cdev c_dev;
} info;

/**
 * Module init function
 */

static int myinit(void)
{
	int ret;

	devno = MKDEV(42, 0);

	ret = register_chrdev_region(devno, 1, "Sysfs_driver");
	if (ret != 0) 
		pr_info("NOT LOADED\n");
	else
		pr_info("LOADED DRIVER\n");

	cdev_init(&info.c_dev, &my_routines);

	info.c_dev.owner = THIS_MODULE;

	ret = cdev_add(&info.c_dev, devno, 1);
	if (ret != 0)
		pr_info("Cdev not loaded\n");
	else
		pr_info("Cdev loaded\n");

	info.len = 0;
	
	kobj_ref = kobject_create_and_add("sysfs_etx",kernel_kobj);

        /*Creating sysfs file for sysfs_value*/
        if(sysfs_create_file(kobj_ref,&etx_attr.attr))
                pr_err("Cannot create sysfs file......\n");

	return 0;
}

/**
 * Module exit function
 */

static void myexit(void)
{
	kobject_put(kobj_ref);
	sysfs_remove_file(kobj_ref, &etx_attr.attr);
	cdev_del(&info.c_dev);
	unregister_chrdev_region(devno, 1);
        pr_info("Device Driver Remove...Done!!!\n");
}

/**
 * Function gets invoked when device file is opened
 */

static int myopen(struct inode *inode, struct file *file)
{
        pr_info("Device File Opened...!!!\n");
        
	return 0;
}
/** 
 * This function will be called when we close the Device file
 */ 
static int myclose(struct inode *inode, struct file *file)
{
        pr_info("Device File Closed...!!!\n");
        
	return 0;
}
 
/** 
 * This function will be called when we read the Device file
 */
static ssize_t myread(struct file *file, char *buff, size_t len, loff_t *offset)
{
        pr_info("Read function\n");
        
	return 0;
}
/**
 * This function will be called when we write the Device file
 */
static ssize_t mywrite(struct file *file, const char *buff, size_t len, loff_t *offset)
{
        pr_info("Write Function\n");

        return (ssize_t)len;
}

/**
 * Function gets invoked when read operation is performed in sysfs
 */

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buff)
{
        pr_info("Sysfs - Read!!!\n");
        return sprintf(buff, "%d\n", sysfs_value);
}

/** 
 * This function will be called when we write the sysfsfs file
 */

static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
        pr_info("Sysfs - Write!!!\n");
        sscanf(buff,"%d",&sysfs_value);
        
	return count;
}

/**
 * IOCTL function
 */

static long ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	pr_info("ioctl entered \n");
	switch (cmd) {
		case WR_VALUE :
			if (copy_from_user((int32_t *)&sysfs_value, (int32_t *)arg, sizeof(sysfs_value))) 
				pr_err("Data Write : Err!\n");
			
			pr_info("Value = %d\n", sysfs_value);

			break;

		case RD_VALUE :
			if (copy_to_user((int32_t *)arg, (int32_t *)&sysfs_value, sizeof(sysfs_value)))
				pr_err("Data Read : Err!\n");

			break;

		default :
			pr_info("Default\n");

	}

	return 0;
}

/**
 * To describe init and exit functions of module
 */

module_init(myinit);
module_exit(myexit);

/**
 * Module info
 */

MODULE_AUTHOR("Saketh");
MODULE_LICENSE("GPL");
