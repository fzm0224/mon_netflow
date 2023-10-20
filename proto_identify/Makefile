PATH_SO = ../release/so
PATH_INCLUDE = ../release/include
PATH_COMMON = ../common

all:
	gcc -g -shared -fPIC -o libproto_identify.so proto_identify.c -I ./ -I $(PATH_INCLUDE) -I $(PATH_COMMON)

install:
	cp libproto_identify.so $(PATH_SO)
	cp libproto_identify.h $(PATH_INCLUDE)

clean:
	rm -rf libproto_identify.so
	rm -rf $(PATH_SO)/libproto_identify.so
	rm -rf $(PATH_INCLUDE)/libproto_identify.h
