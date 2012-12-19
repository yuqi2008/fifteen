#ifndef HEAP_LIB
#define HEAP_LIB
typedef int (*HEAP_CMP_FUNC)(void **, int, int);
typedef void (*HEAP_NODE_CALLBACK)(void *, int);
#endif

void alloc_heap(void ***, int);
void build_min_heap(void **, int, HEAP_CMP_FUNC, HEAP_NODE_CALLBACK);
void *heap_minium(void **);
void heap_extract_min(void **, HEAP_CMP_FUNC, HEAP_NODE_CALLBACK);
void min_heap_insert(void ***, void *, HEAP_CMP_FUNC, HEAP_NODE_CALLBACK);
void min_heap_decrease_key(void **, int, HEAP_CMP_FUNC, HEAP_NODE_CALLBACK);
int heap_not_empty(void);

extern int array_size; 
