#include "libmem_pool.h"

struct small_test {
	int a;
	double b;
	char c;
	char str[8];
};

struct large_test {
	char name[32];
	int age;
	char phone[12];
	char info[1024];
};

int main()
{
	mem_pool_t *my_pool;
	
	mem_test_null(my_pool, mem_create_pool(MEM_DEFAULT_POOL_SIZE), -1);

	int ret = 0;
	int *px = mem_palloc(my_pool, sizeof(int));
	*px = 10;
	printf("px = %p\n*px = %d\n\n", px, *px);
	
	struct small_test *ps = mem_palloc(my_pool, sizeof(struct small_test));
	ps->a = 24;
	ps->b = 3.141592654;
	ps->c = 'A';
	strncpy(ps->str, "abcd", 4);
	printf("ps = %p\nstruct small_test size = %ld\n", ps, sizeof(struct small_test));
	printf("small_test ps: a = %d, b = %lf, c = %c, str = %s\n\n", ps->a, ps->b, ps->c, ps->str);

	struct small_test *ps1 = mem_palloc(my_pool, sizeof(struct small_test));
	ps1->a = 24;
	ps1->b = 3.141592654;
	ps1->c = 'B';
	strncpy(ps1->str, "abcd", 4);
	printf("ps1 = %p\nstruct small_test size = %ld\n", ps1, sizeof(struct small_test));
	printf("small_test ps1: a = %d, b = %lf, c = %c, str = %s\n\n", ps1->a, ps1->b, ps1->c, ps1->str);

	struct small_test *ps2 = mem_palloc(my_pool, sizeof(struct small_test));
	ps2->a = 24;
	ps2->b = 3.141592654;
	ps2->c = 'C';
	strncpy(ps2->str, "abcd", 4);
	printf("ps2 = %p\nstruct small_test size = %ld\n", ps2, sizeof(struct small_test));
	printf("small_test ps2: a = %d, b = %lf, c = %c, str = %s\n\n", ps2->a, ps2->b, ps2->c, ps2->str);

	struct small_test *ps3 = mem_palloc(my_pool, sizeof(struct small_test));
	ps3->a = 24;
	ps3->b = 3.141592654;
	ps3->c = 'D';
	strncpy(ps3->str, "abcd", 4);
	printf("ps3 = %p\nstruct small_test size = %ld\n", ps3, sizeof(struct small_test));
	printf("small_test ps3: a = %d, b = %lf, c = %c, str = %s\n\n", ps3->a, ps3->b, ps3->c, ps3->str);

	struct small_test *ps4 = mem_palloc(my_pool, sizeof(struct small_test));
	ps4->a = 24;
	ps4->b = 3.141592654;
	ps4->c = 'E';
	strncpy(ps4->str, "abcd", 4);
	printf("ps4 = %p\nstruct small_test size = %ld\n", ps4, sizeof(struct small_test));	
	printf("small_test ps4: a = %d, b = %lf, c = %c, str = %s\n\n", ps4->a, ps4->b, ps4->c, ps4->str);

	struct small_test *ps5 = mem_palloc(my_pool, sizeof(struct small_test));
	ps5->a = 24;
	ps5->b = 3.141592654;
	ps5->c = 'F';
	strncpy(ps5->str, "abcd", 4);
	printf("ps5 = %p\nstruct small_test size = %ld\n", ps5, sizeof(struct small_test));	
	printf("small_test ps5: a = %d, b = %lf, c = %c, str = %s\n\n", ps5->a, ps5->b, ps5->c, ps5->str);
	
	struct small_test *ps6 = mem_palloc(my_pool, sizeof(struct small_test));
	ps6->a = 24;
	ps6->b = 3.141592654;
	ps6->c = 'G';
	strncpy(ps6->str, "abcd", 4);
	printf("ps6 = %p\nstruct small_test size = %ld\n", ps6, sizeof(struct small_test));	
	printf("small_test ps6: a = %d, b = %lf, c = %c, str = %s\n\n", ps6->a, ps6->b, ps6->c, ps6->str);
	
	

	struct large_test *pl = mem_palloc(my_pool, sizeof(struct large_test));
	strncpy(pl->name, "LYF", 3);
	pl->age = 24;
	strncpy(pl->phone, "17812345678", 11);
	strncpy(pl->info, "this is a test info!", 20);
	printf("pl = %p\nstruct large_test size = %ld\n", pl, sizeof(struct large_test));
	printf("pl large_test: name = %s, age = %d, phone = %s, info = %s\n\n", pl->name, pl->age, pl->phone, pl->info);

	struct large_test *pl_1 = mem_palloc(my_pool, sizeof(struct large_test));
	strncpy(pl_1->name, "LYL", 3);
	pl_1->age = 24;
	strncpy(pl_1->phone, "17812345678", 11);
	strncpy(pl_1->info, "this is a test info!", 20);
	printf("pl_1 = %p\nstruct large_test size = %ld\n", pl_1, sizeof(struct large_test));
	printf("pl_1 large_test: name = %s, age = %d, phone = %s, info = %s\n\n", pl_1->name, pl_1->age, pl_1->phone, pl_1->info);

	struct large_test *pl_2 = mem_palloc(my_pool, sizeof(struct large_test));
	strncpy(pl_2->name, "LLL", 3);
	pl_2->age = 24;
	strncpy(pl_2->phone, "17812345678", 11);
	strncpy(pl_2->info, "this is a test info!", 20);
	printf("pl_2 = %p\nstruct large_test size = %ld\n", pl_2, sizeof(struct large_test));
	printf("pl_2 large_test: name = %s, age = %d, phone = %s, info = %s\n\n", pl_2->name, pl_2->age, pl_2->phone, pl_2->info);


	mem_destroy_pool(my_pool);
	
	printf("destroy small_test ps: a = %d, b = %lf, c = %c, str = %s\n", ps->a, ps->b, ps->c, ps->str);
	printf("destroy small_test ps1: a = %d, b = %lf, c = %c, str = %s\n", ps1->a, ps1->b, ps1->c, ps1->str);
	printf("destroy small_test ps2: a = %d, b = %lf, c = %c, str = %s\n", ps2->a, ps2->b, ps2->c, ps2->str);
	printf("destroy small_test ps3: a = %d, b = %lf, c = %c, str = %s\n", ps3->a, ps3->b, ps3->c, ps3->str);
	printf("destroy small_test ps4: a = %d, b = %lf, c = %c, str = %s\n", ps4->a, ps4->b, ps4->c, ps4->str);
	printf("destroy small_test ps5: a = %d, b = %lf, c = %c, str = %s\n", ps5->a, ps5->b, ps5->c, ps5->str);
	printf("destroy small_test ps6: a = %d, b = %lf, c = %c, str = %s\n", ps6->a, ps6->b, ps6->c, ps6->str);
	
	printf("pl = %p\n", pl);
	printf("destroy pl large_test: name = %s, age = %d, phone = %s, info = %s\n", pl->name, pl->age, pl->phone, pl->info);
	printf("pl_1 = %p\n", pl_1);
	printf("destroy pl_1 large_test: name = %s, age = %d, phone = %s, info = %s\n", pl_1->name, pl_1->age, pl_1->phone, pl_1->info);
	printf("pl_2 = %p\n", pl_2);
	printf("destroy pl_2 large_test: name = %s, age = %d, phone = %s, info = %s\n", pl_2->name, pl_2->age, pl_2->phone, pl_2->info);

	return 0;
}