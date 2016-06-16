#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>
#include <asm/device.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/err.h>

#define DEVICE_NAME   "i2c_24ls256"



#define DEVIME_NAME     "24lc256_eeprom"
#define CHIP_ADDR       0x50


static struct cdev   i2c_24_cdev;
static struct class  *i2c_24_class;
static struct device *i2c_24_device;
static struct i2c_client *i2c_24_cli;
dev_t dev;



typedef struct eeprom{
    
    unsigned short address;
    char *buf;
    unsigned int len; 


}eeprom_t; 

int i2c_24_open(struct inode *inode, struct file *file){


    printk("open successfully\n");
    file->private_data = i2c_24_cli; 

    return 0;

}

int i2c_24_release(struct inode *inode, struct file *file){

    printk("close successfully");

    return 0;


}

long i2c_24_ioctl(struct file *filp, unsigned int cmd, unsigned long arg){
    
   // struct i2c_client *i2c_cli = (struct i2c_client *)filp->private_data; 
    ;

    return 0;

}

ssize_t i2c_24_write(struct file *filp, const char __user *buf, size_t count, loff_t *offset){

    char *tmp;
    int ret;
    eeprom_t _24lc_256;
    struct i2c_client *i2c_cli = (struct i2c_client *)filp->private_data; 
    struct i2c_msg msgs[1];

    
    if(copy_from_user(&_24lc_256, (eeprom_t *)buf, count)){
        return -1;
    }


    tmp =(char *)kmalloc(_24lc_256.len + sizeof(_24lc_256.address), GFP_KERNEL);
    if(!tmp){

        return -2; 
    }


	if (copy_from_user(tmp + sizeof(_24lc_256.address), _24lc_256.buf, _24lc_256.len)){
		kfree(tmp);
		return -1;
	}

    printk("cli->name = %s, addr = %d, buf = %s, len = %d\n", i2c_cli->name,  _24lc_256.address, _24lc_256.buf, _24lc_256.len);

    *(unsigned short *)tmp = _24lc_256.address;

    msgs[0].addr= CHIP_ADDR;
    msgs[0].flags = 0;
    msgs[0].len = _24lc_256.len + sizeof(_24lc_256.address);
    msgs[0].buf = tmp;

    ret = i2c_transfer(i2c_cli->adapter, msgs, 1);
    if(ret < 0){
        printk("writ i2c_transfer err\n"); 
        return -1; 
    }

    kfree(tmp);

    return 0;

}

ssize_t i2c_24_read(struct file *filp, char __user *buf, size_t count, loff_t *offset){

    char *tmp;
    int ret;
    eeprom_t _24lc_256;
    struct i2c_client *i2c_cli = (struct i2c_client *)filp->private_data; 
    struct i2c_msg msgs[2];
   

    if(copy_from_user(&_24lc_256, (eeprom_t *)buf, count)){
        return -1;
    }
    
    if(_24lc_256.address +  _24lc_256.len > 0x900){
        
        printk("address is %x, len = %d\n", _24lc_256.address, _24lc_256.len);
        return -2;
    }
    
    printk(" read cli->name = %s, addr = %d,  len = %d\n", i2c_cli->name,  _24lc_256.address, _24lc_256.len);
    tmp =(char *)kmalloc(_24lc_256.len, GFP_KERNEL);

    msgs[0].addr = CHIP_ADDR;
    msgs[0].flags = 0;
    msgs[0].len = 2;
    msgs[0].buf = (char *)&(_24lc_256.address);

    msgs[1].flags = 1;
    msgs[1].addr = CHIP_ADDR;
    msgs[1].len = _24lc_256.len;
    msgs[1].buf = tmp; 

    ret = i2c_transfer(i2c_cli->adapter, msgs, 2);
    if(ret < 0){
        printk("read i2c_transfer err\n"); 
        return -1; 
    }


    printk("kernel date form eeprom = %s\n", tmp);

    if(copy_to_user(_24lc_256.buf, tmp, _24lc_256.len)){

        return -1;
    
    }

    kfree(tmp);

    return 0;
    

}

const struct file_operations i2c_24_ops= {

    .owner = THIS_MODULE,
    .open = i2c_24_open,
    .read = i2c_24_read,
    .write = i2c_24_write,
    .release = i2c_24_release,
    .unlocked_ioctl = i2c_24_ioctl,

};









int i2c_24_probe(struct i2c_client * i2c_cli, const struct i2c_device_id *i2c_id){



    int ret;


    printk("i2c_24_probe start\n");

    if(i2c_cli->adapter != NULL && i2c_cli->adapter->name !=NULL) 
        printk("probe: i2c_cli.flag = %d, name = %s, irq = %d, addr = %x, i2c_adapter->name = %s, nr = %d\n", i2c_cli->flags,  i2c_cli->name, i2c_cli->irq, i2c_cli->addr, \
            i2c_cli->adapter->name, i2c_cli->adapter->nr);

    //get the i2c_client corresponding to i2c_master pointer; 

    i2c_24_cli = i2c_cli;

    //register char dev and dymatically alloc dev num
    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if(ret < 0){
    
         goto err0; 
    }

    cdev_init(&i2c_24_cdev, &i2c_24_ops);
    ret = cdev_add(&i2c_24_cdev, dev, 1);
    if(ret < 0){
        
        printk("i2c cdev add failed !\n");
        goto err1;
    
    }
    
    i2c_24_class = class_create(THIS_MODULE, DEVICE_NAME);
    if(IS_ERR(i2c_24_class)){
    
        printk("err: cannot create a class for i2c 24\n");
        goto err2;
    
    
    }
   // ? 
   i2c_24_device =  device_create(i2c_24_class, NULL, dev, NULL, DEVICE_NAME);
   if(IS_ERR(i2c_24_device)){
   
        goto err3; 
   }
   printk("prob complete\n");

    return 0;

err3:
    class_destroy(i2c_24_class);
err2:
    cdev_del(&i2c_24_cdev);
err1:
    unregister_chrdev_region(dev, 1);
err0:
    return -1;
}



int i2c_24_remove(struct i2c_client *i2c_cli){

    printk("remove: i2c_cli.flag = %d, name = %s, irq = %d, i2c_adapter->name = %s, nr = %d\n", i2c_cli->flags,  i2c_cli->name, i2c_cli->irq, \
            i2c_cli->adapter->name, i2c_cli->adapter->nr);

    device_destroy(i2c_24_class, dev);
    class_destroy(i2c_24_class);
    cdev_del(&i2c_24_cdev);
    unregister_chrdev_region(dev, 1);

    return 0;
}



static const struct i2c_device_id i2c_24_device_id[] = {
	{.name = "eeprom"},
	{ }
};

MODULE_DEVICE_TABLE(i2c, i2c_24_device_id);


static const struct of_device_id _24lc_256_dt[] ={

    {.compatible = "jan,eeprom"},

};


MODULE_DEVICE_TABLE(of, _24lc_256_dt);

struct i2c_driver i2c_24_drv = {
    .id_table = i2c_24_device_id, 
    
    .driver = {
        .owner = THIS_MODULE, 
        .name = "jansion i2c",
        .of_match_table = _24lc_256_dt,
    
    },
    .probe = i2c_24_probe, 
    .remove = i2c_24_remove, 


};

MODULE_LICENSE("GPL");
module_i2c_driver(i2c_24_drv);















