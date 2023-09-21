/** 
 * This code represents the communication between driver and sysfs
 */

#include<linux/cdev.h>
#include<linux/errno.h>
#include<linux/fs.h>
#include<linux/init.h>
#include<linux/ioport.h>
#include<linux/kernel.h>
#include<linux/kobject.h>
#include<linux/module.h>
#include<linux/sysfs.h>

char sysfs_value[1024];
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
	if (ret != 0) { 
		pr_info("NOT LOADED\n");
		goto r_unreg;
	} else {
		pr_info("LOADED DRIVER\n");
	}

	cdev_init(&info.c_dev, &my_routines);

	info.c_dev.owner = THIS_MODULE;

	ret = cdev_add(&info.c_dev, devno, 1);
	if (ret != 0) {
		pr_info("Cdev not loaded\n");
		goto r_cdev;
	} else {
		pr_info("Cdev loaded\n");
	}

	info.len = 0;
	
	kobj_ref = kobject_create_and_add("sysfs_etx", kernel_kobj);
	if (kobj_ref == NULL) {
		pr_info("Unable to create kobject\n");
		goto r_kobj;
	}

        /*Creating sysfs file for sysfs_value*/
        if(sysfs_create_file(kobj_ref, &etx_attr.attr)) {
                pr_err("Cannot create sysfs file......\n");
		goto r_sys;
	}

	return 0;
	
	r_sys:
		sysfs_remove_file(kobj_ref, &etx_attr.attr);
	r_kobj:
		kobject_put(kobj_ref);
	r_cdev:
		cdev_del(&info.c_dev);
	r_unreg:
		unregister_chrdev_region(devno, 1);

	return -1;

}

/**
 * Module exit function
 */

static void myexit(void)
{
	sysfs_remove_file(kobj_ref, &etx_attr.attr);
	kobject_put(kobj_ref);
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
        return sprintf(buff, "%s\n", sysfs_value);
}

/** 
 * This function will be called when we write the sysfsfs file
 */

static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
        pr_info("Sysfs - Write!!!\n");
        sscanf(buff,"%s", sysfs_value);
        
	return count;
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
