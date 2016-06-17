#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>


#include <linux/fs.h>
#include <asm/mach-ralink/pinmux.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/cdev.h>
#include <linux/device.h>





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

struct pinctrl_dev {
	struct list_head node;
	struct pinctrl_desc *desc;
	struct radix_tree_root pin_desc_tree;
	struct list_head gpio_ranges;
	struct device *dev;
	struct module *owner;
	void *driver_data;
	struct pinctrl *p;
	struct pinctrl_state *hog_default;
	struct pinctrl_state *hog_sleep;
	struct mutex mutex;
#ifdef CONFIG_DEBUG_FS
	struct dentry *device_root;
#endif
};


struct pinmux_config{
    char *gname; //group name  i.e pin share scheme in datasheet  
    char *fname; //mux function 

};

#define LED_ON          _IOW('h', 0x01,unsigned long)  //len turn on
#define LED_OFF         _IOW('h', 0x02,unsigned long)  //len turn off


int open_state = 0; //if open_state = 1, indicate the led turn on, else turn off.
struct cdev pinmux_dev;
struct class *pinmux_class;
struct device *pinmux_device;
struct pinctrl_dev *pdev;

/*
 * *******************************************************************************
 * *
 * *   Function    :    led_open
 * *
 * *   Input       :    
 * *
 * *   Output      :    
 * *
 * *   Return      :    
 * *
 * *   Description :  in corresponding to open function
 * *
 * *   Author      :  2016-04-19 16:07   by   kyo <kyo2018@gmail.com>
 * *
 * *******************************************************************************
 * */

int pinmux_open(struct inode * minode, struct file *filp){

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


/*
 * *******************************************************************************
 * *
 * *   Function    :    led_release
 * *
 * *   Input       :    
 * *
 * *   Output      :    
 * *
 * *   Return      :    
 * *
 * *   Description : in corresponding to close fuction
 * *
 * *   Author      :    2016-04-19 16:13   by   kyo <kyo2018@gmail.com>
 * *
 * *******************************************************************************
 * */


int pinmux_release(struct inode *minode, struct file *filp){


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
ssize_t pinmux_read(struct file *filp, char __user *buf, size_t len, loff_t *p){



}
ssize_t pinmux_write(struct file *filp, const char __user *buf, size_t len, loff_t *p){

    struct pinmux_config *pc = (struct pinmux_config *)buf;
    struct rt2880_priv *rt_p = (struct rt2880_priv *)(pdev->driver_data); 
    struct pinctrl_desc *pdesc= rt_p->desc;
    int ret, i, j;

    printk("rt_p->group_count = %d\n", rt_p->group_count);
    for(i = 0; i < rt_p->group_count; i++){

        if(!strcmp(pdesc->pctlops->get_group_name(pdev, i), pc->gname)){
            for (j = 0; j < rt_p->groups[i].func_count; j++){
                if(!strcmp(pdesc->pmxops->get_function_name(pdev, j), pc->fname)){
                    
                    ret = pdesc->pmxops->set_mux(pdev, j, i);
                    if(ret < 0){
                    
                        printk("pdesc->name = %s, set mux failed \n", pdesc->name); 
                    }
                }         

            }
        }

    }


}




struct file_operations pinmux_ops = {
        .open    = pinmux_open,
        .read    = pinmux_read,
        .write   = pinmux_write,
        .release = pinmux_release,

};



static int __init pinmux_init(void){

    int ret;
    int tmp;
    dev_t ndev;

   
    ret = alloc_chrdev_region(&ndev, 0, 1, "pinmux");

    if(ret < 0){
        printk("mdev number failed\n"); 
        goto err0;
    
    }
    cdev_init(&pinmux_dev, &pinmux_ops);   
    ret = cdev_add(&pinmux_dev, ndev, 1);
    if(ret < 0){
        printk("cdev_add failed\n");
        goto err1; 
    }
    
    pinmux_class = class_create(THIS_MODULE, "class_pinmux_cdev");
    if(IS_ERR(pinmux_class))
    {
        printk("class_create error\n");
        goto err2;
    }

    pinmux_device = device_create(pinmux_class,
            NULL,
            ndev,
            NULL,
            "pinmux_dev");
    if(IS_ERR(pinmux_device))
    {
        printk("device_create error\n");
        goto err3;
    }


    /************get the pinctrl dev******************/
    

    pdev = get_pinctrl_dev_from_devname("pinctrl");
    if(!pdev){
        
        printk("pinctrl dev get faild!\n");
        goto err4:
    }


    return 0;
err4:
    device_destroy(pinmux_device);
err3:
    class_destroy(pinmux_class);
err2:
    cdev_del(&pinmux_dev);
err1:
    unregister_chrdev_region(ndev, 1);
err0:
    return -1;

}

static void __exit pinmux_exit(void){

    open_state = 0;
    printk("pinmux test End!\n");

}

module_init(pinmux_init);
module_exit(pinmux_exit);

MODULE_LICENSE("GPL");



