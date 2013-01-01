#include "solve_nine.h"
#include <stdio.h>

int test_array[3][3] = { 2, 8, 6, 7, 5, 4, 1, 3, 0};

int 
main(int argc, char **argv)
{
	//A_star(test_array);
	DFA_star(test_array);
	printf("init completed\n");
}
