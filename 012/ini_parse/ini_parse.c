#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ini.h"

typedef struct
{
    const char* ip_addr;
    const char* name;
    int num;
} configuration;

static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    configuration* pconfig = (configuration*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("ip", "ip_addr")) 
    {
        pconfig->ip_addr = strdup(value);
    } 
    else if (MATCH("test", "name")) 
    {
        pconfig->name = strdup(value);
    } 
    else if (MATCH("test", "num")) 
    {
        pconfig->num = atoi(value);
    } 
    else 
    {
        return 0;  /* unknown section/name, error */
    }

    return 1;
}

int main(int argc, char* argv[])
{
    configuration config;
    config.ip_addr = NULL;  /* set defaults */
    config.name = NULL;
    config.num = 0;

    if (ini_parse("ip.ini", handler, &config) < 0) 
    {
        printf("Can't load 'ip.ini'\n");
        return 1;
    }
    printf("Config loaded from 'ip.ini': ip_addr = %s, name = %s, num = %d\n",
        config.ip_addr, config.name, config.num);

    if (config.ip_addr)
        free((void*)config.ip_addr);
    if (config.name)
        free((void*)config.name);

    return 0;
}