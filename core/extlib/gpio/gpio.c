#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>
#include "gpio.h"
#include "log.h"

//可能后面会在上层抽象一套读写数字pin 读写模拟pin之类的东西出来,通过指针注册和底层实现关联

int gpio_init(void)
{
	return wiringPiSetup();
}

int gpio_read(int pin)
{
	return digitalRead(pin);
}

int gpio_set(int pin, GPIO_MODE mode)
{
	switch(mode)
	{
		case DINPUT:
			pinMode(pin, INPUT);
			break;
		case DOUTPUT:
			pinMode(pin, OUTPUT);
			break;
		default:
			LOG_INFO("not supported gpio mode %d\n", mode);
	}
	return 0;
}

int key_read(int pin, GPIO_MODE mode)
{
	switch(mode)
	{
		case DINPUT:
			return digitalRead(pin);
			break;
		default:
			LOG_INFO("key_read not supported mode %d\n", mode);
	}
	return 0;
}

