#include "heap.h"
#include "mpool.h"
#include <stdio.h>
struct tnode{
	long value;
	long id;
};

void **my_heap = NULL;

void my_callback(void *node, int id)
{
	struct tnode *tp = node;
	tp->id = id;
}

int long_cmp(void **A, int i, int j)
{
	struct tnode *u, *v;
	u = (struct tnode *)A[i - 1];
	v = (struct tnode *)A[j - 1];
	if (u->value == v->value)
		return 0;
	else if(u->value > v->value)
		return 1;
	else
		return -1;
}

void print_heap(void **A)
{
	int i;
	struct tnode *p;
	printf("**************************************\n");
	for(i = 0; i < heap_size; i++){
		p = A[i];
		printf("id %ld's value is %ld\n", p->id, p->value);
	}
	printf("***************************************\n");
}


int
main(int argc, char *argv[])
{
	int i;
	struct tnode *t, *l;
	init_mpool(3, sizeof(struct tnode));
	alloc_heap(&my_heap, 5);
	for(i = 100; i >80; i--){
		t = alloc_in_mpool();
		t->value = i;
		min_heap_insert(&my_heap, t, long_cmp, my_callback);
	}
	print_heap(my_heap);
/*
	for(i = 0; i < 20; i++){
		l = heap_minium(my_heap);
		printf("the %d's node is %ld, id is %ld\n", i, l->value, l->id);
		heap_extract_min(my_heap, long_cmp, my_callback);
		print_heap(my_heap);
	}
*/
	for(i = 19; i >= 0; i--){
		l = my_heap[i];
		l->value -= 15;
		min_heap_decrease_key(my_heap, i+1,long_cmp, my_callback);
		print_heap(my_heap);
	}
	free_all_mpool();
}



