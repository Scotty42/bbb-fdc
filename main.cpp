#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <iobb.h>

#include "../bbbfdc/wiringPi.h"

int not_finished = 1;

void check_root_user(void)
{
    if(geteuid()!=0)
        printf("Run as root user! (or use sudo)\n");
}

void ctrl_c_handler(int)
{
    not_finished = 0;
}

int main(void)
{
    signal (SIGINT, ctrl_c_handler);

    check_root_user();

    iolib_init();
    //iolib_setdir(8, 12, DigitalOut);
    //iolib_setdir(8, 13, DigitalIn); 
    //if (is_low(8, 13))  
    BBBIO_sys_Enable_GPIO(BBBIO_GPIO0);
    BBBIO_GPIO_set_dir(BBBIO_GPIO0, 
        BBBIO_GPIO_PIN_31, 
        BBBIO_GPIO_PIN_22);
    
    printf("\nToggling\n");
    while(not_finished)
    {
        //pin_high(8, 12);
        //iolib_delay_ms(500);
        BBBIO_GPIO_high(BBBIO_GPIO0, BBBIO_GPIO_PIN_22);
        delayMicroseconds(500*1000);

        //pin_low(8, 12);
        //iolib_delay_ms(500);
        BBBIO_GPIO_low(BBBIO_GPIO0, BBBIO_GPIO_PIN_22);
        delayMicroseconds(500*1000);
    }
    printf("\nDone Toggling\n");
    
    iolib_free();

    return(0);
} 