#include <pigpiod_if2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <curl/curl.h>
#include <string.h>
#include "signal_utils.h"
#include "pwm_common.h"
#include "cJSON/cJSON.h"

int pi;
const char* URL = "https://api.weather.gov/gridpoints/LWX/87,65/forecast?units=us";
const int CURL_RESPONSE_MAX_LEN = 50000;
const int MIN_TEMP = 40;
const int MAX_TEMP = 110;
const char* MIN_COLOR = "#0000FF";
const char* MAX_COLOR = "#FF0000";

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

size_t curl_callback(void* buffer, size_t size, size_t nmemb, void* userdata)
{
		char** programBuffer = (char**)userdata;
		int copylen = strlen(*programBuffer) + nmemb > CURL_RESPONSE_MAX_LEN ? CURL_RESPONSE_MAX_LEN - strlen(*programBuffer) : nmemb;
		strncat(*programBuffer, (char*)buffer, copylen);
		return nmemb;
}

static bool GetWeatherForecast(char** dataBuf)
{
		char errorBuf[500];

		CURL* curl_handle = curl_easy_init();
		curl_easy_setopt(curl_handle, CURLOPT_URL, URL);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_callback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, dataBuf);
		curl_easy_setopt(curl_handle, CURLOPT_ERRORBUFFER, &errorBuf);
		curl_easy_setopt(curl_handle, CURLOPT_FAILONERROR, 1);
		curl_easy_setopt(curl_handle, CURLOPT_DEFAULT_PROTOCOL, "https");
		curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1);

		curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "curl/7.64.0");
		curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 50L);
		curl_easy_setopt(curl_handle, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
		curl_easy_setopt(curl_handle, CURLOPT_HTTP09_ALLOWED, 1L);
		curl_easy_setopt(curl_handle, CURLOPT_SSH_KNOWNHOSTS, "/home/pi/.ssh/known_hosts");
		curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "GET");
		curl_easy_setopt(curl_handle, CURLOPT_FTP_SKIP_PASV_IP, 1L);
		curl_easy_setopt(curl_handle, CURLOPT_TCP_KEEPALIVE, 1L);

		struct curl_slist* headerList = NULL;
		headerList = curl_slist_append(headerList, "accept: application/geo+json");
		headerList = curl_slist_append(headerList, "Feature-Flags:");
		curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headerList);

		CURLcode response = curl_easy_perform(curl_handle);
		if(response != CURLE_OK)
		{
				fprintf(stderr, "ERRORBUFFER: %s\n", errorBuf);
				fprintf(stdout, "strerror: %s\n", curl_easy_strerror(response));
		}
		curl_easy_cleanup(curl_handle);
		curl_slist_free_all(headerList);

		return response == CURLE_OK;
}

static int GetTemperatureF()
{
		int temperature = 0;
		char* forecast = calloc(CURL_RESPONSE_MAX_LEN, sizeof(char));
		if(!GetWeatherForecast(&forecast))
		{
				printf("Getting the forecast failed, setting temp to 0\n");
				free(forecast);
				return 0;
		}

		cJSON* json = cJSON_Parse(forecast);
		cJSON* properties = cJSON_GetObjectItemCaseSensitive(json, "properties");
		if(cJSON_IsObject(properties))
		{
				cJSON* periods = cJSON_GetObjectItemCaseSensitive(properties, "periods");
				//cJSON* element = NULL;
				//cJSON_ArrayForEach(element, periods)
				if(periods != NULL)
				{
						//just use the first forecast in the list for now
						cJSON* temp = cJSON_GetObjectItemCaseSensitive(periods->child, "temperature");
						temperature = temp->valueint;
				}
				else
						printf("No forecast periods were found.\n");
		}
		else
				printf("Error parsing the forecast (properties does not exist)\n");

		cJSON_Delete(json);
		free(forecast);
		return temperature;
}

static Color LerpColor(int minTemp, int maxTemp, Color minColor, Color maxColor, int value)
{
		Color lerpedColor;
		float tempval = value - minTemp;
		float tempmax = maxTemp - minTemp;
		float valPercentage = tempval / tempmax;
		float clampedValue = valPercentage < 0 ? 0 : valPercentage > 1 ? 1 : valPercentage;

		lerpedColor.R = minColor.R + (maxColor.R - minColor.R) * clampedValue;
		lerpedColor.G = minColor.G + (maxColor.G - minColor.G) * clampedValue;
		lerpedColor.B = minColor.B + (maxColor.B - minColor.B) * clampedValue;
		return lerpedColor;
}

int main(int argc, char** argv)
{
		pi = pwm_init();

		signal_sethandler(SIGINT, (sa_sigaction_t)CMainHandler);

		curl_global_init(CURL_GLOBAL_ALL);

		int temp = GetTemperatureF();
		printf("temperature: %d\n", temp);

		Color color = LerpColor(MIN_TEMP, MAX_TEMP, CreateColor(MIN_COLOR), CreateColor(MAX_COLOR), temp);
		char hexString[8];

		//Reduce all colors by half to protect LEDs
		color.R >>= 1;
		color.G >>= 1;
		color.B >>= 1;
		ColorToHex(color, hexString);

		printf("Output:\nHex: %s\nRed:\t%d\nGreen:\t%d\nBlue:\t%d\n", hexString, color.R, color.G, color.B);
		printf("-------------\n");

		SetPWM(pi, color.R, color.G, color.B);

		//Replace this with some logic to hold the light on until some event happens
		time_sleep(10);
		printf("Setting output to LOW\n");
		SetAllPWM(pi, 0);

		pigpio_stop(pi);
		return 0;
}
