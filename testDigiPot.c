#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

//about 124 ohms for the wiper resistance Rw
//This is a 7-bit device, meaning the max wiper code is 0x80

const int SPICHANNEL = 0;
const int GPIO_CS_RED = 4;
const int GPIO_CS_GREEN = 5;
const int GPIO_CS_BLUE = 6;
const unsigned char INCREMENT_CODE = 0x04;

uint8_t rValue, gValue, bValue = 0;

/**
 * Writes the provided value to all the CS pins
 */
static void WriteAll(int value)
{
		digitalWrite(GPIO_CS_RED, value);
		digitalWrite(GPIO_CS_GREEN, value);
		digitalWrite(GPIO_CS_BLUE, value);
}

/**
 * Parses the given string and sets
 * values for rValue, gValue, and bValue
 */
static void parseHexString(char* str, uint8_t* r, uint8_t* g, uint8_t* b)
{
		uint8_t rgb[3];
		char* buffer = calloc (3, sizeof(char));
		str += 1;

		int i = 0;
		for(i = 0; i < 3; i++)
		{
				strncpy(buffer, str, 2);
				rgb[i] = (uint8_t)strtol(buffer, NULL, 16);
				str += 2;
		}
		free(buffer);

		*r = rgb[0];
		*g = rgb[1];
		*b = rgb[2];
}

/**
 * Converts the current values of rValue, gValue, and bValue
 * into resistance codes for the potentiometers.
 */
static void RGBtoCodes(uint8_t* r, uint8_t* g, uint8_t* b)
{
	*r = (rValue >> 1) + 1;
	*g = (gValue >> 1) + 1;
	*b = (bValue >> 1) + 1;
}

/**
 * Resets the SPI data transfer buffer with the given
 * potentiometer resistance code
 */
static void SetPotValue(char* buf, uint8_t value)
{
	buf[0] = '\0';
	buf[1] = value;
}

/**
 * Parses the program's argument list and sets the RGB values
 * from the appropriate source
 * returns 0 if the values were set successfully, nonzero otherwise.
 */
static int SetRGBValues(int argc, char** argv)
{
		if(argc == 1)
		{
				//get temperature from the internet & set rgb values from that

				//temporary
				rValue = 0x00;
				gValue = 0x00;
				bValue = 0x00;
		}
		else if(argc == 2)
		{
				//make sure the argument is something like #ffffff
				char* hexString = argv[1];
				if(strlen(hexString) >= 7 && hexString[0] == '#')
						parseHexString(hexString, &rValue, &gValue, &bValue);
				else
				{
						printf("usage: ./LED <color-code>\n");
						printf("Color code must be in the form #ffffff\n");
						return 1;
				}
		}

		else
		{
				printf("argument list cannot have more than 2 values.\n");
				printf("usage:\n./LED <color-code>\n\tOR\n./LED\n");
				return 1;
		}
		return 0;
}

int main(int argc, char** argv)
{
		//setup wiringPi
		int gpioFd = wiringPiSetup();
		int spiFd = wiringPiSPISetup(SPICHANNEL, 4000000);
		if(spiFd < 0 || gpioFd < 0)
				printf("One or more interfaces couldn't be setup\n");

		//setup pins initial state
		pinMode(GPIO_CS_RED, OUTPUT);
		pinMode(GPIO_CS_GREEN, OUTPUT);
		pinMode(GPIO_CS_BLUE, OUTPUT);
		WriteAll(HIGH);

		//assign the desired color to a variable for each LED
		int status = SetRGBValues(argc, argv);
		if(status != 0)
				return status;
		uint8_t r, g, b;
		RGBtoCodes(&r, &g, &b);
		printf("RGB: %x %x %x\n", rValue, gValue, bValue);
		printf("RGB codes: %x %x %x\n", r, g, b);

		char* data = calloc(2, sizeof(unsigned char));

		SetPotValue(data, r);
		//printf("Red pot set to value %x%x\n", data[0], data[1]);
		digitalWrite(GPIO_CS_RED, LOW);
		wiringPiSPIDataRW(SPICHANNEL, (unsigned char*)data, 2);
		digitalWrite(GPIO_CS_RED, HIGH);

		SetPotValue(data, g);
		//printf("Green pot set to value %x%x\n", data[0], data[1]);
		digitalWrite(GPIO_CS_GREEN, LOW);
		wiringPiSPIDataRW(SPICHANNEL, (unsigned char*)data, 2);
		digitalWrite(GPIO_CS_GREEN, HIGH);

		SetPotValue(data, b);
		//printf("Blue pot set to value %x%x\n", data[0], data[1]);
		digitalWrite(GPIO_CS_BLUE, LOW);
		wiringPiSPIDataRW(SPICHANNEL, (unsigned char*)data, 2);
		digitalWrite(GPIO_CS_BLUE, HIGH);



		printf("OK\n");

		/*unsigned char counter = 0;
		  while(counter++ < 0xff)
		  {
		  data[0] = INCREMENT;
		  printf("Incremented to counter=%x\n", counter);
		  WriteAll(LOW);
		  wiringPiSPIDataRW(SPICHANNEL, data, 1);
		  WriteAll(HIGH);
		  delay(500);
		  }*/

		return 0;
}
