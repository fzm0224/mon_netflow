#ifndef _COMMON_
#define _COMMON_


typedef short           int16_t;
typedef int             int32_t;
typedef long            int64_t;
typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
typedef unsigned long   uint64_t;


enum PACKET_DIR {
    DIR_REQUEST = 0,
    DIR_RESPONSE = 1
};


enum {
    PROTO_NOSUPPORT = 0,
    PROTO_OICQ,
    PROTO_HTTP,
    PROTO_FTP,
    PROTO_MAX = 255
};


typedef struct _net_info_t {
    enum PACKET_DIR dir;
    uint8_t smac[32];
    uint8_t dmac[32];
	uint32_t sip;
	uint32_t dip;
	uint16_t sport;
	uint16_t dport;
    uint8_t trans_proto;
    uint16_t ethernet_type;
} net_info_t;


#define check_data_len(nnn)                 \
	        do {                            \
		        if (len < (nnn)) return -1; \
	        } while(0)

#define update_offset_len(nnn)              \
	        do {                            \
		        offset += (nnn);            \
		        len -= (nnn);               \
	        } while(0)

#define getdata16_ntoh(p) ((uint16_t)                                       \
                           (uint16_t)*((const uint8_t *)(p) + 1) << 8 |     \
                           (uint16_t)*((const uint8_t *)(p) + 0) << 0)

#define getdata24_ntoh(p) ((uint32_t)*((const uint8_t *)(p) + 2) << 16 |    \
                           (uint32_t)*((const uint8_t *)(p) + 1) << 8 |     \
                           (uint32_t)*((const uint8_t *)(p) + 0) << 0)

#define getdata32_ntoh(p) ((uint32_t)                                       \
                           (uint32_t)*((const uint8_t *)(p) + 3) << 24 |    \
				           (uint32_t)*((const uint8_t *)(p) + 2) << 16 |    \
                           (uint32_t)*((const uint8_t *)(p) + 1) << 8 |     \
                           (uint32_t)*((const uint8_t *)(p) + 0) << 0)

#define getdata16(p) ((uint16_t)                                       \
                      (uint16_t)*((const uint8_t *)(p) + 0) << 8 |     \
                      (uint16_t)*((const uint8_t *)(p) + 1) << 0)

#define getdata24(p) ((uint32_t)*((const uint8_t *)(p) + 0) << 16 |    \
                      (uint32_t)*((const uint8_t *)(p) + 1) << 8  |    \
                      (uint32_t)*((const uint8_t *)(p) + 2) << 0)

#define getdata32(p) ((uint32_t)*((const uint8_t *)(p) + 0) << 24 |    \
                      (uint32_t)*((const uint8_t *)(p) + 1) << 16 |    \
                      (uint32_t)*((const uint8_t *)(p) + 2) << 8 |     \
                      (uint32_t)*((const uint8_t *)(p) + 3) << 0)


#endif