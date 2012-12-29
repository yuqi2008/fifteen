#include "solve_nine.h"
#include <stdio.h>

int test_array[3][3] = { 8, 3, 7, 4, 0, 1, 6, 2, 5};

int 
main(int argc, char **argv)
{
	A_star(test_array);
	printf("init completed\n");
}
