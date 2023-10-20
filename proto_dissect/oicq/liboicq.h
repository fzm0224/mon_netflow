#ifndef _LIBOICQ_
#define _LIBOICQ_

#include "common.h"

typedef struct _oicq_field_flag_t {
    uint32_t flag_flag:1,
            flag_version:1,
            flag_command:1,
            flag_sequence:1,
            flag_oicq_number:1;
} oicq_field_flag_t;

typedef struct _oicq_data_t {
    oicq_field_flag_t field_flag;
    uint8_t flag;
    uint16_t version;
    uint16_t command;
    uint16_t sequence;
    uint32_t oicq_number;
} oicq_data_t;


int oicq_segment_check(char *data, int len);
oicq_data_t *oicq_dissect_init();
int oicq_dissect_data(oicq_data_t *oicq_data, char *data, int len, net_info_t *net_info);
int oicq_dissect_fini();
int oicq_s2j(oicq_data_t *oicq_data, char **oicq_json);

#endif