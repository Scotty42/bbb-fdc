#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <iobb.h>
#include "../bbbfdc/wiringPi.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 


int not_finished = 1;


void wd_config_input(void);
void wd_config_output(void);
void wd_set_data(unsigned int data);
unsigned int wd_get_data(void);

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

    fprintf(stdout, "GPIO Init\n");
    iolib_init();

    // data port pins
    // default OCP pinmux behavior in mode 7 := pulldown
    BBBIO_sys_Enable_GPIO(BBBIO_GPIO1);
    BBBIO_GPIO_set_dir(BBBIO_GPIO1, 
        BBBIO_GPIO_PIN_0, 
        BBBIO_GPIO_PIN_12 | BBBIO_GPIO_PIN_13 | BBBIO_GPIO_PIN_14 | BBBIO_GPIO_PIN_15 | BBBIO_GPIO_PIN_16 | BBBIO_GPIO_PIN_17 | BBBIO_GPIO_PIN_18 | BBBIO_GPIO_PIN_19);

    BBBIO_sys_Enable_GPIO(BBBIO_GPIO2);
    BBBIO_GPIO_set_dir(BBBIO_GPIO2, 
        BBBIO_GPIO_PIN_0,     // input
        BBBIO_GPIO_PIN_1 );   // output, default pulldown/pulldown in OCP mode 7

    // reset WDC
    fprintf(stdout, "WD reset\n");
    delayMicroseconds(10);
    BBBIO_GPIO_high(BBBIO_GPIO2, BBBIO_GPIO_PIN_1);  // assert reset
    delayMicroseconds(100);
    BBBIO_GPIO_low(BBBIO_GPIO2, BBBIO_GPIO_PIN_1);   // deassert reset
    delayMicroseconds(1000);

    fprintf(stdout, "GPIO Init done\n");

    printf("\nTest\n");
    srand (time(NULL));

    wd_config_input();
    //wd_config_output();

    BBBIO_sys_Enable_GPIO(BBBIO_GPIO0);
    BBBIO_GPIO_set_dir(BBBIO_GPIO0, 
      BBBIO_GPIO_PIN_0 | BBBIO_GPIO_PIN_7,     // input
      BBBIO_GPIO_PIN_22 | BBBIO_GPIO_PIN_30 | BBBIO_GPIO_PIN_31 | BBBIO_GPIO_PIN_2 | BBBIO_GPIO_PIN_3 | BBBIO_GPIO_PIN_4 | BBBIO_GPIO_PIN_5 | BBBIO_GPIO_PIN_23 );   // output, default pulldown/pulldown in OCP mode 7

    // MSR config
    BBBIO_GPIO_low(BBBIO_GPIO0, BBBIO_GPIO_PIN_22);
    BBBIO_GPIO_low(BBBIO_GPIO0, BBBIO_GPIO_PIN_30);
    BBBIO_GPIO_high(BBBIO_GPIO0, BBBIO_GPIO_PIN_31);


    unsigned int r = 0;
    while(not_finished)
    {
        //r = rand() % 254 + 1;
        //wd_set_data(r);
	//fprintf(stdout, "%02x\n", r);
        
        r = wd_get_data();
	int m = r;
	printf(" " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(m));
        fprintf(stdout, " %02x", r);
	printf(" => %02x\n", (m & 0xF0));

        delayMicroseconds(2000*1000);
    }
    printf("\nEnd\n");
    
    iolib_free();

    return(0);
} 

void wd_config_input(void)
{
  //BBBIO_sys_Disable_GPIO(BBBIO_GPIO1);

  BBBIO_GPIO_set_dir(BBBIO_GPIO1, 
    BBBIO_GPIO_PIN_12 | BBBIO_GPIO_PIN_13 | BBBIO_GPIO_PIN_14 | BBBIO_GPIO_PIN_15 | BBBIO_GPIO_PIN_16 | BBBIO_GPIO_PIN_17 | BBBIO_GPIO_PIN_18 | BBBIO_GPIO_PIN_19,
    BBBIO_GPIO_PIN_0);
  //fprintf(stdout, "wd_config_input\n");
}

void wd_config_output(void)
{
  //BBBIO_sys_Disable_GPIO(BBBIO_GPIO1);

  BBBIO_GPIO_set_dir(BBBIO_GPIO1, 
    BBBIO_GPIO_PIN_0, 
    BBBIO_GPIO_PIN_12 | BBBIO_GPIO_PIN_13 | BBBIO_GPIO_PIN_14 | BBBIO_GPIO_PIN_15 | BBBIO_GPIO_PIN_16 | BBBIO_GPIO_PIN_17 | BBBIO_GPIO_PIN_18 | BBBIO_GPIO_PIN_19);
  //fprintf(stdout, "wd_config_output\n");
}


const int WD_DATAPINS[] = {BBBIO_GPIO_PIN_12 , BBBIO_GPIO_PIN_13 , BBBIO_GPIO_PIN_14 , BBBIO_GPIO_PIN_15 , BBBIO_GPIO_PIN_16 , BBBIO_GPIO_PIN_17 , BBBIO_GPIO_PIN_18 , BBBIO_GPIO_PIN_19};

void wd_set_data(unsigned int data)
{
  for (int n=0; n<8; n++) {
    (data & 0x01) == 1 ? BBBIO_GPIO_high(BBBIO_GPIO1, WD_DATAPINS[n]) : BBBIO_GPIO_low(BBBIO_GPIO1, WD_DATAPINS[n]);
    data = data >> 1;
  }
}

unsigned int wd_get_data(void)
{
  unsigned int data = 0;
  for (int n = 7; n >= 0; n--) {
    int bit = BBBIO_GPIO_get(BBBIO_GPIO1, WD_DATAPINS[n]) ? 1 : 0;
    data = (data << 1) + bit;
  }

  return data;
}
