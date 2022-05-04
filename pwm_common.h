#ifndef __PWM_COMMON_H
#define __PWM_COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pigpiod_if2.h>

const int GPIO_R = 24;
const int GPIO_G = 23;
const int GPIO_B = 25;

struct _Color {
		int R;
		int G;
		int B;
};
typedef struct _Color Color;

Color CreateColor(const char* hexString)
{
		Color color;
		if(hexString[0] == '#')
				hexString++;
		int rgb = strtol(hexString, NULL, 16);
		color.B = rgb & 0xff;
		rgb >>= 8;
		color.G = rgb & 0xff;
		rgb >>= 8;
		color.R = rgb & 0xff;
		return color;
}

void ColorToHex(Color color, char* hex)
{
		int rgb = color.R & 0xFF;
		rgb <<= 8;
		rgb += color.G & 0xFF;
		rgb <<= 8;
		rgb += color.B & 0xFF;
		hex[0] = '#';
		sprintf(hex + 1, "%x", rgb);
}

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
