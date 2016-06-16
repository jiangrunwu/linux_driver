#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>








typedef struct eeprom{
    
    unsigned short address;
    char *buf;
    unsigned int len; 


}eeprom_t; 






int main(void){


    
    int fd;
    int ret;

    eeprom_t jan;
    char txbuf[20] = "hello world";
    char rxbuf[20];

    
    printf("-------------i2c_24lc256 driver test start-----------------\n");

    fd = open("/dev/i2c_24ls256", O_RDWR);

    if(fd < 0){
        
        printf("open error\n");
        return -1;
    
    }
    
    jan.address = 0x0;
    jan.buf = txbuf; 
    jan.len = strlen(txbuf) + 1; 

    ret = write(fd, &jan, sizeof(jan));
    if(ret < 0){
        printf("usr write err0\n");
        goto err0; 
    }

    jan.address = 0x0;
    jan.buf = rxbuf;
    jan.len = 20;
    ret = read(fd, &jan, sizeof(jan));
    if(ret < 0){
        printf("usr read err0\n");
        goto err0; 
    }

    printf("data from eeprom: %s \n", jan.buf);

    printf("-------------i2c_24lc256 driver test end-----------------\n");







err0:
    close(fd);


}
