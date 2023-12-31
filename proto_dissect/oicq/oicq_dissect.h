#ifndef _OICQ_DISSECT_
#define _OICQ_DISSECT_


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"

typedef struct _value_string_t {
    uint32_t value;
    char *string;
} value_string_t;

static const value_string_t oicq_flag_vals[] = {
	{ 0x02,	"Oicq packet" },
	{ 0,			NULL }
};

static const value_string_t oicq_command_vals[] = {
	{ 0x0001,	"Log out" },
	{ 0x0002,	"Heart Message" },
	{ 0x0004,	"Update User information" },
	{ 0x0005,	"Search user" },
	{ 0x0006,	"Get User informationBroadcast" },
	{ 0x0009,	"Add friend no auth" },
	{ 0x000a,	"Delete user" },
	{ 0x000b,	"Add friend by auth" },
	{ 0x000d,	"Set status" },
	{ 0x0012,	"Confirmation of receiving message from server" },
	{ 0x0016,	"Send message" },
	{ 0x0017,	"Receive message" },
	{ 0x0018,	"Retrieve information" },
	{ 0x001a,	"Reserved " },
	{ 0x001c,	"Delete Me" },
	{ 0x001d,	"Request KEY" },
	{ 0x0021,	"Cell Phone" },
	{ 0x0022,	"Log in" },
	{ 0x0026,	"Get friend list" },
	{ 0x0027,	"Get friend online" },
	{ 0x0029,	"Cell PHONE" },
	{ 0x0030,	"Operation on group" },
	{ 0x0031,	"Log in test" },
	{ 0x003c,	"Group name operation" },
	{ 0x003d,	"Upload group friend" },
	{ 0x003e,	"MEMO Operation" },
	{ 0x0058,	"Download group friend" },
	{ 0x005c,	"Get level" },
	{ 0x0062,	"Request login" },
	{ 0x0065,	"Request extra information" },
	{ 0x0067,	"Signature operation" },
	{ 0x0080,	"Receive system message" },
	{ 0x0081,	"Get status of friend" },
	{ 0x00b5,	"Get friend's status of group" },
	{ 0x03f7,	"Withdraw message" },
	{ 0,			NULL }
};


#endif