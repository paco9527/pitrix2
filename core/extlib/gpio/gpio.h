#ifndef _MY_GPIO
#define _MY_GPIO

typedef enum
{
    DINPUT = 0,
    DOUTPUT,
    AINPUT,
    AOUTPUT
}GPIO_MODE;

int gpio_init(void);
int gpio_set(int pin, GPIO_MODE mode);
int gpio_read(int pin);

#endif