#include <dlfcn.h>

#include "libdissect_engine.h"
#include "dissect_engine.h"
#include "cJSON.h"
#include "libmem_pool.h"
#include "log.h"



/****************************
 *      variable
 ****************************/
mem_pool_t *dissect_engine_poll;


static lib_func_info_t func_info_list[] = {
    {"%s_segment_check", FUNC_FLAG_SEGMENT_CHECK, FUNC_SEGMENT_CHECK}, 
    {"%s_dissect_init", FUNC_FLAG_DISSECT_INIT, FUNC_DISSECT_INIT}, 
    {"%s_dissect_data", FUNC_FLAG_DISSECT_DATA, FUNC_DISSECT_DATA}, 
    {"%s_dissect_fini", FUNC_FLAG_DISSECT_FINI, FUNC_DISSECT_FINI}, 
    {"%s_s2j", FUNC_FLAG_S2J, FUNC_S2J}, 
    {NULL, 0, 0}
};


static proto_key_value_t proto_map[] = {
    {NULL, PROTO_NOSUPPORT},
    {"oicq", PROTO_OICQ},
    {"http", PROTO_HTTP},
    {"ftp", PROTO_FTP},
    {NULL, PROTO_MAX}
};

lib_info_t *lib_info_map[MAX_PROTO_NUM];



/****************************
 *      function
 ****************************/
int get_lib_info_index(char *proto_name) {
    int i = 0;
    for (i = PROTO_NOSUPPORT + 1; i < PROTO_MAX; i++) {
        if (0 == strcmp(proto_name, proto_map[i].proto_name)) {
            return proto_map[i].proto_index;
        }
    }
    return 0;
}


void ipv4_to_str(uint32_t ip_value, char *ip_addr) {
    sprintf(ip_addr, "%d.%d.%d.%d",
			(ip_value & 0x000000ff),
            (ip_value >> 8)  & 0x000000ff,
            (ip_value >> 16) & 0x000000ff,
            (ip_value >> 24) & 0x000000ff);
}


lib_info_t* load_lib(const char *proto_name, const char *lib_file_path, const char *func_name) {
    lib_info_t* lib_info = (lib_info_t *)mem_palloc(dissect_engine_poll, sizeof(lib_info_t));
	if (!lib_info) {
		return NULL;
	}
	memset(lib_info, 0, sizeof(lib_info_t));
	
    void *handle;
	char *error; 
    const char * tmp_lib_file_path = lib_file_path;
	handle = dlopen(lib_file_path, RTLD_NOW);
	if (!handle) {
	    LYF_LOG(FLAG_ERROR, "%s", dlerror());
        return NULL;
	}
	dlerror();

    int i = 0;
    while (func_info_list[i].func_name) {
        int func_name_style_len =  strlen(func_info_list[i].func_name) + strlen(func_name);
        char *func_name_style = (char *)mem_palloc(dissect_engine_poll, func_name_style_len);
        memset(func_name_style, 0, func_name_style_len);
        snprintf(func_name_style, func_name_style_len, func_info_list[i].func_name, func_name);
        LYF_LOG(FLAG_DEBUG, "func_name_style = %s, func_index = %d, func_flag = %d", func_name_style, func_info_list[i].func_index, func_info_list[i].func_flag);

        void* tmp_handler = dlsym(handle, func_name_style);
        if (NULL == tmp_handler) {
            i++;
            continue;
        }

        // register func
        switch(func_info_list[i].func_index) {
            case FUNC_SEGMENT_CHECK: 
                lib_info->proto_segment_check = tmp_handler;
                break;
            case FUNC_DISSECT_INIT:
                lib_info->proto_dissect_init = tmp_handler;
                break;
            case FUNC_DISSECT_DATA:
                lib_info->proto_dissect_data = tmp_handler;
                break;
            case FUNC_DISSECT_FINI:
                lib_info->proto_dissect_fini = tmp_handler;
                break;
            case FUNC_S2J:
                lib_info->proto_s2j = tmp_handler;
                break;
        }

        lib_info->lib_func_flag |= func_info_list[i].func_flag;
        i++;
    }
    lib_info->proto_name = (char *)proto_name;
    
	error = dlerror();
	if (NULL != error) {
	    LYF_LOG(FLAG_ERROR, "Cannot load lib symbol: %s", error);
        return NULL;
	}

    return lib_info;
}


int read_config(char *config_path, char *config_buf) {
    FILE *fp = fopen(config_path, "r");
    if (NULL == fp) {   
        LYF_LOG(FLAG_ERROR, "open config file fail!");
        return -1; 
    }   

    int i = 0;
    char ch;
    while (EOF != (ch = fgetc(fp))) {
        if (i >= MAX_CONFIG_STR_LEN) {
            LYF_LOG(FLAG_ERROR, "config data too long!");
            return -1;
        }
        config_buf[i++] = ch;
    }
    return 0;
}


int load_config(char *config_file_path, char *lib_dir_path) {
    if (!config_file_path) {
        return -1;
    }

    char config_buf[MAX_CONFIG_STR_LEN] = {0};
    if (-1 == read_config(config_file_path, config_buf)) {
        return -1;
    }

    int i = 0;
    cJSON *config_root = cJSON_Parse(config_buf);
    for(i = 0; i < cJSON_GetArraySize(config_root); i++) {
        cJSON *item = cJSON_GetArrayItem(config_root, i);
        if(item != NULL) {
            char *proto_name = NULL, *lib_name = NULL, *func_name = NULL;
            if(cJSON_GetObjectItem(item, "proto_name") != NULL)
                proto_name = cJSON_GetObjectItem(item, "proto_name")->valuestring; 
            if(cJSON_GetObjectItem(item, "lib_name") != NULL)
                lib_name = cJSON_GetObjectItem(item, "lib_name")->valuestring;
            if(cJSON_GetObjectItem(item, "func_name") != NULL)
                func_name =cJSON_GetObjectItem(item, "func_name")->valuestring;
        
            char *lib_file_path = mem_palloc(dissect_engine_poll, strlen(lib_dir_path) + strlen(lib_name) + 2);
            memset(lib_file_path, 0, strlen(lib_dir_path) + strlen(lib_name) + 2);

            strncpy(lib_file_path, lib_dir_path ,strlen(lib_dir_path));
            strncat(lib_file_path, "/", 1);
            strncat(lib_file_path, lib_name, strlen(lib_name));

            lib_info_t* lib_info = load_lib(proto_name, lib_file_path, func_name);
            if (NULL == lib_info){
                continue;
            }

            lib_info_map[get_lib_info_index(func_name)] = lib_info;
        }
    }

    cJSON_Delete(config_root);
	if (!config_root) {
		return -1;
	}

    return 0;
}


int dissect_engine_init(char *config_file_path, char *lib_dir_path) {
    mem_test_null(dissect_engine_poll, mem_create_pool(MEM_DEFAULT_POOL_SIZE), -1);

    if (-1 == load_config(config_file_path, lib_dir_path)) {
        return -1;
    }
    return 0;
}


int dissect_engine_fini() {
    mem_destroy_pool(dissect_engine_poll);
    return 0;
}


int dissect_net_layer234(const uint8_t *packet, net_info_t *net_info) {
    LYF_LOG(FLAG_DEBUG, "========== frame info ==========");

    int payload_len = 0, udp_len = 0;
	uint32_t offset = 0;
    struct ethhdr *eth = (struct ethhdr *)packet;
    
    offset = sizeof(struct ethhdr);
    net_info->ethernet_type  = ntohs(eth->h_proto);

	memcpy(net_info->dmac, eth->h_dest, sizeof(eth->h_dest));
    memcpy(net_info->smac, eth->h_source, sizeof(eth->h_source));

    // VLAN    
    int CURRENT_ETH_LEN = 14;
    while (ETH_P_8021Q == net_info->ethernet_type) {
        net_info->ethernet_type = (packet[offset + 2] << 8) + packet[offset + 3];
        offset += 4;
        CURRENT_ETH_LEN += 4;
    }  

    if (ETH_P_IP != net_info->ethernet_type) {
        return -1;
    }

    struct iphdr *ip = (struct iphdr *)(packet + offset);
    net_info->ethernet_type = ntohs(ip->protocol);
    offset += sizeof(struct iphdr);
    struct tcphdr *tcp = (struct tcphdr *)(packet + offset);

    /* udp header */
    if(ip->protocol == IPPROTO_UDP) {
        udp_len = (packet[offset+4] << 8) + packet[offset+5];
        offset +=8;
        payload_len = udp_len - 8;
		net_info->trans_proto = IPPROTO_UDP;
        LYF_LOG(FLAG_DEBUG, "%s", "UDP");
    }
    /* tcp header */
    else if(ip->protocol == IPPROTO_TCP){
        offset += (tcp->doff << 2);
        payload_len = ntohs(ip->tot_len) + CURRENT_ETH_LEN - offset;
		net_info->trans_proto = IPPROTO_TCP;
        LYF_LOG(FLAG_DEBUG, "%s", "TCP");
    }

    net_info->sip = ip->saddr;
    net_info->dip = ip->daddr;
    net_info->sport = ntohs(tcp->source);
    net_info->dport = ntohs(tcp->dest);

    char buf[32];
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x", net_info->smac[0], net_info->smac[1],
            net_info->smac[2], net_info->smac[3], net_info->smac[4], net_info->smac[5]);
    LYF_LOG(FLAG_DEBUG, "smac: %s", buf);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x", net_info->dmac[0], net_info->dmac[1],
            net_info->dmac[2], net_info->dmac[3], net_info->dmac[4], net_info->dmac[5]);
    LYF_LOG(FLAG_DEBUG, "dmac: %s", buf);
    
    memset(buf, 0, sizeof(buf));
    ipv4_to_str(net_info->sip, buf);
    LYF_LOG(FLAG_DEBUG, "sip: %s", buf);
    
    memset(buf, 0, sizeof(buf));
    ipv4_to_str(net_info->dip, buf);
    LYF_LOG(FLAG_DEBUG, "dip: %s", buf);

    LYF_LOG(FLAG_DEBUG, "sport: %d", net_info->sport);
    LYF_LOG(FLAG_DEBUG, "dport: %d", net_info->dport);

	return offset;
}


int dissect_net_layerapp_frame(const uint8_t *data, int len, int proto_id, net_info_t *net_info) {
    lib_info_t *lib_info = lib_info_map[proto_id];
    if (!lib_info) {
        LYF_LOG(FLAG_ERROR, "lib_info is NULL!\n");
        return -1;
    }

    int ret = 0;
    void *proto_data = NULL;
    if (lib_info->lib_func_flag & FUNC_FLAG_DISSECT_INIT) {
        proto_data = lib_info->proto_dissect_init();
        if (!proto_data) {
            LYF_LOG(FLAG_ERROR, "proto_data is NULL!\n");
            return -1;
        }
    }

    if (lib_info->lib_func_flag & FUNC_FLAG_DISSECT_DATA) {
        ret = lib_info->proto_dissect_data(proto_data, (char *)data, len, net_info);
        if (0 == ret) {
            char* json_str = NULL;
            if (lib_info->lib_func_flag & FUNC_FLAG_S2J) {
                ret = lib_info->proto_s2j(proto_data, &json_str);
                if (0 == ret) {
                    LYF_LOG(FLAG_INFO, "dissect result: %s\n", json_str);
                    free(json_str);
                }
            }
        }
    }
    
    if (lib_info->lib_func_flag & FUNC_FLAG_DISSECT_FINI) {
        lib_info->proto_dissect_fini();
    }
    
    return 0;
}


int dissect_net_layerapp_stream(const uint8_t *data, int len, int proto_id) {

    return 0;
}