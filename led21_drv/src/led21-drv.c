#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <asm/io.h>



#define RALINK_SYSCTL_BASE         0xB0000000 
#define PIO_MODE_CTL               0xB0000060

#define RALINK_GPIO_BASE         0xB0000600

#define PIO_DSET0               0x30
#define PIO_DCLR0               0x40

#define GPIO_HIGH               1
#define GPIO_LOW                0

volatile unsigned int led21_mod_reg;
volatile unsigned int led21_base_reg;

unsigned int *vled21_mod_reg;
unsigned int *vled21_base_reg;

//define some command micro
#define RALINK_REG(offset)            (*(volatile unsigned int *)(offset)) 

#define LED_ON          _IOW('h', 0x01,unsigned long)  //len turn on
#define LED_OFF         _IOW('h', 0x02,unsigned long)  //len turn off


int open_state = 0; //if open_state = 1, indicate the led turn on, else turn off.


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

int led_open(struct inode * minode, struct file *filp){

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


int led_release(struct inode *minode, struct file *filp){


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


/*
 * *******************************************************************************
 * *
 * *   Function    :   led_ioctl 
 * *
 * *   Input       :    
 * *
 * *   Output      :    
 * *
 * *   Return      :    
 * *
 * *   Description :  in corresponding to ioctl function 
 * *
 * *   Author      :    2016-04-19 16:15   by   kyo <kyo2018@gmail.com>
 * *
 * *******************************************************************************
 * */

long led_ioctl(struct file *filp, unsigned int cmd, unsigned long arg){

    int tmp;

    printk("----------------------start-------------------------\n");
    printk("led21_mod_reg = %x\n", led21_mod_reg);
    printk("led21_base_reg = %x\n", led21_base_reg);

    printk("led21_mod_reg = %p\n", vled21_mod_reg);
    printk("led21_base_reg = %p\n", vled21_base_reg);




    led21_mod_reg =  0x10000060;
    led21_base_reg = 0x10000600;
   

    vled21_mod_reg = ioremap(led21_mod_reg,  4);
    vled21_base_reg= ioremap(led21_base_reg,  0x50);


    printk("led21_mod_reg = %x\n", led21_mod_reg);
    printk("led21_base_reg = %x\n", led21_base_reg);

    printk("led21_mod_reg = %p\n", vled21_mod_reg);
    printk("led21_base_reg = %p\n", vled21_base_reg);


    printk("----------------------end-------------------------\n");



    switch(cmd){

        case LED_OFF:
            printk("lend OFF!\n"); 

            tmp = readl(vled21_base_reg + 0x20);      
            printk("DATA = %x!\n", tmp); 

            tmp = 0; 
            tmp |= (1 << 16);
            writel(tmp, vled21_base_reg + PIO_DSET0);

            tmp = readl(vled21_base_reg + 0x20);      
            printk("DATA = %x!\n", tmp); 

            /*
               tmp = readl(vled21_base_reg + PIO_DSET0);      
               printk("DCLR0 f= %x!\n", tmp); 

               tmp |=(1 << 16);

               writel(tmp, vled21_base_reg + PIO_DSET0);

               printk("DATA = %x!\n", tmp); 

               tmp = readl(vled21_base_reg + 0x20);      
               printk("DCLR0 b= %x!\n", tmp); 
               */



            break;
        case LED_ON:
            printk("led ON!\n"); 


            tmp = readl(vled21_base_reg + 0x20);      
            printk("DATA = %x!\n", tmp); 

            tmp |=(1 << 16);
            writel(tmp, vled21_base_reg + 0x20);

            tmp = 0;
            tmp |=(1 << 16);
            writel(tmp, vled21_base_reg + PIO_DCLR0);

            tmp = readl(vled21_base_reg + 0x20);      
            printk("DATA = %x!\n", tmp); 


            break;

        default:
            printk("Error command!\n");
            break;


    }

    return 0;


}

const struct file_operations led_fop = {
    
    .owner = THIS_MODULE,
    .open  = led_open,
    .unlocked_ioctl = led_ioctl,
    .release = led_release,

}; 


struct miscdevice mt7688_led_misc = {

    .minor = 30,
    .fops = &led_fop, 
    .name = "mt7688_led_misc",

};



/*
 * *******************************************************************************
 * *
 * *   Function    :   gpio_init 
 * *
 * *   Input       :    
 * *
 * *   Output      :    
 * *
 * *   Return      :    
 * *
 * *   Description :   init function called in module init stage.
 * *
 * *   Author      :    2016-04-19 16:33   by   kyo <kyo2018@gmail.com>
 * *
 * *******************************************************************************
 * */
 
int helo;
static int __init gpio_init(void){

    int ret;
    int tmp;
    
    ret = misc_register(&mt7688_led_misc);
    if (ret < 0){
        printk("Register Error!\n"); 
        return ret;
    
    }

    led21_mod_reg =  0x10000060;
    led21_base_reg = 0x10000600;
   

    vled21_mod_reg = ioremap(led21_mod_reg,  4);
    vled21_base_reg= ioremap(led21_base_reg,  0x50);


    printk("led21_mod_reg = %x\n", led21_mod_reg);
    printk("led21_base_reg = %x\n", led21_base_reg);
    printk("helo = %p\n", &helo);

    printk("led21_mod_reg = %p\n", vled21_mod_reg);
    printk("led21_base_reg = %p\n", vled21_base_reg);

    //config the gpio18 into gpio mode
    tmp = readl(vled21_mod_reg);
    
    printk("mod_reg f = %x\n", tmp);

    tmp &=~(3 << 2); 
    tmp |=( 1<< 2); 
    writel(tmp, vled21_mod_reg);

    tmp = readl(vled21_mod_reg);
    printk("mod_reg b = %x\n", tmp);

    //config the gpio18 into output mode
 
    tmp = readl(vled21_base_reg);
    printk("mod_reg f= %x\n", tmp);

    tmp |=(1 << 16);
    printk("move = %x\n", tmp);
    writel(tmp, vled21_base_reg);

    tmp = readl(vled21_base_reg);
    printk("mod_reg b= %x\n", tmp);



    return 0;


}

static void __exit gpio_exit(void){

    misc_deregister(&mt7688_led_misc);
    open_state = 0;
    printk("GPIO test End!\n");

}

module_init(gpio_init);
module_exit(gpio_exit);

MODULE_LICENSE("GPL");



























