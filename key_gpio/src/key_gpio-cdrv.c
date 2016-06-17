
#include <linux/gpiolib.h>




static struct input_dev *key_16_dev;



static void key_interrupt(int irq, void *dummy, struct pt_regs *fp){



    input_report_key();


}




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


static void gpio_keys_irq_timer(unsigned long arg){




}


static void gpio_keys_work_queue_func(struct work_struct *work){

    struct gpio_key_data *key_data = container_of(work, struct gpio_key_data, work);
    int state = gpio_get_value_cansleep(key_data->jkey->gpio) ^ key_data->jkey->active_low;


    input_event(key_data->input, type, gpio_key_data->jkey->code, key_data->jkey->value);

    input_sync(key_data->input);




}




static struct jansion_key_t *gpio_keys_get_devtree_pdata(struct device *dev){

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
        return -1;
    }

    gpio = of_get_gpio_flags(pp, 0, &flag);

    printk("gpio num = %d, flag = %d\n", gpio, flag);
    if(gpio < 0){

        return ERR_PTR(-gpio);
    }
    jkey = devm_kzalloc(dev, sizeof(struct jansion_key_t), GFP_KERNEL); 
    if(!jkey){

        return ERR_PTR(-ENOMEM);

    }
    jkey->gpio = gpio;
    jkey->active_low = flag & OF_GPIO_ACTIVE_LOW;

    if(of_property_read_u32(pp, "linux,code", jkey->code)){

        return ERR_PTR(-EINVAL);
    }

    jkey->desc = of_get_property(pp, "label", NULL);

    return jkey;


}


irqreturn_t jkey_irq_handler_t(int, void *);

int key_16_probe(struct platform_device *key_device){
    struct device *dev = &key_device->dev;

    struct gpio_key_data *key_data; 
    struct jansion_key jkey = NULL;
    struct input_dev *input;
    int jkey_irq;
    int ret;


    jkey = gpio_keys_get_devtree_pdata(dev); 
    if(IS_ERR(jkey)){

        return PTR_ERR(-ENOMEM);  

    }

    key_data = devm_kzalloc(dev, sizeof(struct gpio_key_data), GFP_KERNEL);
    if(IS_ERR(key_data)){

        return PTR_ERR(-ENOMEM);  
    }
    input = devm_input_allocate_device(dev);
    if(IS_ERR(input)){

        return PTR_ERR(-ENOMEM);  
    }

    input->dev.parent = dev;
    input->name = key_device->name; 
    input->id.vendor = 0x0001;
    input->id.product = 0x0001;
    input->id.version = 0x0100;


    key_data->jkey = jkey;
    key_data->input = input;

    if(!gpio_is_valid(jkey->gpio)){

        return -1; 
    }

    error = devm_gpio_request_one(dev, jkey->gpio, CONFIG_IN, jkey->desc);
    //config the gpio into input mode
    key_data->irq = gpio_to_irq(jkey->gpio);
    if(jkey_irq < 0){

        return ERR_PTR(-jkey_irq);  
    }

    INIT_WORK(key_data->work, );

    setup_timer(key_data->tiemr, gpio_keys_irq_timer, (unsigned long)&key_data);



    request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,







            }
            }

            int key_16_remove(struct platform_device *key_device){




            }

            static const struct of_match_table = key_16_dt{

            { .compatible = "gpio-keys"   },

            }; 

MODULE_DEVICE_TABLE(of, key_16_dt);


static struct platform_driver  key_16_driver = {

    .probe = key_16_probe,
    .remove = key_16_remove,

    .driver = {

        .owner =  THIS_MODULE,
        .name = "jansion_key",
        .of_match_table =  of_match_ptr(key_16_dt),

    },


};



static int __init key_16_init(void){


    platform_driver_register(&key_16_driver); 


    return 0;
}

static void __exit key_16_exit(){


    platform_driver_unregister(&key_16_driver); 

}




late_initcall();
module_init();






