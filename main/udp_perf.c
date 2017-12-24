/* udp_perf Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <sys/socket.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"


#include "udp_perf.h"


/* FreeRTOS event group to signal when we are connected to WiFi and ready to start UDP test*/
EventGroupHandle_t udp_event_group;


static int mysocket;

static struct sockaddr_in remote_addr;
static unsigned int socklen;

int total_data = 0;
int success_pack = 0;




//wifi_init_sta
void wifi_init_sta()
{
    udp_event_group = xEventGroupCreate();
    
    tcpip_adapter_init();
    //ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_DEFAULT_SSID,
            .password = EXAMPLE_DEFAULT_PWD
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");
    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s \n",
	    EXAMPLE_DEFAULT_SSID,EXAMPLE_DEFAULT_PWD);
}

//create a udp client socket. return ESP_OK:success ESP_FAIL:error
esp_err_t create_udp_client()
{
    ESP_LOGI(TAG, "create_udp_client()");
    ESP_LOGI(TAG, "connecting to %s:%d",
	    EXAMPLE_DEFAULT_SERVER_IP, EXAMPLE_DEFAULT_PORT);
    mysocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (mysocket < 0) {
    	show_socket_error_reason(mysocket);
	return ESP_FAIL;
    }
    /*for client remote_addr is also server_addr*/
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(EXAMPLE_DEFAULT_PORT);
    remote_addr.sin_addr.s_addr = inet_addr(EXAMPLE_DEFAULT_SERVER_IP);

    return ESP_OK;
}


//send or recv data task
void send_recv_data(void *pvParameters)
{
    ESP_LOGI(TAG, "task send_recv_data start!\n");
    
    int len;
    char databuff[EXAMPLE_DEFAULT_PKTSIZE];
    
    /*send&receive first packet*/
    socklen = sizeof(remote_addr);
    memset(databuff, EXAMPLE_PACK_BYTE_IS, EXAMPLE_DEFAULT_PKTSIZE);
#if EXAMPLE_ESP_UDP_MODE_SERVER
    ESP_LOGI(TAG, "first recvfrom:");
    len = recvfrom(mysocket, databuff, EXAMPLE_DEFAULT_PKTSIZE, 0, (struct sockaddr *)&remote_addr, &socklen);
#else
    ESP_LOGI(TAG, "first sendto:");
    len = sendto(mysocket, databuff, EXAMPLE_DEFAULT_PKTSIZE, 0, (struct sockaddr *)&remote_addr, sizeof(remote_addr));
#endif
    
    if (len > 0) {
	ESP_LOGI(TAG, "transfer data with %s:%u\n",
		inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
	xEventGroupSetBits(udp_event_group, UDP_CONNCETED_SUCCESS);
    } else {
    	show_socket_error_reason(mysocket);
	close(mysocket);
	vTaskDelete(NULL);
    } /*if (len > 0)*/
    
#if EXAMPLE_ESP_UDP_PERF_TX
    vTaskDelay(500 / portTICK_RATE_MS);
#endif
    ESP_LOGI(TAG, "start count!\n");
    while(1) {
#if EXAMPLE_ESP_UDP_PERF_TX
	len = sendto(mysocket, databuff, EXAMPLE_DEFAULT_PKTSIZE, 0, (struct sockaddr *)&remote_addr, sizeof(remote_addr));
#else
	len = recvfrom(mysocket, databuff, EXAMPLE_DEFAULT_PKTSIZE, 0, (struct sockaddr *)&remote_addr, &socklen);
#endif
	if (len > 0) {
	    total_data += len;
	    success_pack++;
	} else {
	    if (LOG_LOCAL_LEVEL >= ESP_LOG_DEBUG) {
		show_socket_error_reason(mysocket);
	    }
	} /*if (len > 0)*/
    } /*while(1)*/
}


int get_socket_error_code(int socket)
{
    int result;
    u32_t optlen = sizeof(int);
    if(getsockopt(socket, SOL_SOCKET, SO_ERROR, &result, &optlen) == -1) {
	ESP_LOGE(TAG, "getsockopt failed");
	return -1;
    }
    return result;
}

int show_socket_error_reason(int socket)
{
    int err = get_socket_error_code(socket);
    ESP_LOGW(TAG, "socket error %d %s", err, strerror(err));
    return err;
}

int check_connected_socket()
{
    int ret;
    ESP_LOGD(TAG, "check connect_socket");
    ret = get_socket_error_code(mysocket);
    if(ret != 0) {
    	ESP_LOGW(TAG, "socket error %d %s", ret, strerror(ret));
    }
    return ret;
}

void close_socket()
{
    close(mysocket);
}
