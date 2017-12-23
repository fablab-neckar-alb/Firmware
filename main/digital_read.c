#include "sdkconfig.h"

#include <string.h>

#include <badge_eink.h>
#include <badge_eink_fb.h>
#include <badge_input.h>
#include <badge_pins.h>
#include <badge_leds.h>

#include <font.h>


void
digital_read(void)
{
	esp_err_t err = badge_eink_fb_init();
	assert( err == ESP_OK );

	memset(badge_eink_fb, 0xff, BADGE_EINK_FB_LEN);
	draw_font(badge_eink_fb, 6, 16, 284, "DigitalRead:",
		FONT_INVERT);

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
