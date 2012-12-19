#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

struct tdata{
	struct rb_node rb_tdata;
	long data;
	long id;
};

struct rb_root *rb_root = NULL;
long test_num[20];


void print_test_num(void)
{
	int i;
	for (i = 0; i < 20; i++)
		printf("the %dth number is %ld\n", i, test_num[i]);
	printf("\n\n");
}

void generate_test_num(int seed)
{
	int i;
	srandom(seed);
	for (i = 0; i < 20; i++)
		test_num[i] = random();
}

void init_rb_root(struct rb_root **pproot)
{
	*pproot = malloc(sizeof(struct rb_root));
	if (!*pproot){
		fprintf(stderr, "malloc proot failed\n");
		abort();
	}
	RB_EMPTY_ROOT(*pproot);
}

void init_tdata_node(struct tdata *t, long data, int id)
{
	t->data = data;
	t->id = id;
	rb_init_node(&t->rb_tdata);
}
	

struct tdata* insert_num_to_rbtree(long new_data, int id, struct rb_root *proot)
{
	struct rb_node **insert_point, *parent, **p;
	struct tdata *t_node, *new_node;
	parent = NULL;
	p = &(proot->rb_node);

	while(*p){
		parent = *p;
		t_node = rb_entry(*p, struct tdata,rb_tdata);
		if (t_node->data == new_data){
			fprintf(stderr, "data %ld already in tree\n", new_data);
			return t_node;
		}else if(new_data < t_node->data)
			p = &(*p)->rb_left;
		else
			p = &(*p)->rb_right;
	}
	new_node = malloc(sizeof(struct tdata));
	init_tdata_node(new_node, new_data, id);
	rb_link_node(&new_node->rb_tdata, parent, p);
	rb_insert_color(&new_node->rb_tdata, proot);
	return NULL;
}

int
main(int argc, char *argv[])
{
	int seed, i;
	struct tdata *t = NULL;
	long q;
	if (argc == 1)
		seed = 7;
	else
		seed = atoi(argv[1]);
	generate_test_num(seed);
	print_test_num();
	init_rb_root(&rb_root);
	for(i = 0; i < 20; i++){
		t = insert_num_to_rbtree(test_num[i], i, rb_root);
		if (t)
			fprintf(stderr, "data %ld is pluged\n", t->data);
		else
			fprintf(stderr, "insert %ld\n", test_num[i]);
	}
	printf("\n");
	for(i = 0; i <20; i++){
		t = insert_num_to_rbtree(test_num[i], i, rb_root);
		if (!t)
			fprintf(stderr,"%ld can't be found\n", test_num[i]);
		else{
			rb_erase(&t->rb_tdata, rb_root);
			fprintf(stderr, "%ld has been deleted\n", test_num[i]);
		}
	}

	printf("\n");

}
			



		



