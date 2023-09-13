/** HEADERS */

#include<linux/cdev.h>
#include<linux/errno.h>
#include<linux/fs.h>
#include<linux/init.h>
#include<linux/ioctl.h>
#include<linux/ioport.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/poll.h>

/** IOCTL MACROS for read and write */

#define WR_VALUE _IOW('a', 'a', int32_t *)
#define RD_VALUE _IOR('a', 'b', int32_t *)

dev_t devno;
int32_t val;

/** Function Prototypes */

static int myopen(struct inode *inode, struct file *file);
static int myclose(struct inode *inode, struct file *file);
static ssize_t myread(struct file *file, char *buff, size_t len, loff_t *offset);
static ssize_t mywrite(struct file *file, const  char *buff, size_t len, loff_t *offset);
static long ioctl(struct file *file, unsigned int cmd, unsigned long arg);


/** File operations and device info structure */

static struct file_operations myroutins = {
	.owner = THIS_MODULE,
	.read = myread,
	.write = mywrite,
	.open = myopen,
	.release = myclose,
	.unlocked_ioctl = ioctl,
};

struct d_info {
	char buff[1024];
	int irq;
	int no;
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

	ret = register_chrdev_region(devno, 1, "IOCTL_BASIC");
	if (ret != 0)
		pr_info("IOCTL NOT LOADED\n");
	else
		pr_info("IOCTL LOADED\n");

	cdev_init(&info.c_dev, &myroutins);

	ret = cdev_add(&info.c_dev, devno, 1);
	if (ret != 0)
		pr_info("CDEV UNSUCCESSFUL\n");
	else
		pr_info("CDEV SUCCESSFUL\n");
	
	info.c_dev.owner = THIS_MODULE;
	info.no = 0;

	return ret;
}

/** 
 * Module exit function 
 */

static void myexit(void)
{
	cdev_del(&info.c_dev);
	unregister_chrdev_region(devno, 1);
	pr_info("IOCTL UNLOADED\n");
}

/** 
 * Function gets invoked when device file is opened 
 */

static int myopen(struct inode *inode, struct file *file)
{
	file->private_data = &info;
	
	try_module_get(THIS_MODULE);

	pr_info("FILE OPENED\n");

	return 0;
}

/** 
 * This function is invoked when device file is closed 
 */

static int myclose(struct inode *inode, struct file *file)
{
	module_put(THIS_MODULE);
	pr_info("FILE CLOSED\n");
	
	return 0;
}

/** 
 * This function is invoked when device file is performing read operation 
 */

static ssize_t myread(struct file *file, char *buff, size_t len, loff_t *offset)
{
	pr_info("READ OPENED\n");

	return (ssize_t)len;
}

/** 
 * This function is invoked when device file is performing write operation 
 */

static ssize_t mywrite(struct file *file, const char *buff, size_t len, loff_t *offset)
{
	pr_info("WRITE OPENED\n");

	return (ssize_t)len;
}

/** 
 * IOCTL operations function 
 */

static long ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
		case WR_VALUE :
			if( copy_from_user(&val ,(int32_t*) arg, sizeof(val)) ) {
				pr_err("Data Write : Err!\n");
			}

			pr_info("Value = %d\n", val);

			break;

		case RD_VALUE :
				if( copy_to_user((int32_t*) arg, &val, sizeof(val)) )
                        	{	
                                	pr_err("Data Read : Err!\n");
                        	}
                        	
				break;

		 default:
				return -ENOTTY;
				
	}

	return 0;
}

/** 
 * MODULE INFO 
 */

MODULE_AUTHOR("Saketh");
MODULE_LICENSE("GPL");

/** 
 * TO DESCRIBE MODULE INIT AND EXIT FUNCTIONS 
 */

module_init(myinit);
module_exit(myexit);
