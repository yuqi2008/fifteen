#include <stdio.h>
#include <stdlib.h>
#include "heap.h"

int heap_size = 0;
int array_size = 0;


int heap_not_empty(void)
{
	return heap_size;
}

void alloc_heap(void ***PA, int s)
{
	void **A = *PA;
	if (!A){
		A = malloc(s * sizeof(void *));
		heap_size = 0;
		array_size = s;
	}else{
		A = realloc(A, s * sizeof(void *));
		array_size = s;
	}
	*PA = A;
}

int heap_parent(int i)
{
	return i / 2;
}

int heap_left(int i)
{
	return 2*i;
}

int heap_right(int i)
{
	return 2*i + 1;
}

void heap_swap(void **A, int i, int j, HEAP_NODE_CALLBACK callback)
{
	void *tmp;
	tmp = A[i - 1];
	A[i - 1] = A[j - 1];
	A[j - 1] = tmp;
	if (callback){
		callback(A[i - 1], i);
		callback(A[j - 1], j);
	}
}



void min_heapify(void **A, int i, HEAP_CMP_FUNC heap_cmp, HEAP_NODE_CALLBACK callback)
{
	int l, r, least;

	l = heap_left(i);
	r = heap_right(i);
	if (l <= heap_size && (*heap_cmp)(A, l, i) < 0)
		least = l;
	else
		least = i;
	if (r <= heap_size && (*heap_cmp)(A, r, least) < 0)
		least = r;
	if (least != i){
		heap_swap(A, least, i, callback);
		min_heapify(A, least, heap_cmp, callback);
	}
}
	
void build_min_heap(void **A, int len, HEAP_CMP_FUNC heap_cmp, HEAP_NODE_CALLBACK callback)
{
	int i;
	heap_size = len;
	for (i = len / 2; i > 0; i--)
		min_heapify(A, i, heap_cmp, callback);
}

void * heap_minium(void **A)
{
	return A[0];
}

void heap_extract_min(void **A, HEAP_CMP_FUNC heap_cmp, HEAP_NODE_CALLBACK callback)
{
	if (heap_size < 1){
		fprintf(stderr, "heap underflow\n");
		abort();
	}
	A[0] = A[heap_size - 1];
	if(callback)
		callback(A[0], 1);
	heap_size--;
	min_heapify(A, 1, heap_cmp, callback);
}


void min_heap_insert(void ***PA, void *key, HEAP_CMP_FUNC heap_cmp, HEAP_NODE_CALLBACK callback)
{
	int i;
	void **A = *PA;
	heap_size++;
	if (heap_size == array_size)
		alloc_heap(&A, array_size * 2);
	A[heap_size - 1] = key;
	if (callback)
		callback(key, heap_size);
	for (i = heap_size; i > 1 && (*heap_cmp)(A, i, heap_parent(i)) < 0; i = heap_parent(i))
		heap_swap(A, i, heap_parent(i), callback);
	*PA = A;
}

void min_heap_decrease_key(void **A, int i, HEAP_CMP_FUNC heap_cmp, HEAP_NODE_CALLBACK callback)
{
	while (i > 1 && (*heap_cmp)(A, i, heap_parent(i)) <= 0){
		heap_swap(A, i, heap_parent(i), callback);
		i = heap_parent(i);
	}
}

void free_heap(void **A)
{
	free(A);
	array_size = 0;
	heap_size = 0;
}

