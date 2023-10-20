PATH_SO = ../../release/so
PATH_INCLUDE = ../../release/include
PATH_COMMON = ../../common


all:
	gcc -g -shared -fPIC http_dissect.c -o libhttp.so $(PATH_COMMON)/json/cJSON.c -L $(PATH_SO) -lmem_pool -I $(PATH_COMMON) -I $(PATH_COMMON)/json -I $(PATH_INCLUDE)

install:
	cp libhttp.so $(PATH_SO)
	cp libhttp.h $(PATH_INCLUDE)

clean:
	rm -rf libhttp.so
	rm -rf $(PATH_SO)/libhttp.so
	rm -rf $(PATH_INCLUDE)/libhttp.h