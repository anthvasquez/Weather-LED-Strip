#include <pigpiod_if2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <curl/curl.h>
#include <string.h>
#include "signal_utils.h"
#include "pwm_common.h"

int pi;
const char* URL = "https://api.weather.gov/gridpoints/LWX/87,65/forecast?units=us";
const int CURL_RESPONSE_MAX_LEN = 50000;

static void CMainHandler(int sig)
{
		//sig is blocked by default
		//bool signalwasblocked = mask_signal(sig, SIG_BLOCK);
		if(pi < 0)
		{
				printf("Force quitting...\n");
				exit(EXIT_SUCCESS);
		}

		printf("Setting output to LOW\n");
		if(!SetAllPWM(pi, 0))
				exit(EXIT_FAILURE);

		exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
		pi = pwm_init();

		signal_sethandler(SIGINT, (sa_sigaction_t)CMainHandler);

		int pwmR = 128;
		int pwmG = 32;
		int pwmB = 64;
		while(true)
		{
				pwmR = (pwmR + 16) % 128;
				pwmG = (pwmG + 16) % 128;
				pwmB = (pwmB + 16) % 128;

				printf("Output:\nRed:\t%d\nGreen:\t%d\nBlue:\t%d\n", pwmR, pwmG, pwmB);
				printf("-------------\n");

				SetPWM(pi, pwmR, pwmG, pwmB);

				time_sleep(2);
		}

		pigpio_stop(pi);
		return 0;
}
