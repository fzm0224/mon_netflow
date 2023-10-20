PATH_SO = ../release/so
PATH_INCLUDE = ../release/include
PATH_COMMON = ../common

all:
	gcc -g -shared -fPIC -o libdissect_engine.so dissect_engine.c $(PATH_COMMON)/json/cJSON.c -I ./ -I $(PATH_INCLUDE) -I $(PATH_COMMON) -I $(PATH_COMMON)/json -L $(PATH_SO) -lmem_pool -lm -ldl

install:
	cp libdissect_engine.so $(PATH_SO)
	cp libdissect_engine.h $(PATH_INCLUDE)

clean:
	rm -rf libdissect_engine.so
	rm -rf $(PATH_SO)/libdissect_engine.so
	rm -rf $(PATH_INCLUDE)/libdissect_engine.h
