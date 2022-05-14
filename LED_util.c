#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <strings.h>
#include <pigpiod_if2.h>
#include "pwm_common.h"

/**
 * Parses the given string and sets
 * values for rValue, gValue, and bValue
 */
static void ParseHexString(char* str, int* r, int* g, int* b)
{
		if(str[0] == '#')
				str++;
		int rgb = strtol(str, NULL, 16);
		*b = rgb & 0xff;
		rgb >>= 8;
		*g = rgb & 0xff;
		rgb >>= 8;
		*r = rgb & 0xff;
}

static int clamp(int in, int min, int max)
{
		return in > max ? max : in < min ? min : in;
}

int main(int argc, char** argv)
{
		int r, g, b;

		int pi = pwm_init();
		if(pi < 0)
		{
				printf("pwm_init() failed. Exiting...\n");
				return EXIT_FAILURE;
		}
		
		if(argc != 2 && argc != 4)
		{
				printf("Invalid argument list.  Provide a hex string (\"#ffffff\") or 3 arguments for each color (0-255).\n");
				return EXIT_FAILURE;
		}

		if(argc == 2)
		{
				ParseHexString(argv[1], &r, &g, &b);
		}
		else if(argc == 4)
		{
				r = atoi(argv[1]);
				g = atoi(argv[2]);
				b = atoi(argv[3]);
		}

		r = clamp(r, 0, 128);
		g = clamp(g, 0, 128);
		b = clamp(b, 0, 128);

		printf("Setting\t\tR:%d\tG:%d\tB:%d\n", r, g, b);
		if(SetPWM(pi, r, g, b))
				return EXIT_SUCCESS;
		return EXIT_FAILURE;
}
