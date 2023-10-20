#ifndef _PROTO_IDENTIFY_
#define _PROTO_IDENTIFY_

#include "common.h"


int identify_oicq(const uint8_t *data, int len, net_info_t *net_info);
int identify_http(const uint8_t *data, int len, net_info_t *net_info);
int identify_ftp(const uint8_t *data, int len, net_info_t *net_info);


#endif