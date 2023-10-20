#include "libproto_identify.h"
#include "proto_identify.h"



int identify_oicq(const uint8_t *data, int len, net_info_t *net_info) {
    if (IPPROTO_UDP == net_info->trans_proto) {
        if (8000 == net_info->dport) {
            net_info->dir = DIR_REQUEST;
            return 1;
        } else if (8000 == net_info->sport) {
            net_info->dir = DIR_RESPONSE;
            return 1;
        }
    }
    return 0;
}


int identify_http(const uint8_t *data, int len, net_info_t *net_info) {
    if (IPPROTO_TCP == net_info->trans_proto) {
        if (80 == net_info->dport) {
            net_info->dir = DIR_REQUEST;
            return 1;
        } else if (80 == net_info->sport) {
            net_info->dir = DIR_RESPONSE;
            return 1;
        }

        if ('g' == tolower(data[0]) && 'e' == tolower(data[1]) 
                && 't' == tolower(data[2])) {
            net_info->dir = DIR_REQUEST;
            return 1;
        } else if ('p' == tolower(data[0]) && 'o' == tolower(data[1]) 
                && 's' == tolower(data[2]) && 't' == tolower(data[3])) {
            net_info->dir = DIR_REQUEST;
            return 1;
        } else if ('h' == tolower(data[0]) && 'e' == tolower(data[1]) 
                && 'a' == tolower(data[2]) && 'd' == tolower(data[3])) {
            net_info->dir = DIR_REQUEST;
            return 1;
        } else if ('p' == tolower(data[0]) && 'u' == tolower(data[1]) 
                && 't' == tolower(data[2])) {
            net_info->dir = DIR_REQUEST;
            return 1;
        } else if ('d' == tolower(data[0]) && 'e' == tolower(data[1]) && 'l' == tolower(data[2])
                && 'e' == tolower(data[3]) && 't' == tolower(data[4]) && 'e' == tolower(data[5])) {
            net_info->dir = DIR_REQUEST;
            return 1;
        } else if ('c' == tolower(data[0]) && 'o' == tolower(data[1]) && 'n' == tolower(data[2])
                && 'n' == tolower(data[3]) && 'e' == tolower(data[4]) && 'c' == tolower(data[5])
                && 't' == tolower(data[5])) {
            net_info->dir = DIR_REQUEST;
            return 1;
        } else if ('o' == tolower(data[0]) && 'p' == tolower(data[1]) && 't' == tolower(data[2])
                && 'i' == tolower(data[3]) && 'o' == tolower(data[4]) && 'n' == tolower(data[5])
                && 's' == tolower(data[5])) {
            net_info->dir = DIR_REQUEST;
            return 1;
        } else if ('t' == tolower(data[0]) && 'r' == tolower(data[1]) && 'a' == tolower(data[2])
                && 'c' == tolower(data[3]) && 'e' == tolower(data[4])) {
            net_info->dir = DIR_REQUEST;
            return 1;
        } else if ('p' == tolower(data[0]) && 'a' == tolower(data[1]) && 't' == tolower(data[2])
                && 'c' == tolower(data[3]) && 'h' == tolower(data[4])) {
            net_info->dir = DIR_REQUEST;
            return 1;
        }

        if ('h' == tolower(data[0]) && 't' == tolower(data[1])
                 && 't' == tolower(data[2]) && 'p' == tolower(data[3])) {
            net_info->dir = DIR_RESPONSE;
            return 1;
        }
    }
    return 0;
}


int identify_ftp(const uint8_t *data, int len, net_info_t *net_info) {
    if (IPPROTO_TCP == net_info->trans_proto) {
        if (20 == net_info->dport || 21 == net_info->dport) {
            net_info->dir = DIR_REQUEST;
            return 1;
        }
        if (20 == net_info->sport || 21 == net_info->sport) {
            net_info->dir = DIR_RESPONSE;
            return 1;
        }
    }
    return 0;
}


int dissect_proto_identify(const uint8_t *data, int len, net_info_t *net_info) {
    if (1 == identify_oicq(data, len, net_info)) {
        return PROTO_OICQ;
    } else if (1 == identify_http(data, len, net_info)) {
        return PROTO_HTTP;
    } else if (1 == identify_ftp(data, len, net_info)) {
        return PROTO_FTP;
    }
    return 0;
}