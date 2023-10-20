#ifndef _MEM_POOL_
#define _MEM_POOL_


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libmem_pool.h"


// large memory boundary value
#define MEM_MAX_ALLOC_FROM_POOL  (mem_pagesize - 1)

#define MEM_ALIGN  (sizeof(unsigned long) - 1)
#define MEM_ALIGN_CAST  (unsigned long)
#define mem_align(p)    (char *) ((MEM_ALIGN_CAST p + MEM_ALIGN) & ~MEM_ALIGN)

void *mem_alloc(size_t size);
int mem_pfree_large(mem_pool_t *pool, void *p);


#endif
