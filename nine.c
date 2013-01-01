#include "nine.h"
#include <stdio.h>
#include <stdlib.h>

#define GOAL_NUM 0x87654321
#define NEGP(n) ((~n + 1) & 0x1F)
#define  GOAL_ODD  (NEGP(1) | NEGP(3) << 8 | NEGP(5) << 16 | NEGP(7) << 24)
#define  GOAL_EVEN (NEGP(2) | NEGP(4) << 8 | NEGP(6) << 16 | NEGP(8) << 24)

char goal_num_array[] = {1, 2, 3, 4, 5, 6, 7, 8, 0};
#define ABS(n) ((n)<0?-(n):(n))
#define ABS_ARRAY(i) {i, ABS(i-1), ABS(i-2), ABS(i-3), ABS(i-4), ABS(i-5), ABS(i-6), ABS(i-7), ABS(i-8)}


char abs_array[9][9]  = { ABS_ARRAY(0), ABS_ARRAY(1), ABS_ARRAY(2), ABS_ARRAY(3), ABS_ARRAY(4), ABS_ARRAY(5),ABS_ARRAY(6), ABS_ARRAY(7), ABS_ARRAY(8) }; 

char top[] = {-1, -1, -1, 0, 1, 2, 3, 4, 5};
char left[] = {-1, 0, 1, -1, 3, 4, -1, 6, 7};
char right[] = {1, 2, -1, 4, 5, -1, 7, 8, -1};
char down[] = {3, 4, 5, 6, 7, 8, -1, -1, -1};

unsigned long goal_partern;

uint64_t zero_heap_id(uint64_t data)
{
	return data & 0xFFFFFFFFFFFFF;
}

int8_t equal_goal(uint64_t data)
{
	uint64_t t = 0xFFFFFFFFF;
	t &= data;
	return t == GOAL_NUM;
}

void init_goal(void)
{
	goal_partern = (unsigned long)GOAL_EVEN;
	goal_partern = goal_partern << 32 | (unsigned long)GOAL_ODD;
}

unsigned long array2long(int A[3][3], char *z)
{
	unsigned long t = 0;
	unsigned int row, col;
	char i = 0;

	for (row = 0; row < 3; row++)
		for (col = 0; col < 3; col++){
			if (A[row][col] == 0)
				*z = i;
			i++;
			t |= (unsigned long)A[row][col] << (row * 3 + col)*4;
		}
	return t;
}


unsigned char get_zero_pos(unsigned long data)
{
	return data>>36 & 0xF;
}

int8_t get_rd(uint64_t data)
{
	return data >> 46 & 0x3F;
}

uint64_t set_rd(uint64_t data, uint8_t new_rd)
{
	 uint64_t part = data & 0xFFFFFFFFF;
	 uint8_t zpos = data >> 36 & 0xF;
	 uint8_t mht  = data >> 40 & 0x3F;
	 uint8_t rd   = new_rd;
	 uint16_t heap_id = data >> 52 &0xFFF;
	 return join_data(part, zpos, mht, rd, heap_id);
}


uint16_t get_heap_id(uint64_t data)
{
	return data>>52 & 0xFFF;
}


uint64_t set_serial(uint64_t data, uint16_t serial)
{
	uint64_t t = serial & 0xFFF;
	t <<= 52;
	return zero_heap_id(data) | t;
}


unsigned long get_data_partern(unsigned long data)
{
	return data & 0xFFFFFFFFF;
}

int8_t get_all_dist(uint64_t data)
{
	return (data >> 40 & 0x3F) + (data >> 46 & 0x3F);
}

char mht_dist(unsigned long data)
{
	
	int i;
	unsigned char num;
	char sum, sig, element;
	unsigned long t = data & 0xFFFFFFFF;

	sum = 0;

	t = t & 0xF0F0F0F | (t & 0xF0F0F0F0)<< 28 ;
	t += goal_partern;
	for (i = 0; i < 8; i++){
		element = t >> (i * 8) & 0xFF;
		sig = element << 3;
		sig = sig >> 7;
		num =  ((element ^ sig) & 0xF) + (sig & 0x1);
		sum += num;
	}
	
	sum +=  data>>32 & 0xF;
	return sum;
}

unsigned long partern_swap(unsigned long data_partern, char direct, char zpos,  char *rzpos)
{
	char dpos;
	unsigned long exchange_mask, set_zero_mask = 0xF;
	switch (direct){
		case TOP:
			dpos = top[zpos];
			break;
		case LEFT:
			dpos = left[zpos];
			break;
		case RIGHT:
			dpos = right[zpos];
			break;
		case DOWN:
			dpos = down[zpos];
			break;
		default:
			fprintf(stderr, "unknown direction %d\n", direct);
			abort();
	}
	if (dpos == -1)
		return 0;
	*rzpos = dpos;
	exchange_mask  = (data_partern >> 4 *dpos & 0xF) << zpos * 4;
	set_zero_mask = ~(set_zero_mask << dpos * 4);
	return (data_partern | exchange_mask) & set_zero_mask;
}


void split_data(uint64_t data, uint64_t *ppart, int8_t *pzpos, int8_t *pmht, int8_t *prd, uint16_t *pheap_id)
{
	*ppart = data & 0xFFFFFFFFF;
	*pzpos = data >> 36 & 0xF;
	*pmht  = data >> 40 & 0x3F;
	*prd   = data >> 46 & 0x3F;
	*pheap_id = data >> 52 &0xFFF;
}

int8_t get_mht(uint64_t data)
{
	return data >> 40 &0x3F;
}

uint64_t join_data(uint64_t part, int8_t zpos, int8_t mht, int8_t rd, uint16_t heap_id)
{
	unsigned long t = heap_id;
	t = t << 6 | rd;
	t = t << 6 | mht;
	t = t << 4 | zpos;
	t = t << 36 | part;
	return t;
}

char update_mht_dist(unsigned long part, char old_mht, char old_zpos, char new_zpos)
{
	char g0, gn;
	char e;

	g0 = goal_num_array[old_zpos];
	gn = goal_num_array[new_zpos];
	e = part >> new_zpos * 4 & 0xF;

	return old_mht - g0 -abs_array[e][gn] + gn + abs_array[e][g0];
}

char get_move_direct(char pre_zpos, char suc_zpos)
{
	char t = suc_zpos - pre_zpos;
	switch(t){
		case  -3:
			return TOP;
		case  3:
			return DOWN;
		case -1:
			return LEFT;
		case  1:
			return RIGHT;
		default:
			fprintf(stderr, "wrong pre: %d, suc: %d\n", pre_zpos, suc_zpos);
			abort();
	}
}


