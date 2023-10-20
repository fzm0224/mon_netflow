#ifndef _DISSECT_ENGINE_
#define _DISSECT_ENGINE_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "common.h"


enum {
    FUNC_NONSUPPORT = 0,
    FUNC_SEGMENT_CHECK,
    FUNC_DISSECT_INIT,
    FUNC_DISSECT_DATA,
    FUNC_DISSECT_FINI,
    FUNC_S2J
};


#define MAX_PROTO_NUM               256
#define MAX_CONFIG_STR_LEN          10240

#define FUNC_FLAG_SEGMENT_CHECK     0x01
#define FUNC_FLAG_DISSECT_INIT      0x01 << 1
#define FUNC_FLAG_DISSECT_DATA      0x01 << 2
#define FUNC_FLAG_DISSECT_FINI      0x01 << 3
#define FUNC_FLAG_S2J               0x01 << 4


typedef struct _lib_func_info_t {
    char    *func_name;
    int     func_flag;
    int     func_index;
} lib_func_info_t;


typedef struct _proto_key_value_t {
    char    *proto_name;
    char    proto_index;
} proto_key_value_t;


typedef struct _lib_info_t {
    char        *proto_name;
    uint32_t    proto_id;
    uint32_t    lib_func_flag;
    int         (*proto_segment_check)(char *, int );
    void*       (*proto_dissect_init)();
    int         (*proto_dissect_data)(void *, char *, int , void *);
    int         (*proto_dissect_fini)();
    int         (*proto_s2j)(void *, char **);
} lib_info_t;


void ipv4_to_str(uint32_t ip_value, char *ip_addr);
int load_config(char *config_file_path, char *lib_dir_path);


#endif