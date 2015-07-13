//#include <stdbool.h>

//#include <string.h>
#include "esp_common.h"
#include "coap.h"
#include "dmsg.h"
#include "shell.h"


static char light = '0';

const uint16_t rsplen = 1500;
static char rsp[1500] = "";
void build_rsp(void);

//#include <stdio.h>
void ICACHE_FLASH_ATTR endpoint_setup(void)
{
    build_rsp();
}


// coap://<ip>:5683/light
//static const coap_endpoint_path_t path_light = {1, {"light"}};


// coap://<ip>:5683/test_json
// {"value":12}
//static const coap_endpoint_path_t path_test_json = {1, {"test_json"}};

// coap://<ip>:5683/temperature
// 22.3
//static const coap_endpoint_path_t path_temperature = {1, {"temperature"}};


static const coap_endpoint_path_t path_query = {1, {"query"}};
static ICACHE_FLASH_ATTR int handle_get_query(coap_rw_buffer_t *scratch, 
                          const coap_packet_t *inpkt, 
                          coap_packet_t *outpkt, 
                          uint8_t id_hi, uint8_t id_lo)
{
    const coap_option_t *opt;
    uint8_t count;
    int ret = 0;
    char* query = malloc(32);

    if (NULL != (opt = coap_findOptions(inpkt, COAP_OPTION_URI_QUERY, &count))) 
    {
        shell_printf("count:%d\r\n", count);
        shell_printf("num: %d\r\n", opt->num);
       int i; 
        for (i = 0; i < count; i++){
            memcpy(query, opt[i].buf.p, opt[i].buf.len);
            query[opt[i].buf.len] = '\0';
            shell_printf("pay: %s \r\n", query);
        }
    }
    
    ret = coap_make_response(scratch, outpkt,
                             NULL, 0, 
                             id_hi, id_lo, 
                             &inpkt->tok, 
                             COAP_RSPCODE_CONTENT, 
                             COAP_CONTENTTYPE_TEXT_PLAIN);
    //rt_free(query);
    return ret;
}

const coap_endpoint_t endpoints[] =
{
    
    {COAP_METHOD_GET, handle_get_query, &path_query, "ct=40"},

    

    {(coap_method_t)0, NULL, NULL, NULL}
};

void ICACHE_FLASH_ATTR build_rsp(void)
{
    uint16_t len = rsplen;
    const coap_endpoint_t *ep = endpoints;
    int i;

    len--; // Null-terminated string

    while (NULL != ep->handler)
    {
        if (NULL == ep->core_attr) {
            ep++;
            continue;
        }

        if (0 < strlen(rsp)) {
            strncat(rsp, ",", len);
            len--;
        }

        strncat(rsp, "<", len);
        len--;

        for (i = 0; i < ep->path->count; i++) {
            strncat(rsp, "/", len);
            len--;

            strncat(rsp, ep->path->elems[i], len);
            len -= strlen(ep->path->elems[i]);
        }

        strncat(rsp, ">;", len);
        len -= 2;

        strncat(rsp, ep->core_attr, len);
        len -= strlen(ep->core_attr);

        ep++;
    }
}

