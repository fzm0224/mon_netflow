PATH_SO = ../../release/so
PATH_INCLUDE = ../../release/include
PATH_COMMON = ../../common


all:
	gcc -g -shared -fPIC oicq_dissect.c -o liboicq.so $(PATH_COMMON)/json/cJSON.c -L $(PATH_SO) -lmem_pool -I $(PATH_COMMON) -I $(PATH_COMMON)/json -I $(PATH_INCLUDE)

install:
	cp liboicq.so $(PATH_SO)
	cp liboicq.h $(PATH_INCLUDE)

clean:
	rm -rf liboicq.so
	rm -rf $(PATH_SO)/liboicq.so
	rm -rf $(PATH_INCLUDE)/liboicq.h