#ifndef HEAP_LIB
#define HEAP_LIB
typedef long (*HEAP_CMP_FUNC)(void **, long, long);
typedef void (*HEAP_NODE_CALLBACK)(void *, long);
#endif

void alloc_heap(void ***, long);
void build_min_heap(void **, long, HEAP_CMP_FUNC, HEAP_NODE_CALLBACK);
void *heap_minium(void **);
void heap_extract_min(void **, HEAP_CMP_FUNC, HEAP_NODE_CALLBACK);
void min_heap_insert(void ***, void *, HEAP_CMP_FUNC, HEAP_NODE_CALLBACK);
void min_heap_decrease_key(void **, long, HEAP_CMP_FUNC, HEAP_NODE_CALLBACK);
long heap_not_empty(void);

extern long array_size;
extern long heap_size;
