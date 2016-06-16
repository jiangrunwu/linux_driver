#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/types.h>
#include <asm/device.h>
#include <linux/device.h>
#include <linux/cdev.h>


#include <pinctrl.h>




#define DEVICE_NAME   "gpios_cdrv"
#define GPIO_OUT_16        16
#define GPIO_IN_40         40 

#define GPIO_ON          _IOW('g', 0x03, unsigned long)  //len turn on
#define GPIO_OFF         _IOW('g', 0x04, unsigned long)  //len turn off


struct rt2880_priv {
    struct device *dev;

    struct pinctrl_pin_desc *pads;
    struct pinctrl_desc *desc;

    struct rt2880_pmx_func **func;
    int func_count;

    struct rt2880_pmx_group *groups;
    const char **group_names;
    int group_count;

    uint8_t *gpio;
    int max_pins;
};


static struct cdev gpios_cdev;
static struct class *gpios_class;


extern int gpio_request(unsigned gpio, const char *label);

static int open_state = 0; //if open_state = 1, indicate the led turn on, else turn off.






int df_pinmux_open(struct inode * minode, struct file *filp){


    
    if(open_state == 0){
        
        open_state = 1;
        printk("Open file successful!\n");
        
        return 0;
    
    }
    else{
    
        printk("The file has opened!\n");  
        return -1;
    
    }



}


int df_pinmux_release(struct inode *minode, struct file *filp){


    if(open_state == 1){
        
        open_state = 0;
        printk("clsoe file successful!\n");
        
        return 0;
    
    }
    else{
    
        printk("The file has closed!\n");  
        return -1;
    
    }


}


ssize_t df_pinmux_read(struct file *f, char __user *buf, size_t, loff_t *blen){




}
ssize_t df_pinmux_write(struct file *f, const char __user *buf, size_t, loff_t *blen){





}




const struct file_operations gpios_fop= {
    .owner = THIS_MODULE,
    .open  = df_pinmux_open,
    .read = df_pinmux_read,
    .write = df_pinmux_write,
    .release = df_pinmux_release,
};


static int __init df_pinmux_init(void){

    int ret;
    dev_t dev;


    struct pinctrl_dev *mt7688_dev = NULL;
    struct rt2880_priv *p = NULL;
    
    mt7688_dev = get_pinctrl_dev_from_devname("pinctrl");
    
    p = pinctrl_dev_get_drvdata(pctldev);


     = mt7688_dev->driver_data;


    if (ret == 0){
    
        printk("gpio num is novalid!\n");
        goto err; 
    }
 
    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret != 0){
    
        printk("major assign failed!\n");
        goto err0; 
    }
    
    cdev_init(&gpios_cdev, &gpios_fop);
    ret = cdev_add(&gpios_cdev, dev, 1);
    if(ret != 0){
    
        printk("i2c cdev add failed ret = %d!\n", ret); 
        goto err0;
    
    }

    gpios_class = class_create(THIS_MODULE, DEVICE_NAME);
    if(IS_ERR(gpios_class)){
        printk("err: cannot create a gpios_class\n");
        goto err0;
    
    }
    device_create(gpios_class, NULL, dev, NULL, DEVICE_NAME);


    /*init gpios dev*/
    ret = gpio_request(GPIO_OUT_16, "df_gpios16"); 
  
    if(ret != 0){
    
        printk("gpio request gpio id%d failed!\n", GPIO_OUT_16); 
        goto err1;
    }

    ret = gpio_request(GPIO_IN_40, "df_gpios42"); 
    if(ret != 0){
    
        printk("gpio request gpio id%d failed!\n", GPIO_IN_40); 
        goto err1;
    }
    ret = gpio_direction_output(GPIO_OUT_16, 1); 
    if(ret != 0){

        printk("gpio request gpio id failed!\n"); 
        goto err2;
    }
    ret = gpio_direction_input(GPIO_IN_40); 
    if(ret != 0){

        printk("gpio request gpio id failed!\n"); 
        goto err2;
    }


    printk("gpio-cdev init success!\n");    
    return 0;


err2:
    
    pinctrl_free_gpio(GPIO_OUT_16);

err1:
    class_destroy(gpios_class);

err0:
    unregister_chrdev_region(dev, 1);
err:
    return -1;

}

static void __exit df_pinmux_exit(void){

    printk("gpio-cdev eixt success!\n");    
    gpio_free(GPIO_OUT_16);
    gpio_free(GPIO_IN_40);
    class_destroy(gpios_class);
}




module_init(df_pinmux_init);
module_exit(df_pinmux_exit);

MODULE_LICENSE("GPL");





