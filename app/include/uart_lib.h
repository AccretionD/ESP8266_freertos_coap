#ifndef _EL_UART_H_
#define _EL_UART_H_

#define UART0 0
#define UART1 1
typedef enum {
    BIT_RATE_9600     = 9600,
    BIT_RATE_19200   = 19200,
    BIT_RATE_38400   = 38400,
    BIT_RATE_57600   = 57600,
    BIT_RATE_74880   = 74880,
    BIT_RATE_115200 = 115200,
    BIT_RATE_230400 = 230400,
    BIT_RATE_460800 = 460800,
    BIT_RATE_921600 = 921600
} UartBautRate;


#include "esp_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

extern xQueueHandle  uart_rx_queue;
extern volatile uint16_t uart_rx_overruns;
extern volatile uint16_t uart_rx_bytes;

int ICACHE_FLASH_ATTR uart_getchar_ms(int timeout);
#define uart_getchar() uart_getchar_ms(-1)
#define uart_rx_flush() xQueueReset(uart_rx_queue)
int ICACHE_FLASH_ATTR uart_rx_available(void);
void ICACHE_FLASH_ATTR uart_rx_init(void);
void ICACHE_FLASH_ATTR uart_set_baud(int uart, int baud);

#endif
// vim: ts=4 sw=4 noexpandtab
