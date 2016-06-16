#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>




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
    
    copy_to_user(buf, &decimal, 4); 

    return 0;

}

ssize_t pro_bin2dec_write(struct file *filp, const char __user *buf, size_t len, loff_t *offset){
     
   int ilen = strlen();
   int result = 0;
   int i;
   char *tmp = kmalloc(len);



   copy_from_user(tmp, buf, len);

   for (i = 0; i < ilen; i++){
    
       result += calc_to_d(tmp[ilen - i - 1], i); 

    }

    decimal = result;

    kfree(tmp);

    return 0;
   


}




static struct file_operations  proc_bi2dec_ops{
    
    .owner = THIS_MODULE;
    .read = proc_bin2dec_read;
    .write = proc_bin2dec_write;

}

static struct file_operations  proc_readonly_ops{
    .owner = THIS_MODULE;
    .read = proc_readonly_read;

}

int __init__ proc_demo_init(void){

    printk("------------module init--------------------\n");

    proc_entry_base = proc_mkdir(PROC_DIR_NAME, NULL);   
    proc_bin2dec_entry = create_proc_entry(PROC_BIN2DEC_NAME, proc_entry_base);
    proc_readonly_entry = create_proc_read_entry(PROC_READONLY_NAME, proc_entry_base);

    if(!(proc_entry_base || proc_bin2dec_entry || proc_entry_readonly)){
    
        return -1;
        

    } 

    proc_bin2dec_entry.proc_fops = proc_bin2dec_entry;
    proc_readonly_entry.proc_fops =  proc_readonly_entry;

    printk("------------module init finish--------------------\n");
    return 0;

}

void __exit__ proc_demo_exit(void){

    printk("------------module exit--------------------\n");

}


MODULE_LICENSE("GPL");
module_init(proc_demo_init);
module_exit(proc_demo_exit);




