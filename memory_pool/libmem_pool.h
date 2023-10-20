#ifndef _LIB_MEM_POOL_
#define _LIB_MEM_POOL_


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MEM_DEFAULT_POOL_SIZE   1024
#define mem_test_null(p, alloc, rc)  if (NULL == (p = alloc)) { return rc; }

typedef struct mem_pool_large_s {
    struct mem_pool_large_s *next;
    int                     large_size;
    void              	    *alloc;
} mem_pool_large_t;


typedef struct mem_pool_s {
    char              	*last;
    char              	*end;
    struct mem_pool_s   *next;
    mem_pool_large_t  	*large;
} mem_pool_t;

mem_pool_t *mem_create_pool(size_t size);
void mem_destroy_pool(mem_pool_t *pool);
void *mem_palloc(mem_pool_t *pool, size_t size);


#endif
