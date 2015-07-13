/*
	The obligatory blinky demo
	Blink an LED on GPIO pin 2
*/


// see eagle_soc.h for these definitions
#define LED_GPIO 2
#define LED_GPIO_MUX PERIPHS_IO_MUX_GPIO2_U
#define LED_GPIO_FUNC FUNC_GPIO2


#include "esp_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#include "uart_lib.h"

#include <gpio.h>

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#define WIFI_APSSID	"hackbo"
#define WIFI_APPASSWORD	"12345678"
#define WIFI_APWPA	true
#define TCPSERVERIP	"10.10.10.100"
#define TCPSERVERPORT	5555
#define PLATFORM_DEBUG	true
#define BTNGPIO		0


#include "udhcp/dhcpd.h"
static char macaddr[6];
#define server_ip "192.168.101.142"
#define server_port 9669

#include "freertos/semphr.h"
#include "freertos/portmacro.h"

#include "uart.h"
#include "dmsg.h"
#include "wifi.h"
#include "shell.h"
#include "xmit.h"
#include "coap_server.h"

#define PORT 5683

uint8_t buf[1024];
uint8_t scratch_raw[1024];

void blink_hi(void *pvParameters)
{

	uint8_t state=0;
	const portTickType xDelay = 1000 / portTICK_RATE_MS;
	for(;;)
	{

		uart0_sendStr("Hello World!\r\n");
		GPIO_OUTPUT_SET(LED_GPIO, state);
		state ^=1;
		printf("Hello World!\n");
		vTaskDelay( xDelay);
	}
}

void ICACHE_FLASH_ATTR uart_init(void)
{
    uart_param_t uart_param;
    uart_param.uart_baud_rate = UART_BAUD_RATE_115200;
    uart_param.uart_xfer_bit = UART_XFER_8_BIT;
    uart_param.uart_parity_mode = UART_PARITY_NONE;
    uart_param.uart_stop_bit = UART_1_STOP_BIT;
    uart_param.uart_flow_ctrl = UART_NONE_FLOW_CTRL;
    uart0_init(&uart_param);
}



LOCAL void ICACHE_FLASH_ATTR shell_task(void *pvParameters)
{
    os_event_t e;
    char ch;

    for (;;)
    {
        if (xQueueReceive(xQueueUart, (void *)&e, (portTickType)portMAX_DELAY))
        {
            switch (e.event)
            {
                case UART_EVENT_RX_CHAR:
                    ch = (char)e.param;
                    shell_process_char(ch);
                    break;
                default:
                    break;
            }
        }
    }
    vTaskDelete(NULL);
}



/*
 * This is entry point for user code
 */
void ICACHE_FLASH_ATTR
user_init(void){

//	uart_div_modify(UART0, UART_CLK_FREQ / (BIT_RATE_115200));

//	uart_rx_init();
	//PIN_FUNC_SELECT(LED_GPIO_MUX, LED_GPIO_FUNC);
	//setap_task();
//	xTaskCreate(wifi_init, "ap", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	 int ret;
    dmsg_init();
    uart_init();
    shell_init(uart0_putchar);
    wifi_init();
    coap_init();
    //coap_server();
    //xTaskCreate(coap_server, "ap", 512, NULL, 2, NULL);

    //xTaskCreate(shell_task, "shell", 512, NULL, tskIDLE_PRIORITY + 2, NULL);
    //shell_do_draw();
   
}






