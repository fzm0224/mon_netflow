#ifndef _HTTP_DISSECT_H_
#define _HTTP_DISSECT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"


#define HTTP_HEAD_MAX_NUM   128
#define HTTP_METHOD_NUM     9


typedef struct _http_method_t {
    char method[16];
    int len;
} http_method_t;


http_method_t g_method[HTTP_METHOD_NUM] = {
    {"GET", 3},
    {"HEAD", 4},
    {"POST", 4},
    {"PUT", 3},
    {"DELETE", 6},
    {"CONNECT", 7},
    {"OPTIONS", 7},
    {"TRACE", 5},
    {"PATCH", 5}
};


#endif
