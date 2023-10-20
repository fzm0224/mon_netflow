#ifndef _LIB_DISSECT_ENGINE_
#define _LIB_DISSECT_ENGINE_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "common.h"





int dissect_engine_init(char *config_file_path, char *lib_dir_path);
int dissect_net_layer234(const uint8_t *packet, net_info_t *net_info);
int dissect_net_layerapp_frame(const uint8_t *data, int len, int proto_id, net_info_t *net_info);
int dissect_net_layerapp_stream(const uint8_t *data, int len, int proto_id);
int dissect_engine_fini();


#endif