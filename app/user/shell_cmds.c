/*
 * shell_cmds.c
 *
 *  Created on: Dec 20, 2014
 *      Author: Baoshi
 */


#include "esp_common.h"
#include "dmsg.h"
#include "shell.h"



shell_errno_t ICACHE_FLASH_ATTR shell_do_help(const char* args)
{
    if (args && *args)
    {
        dmsg_info_printf("Show help(%s)\r\n", args)
    }
    else
    {
        dmsg_info_puts("Show help\r\n");
    }
    return SHELL_EOK;
}


shell_errno_t ICACHE_FLASH_ATTR shell_do_status(const char* args)
{
    struct ip_info info;
    dmsg_info_puts("Show status\r\n");
    if (wifi_get_ip_info(STATION_IF, &info))
    {
        shell_puts("Station IP   : "); shell_printf(IPSTR, IP2STR(&(info.ip))); shell_puts("\r\n");
        shell_puts("Station GW   : "); shell_printf(IPSTR, IP2STR(&(info.gw))); shell_puts("\r\n");
        shell_puts("Station Mask : "); shell_printf(IPSTR, IP2STR(&(info.netmask))); shell_puts("\r\n");
    }
    else
    {
        shell_puts("wifi_get_ip_info failed.\r\n");
    }
    return SHELL_EOK;
}


shell_errno_t ICACHE_FLASH_ATTR shell_do_reset(const char* args)
{
    system_restart();
    return SHELL_EOK;
}



shell_command_t shell_commands[] =
{
    {"HELP", shell_do_help},
    {"STATUS", shell_do_status},
    {"RESET", shell_do_reset},
   // {"DRAW", shell_do_draw},
    {0, 0}
};

