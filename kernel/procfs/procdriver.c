
/** Headers */

#include<linux/cdev.h>
#include<linux/errno.h>
#include<linux/fs.h>
#include<linux/init.h>
#include<linux/ioport.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/poll.h>
#include<linux/proc_fs.h>


dev_t devno;

/** Structure to create directory in procfs */
static struct proc_dir_entry *parent;


/**
 * Function prototypes 
 */

static int myopen(struct inode *inode, struct file *file);
static int myclose(struct inode *inode, struct file *file);
static ssize_t myread(struct file *file, char *buff, size_t len, loff_t *offset);
static ssize_t mywrite(struct file *file, const char *buff, size_t len, loff_t *offset);

/**
 * Procfs function prototypes
 */

static int proc_open(struct inode *inode, struct file *file);
static int proc_close(struct inode *inode, struct file *file);
static ssize_t proc_read(struct file *file, char *buff, size_t len, loff_t *offset);
static ssize_t proc_write(struct file *file, const char *buff, size_t len, loff_t *offset);

/**
 * File operations structure
 */

static struct file_operations myroutins = {
	.owner = THIS_MODULE,
	.read = myread,
	.write = mywrite,
	.open = myopen,
	.release = myclose,
};

/**
 * Procfs file operations structure
 */

static struct proc_ops proc_routins = {
	.proc_read = proc_read,
	.proc_write = proc_write,
	.proc_open = proc_open,
	.proc_release = proc_close,
};

/**
 * Device info structure
 */

struct d_info {
	char buff[1024];
	int cnt;
	int irq;
	void *addr;
	struct cdev c_dev;
} info;

/**
 * Module Init function
 */

static int myinit(void) 
{
	int ret;

	devno = MKDEV(42, 0);

	ret = register_chrdev_region(devno, 1, "Procfs_basic");
	if (ret != 0)
		pr_info("Procfs_baasic not loaded\n");
	else
		pr_info("Procfs_basic loaded\n");

	cdev_init(&info.c_dev, &myroutins);

	info.c_dev.owner = THIS_MODULE;

	ret = cdev_add(&info.c_dev, devno, 1);
	if (ret != 0)
		pr_info("Cdev not loaded\n");
	else
		pr_info("Cdev loaded\n");
	
	info.cnt = 0;

	parent = proc_mkdir("saketh",NULL);
	if (parent == NULL) 
		pr_info("Unable to create parent in proc\n");

	proc_create("proc", 0666, parent, &proc_routins);

	pr_info("DRIVER LOADED\n");

	return 0;
}

/**
 * Module Exit function
 */

static void myexit(void)
{
	proc_remove(parent);
	cdev_del(&info.c_dev);
	unregister_chrdev_region(devno, 1);
	pr_info("DRIVER UNLOADED\n");
}

/**
 * Function will be implemented when device file is opened
 */

static int myopen(struct inode *inode, struct file *file)
{
	try_module_get(THIS_MODULE);

	pr_info("FILE OPENED\n");

	file->private_data = &info;

	return 0;
}

/**
 * Function will be implemented when device file is closed
 */

static int myclose(struct inode *inode, struct file *file)
{
	module_put(THIS_MODULE);
	pr_info("FILE CLOSED\n");
	
	return 0;
}

/**
 * Function will be implemented when write is performed in device file 
 */

static ssize_t mywrite(struct file *file, const char *buf, size_t len, loff_t *offset)
{
	struct d_info *tdev;
	unsigned long res;

	tdev = file->private_data;

	if (len > 1024 - tdev->cnt)
		len = 1024 - tdev->cnt;
	
	res = copy_from_user(tdev->buff, buf, len);
	if (res != 0)
		pr_info("UNSUCCESSFUL WRITE\n");

	tdev->cnt += len;

	pr_info("DATA WRITTEN\n");

	return (ssize_t)len;
}

/**
 * Function will be implemented when read is performed in device file 
 */

static ssize_t myread(struct file *file, char *buf, size_t len, loff_t *offset)
{
	struct d_info *tdev;
	unsigned long res;

	tdev = file->private_data;

	if (len > tdev->cnt)
		len = tdev->cnt;
	
	res = copy_to_user(buf, tdev->buff, len);
	if (res != 0)
		pr_info("UNSUCCESSFUL READ\n");

	tdev->cnt = 0;

	pr_info("DATA READ\n");

	return (ssize_t)len;
}

/**
 * Function will be implemented when procfs file is opened
 */

static int proc_open(struct inode *inode, struct file *file)
{
	file->private_data = &info;
	try_module_get(THIS_MODULE);

        pr_info("PROC FILE OPENED\n");

        return 0;
}

/**
 * Function will be implemented when procfs file is closed
 */

static int proc_close(struct inode *inode, struct file *file)
{
        module_put(THIS_MODULE);
	pr_info("PROC FILE CLOSED\n");

        return 0;
}

/**
 * Function will be implemented when write is performed in procfs file 
 */

static ssize_t proc_write(struct file *file, const char *buf, size_t len, loff_t *offset)
{
        struct d_info *tdev;
        unsigned long res;

        tdev = file->private_data;

        if (len > 1024 - tdev->cnt)
                len = 1024 - tdev->cnt;

        res = copy_from_user(tdev->buff, buf, len);
        if (res != 0)
                pr_info("UNSUCCESSFUL WRITE\n");

        tdev->cnt += len;

        pr_info("DATA WRITTEN IN PROCFS\n");

        return (ssize_t)len;
}

/**
 * Function will be implemented when read is performed in procfs file 
 */

static ssize_t proc_read(struct file *file, char *buf, size_t len, loff_t *offset)
{
        struct d_info *tdev;
        unsigned long res;

        tdev = file->private_data;

        if (len > tdev->cnt)
                len = tdev->cnt;

        res = copy_to_user(buf, tdev->buff, len);
        if (res != 0)
                pr_info("UNSUCCESSFUL READ\n");

        tdev->cnt = 0;

        pr_info("DATA READ IN PROCFS\n");

        return (ssize_t)len;
}

/**
 * Module information
 */

MODULE_AUTHOR("Saketh");
MODULE_LICENSE("GPL");

/**
 * To describe module init and exit function
 */

module_init(myinit);
module_exit(myexit);
