#include "solve_nine.h"
#include <stdio.h>

int test_array[3][3] = {1, 6, 4, 3, 5, 7, 8, 2, 0}; //{ 2, 8, 6, 7, 5, 4, 1, 3, 0};

int 
main(int argc, char **argv)
{
	//A_star(test_array);
	DFA_star(test_array);
	printf("init completed\n");
}
