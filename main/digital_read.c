#include "sdkconfig.h"

#include <string.h>

#include <badge_eink.h>
#include <badge_eink_fb.h>
#include <badge_input.h>
#include <badge_pins.h>
#include <badge_leds.h>
#include <esp_log.h>

#include "driver/gpio.h"

#include <font.h>
#include "udp_main.h"
//bei uns pin 0
#define READ_GPIO PIN_NUM_EXT_IO_4
//bei uns pin 1
#define WRITE_GPIO PIN_NUM_EXT_IO_3

void
digital_read(void)
{

   udp_main();
	esp_err_t err = badge_eink_fb_init();
	assert( err == ESP_OK );

	memset(badge_eink_fb, 0xff, BADGE_EINK_FB_LEN);
	draw_font(badge_eink_fb, 6, 16, 284, "DigitalRead:",
		FONT_INVERT);
	badge_eink_display(badge_eink_fb, DISPLAY_FLAG_LUT(2));
    gpio_pad_select_gpio(WRITE_GPIO);
    gpio_pad_select_gpio(READ_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(WRITE_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(READ_GPIO, GPIO_MODE_INPUT);
    int level = 0;
    while(1) {
        /* Blink off (output low) */
        level = gpio_get_level(READ_GPIO);
        //vTaskDelay(1000 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(WRITE_GPIO, level);
/*#        if(level)
#         ets_printf("1.\n");
#        else
 #         ets_printf("0.\n");*/
        //vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

	while (1) {
		// exit on random keypress
		if (badge_input_get_event(10) != 0)
			break;

     //serial print status
	}


	memset(badge_eink_fb, 0xff, BADGE_EINK_FB_LEN);
	draw_font(badge_eink_fb, 6, 16, 264, "Goodbye.",
		FONT_INVERT);
	badge_eink_display(badge_eink_fb, DISPLAY_FLAG_LUT(2));
}
