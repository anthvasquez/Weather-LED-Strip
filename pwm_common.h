#ifndef __PWM_COMMON_H
#define __PWM_COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pigpiod_if2.h>

const int GPIO_R = 24;
const int GPIO_G = 23;
const int GPIO_B = 25;

/*
 * Returns the device descriptor for the pi
 */
int pwm_init()
{
		int pi = pigpio_start(NULL, NULL);
		if(pi < 0)
		{
				fprintf(stderr, "Unable to start pigpio.\n");
				return -1;
		}

		int RFlag = set_mode(pi, GPIO_R, PI_OUTPUT);
		int GFlag = set_mode(pi, GPIO_G, PI_OUTPUT);
		int BFlag = set_mode(pi, GPIO_B, PI_OUTPUT);
		if(RFlag || GFlag || BFlag)
		{
				fprintf(stderr, "An error occurred when trying to set the gpio output pins:\n");
				fprintf(stderr, "%s\n", pigpio_error(RFlag));
				return -1;
		}

		RFlag = set_pull_up_down(pi, GPIO_R, PI_PUD_DOWN);
		GFlag = set_pull_up_down(pi, GPIO_G, PI_PUD_DOWN);
		BFlag = set_pull_up_down(pi, GPIO_B, PI_PUD_DOWN);
		if(RFlag || GFlag || BFlag)
		{
				fprintf(stderr, "An error occurred when trying to set the pull up/down resistors\n");
				fprintf(stderr, "%s\n", pigpio_error(RFlag));
				return -1;
		}
		return pi;
}

bool SetAllPWM(int pi, int value)
{	
		int RFlag = set_PWM_dutycycle(pi, GPIO_R, value);
		int GFlag = set_PWM_dutycycle(pi, GPIO_G, value);
		int BFlag = set_PWM_dutycycle(pi, GPIO_B, value);
		if(RFlag || GFlag || BFlag)
		{
				fprintf(stderr, "An error occurred when trying to set the PWM duty cycle\n");
				fprintf(stderr, "%s\n", pigpio_error(RFlag));
				return false;
		}
		return true;
}

bool SetPWM(int pi, int r, int g, int b)
{
		int RFlag = set_PWM_dutycycle(pi, GPIO_R, r);
		int GFlag = set_PWM_dutycycle(pi, GPIO_G, g);
		int BFlag = set_PWM_dutycycle(pi, GPIO_B, b);
		if(RFlag || GFlag || BFlag)
		{
				fprintf(stderr, "An error occurred when trying to set the PWM duty cycle\n");
				fprintf(stderr, "%s\n", pigpio_error(RFlag | GFlag | BFlag));
				return false;
		}
		return true;
}

#endif //__PWM_COMMON_H
