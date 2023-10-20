#ifndef _LIB_PROTO_IDENTIFY_
#define _LIB_PROTO_IDENTIFY_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>

#include "common.h"


int dissect_proto_identify(const uint8_t *data, int len, net_info_t *net_info);


#endif