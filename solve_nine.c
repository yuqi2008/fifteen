#include <stdio.h>
#include <stdlib.h>
#include "mpool.h"
//#include "solve_nine.h"
#include "rbtree.h"
#include "nine.h"
#include "heap.h"
#include "direct_stack.h"
#define MPOOL_ROW_SIZE 1024*16
#define INIT_HEAP_LENTH 1024
//#define _NINE_DEBUG 



int8_t tentative_g_score = 0;

#ifdef _NINE_DEBUG
FILE *debug_log;
int loop_num = 8192;
char heap_change = 0;
#endif

struct n3_node{
        struct rb_node rb_nine;
	uint64_t    compact;
	struct n3_node *pre;
};


struct rb_root *proot = NULL;
void **nine_heap = NULL;

void print_matrix(FILE *fp, uint64_t part)
{
	char n, i;
	for (i = 0; i < 9; i++){
		n = part >> 4 * i & 0xF;
		fprintf(fp, "%d\t", n);
		if ((i+1)%3 == 0)
			fprintf(fp, "\n");
	}
}


void log_node(FILE * in, uint64_t part, char zpos, char mht, char rd, short heap_id)
{
	fprintf(in, "part is %lx#zpos is %d#mht is %d#rd is %d#heap_id is %d\n", part, zpos, mht, rd, heap_id);
	print_matrix(in, part);
	fprintf(in, "********************************************************\n");
}

	
	
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
	#ifdef _NINE_DEBUG
	debug_log = fopen("solve_nine.log", "a+");
	#endif


}

void free_A_star(void){
	if (nine_heap)
		free_heap(nine_heap);
	free_all_mpool();
	if (proot)
		free(proot);
}

void reconstruct_path(struct n3_node *data_node)
{
	struct n3_node  *pre_node, *suc_node;
	char szpos, pzpos;

	suc_node = data_node;
	pre_node = suc_node->pre;
	dstack_empty();

	while(pre_node){
		szpos = get_zero_pos(suc_node->compact);
		pzpos = get_zero_pos(pre_node->compact);
		dstack_push(get_move_direct(pzpos, szpos));
		suc_node = pre_node;
		pre_node = pre_node->pre;
	}
	pop_print_dstack();
}



	

char A_star(int nine[3][3])
{
	init_A_star(nine);
	int i;
	struct n3_node *select_node, *current = NULL;
	uint64_t part0, part1;
	int8_t zpos0, zpos1;
	int8_t mht0, mht1;
	int8_t rd0, rd1; //tentative_g_score;
	uint16_t heap_id0, heap_id1;
	struct rb_node **select_rb_pos, *select_rb_parent;
	char success = 0;


	while(heap_not_empty()){
		#ifdef _NINE_DEBUG
		if (--loop_num == 0)
			abort();
		#endif
		current = heap_minium(nine_heap);
		if (equal_goal(current->compact)){
			fprintf(stderr,"binggo!, %d\n", heap_size);
			reconstruct_path(current);
			success = 1;
			break;
		}
		heap_extract_min(nine_heap, nine_heap_cmp, nine_heap_node);
		split_data(current->compact, &part0, &zpos0, &mht0, &rd0, &heap_id0);
		#ifdef _NINE_DEBUG
		fprintf(debug_log, "checkout a node:\n");
		log_node(debug_log, part0, zpos0, mht0, rd0, heap_id0);
		fprintf(debug_log, "\n\n");
		#endif
		current->compact = zero_heap_id(current->compact);
		for (i = 0; i < 4; i++){
			part1 = partern_swap(part0, i, zpos0, &zpos1);
			if (!part1)
				continue;
			mht1 = update_mht_dist(part0, mht0, zpos0, zpos1);
			tentative_g_score = rd0 + 1;
			#ifdef _NINE_DEBUG
			fprintf(debug_log, "calculate  node number:\n");
			log_node(debug_log, part1, zpos1, mht1, tentative_g_score, 0);
			#endif
			select_node = search_node(part1, proot, &select_rb_pos, &select_rb_parent);
			if (!select_node){
				part1 = join_data(part1, zpos1, mht1, tentative_g_score, 0);
				generate_node(&select_node, part1, current);
				link_node(select_node, proot, select_rb_pos, select_rb_parent);
				min_heap_insert(&nine_heap, select_node, nine_heap_cmp, nine_heap_node);
				#ifdef _NINE_DEBUG
				fprintf(debug_log, "genenrate new  node add in: %lx, heap id is %d\n", select_node->compact, get_heap_id(select_node->compact));
				fprintf(debug_log, "\n\n");
				#endif
				continue;
			}
			if ((heap_id1 = get_heap_id(select_node->compact)) == 0){
				#ifdef _NINE_DEBUG
				fprintf(debug_log, "node %lx already in close set, with heap id %d", select_node->compact, get_heap_id(select_node->compact));
				fprintf(debug_log, "\n\n");
				#endif
				continue;
			}
			if (get_rd(select_node->compact) >= tentative_g_score){
				#ifdef _NINE_DEBUG
				if (get_rd(select_node->compact) == tentative_g_score){
					fprintf(stderr, "node %lx's heap_id is %d\n", select_node->compact, get_heap_id(select_node->compact));
					heap_change = 1;
				}
				#endif
				select_node->pre = current;
				set_rd(&select_node->compact, tentative_g_score);
				#ifdef _NINE_DEBUG
				fprintf(debug_log, "node %lx's rd can be shorter to %d, with origin heap id %d\n", select_node->compact, tentative_g_score,get_heap_id(select_node->compact));
				#endif
				min_heap_decrease_key(nine_heap, heap_id1, nine_heap_cmp, nine_heap_node);
				#ifdef _NINE_DEBUG
				fprintf(debug_log, "node %lx's heap id is %d", select_node->compact, get_heap_id(select_node->compact));
				fprintf(debug_log, "\n\n");
				if (heap_change == 1){
					heap_change = 0;
					fprintf(stderr, "node %lx's heap_id is %d\n", select_node->compact, get_heap_id(select_node->compact));
					fprintf(stderr, "*******************************************\n\n");
				}
				#endif

			}
		}
	}
	#ifdef _NINE_DEBUG
	fprintf(stderr, "failed at array size %d\n", array_size);
	print_mpool_consumed();
	#endif
	free_A_star();
	return success;
	
}

		



	









