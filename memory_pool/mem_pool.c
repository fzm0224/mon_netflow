#include "mem_pool.h"
#include "libmem_pool.h"


int mem_pagesize = 0;


int init_mem_pagesize() {
	mem_pagesize = getpagesize();
	return mem_pagesize;
}


void *mem_alloc(size_t size) {
    void *p;
	
    if (!(p = malloc(size))) {
        return NULL;
    }
	memset(p, 0, size);

    return p;
}


mem_pool_t *mem_create_pool(size_t size) {
    mem_pool_t  *p;
	
	if ( 0 == init_mem_pagesize()) {
		mem_pagesize = 4095;
	}

    if (!(p = mem_alloc(size))) {
       return NULL;
    }
    p->last = (char *) p + sizeof(mem_pool_t);
    p->end = (char *) p + size;
    p->next = NULL;
    p->large = NULL;

    return p;
}


void mem_destroy_pool(mem_pool_t *pool) {
    mem_pool_t        *p, *n;
    mem_pool_large_t  *l;

    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            memset(l->alloc, 0, l->large_size);
            free(l->alloc);
            l->alloc = NULL;
        }
    }
    for (p = pool, n = pool->next; /* void */; p = n, n = n->next) {
        memset(p, 0, p->end - (char *)p);
        free(p);
        p = NULL;
        if (NULL == n) {
            break;
        }
    }
}


void *mem_palloc(mem_pool_t *pool, size_t size) {
    char                *m;
    mem_pool_t          *p, *n;
    mem_pool_large_t    *large, *last;

    if (size <= (size_t)MEM_MAX_ALLOC_FROM_POOL
        && size <= (size_t)(pool->end - (char *)pool) - sizeof(mem_pool_t)) {
        for (p = pool, n = pool->next; /* void */; p = n, n = n->next) {
            m = mem_align(p->last);
            if ((size_t) (p->end - m) >= size) {
                p->last = m + size ;
                return m;
            }
            if (NULL == n) {
                break;
            }
        }
        if (!(n = mem_create_pool((size_t)(p->end - (char *)p)))) {
            return NULL;
        }
        p->next = n;
        m = n->last;
        n->last += size;
        return m;
    }

    large = NULL;
    last = NULL;

    if (pool->large) {
        for (last = pool->large; /* void */ ; last = last->next) {
            if (NULL == last->alloc) {
                large = last;
                last = NULL;
                break;
            }
            if (NULL == last->next) {
                break;
            }
        }
    }

    if (NULL == large) {
        if (!(large = mem_palloc(pool, sizeof(mem_pool_large_t)))) {
            return NULL;
        }
        large->next = NULL;
    }

    if (!(p = mem_alloc(size))) {
        return NULL;
    }

    if (NULL == pool->large) {
        pool->large = large;
    } else if (last) {
        last->next = large;
    }
    large->alloc = p;
    large->large_size = size;

    return p;
}


int mem_pfree_large(mem_pool_t *pool, void *p) {
    mem_pool_large_t  *l;
	
    for (l = pool->large; l; l = l->next) {
        if (p == l->alloc) {
            memset(l->alloc, 0, l->large_size);
            free(l->alloc);
            l->alloc = NULL;
            return 0;
        }
    }

    return -1;
}
