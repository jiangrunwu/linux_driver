#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>


#include <linux/slab.h>


#define PROC_DIR_NAME           "jproc_demo"
#define PROC_BIN2DEC_NAME       "bin2dec"
#define PROC_READONLY_NAME      "read_only"


static struct proc_dir_entry *proc_entry_base = NULL;
static struct proc_dir_entry *proc_bin2dec_entry = NULL;
static struct proc_dir_entry *proc_readonly_entry = NULL;


static int decimal;

int calc_to_d(int value, int index){
    int result = 0;
    int i;

    result = value;
    for (i = 0; i < index; i++){
    
        result += result * 2; 
    
    }

    return result;


}


ssize_t proc_bin2dec_read(struct file *filp, char __user *buf, size_t len, loff_t *ofset){
    

    if(*ofset > 0){
        *ofset = 0; 
        return -1;
    
    }

    *ofset = 0;
    len = sprintf(buf, "%d\n", decimal);

    return len;

}

ssize_t proc_bin2dec_write(struct file *filp, const char __user *buf, size_t len, loff_t *offset){
     
   int ret = 0;

   char *tmp = (char *)kmalloc(len, GFP_KERNEL);


   if(copy_from_user(tmp, buf, len)){
   
        ret = -1;  
   }else{

        decimal = simple_strtoul(tmp, NULL, 2);
        return len;
   }
       
   kfree(tmp);

   return  ret;
   


}

ssize_t proc_readonly_read(struct file *filp, char __user *buf, size_t len, loff_t *ofset){

    if(*ofset > 0){
        *ofset = 0; 
        return -1; 
    }

    len = sprintf(buf, "debug massege infor\n");
    return len;
    
}
    



static struct file_operations  proc_bin2dec_ops = {
    
    .owner = THIS_MODULE,
    .read = proc_bin2dec_read,
    .write = proc_bin2dec_write,

};

static struct file_operations  proc_readonly_ops = {
    .owner = THIS_MODULE,
    .read = proc_readonly_read,

};

int __init proc_demo_init(void){

    printk("------------module init--------------------\n");

    proc_entry_base = proc_mkdir(PROC_DIR_NAME, NULL);   
    proc_bin2dec_entry = proc_create(PROC_BIN2DEC_NAME, 0666, proc_entry_base, &proc_bin2dec_ops);
    proc_readonly_entry = proc_create(PROC_READONLY_NAME, 0444, proc_entry_base, &proc_readonly_ops);

    if(!(proc_entry_base || proc_bin2dec_entry || proc_readonly_entry)){
    
        return -1;
        

    } 


    printk("------------module init finish--------------------\n");
    return 0;

}

void __exit proc_demo_exit(void){

    printk("------------module exit--------------------\n");

}


MODULE_LICENSE("GPL");
module_init(proc_demo_init);
module_exit(proc_demo_exit);




