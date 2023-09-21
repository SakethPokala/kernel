/**
 * This code functionality is to turn on and turn off the user led of beaglebone black.
 *
 * In this code my choice is userled-1 of beaglebone black
 */


#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/err.h>
#include<linux/fs.h>
#include<linux/gpio.h>
#include<linux/init.h>
#include<linux/interrupt.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/uaccess.h>

#define GPIO_PIN 54

dev_t devno;
int val;

static struct class *cls;

static int my_open(struct inode *inode, struct file *file);
static int my_close(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *file, char *buff, size_t len, loff_t *offset);
static ssize_t my_write(struct file *file, const char *buff, size_t len, loff_t *offset);

static struct file_operations my_routins = {
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.read = my_read,
	.write = my_write,
};

struct cdev c_dev;

static int my_open(struct inode *inode, struct file *file)
{
	pr_info("File opened\n");

	return 0;
}

static int my_close(struct inode *inode, struct file *file)
{
	pr_info("File closed\n");

	return 0;
}

static ssize_t my_read(struct file *file, char *buff, size_t len, loff_t *offset)
{
	pr_info("File opened for reading\n");

	val = gpio_get_value(GPIO_PIN);

	pr_info("GPIO VAL %d\n", val);

	if (copy_to_user(buff, &val, sizeof(val) > 0))
		pr_err("Not all bytes have been copied\n");

	return (ssize_t)sizeof(val);

}

static ssize_t my_write(struct file *file, const char *buff, size_t len, loff_t *offset)
{
	pr_info("File opened for write\n");

	if (copy_from_user(&val, (int *)buff, len))
		pr_err("Not all bytes written\n");

	if (val == 0)
		gpio_set_value(GPIO_PIN, 0);
	else if (val == 1)
		gpio_set_value(GPIO_PIN, 1);
	else
		pr_err("Unknown\n");

	return (ssize_t)len;
}

static int my_init(void)
{
	if((alloc_chrdev_region(&devno, 0, 1, "gpio_own")) <0) {
		pr_err("Cannot allocate major number\n");
		goto r_unreg;
	}

	/*Creating cdev structure*/
	cdev_init(&c_dev, &my_routins);
	/*Adding character device to the system*/
	if((cdev_add(&c_dev, devno, 1)) < 0){
		pr_err("Cannot add the device to the system\n");
		goto r_del;
	}
	/*Creating struct class*/
	if(IS_ERR(cls = class_create(THIS_MODULE, "gpioown_class"))){
		pr_err("Cannot create the struct class\n");
		goto r_class;
	}
	/*Creating device*/
	if(IS_ERR(device_create(cls, NULL, devno, NULL, "mygpiodev"))){
		pr_err( "Cannot create the Device \n");
		goto r_device;
	}

	//Checking the GPIO is valid or not
	if(gpio_is_valid(GPIO_PIN) == false){
		pr_err("GPIO %d is not valid\n", GPIO_PIN);
		goto r_device;
	}

	//Requesting the GPIO
	if(gpio_request(GPIO_PIN,"GPIO_PIN") < 0){
		pr_err("ERROR: GPIO %d request\n", GPIO_PIN);
		goto r_gpio_out;
	}

	//configure the GPIO as output
	gpio_direction_output(GPIO_PIN, 0);

	pr_info("Device Driver Insert...Done!!!\n");
	return 0;

r_gpio_out:
	gpio_free(GPIO_PIN);
r_device:
	device_destroy(cls, devno);
r_class:
	class_destroy(cls);
r_del:
	cdev_del(&c_dev);
r_unreg:
	unregister_chrdev_region(devno, 1);

	return -1;
}

static void my_exit(void)
{
	gpio_free(GPIO_PIN);
	device_destroy(cls, devno);
	class_destroy(cls);
	cdev_del(&c_dev);
	unregister_chrdev_region(devno, 1);
	pr_info("Driver removed\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
