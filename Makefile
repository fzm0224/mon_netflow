PATH_INCLUDE = ./release/include
PATH_SO = ./release/so
PATH_COMMON = ./common

all:
	gcc -g main.c  -o mon_netflow -Wl,-rpath=$(PATH_SO) -I $(PATH_INCLUDE) -I $(PATH_COMMON) -L $(PATH_SO) -ldissect_engine -lproto_identify -lpcap

clean:
	rm -rf mon_netflow