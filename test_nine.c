#include <stdio.h>
#include <stdlib.h>
#include "nine.h"

int test_array[3][3] = { 6, 5, 4, 3, 7, 2, 1, 8, 0};

int
main(int argc, char **argv)
{
	char z, rz, dist0, dist1;
	int direct;
	unsigned long result, rresult;
	
	init_goal();
	result = array2long(test_array, &z);
	dist0 = mht_dist(result);
	printf("zero pos is %d\n", z);
	printf("0x%lx\n", result);
	printf("mht dist is %d\n", dist0);

	while(scanf("%d", &direct) == 1){
		direct %= 4;
		rresult = partern_swap(result, direct, z, &rz);
		dist1 = update_mht_dist(result, dist0, z, rz);
		dist0 = mht_dist(rresult);
		z = rz;
		result = rresult;
		printf("zero pos is %d\n", z);
		printf("0x%lx\n", result);
		printf("mht dist0 is %d\n", dist0);
		printf("another mht dist1 is %d\n", dist1);
		if (dist0 != dist1)
			abort();
	}
}
