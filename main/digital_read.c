#include "sdkconfig.h"

#include <string.h>

#include <badge_eink.h>
#include <badge_eink_fb.h>
#include <badge_input.h>
#include <badge_pins.h>
#include <badge_leds.h>
#include <esp_log.h>

#include "driver/gpio.h"
#include <sha2017_ota.h>
#include <font.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "freertos/task.h"
#include "lwip/inet.h"
#include "lwip/ip4_addr.h"
#include "lwip/dns.h"
#include "lwip/udp.h"
#include "wifi.h"
//bei uns pin 0
#define READ_GPIO PIN_NUM_EXT_IO_4
//bei uns pin 1
#define WRITE_GPIO PIN_NUM_EXT_IO_3


//err_t udp_bind(struct udp_pcb * pcb, struct ip_addr * ipaddr, u16_t port);

void einkText(char * text)
{
	memset(badge_eink_fb, 0xff, BADGE_EINK_FB_LEN);
	draw_font(badge_eink_fb, 6, 16, 284, text, FONT_INVERT);
	badge_eink_display(badge_eink_fb, DISPLAY_FLAG_LUT(2));
}
void digital_read(void)
{
	esp_err_t err = badge_eink_fb_init();
   einkText("digitalRead");
	assert( err == ESP_OK );
   badge_initialise_wifi();

   //INIT GPIO
   gpio_pad_select_gpio(WRITE_GPIO);
   gpio_pad_select_gpio(READ_GPIO);
   /* Set the GPIO as a push/pull output */
   gpio_set_direction(WRITE_GPIO, GPIO_MODE_OUTPUT);
   gpio_set_direction(READ_GPIO, GPIO_MODE_INPUT);
   
//   struct udp_pcb*  socket = udp_new();
//   udp_bind(socket, IP_ADDR_ANY, 1234);
//   ip4_addr_t ipaddr;
//   IP4_ADDR(&ipaddr, 192,168,2,49);
//   udp_connect(socket,ipaddr,1234);
//   struct pbuf * buffer; 
   
//   udp_send(socket,buffer);

   int level = 0;
   while(1) {
      level = gpio_get_level(READ_GPIO);
      gpio_set_level(WRITE_GPIO, level);
      ets_printf("1.\n");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
   }

   einkText("Goodbye.");
}
