PATH_SO = ../release/so
PATH_INCLUDE = ../release/include

all:
	gcc -g -shared -fPIC -o libmem_pool.so mem_pool.c -I ./

install:
	cp libmem_pool.so $(PATH_SO)
	cp libmem_pool.h $(PATH_INCLUDE)

clean:
	rm -rf libmem_pool.so
	rm -rf $(PATH_SO)/libmem_pool.so
	rm -rf $(PATH_INCLUDE)/libmem_pool.h
