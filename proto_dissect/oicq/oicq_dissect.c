#include "liboicq.h"
#include "oicq_dissect.h"
#include "libmem_pool.h"
#include "common.h"
#include "cJSON.h"

mem_pool_t *oicq_poll;

int oicq_segment_check(char *data, int len) {
    return len;
}

oicq_data_t *oicq_dissect_init() {
    mem_test_null(oicq_poll, mem_create_pool(MEM_DEFAULT_POOL_SIZE), NULL);

    oicq_data_t *oicq_data = mem_palloc(oicq_poll, sizeof(oicq_data_t));
    if (!oicq_data) {
        return NULL;
    }
    return oicq_data;
}

int oicq_dissect_data(oicq_data_t *oicq_data, char *data, int len, net_info_t *net_info) {
    if (!oicq_data || !data || len <= 0) {
        return -1;
    }

    int offset = 0;
    check_data_len(1);
    oicq_data->field_flag.flag_flag = 1;
    oicq_data->flag = data[offset];
    update_offset_len(1);

    check_data_len(2);
    oicq_data->field_flag.flag_version = 1;
    oicq_data->version = getdata16(data + offset);
    update_offset_len(2);

    check_data_len(2);
    oicq_data->field_flag.flag_command = 1;
    oicq_data->command = getdata16(data + offset);
    update_offset_len(2);

    check_data_len(2);
    oicq_data->field_flag.flag_sequence = 1;
    oicq_data->sequence = getdata16(data + offset);
    update_offset_len(2);

    check_data_len(4);
    oicq_data->field_flag.flag_oicq_number = 1;
    oicq_data->oicq_number = getdata32(data + offset);
    update_offset_len(4);

    return 0;
}

int oicq_dissect_fini() {
    mem_destroy_pool(oicq_poll);
    return 0;
}

int oicq_s2j(oicq_data_t *oicq_data, char **oicq_json) {
    if (!oicq_data) {
        return -1;
    }

    cJSON *json_data = NULL;
    json_data = cJSON_CreateObject();
    if (!json_data) {
    	return -1;
    }

    int i = 0;
    if (oicq_data->field_flag.flag_flag) {
        i = 0;
        while (NULL != oicq_flag_vals[i].string) {
            if (oicq_flag_vals[i].value == oicq_data->flag) {
                cJSON_AddStringToObject(json_data, "flag", oicq_flag_vals[i].string);
                break;
            }
            i++;
        }
    }
    if (oicq_data->field_flag.flag_version) {
        cJSON_AddNumberToObject(json_data, "version", oicq_data->version);
    }
    if (oicq_data->field_flag.flag_command) {
        i = 0;
        while (NULL != oicq_command_vals[i].string) {
            if (oicq_command_vals[i].value == oicq_data->command) {
                cJSON_AddStringToObject(json_data, "command", oicq_command_vals[i].string);
                break;
            }
            i++;
        }
    }
    if (oicq_data->field_flag.flag_sequence) {
        cJSON_AddNumberToObject(json_data, "sequence", oicq_data->sequence);
    }
    if (oicq_data->field_flag.flag_oicq_number) {
        cJSON_AddNumberToObject(json_data, "oicq_number", oicq_data->oicq_number);
    }

    char *pfmt = cJSON_PrintUnformatted(json_data);
	if (!pfmt) {
		cJSON_Delete(json_data);
		return -1;
	}

	cJSON_Delete(json_data);
	*oicq_json = pfmt;

    return 0;
}