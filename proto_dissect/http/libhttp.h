#ifndef _LIB_HTTP_H_
#define _LIB_HTTP_H_



#include <stdint.h>
#include <linux/if_ether.h>

#include "common.h"


////// http request
typedef struct _http_request_field_flag_t {
    uint32_t have_method:1,
            have_uri:1,
            have_version:1,
			have_host:1,
			have_user_agent:1,
			have_referer:1;
}http_request_field_flag_t;

typedef struct _http_request_data_t {
	http_request_field_flag_t req_flag;
	
	// http request line
	char 		req_method[16];
	char 		*req_uri;
	char 		req_http_version[16];
	
	// http request head
	char 		*req_host;
	char 		*req_user_agent;
	char 		*req_referer;
} http_request_data_t;


////// http response
typedef struct _http_response_field_flag {
    uint32_t have_version:1,
            have_status_code:1,
            have_status_string:1,
			have_server:1,
			have_content_len:1,
			have_content_type:1;
} http_response_field_flag;

typedef struct _http_response_data_t {
	http_response_field_flag resp_flag;
	
	// http response line
	char 		resp_version[16];
	char   		resp_status_code[8];
	char 		*resp_status_string;
	
	// http response head
	char 		*resp_server;
	uint32_t  	resp_content_len;
	char 		*resp_content_type;
} http_response_data_t;

typedef struct _http_data_t {
	enum PACKET_DIR 		dir;
	http_request_data_t 	req;
	http_response_data_t 	resp;
} http_data_t;



int http_segment_check(char *data, int len);
http_data_t *http_dissect_init();
int http_dissect_data(http_data_t *http_data, char *data, int len, net_info_t *net_info);
int http_dissect_fini();
int http_s2j(http_data_t *http_data, char **http_json);


#endif
