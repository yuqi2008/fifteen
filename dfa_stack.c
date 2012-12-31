#include <stdio.h>
#define DFA_STACK_SIZE 64

void * dfa_stack[DFA_STACK_SIZE];
int dfa_stack_top_index;

void dfa_empty_stack(void)
{
	dfa_stack_top_index = -1;
}

void *dfa_stack_top(void)
{
	if (dfa_stack_top_index == -1)
		return NULL;
	return 
		dfa_stack[dfa_stack_top_index];
}

int dfa_stack_push(void *np)
{
	if (dfa_stack_top_index == DFA_STACK_SIZE - 1 || !np)
		return -1;
	dfa_stack[++dfa_stack_top_index] = np;
	return dfa_stack_top_index;
}

void *dfa_stack_pop(void)
{

	if (dfa_stack_top_index == -1)
		return NULL;
	return dfa_stack[dfa_stack_top_index--];
}
	

