#include "mpool.h"

int
main(int argc, char *argv[])
{
	int i;
	long *v;
	init_mpool(3, sizeof(long));
	for (i = 0; i < 37; i++){
		v = alloc_in_mpool();
		*v = i;
	}
	print_mpool_as_long();
	free_all_mpool();

}


