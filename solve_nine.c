#include <stdio.h>
#include <stdlib.h>
#include "mpool.h"
//#include "solve_nine.h"
#include "rbtree.h"
#include "nine.h"
#include "heap.h"
#define MPOOL_ROW_SIZE 1024*64
#define INIT_HEAP_LENTH 1024

int8_t tentative_g_score = 0;

struct n3_node{
        struct rb_node rb_nine;
	uint64_t    compact;
	struct n3_node *pre;
};


struct rb_root *proot = NULL;
void **nine_heap = NULL;

int nine_heap_cmp(void **A, int i, int j)
{
	struct n3_node *p, *q;
	int m, n;

	p = A[i - 1];
	q = A[j - 1];
	m = get_all_dist(p->compact); 
	n = get_all_dist(q->compact);
	return m - n;
}

void nine_heap_node(void *element, int i)
{
	struct n3_node *p;
	long t = 0xFFF;
	if (i == 1<<12){
		fprintf(stderr, "overflow, %d\n", tentative_g_score);
		abort();
	}

	p = element;
	p->compact &= ~(t << 52);
	t = i;
	p->compact |= t << 52;
}
	
int part_cmp(uint64_t p, uint64_t q)
{
	long i, j;
	
	i = get_data_partern(p);
	j = get_data_partern(q);
	return i - j;
}

	
struct n3_node *search_node(uint64_t part, struct rb_root *proot, struct rb_node ***pp, struct rb_node **pparent)
{
	struct rb_node **p, *parent;
	long pcmp;
	struct n3_node *t_node;

	p = &proot->rb_node;
	parent = NULL;

	while(*p){
		parent = *p;
		t_node = rb_entry(*p, struct n3_node, rb_nine);
		pcmp = part_cmp(part, t_node->compact);
		if (pcmp == 0)
			return t_node;
		else if (pcmp < 0)
			p = &(*p)->rb_left;
		else
			p = &(*p)->rb_right;
	}
	*pp = p;
	*pparent = parent;
	return NULL;
}

void link_node(struct n3_node *node, struct rb_root *proot, struct rb_node **insert_pos, struct rb_node *parent)
{
	rb_link_node(&node->rb_nine, parent, insert_pos);
	rb_insert_color(&node->rb_nine, proot);
}
			

/*
struct n3_node *link_node(struct n3_node *node, struct rb_root *proot, char search_only)
{
	struct rb_node **p, *parent;
	struct n3_node *t_node;
	int ncmp;

	p = &proot->rb_node;
	parent = NULL;

	while(*p){
		parent = *p;
		t_node = rb_entry(*p, struct n3_node, rb_nine);
		ncmp = node_cmp(node, t_node);
		if (ncmp == 0)
			return node;
		else if (ncmp < 0)
			p = &(*p)->rb_left;
		else
			p = &(*p)->rb_right;
	}
	if (!search_only){
		rb_link_node(&node->rb_nine, parent, p);
		rb_insert_color(&node->rb_nine, proot);
	}
	return NULL;
}
*/


	
void generate_node(struct n3_node **pnode, uint64_t data, struct n3_node *pre_node)
{
	*pnode = alloc_in_mpool();
	(*pnode)->compact = data;
	(*pnode)->pre = pre_node;
	rb_init_node(&(*pnode)->rb_nine);
}

void init_A_star(int nine[3][3])
{
	//init mpool
	init_mpool(MPOOL_ROW_SIZE, sizeof(struct n3_node));
	
	//init rbtree
	proot = malloc(sizeof(struct rb_root));
	RB_EMPTY_ROOT(proot);

	//init heap
	alloc_heap(&nine_heap, INIT_HEAP_LENTH);
	
	//init partern
	init_goal();
	uint64_t part;
	uint8_t zpos, mht, rd = 0;
	uint16_t heap_id = 1;
	part = array2long(nine, &zpos);
	mht = mht_dist(part);
	part = join_data(part, zpos, mht, rd, heap_id);
	
	//generate first node
	struct n3_node *current;
	generate_node(&current, part, NULL);

	//put node in rbtree
	struct rb_node **nine_rb_pos, *nine_parent;
	search_node(current->compact, proot, &nine_rb_pos, &nine_parent);
	link_node(current, proot, nine_rb_pos, nine_parent);

	//put node in heap
	min_heap_insert(&nine_heap, current, nine_heap_cmp, nine_heap_node);
}

void A_star(int nine[3][3])
{
	init_A_star(nine);
	int i;
	struct n3_node *select_node, *current = NULL;
	uint64_t part0, part1;
	int8_t zpos0, zpos1;
	int8_t mht0, mht1;
	int8_t rd0, rd1; //tentative_g_score;
	int16_t heap_id0, heap_id1;
	struct rb_node **select_rb_pos, *select_rb_parent;


	while(heap_not_empty()){
		current = heap_minium(nine_heap);
		if (equal_goal(current->compact)){
			fprintf(stderr,"binggo!\n");
			exit(0);
		}
		heap_extract_min(nine_heap, nine_heap_cmp, nine_heap_node);
		split_data(current->compact, &part0, &zpos0, &mht0, &rd0, &heap_id0);
		current->compact = zero_heap_id(current->compact);
		for (i = 0; i < 4; i++){
			part1 = partern_swap(part0, i, zpos0, &zpos1);
			if (!part1)
				continue;
			mht1 = update_mht_dist(part0, mht0, zpos0, zpos1);
			tentative_g_score = rd0 + 1;
			select_node = search_node(part1, proot, &select_rb_pos, &select_rb_parent);
			if (!select_node){
				part1 = join_data(part1, zpos1, mht1, tentative_g_score, 0);
				generate_node(&select_node, part1, current);
				link_node(select_node, proot, select_rb_pos, select_rb_parent);
				min_heap_insert(&nine_heap, select_node, nine_heap_cmp, nine_heap_node);
				continue;
			}
			if (heap_id1 = get_heap_id(select_node->compact))
				continue;
			if (get_rd(select_node->compact) >= tentative_g_score){
				select_node->pre = current;
				set_rd(&select_node->compact, tentative_g_score);
				min_heap_decrease_key(nine_heap, heap_id1, nine_heap_cmp, nine_heap_node);
			}
		}
	}

	fprintf(stderr, "failed at array size %d\n", array_size);

	
}

		



	









