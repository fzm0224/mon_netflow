#include "libhttp.h"
#include "http_dissect.h"
#include "libmem_pool.h"
#include "common.h"
#include "cJSON.h"


mem_pool_t *http_poll;


/* tools */
void split(char *src, const char *separator, char **dest, int *num) {
	char *pNext;
	int count = 0;

	if (NULL == src || 0 == strlen(src))
		return;
	if (NULL == separator || 0 == strlen(separator))
		return;
	pNext = (char *)strtok(src, separator);
	while (pNext != NULL) {
		*dest++ = pNext;
		++count;
		if (HTTP_HEAD_MAX_NUM == count) {
			*num = HTTP_HEAD_MAX_NUM;
			return;
		}
		pNext = (char *)strtok(NULL, separator);
	}
	*num = count;
}


char *strlwr(char *str) {
    char *orign = str;
    for (; *str != '\0'; str++)
        *str = tolower(*str);
    return orign;
}


int find_http_line(uint8_t *data, int start, int end) {
	int i;
	for (i = start; i < end - 1; i++) {
		if ('\r' == data[i] && '\n' == data[i + 1]) {
			return i;
		}
	}
	return -1;
}


int find_http_head(uint8_t *data, int start, int end) {
	int i;
	for (i = start; i < end - 3; i++) {
		if ('\r' == data[i] && '\n' == data[i + 1] && '\r' == data[i + 2] && '\n' == data[i + 3]) {
			return i;
		}
	}
	return -1;
}


int get_content_length(char *data, int index) {
	int content_len = 0;
	char *tmp_buffer = (char *)malloc(index + 1);
	if (!tmp_buffer) {
		return -1;
	}
	memset(tmp_buffer, 0, index + 1);
	memcpy(tmp_buffer, data, index);

	int num = 0;
	int i = 0;
	char *str[HTTP_HEAD_MAX_NUM] = {0};
	char *str_content_len = NULL;

	split(tmp_buffer, "\r\n", str, &num);

	for (i = 0; i < num; i++) {
		if (NULL != str[i]) {
			if (0 == strncasecmp(str[i], "Content-length:", 15)) {
				str[i] += 15;
				str[i] += strspn(str[i], " \t");
				str_content_len = str[i];

				content_len = atoi(str_content_len);
				break;
			}
		}
	}

	free(tmp_buffer);
	tmp_buffer = NULL;

	return content_len;
}


int check_method(uint8_t *method) {
	int i = 0;
	for (i = 0; i < HTTP_METHOD_NUM; i++) {
		if (0 == strncasecmp(method, g_method[i].method, g_method[i].len)) {
			return 0;
		}
	}
	return -1;
}


int check_version(uint8_t *version) {
	if (0 == strncasecmp(version, "http", 4)) {
		return 0;
	}
	return -1;
}



int http_segment_check(char *data, int len) {
    int index = find_http_head(data, 0, len);
	// not http package
	if (-1 == index) {
		return -1;
	}
	int content_len = get_content_length(data, index);
	if (content_len >= 0) {
		return index + 4 + content_len;
	}
	return -1;
}


http_data_t *http_dissect_init() {
    mem_test_null(http_poll, mem_create_pool(MEM_DEFAULT_POOL_SIZE), NULL);

    http_data_t *http_data = mem_palloc(http_poll, sizeof(http_data_t));
    if (!http_data) {
        return NULL;
    }
    return http_data;
}


/* http request dissect */
int dissect_request_head(http_data_t *http_data, uint8_t *head) {
	int ret = 0;
	uint8_t *host = NULL;
	int host_len = 0;
	uint8_t *user_agent = NULL;
	int user_agent_len = 0;
	uint8_t *referer = NULL;
	int referer_len = 0;

	if (0 == strncasecmp(head, "Host:", 5)) {
		head += 5;
		head += strspn(head, " \t");
		host = head;
		host_len = strlen(host);

		http_data->req.req_host = (uint8_t *)mem_palloc(http_poll, host_len + 1);
		if (!http_data->req.req_host) {
			return -1;
		}
		memset(http_data->req.req_host, 0, host_len + 1);
		strncpy(http_data->req.req_host, host, host_len);
		http_data->req.req_flag.have_host = 1;
	}
	else if (0 == strncasecmp(head, "User-Agent:", 11)) {
		head += 11;
		head += strspn(head, " \t");
		user_agent = head;
		user_agent_len = strlen(user_agent);

		http_data->req.req_user_agent = (uint8_t *)mem_palloc(http_poll, user_agent_len + 1);
		if (!http_data->req.req_user_agent) {
			return -1;
		}
		memset(http_data->req.req_user_agent, 0, user_agent_len + 1);
		strncpy(http_data->req.req_user_agent, user_agent, user_agent_len);
		http_data->req.req_flag.have_user_agent = 1;
	}
	else if (0 == strncasecmp(head, "Referer:", 8)) {
		head += 8;
		head += strspn(head, " \t");
		referer = head;
		referer_len = strlen(referer);

		http_data->req.req_referer = (uint8_t *)mem_palloc(http_poll, referer_len + 1);
		if (!http_data->req.req_referer) {
			return -1;
		}
		memset(http_data->req.req_referer, 0, referer_len + 1);
		strncpy(http_data->req.req_referer, referer, referer_len);
		http_data->req.req_flag.have_referer = 1;
	}
	else {
		ret = -1;
	}
	return ret;
}


int dissect_http_request_line(http_data_t *http_data, uint8_t *req_line) {
	int ret = 0;
	uint8_t *uri = NULL;
	int uri_len = 0;
	uint8_t *version = NULL;
	uint8_t *method = NULL;

	// method
	uri = strpbrk(req_line, " \t");
	if (!uri) {
		return -1;
	}
	*uri++ = '\0';
	method = req_line;
	ret = check_method(method);
	if (-1 == ret) {
		return -1;
	}
	strncpy(http_data->req.req_method, method, strlen(method));
	http_data->req.req_flag.have_method = 1;

	// uri
	uri += strspn(uri, " \t");
	version = strpbrk(uri, " \t");
	if (!version) {
		return -1;
	}
	*version++ = '\0';
	uri_len = strlen(uri);
	http_data->req.req_uri = (uint8_t *)mem_palloc(http_poll, uri_len + 1);
	if (!http_data->req.req_uri) {
		return -1;
	}
	memset(http_data->req.req_uri, 0, uri_len + 1);
	strncpy(http_data->req.req_uri, uri, uri_len);
	http_data->req.req_flag.have_uri = 1;

	// version
	version += strspn(version, " \t");
	strncpy(http_data->req.req_http_version, version, strlen(version));
	http_data->req.req_flag.have_version = 1;

	return ret;
}


int dissect_http_request_head(http_data_t *http_data, uint8_t *req_head) {
	int num = 0;
	int i = 0;
	char *req_head_str[HTTP_HEAD_MAX_NUM] = {0};

	split(req_head, "\r\n", req_head_str, &num);

	for (i = 0; i < num; i++) {
		if (req_head_str[i] != NULL) {
			dissect_request_head(http_data, req_head_str[i]);
		}
		else {
			break;
		}
	}

	return 0;
}


int dissect_http_request(http_data_t *http_data, uint8_t *data, uint32_t len, net_info_t *net_info) {
	int ret = 0;
	int offset = 0;
	int index = 0;
	int tmp_len = 0;
	uint8_t *tmp_req_line = NULL;
	uint8_t *tmp_req_head = NULL;

	////// http request line
	index = find_http_line(data, offset, len);
	if (-1 == index) {
		return -1;
	}

	tmp_len = index - offset;
	if (0 >= tmp_len) {
		return -1;
	}

	tmp_req_line = (uint8_t *)mem_palloc(http_poll, tmp_len + 1);
	if (!tmp_req_line) {
		return -1;
	}
	memset(tmp_req_line, 0, tmp_len + 1);
	strncpy(tmp_req_line, data + offset, tmp_len);
	
	ret = dissect_http_request_line(http_data, tmp_req_line);
	if (-1 == ret) {
		return -1;
	}

	offset += index + 2;	// skip http request line and \r\n

	////// http request head
	index = find_http_head(data, offset, len);
	tmp_len = index - offset;

	tmp_req_head = (uint8_t *)mem_palloc(http_poll, tmp_len + 1);
	if (!tmp_req_head) {
		return -1;
	}
	memset(tmp_req_head, 0, tmp_len + 1);
	strncpy(tmp_req_head, data + offset, tmp_len);

	ret = dissect_http_request_head(http_data, tmp_req_head);
	if (-1 == ret) {
		return -1;
	}

	offset += index + 4;	// skip http request head and \r\n\r\n

	////// http request connect
	// ...

	return ret;
}


/* http response dissect */
int dissect_response_head(http_data_t *http_data, uint8_t *head) {
	int ret = 0;
	uint8_t *server = NULL;
	int server_len = 0;
	uint8_t *content_type = NULL;
	int content_type_len = 0;
	uint8_t *content_len = NULL;

	if (0 == strncasecmp(head, "Server:", 7)) {
		head += 7;
		head += strspn(head, " \t");
		server = head;
		server_len = strlen(server);

		http_data->resp.resp_server = (uint8_t *)mem_palloc(http_poll, server_len + 1);
		if (!http_data->resp.resp_server) {
			return -1;
		}
		memset(http_data->resp.resp_server, 0, server_len + 1);
		strncpy(http_data->resp.resp_server, server, server_len);
		http_data->resp.resp_flag.have_server = 1;
	}
	else if (0 == strncasecmp(head, "Content-length:", 15)) {
		head += 15;
		head += strspn(head, " \t");
		content_len = head;

		http_data->resp.resp_content_len = atoi(content_len);
		http_data->resp.resp_flag.have_content_len = 1;
	}
	else if (0 == strncasecmp(head, "Content-type:", 13)) {
		head += 13;
		head += strspn(head, " \t");
		content_type = head;
		content_type_len = strlen(content_type);

		http_data->resp.resp_content_type = (uint8_t *)mem_palloc(http_poll, content_type_len + 1);
		if (!http_data->resp.resp_content_type) {
			return -1;
		}
		memset(http_data->resp.resp_content_type, 0, content_type_len + 1);
		strncpy(http_data->resp.resp_content_type, content_type, content_type_len);
		http_data->resp.resp_flag.have_content_type = 1;
	}
	else {
		ret = -1;
	}
	return ret;
}


int dissect_http_response_line(http_data_t *http_data, uint8_t *resp_line) {
	int ret = 0;
	uint8_t *version = NULL;
	uint8_t *status_code = NULL;
	uint8_t *status_string = NULL;
	int status_string_len = 0;

	// version
	status_code = strpbrk(resp_line, " \t");
	if (!status_code) {
		return -1;
	}
	*status_code++ = '\0';
	version = resp_line;
	ret = check_version(version);
	if (-1 == ret) {
		return -1;
	}
	strncpy(http_data->resp.resp_version, version, strlen(version));
	http_data->resp.resp_flag.have_version = 1;

	// status_code
	status_code += strspn(status_code, " \t");
	status_string = strpbrk(status_code, " \t");
	if (!status_string) {
		return -1;
	}
	*status_string++ = '\0';
	strncpy(http_data->resp.resp_status_code, status_code, strlen(status_code));
	http_data->resp.resp_flag.have_status_code = 1;

	// status_string
	status_string += strspn(status_string, " \t");
	status_string_len = strlen(status_string);
	http_data->resp.resp_status_string = (uint8_t *)mem_palloc(http_poll, status_string_len);
	if (!http_data->resp.resp_status_string) {
		return -1;
	}
	memset(http_data->resp.resp_status_string, 0, status_string_len + 1);
	strncpy(http_data->resp.resp_status_string, status_string, status_string_len);
	http_data->resp.resp_flag.have_status_string = 1;

	return ret;
}


int dissect_http_response_head(http_data_t *http_data, uint8_t *resp_head) {
	int num = 0;
	int i = 0;
	char *resp_head_str[HTTP_HEAD_MAX_NUM] = {0};

	split(resp_head, "\r\n", resp_head_str, &num);

	for (i = 0; i < num; i++) {
		if (resp_head_str[i] != NULL) {
			dissect_response_head(http_data, resp_head_str[i]);
		}
		else {
			break;
		}
	}

	return 0;
}


int dissect_http_response(http_data_t *http_data, uint8_t *data, uint32_t len, net_info_t *net_info) {
	int ret = 0;
	int offset = 0;
	int index = 0;
	int tmp_len = 0;
	uint8_t *tmp_resp_line = NULL;
	uint8_t *tmp_resp_head = NULL;

	////// http response line
	index = find_http_line(data, offset, len);
	if (-1 == index) {
		return -1;
	}

	tmp_len = index - offset;
	if (0 >= tmp_len) {
		return -1;
	}

	tmp_resp_line = (uint8_t *)mem_palloc(http_poll, tmp_len + 1);
	if (!tmp_resp_line) {
		return -1;
	}
	memset(tmp_resp_line, 0, tmp_len + 1);
	strncpy(tmp_resp_line, data + offset, tmp_len);
	
	ret = dissect_http_response_line(http_data, tmp_resp_line);
	if (-1 == ret) {
		return -1;
	}

	offset += index + 2;	// skip http response line and \r\n

	////// http response head
	index = find_http_head(data, offset, len);
	tmp_len = index - offset;

	tmp_resp_head = (uint8_t *)mem_palloc(http_poll, tmp_len + 1);
	if (!tmp_resp_head) {
		return -1;
	}
	memset(tmp_resp_head, 0, tmp_len + 1);
	strncpy(tmp_resp_head, data + offset, tmp_len);

	ret = dissect_http_response_head(http_data, tmp_resp_head);
	if (-1 == ret) {
		return -1;
	}

	offset += index + 4;	// skip http response head and \r\n\r\n

	////// http response connect
	// ...

	return ret;
}


int http_dissect_data(http_data_t *http_data, char *data, int len, net_info_t *net_info) {
	if (!http_data || !data || len <= 0) {
        return -1;
    }

	uint8_t *tmp_data;
	uint32_t tmp_len;
	int ret = 0;

	tmp_data = (uint8_t *)mem_palloc(http_poll, len);
	if (!tmp_data) {
		return -1;
	}
	memset(tmp_data, 0, len);
	memcpy(tmp_data, data, len);
	tmp_len = (uint32_t)len;

	// http request
	if (DIR_REQUEST == net_info->dir) {
		http_data->dir = DIR_REQUEST;
		ret = dissect_http_request(http_data, tmp_data, tmp_len, net_info);
		if (ret < 0) {
			ret = -1;
		}
	}
	// http response
	else if (DIR_RESPONSE == net_info->dir) {
		http_data->dir = DIR_RESPONSE;
		ret = dissect_http_response(http_data, tmp_data, tmp_len, net_info);
		if (ret < 0) {
			ret = -1;
		}
	}

	return ret;
}


int http_dissect_fini() {
    mem_destroy_pool(http_poll);
    return 0;
}


int http_s2j(http_data_t *http_data, char **http_json) {
    if (!http_data) {
        return -1;
    }

    cJSON *json_data = NULL;
    json_data = cJSON_CreateObject();
    if (NULL == json_data) {
    	return -1;
    }

    if (DIR_REQUEST == http_data->dir) {
    	if (http_data->req.req_flag.have_method)
    		cJSON_AddStringToObject(json_data, "method", http_data->req.req_method);
    	if (http_data->req.req_flag.have_uri)
    		cJSON_AddStringToObject(json_data, "uri", http_data->req.req_uri);
    	if (http_data->req.req_flag.have_version)
    		cJSON_AddStringToObject(json_data, "version", http_data->req.req_http_version);
    	if (http_data->req.req_flag.have_host)
    		cJSON_AddStringToObject(json_data, "host", http_data->req.req_host);
    	if (http_data->req.req_flag.have_user_agent)
    		cJSON_AddStringToObject(json_data, "user-agent", http_data->req.req_user_agent);
    	if (http_data->req.req_flag.have_referer)
    		cJSON_AddStringToObject(json_data, "referer", http_data->req.req_referer);
    }
    else if (DIR_RESPONSE == http_data->dir) {
    	if (http_data->resp.resp_flag.have_version)
    		cJSON_AddStringToObject(json_data, "version", http_data->resp.resp_version);
    	if (http_data->resp.resp_flag.have_status_code)
    		cJSON_AddStringToObject(json_data, "status_code", http_data->resp.resp_status_code);
    	if (http_data->resp.resp_flag.have_status_string)
    		cJSON_AddStringToObject(json_data, "status_string", http_data->resp.resp_status_string);
    	if (http_data->resp.resp_flag.have_server)
    		cJSON_AddStringToObject(json_data, "server", http_data->resp.resp_server);
    	if (http_data->resp.resp_flag.have_content_len)
    		cJSON_AddNumberToObject(json_data, "content-length", http_data->resp.resp_content_len);
    	if (http_data->resp.resp_flag.have_content_type)
    		cJSON_AddStringToObject(json_data, "content-type", http_data->resp.resp_content_type);
    }

    char *pp = cJSON_PrintUnformatted(json_data);
	if (NULL == pp) {
		cJSON_Delete(json_data);
		return -1;
	}

	cJSON_Delete(json_data);
	*http_json = pp;

	return 0;
}