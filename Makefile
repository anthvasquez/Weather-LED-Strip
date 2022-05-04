all: pwmprog cycle_LED set_color
#	pwmprog
#	led_util

pwmprog: pwmprog.c pwm_common.h signal_utils.h signal_utils.c cJSON/cJSON.h cJSON/cJSON.c
	gcc -Wall -pthread -o pwmprog pwmprog.c pwm_common.h signal_utils.h signal_utils.c cJSON/cJSON.h cJSON/cJSON.c -lpigpiod_if2 -lrt -lcurl

cycle_LED: WeatherLED.c pwm_common.h signal_utils.h signal_utils.c
	gcc -Wall -pthread -o cycle_LED WeatherLED.c pwm_common.h signal_utils.h signal_utils.c -lpigpiod_if2 -lrt -lcurl


set_color: LED_util.c pwm_common.h
	gcc -Wall -pthread -o set_color LED_util.c pwm_common.h -lpigpiod_if2 -lrt

clean:
	rm pwmprog
	rm set_color
