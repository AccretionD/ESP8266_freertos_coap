/*
 * wifi.c
 *
 *  Created on: Dec 14, 2014
 *      Author: Baoshi
 */

#include "esp_common.h"
#include "wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "udhcp/dhcpd.h"



#define WIFI_APSSID	"HACKBO"
#define WIFI_APPASSWORD	"12345678"
#define PLATFORM_DEBUG	true

static char macaddr[6];



void wifi_event_cb(System_Event_t *evt) {
   static int serverinit=0;
   switch (evt->event_id) {
      case EVENT_SOFTAPMODE_STACONNECTED:
         shell_printf("EVENT_SOFTAPMODE_STACONNECTED:\n");
	 break;
      case EVENT_SOFTAPMODE_STADISCONNECTED:
         shell_printf("EVENT_SOFTAPMODE_STADISCONNECTED:\n");
         break;
      case EVENT_STAMODE_CONNECTED:
         shell_printf("EVENT_STAMODE_CONNECTED:\n");
	 break;
      case EVENT_STAMODE_DISCONNECTED:
         shell_printf("EVENT_STAMODE_DISCONNECTED:\n");
	 break;
      case EVENT_STAMODE_AUTHMODE_CHANGE:
         shell_printf("EVENT_STAMODE_AUTHMODE_CHANGE:\n");
	 break;
      case EVENT_STAMODE_GOT_IP:
         shell_printf("EVENT_STAMODE_GOT_IP:\n");
	 break;
    case EVENT_MAX:
         shell_printf("EVENT_MAX:\n");
	break;
      default:
         break;
   }
}

void mode_info(){
	int CUR_MODE=wifi_get_opmode();
	switch(CUR_MODE){
	    case NULL_MODE:
		shell_printf("NULL\n");
		break;
	    case STATION_MODE:
		shell_printf("mode selected: STATION\n");
		break;
	    case SOFTAP_MODE:
		shell_printf("mode selected SOFTAP\n");
		break;
	    case STATIONAP_MODE:
		shell_printf("mode selected STATIONAP\n");
		break;
	    case MAX_MODE:
		shell_printf("mode selected MAX\n");
	        break;
	}

}



void wifi_softap(void)
//void wifi_d(void *pvParameters)
{

	char debug[256];
   sprintf(debug,"\n\nSDK version:%s\n\n", system_get_sdk_version());
	shell_printf(debug);
    
	struct softap_config apConfig;
	struct ip_info ipinfo;
	struct ip_info getinfo;
	char ssid[33];
	char password[33];
	char macaddress[17];
	char info[150];
	wifi_softap_dhcps_stop();
	{
	    IP4_ADDR(&ipinfo.ip, 192, 168, 50, 1);
	    IP4_ADDR(&ipinfo.gw, 192, 168, 50, 1);
	    IP4_ADDR(&ipinfo.netmask, 255, 255, 255, 0);
	    int set_result=wifi_set_ip_info(SOFTAP_IF, &ipinfo);
            shell_printf("Set returned:%d", set_result );
	}


	if(wifi_get_opmode() != SOFTAP_MODE)
	{
		wifi_set_opmode(SOFTAP_MODE);
	}
	mode_info();
	wifi_get_macaddr(SOFTAP_IF, macaddr);
	wifi_softap_get_config(&apConfig);
	memset(apConfig.ssid, 0, sizeof(apConfig.ssid));
	sprintf(ssid, "%s", WIFI_APSSID);
	memcpy(apConfig.ssid, ssid, strlen(ssid));
	wifi_set_event_handler_cb(wifi_event_cb);
	{
		memset(apConfig.password, 0, sizeof(apConfig.password));
		sprintf(password, "%s", WIFI_APPASSWORD);
		memcpy(apConfig.password, password, strlen(password));
		apConfig.authmode = AUTH_WPA_WPA2_PSK;
		apConfig.channel = 7;
		apConfig.max_connection = 255;
		apConfig.ssid_hidden = 0;
		wifi_softap_set_config(&apConfig);

	}
	
	{
		wifi_softap_get_config(&apConfig);
		sprintf(macaddress, MACSTR, MAC2STR(macaddr));

		shell_printf("\n OPMODE: %u, SSID: %s, PASSWORD: %s, CHANNEL: %d, AUTHMODE: %d, MACADDRESS: %s\r\n",
					wifi_get_opmode(),
					apConfig.ssid,
					apConfig.password,
					apConfig.channel,
					apConfig.authmode,
					macaddress);

	}
		wifi_softap_dhcps_start();
//vTaskDelete(NULL);

}

void wifi_init(void)
{
 //xTaskCreate(wifi_daemon, "ap", 512, NULL, tskIDLE_PRIORITY+2, NULL);
    wifi_softap();
}
