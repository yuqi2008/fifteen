#define SET_MAX 16

#include <stdlib.h>
#include <stdio.h>

int mpool_set_size = 0;
int mpool_set_used= 0;
int mpool_quantum_size = 0;

struct mpool_set{
	void * set_start;
	int current;
};

struct mpool_set mpool_set_array[SET_MAX];

void init_mpool(int set_size, int quantum_size)
{
	struct mpool_set *p;
	p = mpool_set_array;
	posix_memalign(&p->set_start, 8, set_size * quantum_size);

	if (!p->set_start){
		fprintf(stderr, "init alloc mpool failed\n");
		exit(1);
	}
	p->current = 0;
	mpool_set_size = set_size;
	mpool_set_used = 0;
	mpool_quantum_size = quantum_size;
}

void add_mpool(void)
{
	struct mpool_set *p;
	if ((mpool_set_array[mpool_set_used]).current != mpool_set_size){
		fprintf(stderr,"gen_mpool: have not complete used\n");
		exit(1);
	}
	if (mpool_set_used == SET_MAX - 1){
		fprintf(stderr, "mpool full ,can't alloc\n");
		exit(1);
	}
	p = ++mpool_set_used + mpool_set_array;
	posix_memalign(&p->set_start, 8, mpool_set_size * mpool_quantum_size);
	p->current = 0;
}

void * alloc_in_mpool(void)
{
	struct mpool_set *p;
	void *r;
	if (mpool_set_size == 0){
		fprintf(stderr, "mpool did't init\n");
		exit(1);
	}
	p = mpool_set_array + mpool_set_used;
	if (p->current == mpool_set_size){
		add_mpool();
		p++;
	}
	r = (void *)(p->set_start + p->current * mpool_quantum_size);
	p->current++;
	return r;
}

void print_mpool_as_long(void)
{
	int i,j;
	struct mpool_set *p;
	for(i = 0; i <= mpool_set_used; i++){
		p = mpool_set_array + i;
		for(j = 0; j < p->current; j++)
			printf("%ld\t", *((long *)(p->set_start + mpool_quantum_size * j)));
		printf("\n");
	}
}

void free_all_mpool(void)
{
	int i;
	struct mpool_set *p;
	for (i = 0; i < mpool_set_used; i++){
		p = mpool_set_array + i;
		free(p->set_start);
		p->current = 0;
	}
	mpool_set_used = 0;
	mpool_set_size= 0;
	mpool_quantum_size = 0;
}


		



		
	
