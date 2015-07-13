/**
 * @brief       : coap server example
 *
 * @file        : coap_server.c
 * @author      : xukai
 * @version     : v0.0.1
 * @date        : 2015/5/11
 *
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2015/5/11    v0.0.1      xukai    some notes
 */


#include "esp_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <lwip/sockets.h> 
#include "coap_server.h"
//#include "coap.h"

#include "shell.h"
#define PORT 5683


uint8_t buf[1024];
uint8_t scratch_raw[1024];

void ICACHE_FLASH_ATTR coap_server(void *pvParameters)

{
    int fd;
    struct sockaddr_in servaddr, cliaddr;

    coap_rw_buffer_t scratch_buf = {scratch_raw, sizeof(scratch_raw)};

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
    {
        shell_printf("\nSocket Error\r\n");
        return;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    memset(&(servaddr.sin_zero), 0, sizeof(servaddr.sin_zero));
    
    if ((bind(fd, (struct sockaddr *)&servaddr, sizeof(servaddr))) == -1)
    {
       shell_printf("Bind error\r\n");
       return;        
    }

    endpoint_setup();
    shell_printf("Coap Server Start!\r\n");
    while(1)
    {
        int n, rc;
        socklen_t len = sizeof(cliaddr);
        coap_packet_t pkt;

        n = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&cliaddr, &len);
#ifdef MICROCOAP_DEBUG
        shell_printf("\r\n--------------------\r\n");
        shell_printf("Received Buffer: \r\n");
        coap_dump(buf, n, true);
        shell_printf("\r\n");
#endif

        if (0 != (rc = coap_parse(&pkt, buf, n)))
        {
            //shell_printf("Bad packet rc\r\n");
            shell_printf("Bad packet rc=%d\r\n", rc);
        }
        else
        {
            size_t rsplen = sizeof(buf);
            coap_packet_t rsppkt;
            printf("Dump Packet: \r\n");
            coap_dumpPacket(&pkt);
            coap_handle_req(&scratch_buf, &pkt, &rsppkt);

            if (0 != (rc = coap_build(buf, &rsplen, &rsppkt)))
            {
                 printf("coap_build failed rc=%d\n", rc);
            }
            else
            {
                shell_printf("--------------------\r\n");
                shell_printf("Sending Buffer: \r\n");
                coap_dump(buf, rsplen, true);
                printf("\r\n");
                coap_dumpPacket(&rsppkt);
                sendto(fd, buf, rsplen, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
            }
        }
    }
    
}

void coap_init(void){
	xTaskCreate(coap_server, "coap", 512, NULL, 2, NULL);
}
