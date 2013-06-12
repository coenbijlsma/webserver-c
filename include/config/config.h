#ifndef CWS_CONFIG_H
#define CWS_CONFIG_H

#include <stdio.h>

typedef struct cws_config {
    
} cws_config;



char* cws_config_get_value(const cws_config* config, const char* key);

void cws_config_set_value(const cws_config* config, const char* key, const char* value);

const cws_config* read_config(char* path);

#endif
