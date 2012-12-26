#include <stdio.h>
#include <stdlib.h>

char dstack[64];
char dstack_p = -1;

void dstack_empty(void)
{
	dstack_p = -1;
}

dstack_push(char direct)
{
	if (dstack_p == 64){
		fprintf(stderr, "stack overflow\n");
		abort();
	}
	dstack[++dstack_p] = direct;
}

char dstack_pop(void)
{
	if (dstack_p == -1)
		return -1;
	else
		return dstack[dstack_p--];
}

void pop_print_dstack(void)
{
	char t;
	printf("direct sequeues: ");
	while ((t = dstack_pop()) != -1)
		printf("%d ", t);
	putchar('\n');
}


