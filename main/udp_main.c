
#include <errno.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_err.h"

#include "udp_perf.h"


//this task establish a UDP connection and receive data from UDP
static void udp_conn(void *pvParameters)
{
    ESP_LOGI(TAG, "task udp_conn start.");
    /*wating for connecting to AP*/
    //xEventGroupWaitBits(udp_event_group, WIFI_CONNECTED_BIT,false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "sta has connected to ap.");
    
    /*create udp socket*/
    int socket_ret;
    
    ESP_LOGI(TAG, "create udp client after 20s...");
    vTaskDelay(20000 / portTICK_RATE_MS);
    ESP_LOGI(TAG, "create_udp_client.");
    socket_ret = create_udp_client();
    if(socket_ret == ESP_FAIL) {
	ESP_LOGI(TAG, "create udp socket error,stop.");
	vTaskDelete(NULL);
    }
    
    /*create a task to tx/rx data*/
    TaskHandle_t tx_rx_task;
    xTaskCreate(&send_recv_data, "send_recv_data", 4096, NULL, 4, &tx_rx_task);

    /*waiting udp connected success*/
    xEventGroupWaitBits(udp_event_group, UDP_CONNCETED_SUCCESS,false, true, portMAX_DELAY);
    int bps;
    while (1) {
	total_data = 0;
	vTaskDelay(3000 / portTICK_RATE_MS);//every 3s
	bps = total_data / 3;

	/*if (total_data <= 0) {
	    int err_ret = check_connected_socket();
	    if (err_ret == -1) {  //-1 reason: low level netif error
		ESP_LOGW(TAG, "udp send & recv stop.\n");
		break;
	    }
	}*/

	ESP_LOGI(TAG, "udp send %d byte per sec! total pack: %d \n", bps, success_pack);
    }
    close_socket();
    vTaskDelete(tx_rx_task);
    vTaskDelete(NULL);
}



void udp_main(void)
{
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    //wifi_init_sta();
    xTaskCreate(&udp_conn, "udp_conn", 4096, NULL, 5, NULL);
}
