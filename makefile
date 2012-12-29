OPTIONS = -ggdb -D_NINE_DEBUG


test_solve:
	gcc $(OPTIONS) -o test_solve test_solve.c mpool.c rbtree.c heap.c nine.c solve_nine.c direct_stack.c



