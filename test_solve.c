#include "solve_nine.h"
#include <stdio.h>

int test_array[3][3] = { 6, 5, 1, 3, 2, 7, 8, 4, 0};

int 
main(int argc, char **argv)
{
	A_star(test_array);
	printf("init completed\n");
}
