// headers
#include <linux/gpio.h> 
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

// definitions
#define coin 22
#define coin_MAJOR 64
#define coin_MINOR 0
#define coin_NBR_MINOR 1
#define CLASS_NAME "coin_class"
int err;
dev_t devno;

// license
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Heja&Tonny");
MODULE_DESCRIPTION("coin Driver");

// structs
static struct cdev my_cdev;
struct file_operations my_fops;

// init
static int __init coin_mygpio_init (void)
{
    // startup
    printk (KERN_INFO "Loading coin module ...\n");


    // gpio request
    err = gpio_request (coin, "coin");
    if (err)
    {
        return err;
    }


    // direction
    err = gpio_direction_input (coin);
    if (err)
    {
        gpio_free (coin);
        return err;
    }

    // Made a class. Needed for device_create
    struct class* someClass = class_create(THIS_MODULE, CLASS_NAME);
    
    // chr driver
    devno = MKDEV (coin_MAJOR, coin_MINOR);
    err = alloc_chrdev_region (&devno, 0, 255, "coin_driv");
    if (err)
    {
        unregister_chrdev_region (devno, coin_NBR_MINOR);
        return err;
    }


    // cdev check
    cdev_init (&my_cdev, &my_fops);
    err = cdev_add (&my_cdev, devno, coin_NBR_MINOR);
    if (err)
    {
        cdev_del (&my_cdev);
        return err;
    }

    struct device* dev;
    
    // added device_create to make device in /dev/coin_device
    dev = device_create(someClass, NULL, MKDEV(MAJOR(devno), 0), NULL, "coin_device");
    // end
    return 0;
}

// exit
static void __exit coin_mygpio_exit (void){
    printk (KERN_INFO "Unloading coin module ...\n");
    cdev_del (&my_cdev);

    unregister_chrdev_region (devno, coin_NBR_MINOR);
    gpio_free (coin);
}

// open
int coin_mygpio_open (struct inode *inode, struct file *filep) //Udleveret kode
 {
    int major;
    int minor;

    major = MAJOR (inode->i_rdev);
    minor = MINOR (inode->i_rdev);
    printk ("Opening coin Device [major], [minor]: %i, %i\n", major, minor);
    
    return 0;
 }

// release
 int coin_mygpio_release (struct inode *inode, struct file *filep) //Udleveret kode
 {
    int major;
    int minor;

    major = MAJOR (inode->i_rdev);
    minor = MINOR (inode->i_rdev);
    printk ("Closing/Releasing coin Device [major], [minor]: %i, %i\n", major, minor);

    return 0;
 }


// read
ssize_t coin_mygpio_read (struct file *filep, char __user *buf, size_t count, loff_t *f_pos)
{
    // get gpio
    int temp = gpio_get_value(coin);

    // make buffer
    char buffer[16];
    int lenTemp = sprintf(buffer,"%d", temp);
    lenTemp = lenTemp > count ? count : lenTemp;

    // Need to copy to userspace - M
    int copied = copy_to_user(buf, buffer, ++lenTemp);

    *f_pos += lenTemp;

    return lenTemp;    
}


// file operations
struct file_operations my_fops = 
{
    .owner = THIS_MODULE,
    .open = coin_mygpio_open,
    .read = coin_mygpio_read,
    .release = coin_mygpio_release,
};

// call
module_init(coin_mygpio_init);
module_exit(coin_mygpio_exit);