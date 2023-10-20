#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <pcap.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#include "libdissect_engine.h"
#include "libproto_identify.h"
#include "common.h"
#include "log.h"


#define MAX_CMD_ARG_INTERFACE   32
#define MAX_CMD_ARG_PCAP        128
#define MAX_CMD_ARG_FILTER      256
#define MAX_CMD_ARG_BPF         128


typedef struct _cmd_arg_flag_t {
    uint8_t flag_pcap:1,
            flag_interface:1,
            flag_filter:1,
            flag_bpf:1;
} cmd_arg_flag_t;


typedef struct _cmd_arg_t {
    cmd_arg_flag_t arg_flag;
    uint8_t arg_pcap[MAX_CMD_ARG_PCAP];
    uint8_t arg_interface[MAX_CMD_ARG_INTERFACE];
    uint8_t arg_filter[MAX_CMD_ARG_FILTER];
    uint8_t arg_bpf[MAX_CMD_ARG_BPF];
} cmd_arg_t;


void help() {
    printf("--pcap, -p: [arg] pcap file.\n");
    printf("--interface, -i: [arg] network interface.\n");
    printf("--filter, -f: [arg] filter expression.\n");
    printf("--bpf, -b: [arg] bpf expression.\n");
    printf("--version, -v: version.\n");
    printf("--help, -h: help info.\n");
}


void pcap_loop_data_handler(uint8_t *userarg, const struct pcap_pkthdr *pkthdr, const uint8_t *packet) {
    LYF_LOG(FLAG_DEBUG, "===== pcap_loop_data_handler =====");

    if(!pkthdr || !packet) {
		return;
	}

	net_info_t net_info;
	int offset = dissect_net_layer234(packet, &net_info);
    int proto_id = dissect_proto_identify(packet + offset, pkthdr->caplen - offset, &net_info);
    if (proto_id <= 0) {
        return ;
    }
	dissect_net_layerapp_frame(packet + offset, pkthdr->caplen - offset, proto_id, &net_info);
}


int main(int argc, char **argv) {
    int ret = 0;
    const char *optstring = "p:i:f:b:vh";
    int c, index;
    struct option opts[] = { 
        {"pcap", required_argument, NULL, 'p'}, 
        {"interface", required_argument, NULL, 'i'}, 
        {"filter", required_argument, NULL, 'f'}, 
        {"bpf", required_argument, NULL, 'b'}, 
        {"version", no_argument, NULL, 'v'}, 
        {"help", no_argument, NULL, 'h'},
        {0, 0, 0, 0}
    };  
    cmd_arg_t cmd_arg;

    while(-1 != (c = getopt_long_only(argc, argv, optstring, opts, &index))) {   
        switch(c) {
            case 'p':
                cmd_arg.arg_flag.flag_pcap = 1;
                if (MAX_CMD_ARG_PCAP <= strlen(optarg)) {
                    printf("[Error]: cmd arg pcap len too long!");
                    return -1;
                }
                strcpy(cmd_arg.arg_pcap, optarg);
                LYF_LOG(FLAG_DEBUG, "pcap is %s", cmd_arg.arg_pcap);
                break;
            case 'i':
                cmd_arg.arg_flag.flag_interface = 1;
                if (MAX_CMD_ARG_INTERFACE <= strlen(optarg)) {
                    printf("[Error]: cmd arg interface len too long!");
                    return -1;
                }
                strcpy(cmd_arg.arg_interface, optarg);
                LYF_LOG(FLAG_DEBUG, "interface is %s", cmd_arg.arg_interface);
                break;
            case 'f':
                cmd_arg.arg_flag.flag_filter = 1;
                if (MAX_CMD_ARG_FILTER <= strlen(optarg)) {
                    printf("[Error]: cmd arg filter len too long!");
                    return -1;
                }
                strcpy(cmd_arg.arg_filter, optarg);
                LYF_LOG(FLAG_DEBUG, "filter is %s", cmd_arg.arg_filter);
                break;
            case 'b':
                cmd_arg.arg_flag.flag_bpf = 1;
                if (MAX_CMD_ARG_BPF <= strlen(optarg)) {
                    printf("[Error]: cmd arg bpf len too long!");
                    return -1;
                }
                strcpy(cmd_arg.arg_bpf, optarg);
                LYF_LOG(FLAG_DEBUG, "bpf is %s", cmd_arg.arg_bpf);
                break;
            case 'v':
                printf("version is 0.0.1\n");
                return 0;
            case 'h':
                help();
                return 0;
            default:
                return 0;
        }
    }

    pcap_t *pcap_hander = NULL;
    char errbuf[PCAP_ERRBUF_SIZE];
    struct bpf_program fp;
    bpf_u_int32 mask;
    bpf_u_int32 net = 0;;
    struct pcap_pkthdr header;
    const uint8_t *packet;

    if (1 == cmd_arg.arg_flag.flag_interface) {
        if (-1 == pcap_lookupnet(cmd_arg.arg_interface, &net, &mask, errbuf)) {
            printf("[Error]: Couldn't get netmask for device %s: %s\n", cmd_arg.arg_interface, errbuf);
            net = 0;
            mask = 0;
        }
        pcap_hander = pcap_open_live(cmd_arg.arg_interface, BUFSIZ, 1, 1000, errbuf);
    }

    if (1 == cmd_arg.arg_flag.flag_pcap) {
        pcap_hander = pcap_open_offline(cmd_arg.arg_pcap, errbuf);
    }

    if (NULL == pcap_hander) {
        return -1;
    }

    dissect_engine_init("./release/config/proto_list.json", "./release/so");

    if (1 == cmd_arg.arg_flag.flag_bpf) {
        if (-1 == pcap_compile(pcap_hander, &fp, cmd_arg.arg_bpf, 0, net)) {
            printf("[Error]: Couldn't parse bpf %s: %s\n", cmd_arg.arg_bpf, pcap_geterr(pcap_hander));
            pcap_close(pcap_hander);
            return -1;
        }
        if (-1 == pcap_setfilter(pcap_hander, &fp)) {
            printf("[Error]: Couldn't install bpf %s: %s\n", cmd_arg.arg_bpf, pcap_geterr(pcap_hander));
            pcap_close(pcap_hander);
            return -1;
        }
    }

    pcap_loop(pcap_hander, -1, pcap_loop_data_handler, NULL);

    return 0;
}

