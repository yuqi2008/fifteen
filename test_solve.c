#include "solve_nine.h"
#include <stdio.h>

int test_array[3][3] = { 1, 3, 5, 7, 2, 4, 6, 8, 0};

int 
main(int argc, char **argv)
{
	A_star(test_array);
	printf("init completed\n");
}
