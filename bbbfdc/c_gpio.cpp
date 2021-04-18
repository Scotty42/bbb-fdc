#include <python3.7/Python.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <iobb.h>

#include "c_gpio.h"

//#define GPIO_IN(g)    *(gpio+((g)/10))   &= ~(7<<(((g)%10)*3))
//#define GPIO_OUT(g)   *(gpio+((g)/10))   |=  (1<<(((g)%10)*3))
//#define GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

//#define GPIO_SET(g)   *(gpio+7)  = 1<<(g)  /* sets   bit which are 1, ignores bit which are 0 */
//#define GPIO_CLR(g)   *(gpio+10) = 1<<(g)  /* clears bit which are 1, ignores bit which are 0 */
//#define GPIO_LEV(g)  (*(gpio+13) >> (g)) & 0x00000001

//#define BCM2708_PERI_BASE  0xFE000000  // pi4
//                         //0x20000000 some other pi
//#define GPIO_BASE          (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define BLOCK_SIZE         (256)

//int                mem_fd;
//void              *gpio_map;
//volatile uint32_t *gpio;

void check_root_user(void)
{
    if(geteuid()!=0)
        fprintf(stderr, "Run as root user! (or use sudo)\n");
}

void myGpioInit(void)
{
  check_root_user();
  iolib_init();
  fprintf(stdout, "GPIO Init\n");
}

void myGpioClose(void)
{
  iolib_free();
  fprintf(stdout, "GPIO DeInit\n");
}

unsigned int myDigitalRead(unsigned int pin)
{
  /*return GPIO_LEV(pin);*/
}

void myDigitalWrite(unsigned int pin, unsigned int val)
{
  /*if (val) {
    fprintf(stdout, "set %d\n", pin);
    GPIO_SET(pin);
  } else {
    fprintf(stdout, "clr %d\n", pin);
    GPIO_CLR(pin);
  }*/
}

void myPinModeInput(unsigned int pin)
{
    /*fprintf(stdout, "input %d\n", pin);
    GPIO_IN(pin);*/
}

void myPinModeOutput(unsigned int pin)
{
    /*fprintf(stdout, "output %d\n", pin);
    GPIO_IN(pin);
    GPIO_OUT(pin);*/
}
