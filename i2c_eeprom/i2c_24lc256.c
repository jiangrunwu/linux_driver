#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>



#include <linux/i2c.h>
#include <linux/types.h>
#include <asm/device.h>
#include <linux/device.h>
#include <linux/cdev.h>

#define DEVICE_NAME   "i2c_24ls256"

static struct cdev i2c_cdev;
static struct class *i2c_class;





























const struct file_operations i2c_24lc256_fop = {
    .owner = THIS_MODULE,
    .open  = led_open,
    .unlocked_ioctl = led_ioctl,
    .release = led_release,
};


static int __init i2c_24lc256_init(void){

    int ret;
    int tmp;
    dev_t dev;

    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret != 0){
    
        printk("major assign failed!\n");
        goto err0; 
    }
    cdev_init(&i2c_cdev, &i2c_24lc256_fop);
    ret = cdev_add(&i2c_cdev, dev, 1);
    if(ret != 0){
        printf("i2c cdev add failed!\n"); 
        goto err0;
    }

    i2c_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(i2c_class)){
    
        printk("ERR:cannot create a i2c_class\n"); 
        goto err0;
    }

    device_create(i2c_class, NULL, dev, DEVICE_NAME);


    /*register a i2c client */

module_i2c_driver
    i2c_master_send
i2c_transfer
i2c_attach_client
i2c_rdwr_ioctl_data


    return 0;

err0:
    unregister_chrdev_region(dev, 1);
    return -1;

}

static void __exit i2c_24lc256_exit(void){




}

module_init(i2c_24lc256_init);
module_exit(i2c_24lc256_exit);

MODULE_LICENSE("GPL");








