#ifndef	__C_GPIO_H__
#define	__C_GPIO_H__

void myGpioInit(void);
void myGpioClose(void);
unsigned int myDigitalRead(unsigned int pin);
void myDigitalWrite(unsigned int pin, unsigned int val);
void myPinModeInput(unsigned int pin);
void myPinModeOutput(unsigned int pin);

#endif