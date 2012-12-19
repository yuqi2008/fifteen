#include <stdint.h>

#define TOP 0
#define LEFT 1
#define RIGHT 2
#define DOWN 3


void init_goal(void);
unsigned long array2long(int [3][3], char *);
char mht_dist(unsigned long);
unsigned long partern_swap(unsigned long, char,  char, char *);
char update_mht_dist(unsigned long, char, char, char);
unsigned char get_zero_pos(unsigned long);
unsigned long get_data_partern(unsigned long);
void split_data(uint64_t, uint64_t *, int8_t *, int8_t *, int8_t *, uint16_t *);
uint64_t join_data(uint64_t, int8_t, int8_t, int8_t, uint16_t);
int8_t equal_goal(uint64_t);
uint64_t zero_heap_id(uint64_t);
int8_t get_heap_id(uint64_t);
int8_t get_rd(uint64_t);
void set_rd(uint64_t *, int8_t);
int8_t get_all_dist(uint64_t);




