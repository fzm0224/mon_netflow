PATH_SO = ../../release/so
PATH_INCLUDE = ../../release/include

all:
	gcc test.c -o test -Wl,-rpath=$(PATH_SO) -I $(PATH_INCLUDE) -L $(PATH_SO) -lmem_pool -g
clean:
	rm -rf test
