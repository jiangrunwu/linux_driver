#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/gpio_keys.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/spinlock.h>

#include <linux/of.h>



typedef struct  jansion_key{

    int gpio;
    int active_low;
    const char* desc;
    int code;
    int value;


}jansion_key_t;


struct gpio_key_data {
    jansion_key_t *jkey;
    struct input_dev *input;
    struct timer_list timer;
    int irq;
    unsigned int timer_debounce;	/* in msecs */
    struct work_struct work;
    spinlock_t lock;
    bool disabled;
    bool key_pressed;

};



static irqreturn_t gpio_keys_irq(int irq, void *dev_id){

    struct gpio_key_data *key_data = (struct gpio_key_data *)dev_id;
        
    schedule_work(&key_data->work);

    return IRQ_HANDLED;
    

}



static void gpio_keys_work_queue_func(struct work_struct *work){

    struct gpio_key_data *key_data = container_of(work, struct gpio_key_data, work);
    int state = gpio_get_value_cansleep(key_data->jkey->gpio) ^ key_data->jkey->active_low;


    input_event(key_data->input, EV_KEY, key_data->jkey->code, !!state);

    input_sync(key_data->input);




}


static jansion_key_t *gpio_keys_get_devtree_pdata(struct device *dev){

    struct device_node *node, *pp; 
    jansion_key_t *jkey;
    int count;
    int gpio;
    enum of_gpio_flags flag;



    node = dev->of_node;

    if(!node){
        return ERR_PTR(-ENODEV);
    }

    count = of_get_child_count(node);
    if(count == 0){

        return ERR_PTR(-ENODEV);

    }


    pp = of_get_next_child(node, NULL);
    if(!of_get_property(pp, "gpios", NULL)){

        printk("find button without gpios\n");
        return ERR_PTR(-ENODEV);
    }

    gpio = of_get_gpio_flags(pp, 0, &flag);

    printk("gpio num = %d, flag = %d\n", gpio, flag);
    if(gpio < 0){

        return ERR_PTR(-gpio);
    }
    jkey = devm_kzalloc(dev, sizeof(jansion_key_t), GFP_KERNEL); 
    if(!jkey){

        return ERR_PTR(-ENOMEM);

    }
    jkey->gpio = gpio;
    jkey->active_low = flag & OF_GPIO_ACTIVE_LOW;

    if(of_property_read_u32(pp, "linux,code", &jkey->code)){

        return ERR_PTR(-EINVAL);
    }

    jkey->desc = of_get_property(pp, "label", NULL);

    return jkey;


}



int key_16_probe(struct platform_device *key_device){
    struct device *dev = &key_device->dev;

    struct gpio_key_data *key_data; 
    struct jansion_key *jkey = NULL;
    struct input_dev *input;
    int ret;
    unsigned long irqflags;
    irq_handler_t isr;



    printk("---------------------init start--------------------------\n");

    jkey = gpio_keys_get_devtree_pdata(dev); 
    if(IS_ERR(jkey)){

        return (-ENOMEM);  

    }

    key_data = devm_kzalloc(dev, sizeof(struct gpio_key_data), GFP_KERNEL);
    if(IS_ERR(key_data)){

        return (-ENOMEM);  
    }

    printk("---------------------stage   1--------------------------\n");
    input = devm_input_allocate_device(dev);
    if(IS_ERR(input)){

        return (-ENOMEM);  
    }
    

    key_data->jkey = jkey;
    key_data->input = input;
    

	platform_set_drvdata(key_device, key_data);
	input_set_drvdata(input, key_data);

    input->dev.parent = dev;
    input->name = key_device->name; 
    input->id.vendor = 0x0001;
    input->id.product = 0x0001;
    input->id.version = 0x0100;

	input_set_capability(key_data->input, EV_KEY, jkey->code);

    printk("---------------------stage   2--------------------------\n");
    if(!gpio_is_valid(jkey->gpio)){

        return -1; 
    }

    printk("---------------------stage   3--------------------------\n");
    ret = devm_gpio_request_one(dev, jkey->gpio, GPIOF_IN, jkey->desc);
    if(ret < 0){
        
        return -ret;
    
    }
    //config the gpio into input mode
    key_data->irq = gpio_to_irq(jkey->gpio);
    

    INIT_WORK(&key_data->work, gpio_keys_work_queue_func);
    //setup_timer(key_data->tierm, gpio_keys_irq_timer, (unsigned long)&key_data);


    isr = gpio_keys_irq; 
	irqflags = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING;
	ret = devm_request_any_context_irq(dev, key_data->irq, \
					     isr, irqflags, key_data->jkey->desc, key_data);


    printk("---------------------stage   4--------------------------\n");
    ret = input_register_device(key_data->input);
    if(ret < 0){
    
        return -ret;
    }
    printk("---------------------init finished--------------------------\n");
    
    return 0;

}



int key_16_remove(struct platform_device *key_device){


    return 0;

}

static const struct of_device_id key_16_dt[] = {

    { .compatible = "gpio-keys",   },
    {},

}; 

MODULE_DEVICE_TABLE(of, key_16_dt);


static struct platform_driver  key_16_driver = {

    .probe = key_16_probe,
    .remove = key_16_remove,

    .driver = {

        .owner =  THIS_MODULE,
        .name = "gpio-keysi",
        .of_match_table =  key_16_dt,

    },


};



static int __init key_16_init(void){


    printk("---------------------init f--------------------------\n");
    
    platform_driver_register(&key_16_driver); 

    printk("---------------------init f--------------------------\n");

    return 0;
}

static void __exit key_16_exit(void){


    platform_driver_unregister(&key_16_driver); 

    printk("---------------------exit func--------------------------\n");
}




MODULE_LICENSE("GPL");
late_initcall(key_16_init);
module_exit(key_16_exit);






