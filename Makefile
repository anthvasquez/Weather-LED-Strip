all: pwmprog set_color
#	pwmprog
#	led_util

pwmprog:
	gcc -Wall -pthread -o pwmprog pwmprog.c pwm_common.h signal_utils.h signal_utils.c -lpigpiod_if2 -lrt

set_color:
	gcc -Wall -pthread -o set_color LED_util.c pwm_common.h -lpigpiod_if2 -lrt

clean:
	rm pwmprog
	rm set_color
