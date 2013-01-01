/***************************************************************************
 * fifteen.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Implements The Game of Fifteen (generalized to d x d).
 *
 * Usage: fifteen d
 *
 * whereby the board's dimensions are to be d x d,
 * where d must be in [DIM_MIN,DIM_MAX]
 *
 * Note that usleep is obsolete, but it offers more granularity than
 * sleep and is simpler to use than nanosleep; `man usleep` for more.
 ***************************************************************************/
 
#define _XOPEN_SOURCE 500

#include "cs50.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "nine.h"
#include "term_mode.h"


// constants
#define DIM_MIN 3
#define DIM_MAX 9
#define EX_TIMES  8192
#define INIT_QSIZE 1024


// board
int board[DIM_MAX][DIM_MAX];

// dimensions
int d;
int x_pos;
int y_pos;





// prototypes
void clear(void);
void greet(void);
void init(void);
void draw(void);
bool move(int tile);
bool won(void);
int random4(int);
bool swap(int);
void _swap(int, int, int, int);
int GetMove(void);

int god_mode_open =0;


int
main(int argc, char *argv[])
{
    // greet user with instructions
    greet();

    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: fifteen d\n");
        return 1;
    }

    // ensure valid dimensions
    d = atoi(argv[1]);
    if (d < DIM_MIN || d > DIM_MAX)
    {
        printf("Board must be between %d x %d and %d x %d, inclusive.\n",
         DIM_MIN, DIM_MIN, DIM_MAX, DIM_MAX);
        return 2;
    }

    // initialize the board
    init();

    // accept moves until game is won
    while (true)
    {
        // clear the screen
        clear();

        // draw the current state of the board
        draw();

        // check for win
        if (won())
        {
            printf("ftw!\n");
            break;
        }

        // prompt for move
        printf("Tile to move: ");
	fflush(stdout);
        int tile = GetMove();

        // move if possible, else report illegality
        if (!move(tile))
        {
            printf("\nIllegal move.\n");
            usleep(500000);
        }

        // sleep thread for animation's sake
        usleep(500000);
    }

    // that's all folks
    return 0;
}


/*
 * Clears screen using ANSI escape sequences.
 */

void
clear(void)
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}


/*
 * Greets player.
 */

void
greet(void)
{
    clear();
    printf("WELCOME TO THE GAME OF FIFTEEN\n");
    /*usleep(2000000);*/
}


/*
 * Initializes the game's board with tiles numbered 1 through d*d - 1
 * (i.e., fills 2D array with values but does not actually print them).  
 */

void
init(void)
{
	int i, j;
	god_mode_open = 0;
	for (i = 0; i < d ; i++)
		for (j = 0; j < d; j++)
			board[i][j] = i * d + j + 1;
	
	board[d - 1][d - 1] = 0;
	x_pos = y_pos = d - 1;

	for (i = 0; i < EX_TIMES; i++)
		swap(random4(i));
			 
		
	while(x_pos + 1 < d){
		_swap(x_pos, y_pos, x_pos + 1, y_pos);
		x_pos++;
	}
	while(y_pos + 1 < d){
		_swap(x_pos, y_pos, x_pos, y_pos + 1);
		y_pos++;
	}
    
}


/* 
 * Prints the board in its current state.
 */

void
draw(void)
{
	for (int i = 0; i < d ; i++){
		for (int j = 0; j < d; j++){
			if (!(i == y_pos && j == x_pos))
				printf("%d\t", board[i][j]);
			else
				printf(" \t");
		}
		printf("\n\n");
	}
     
}


/* 
 * If tile borders empty space, moves tile and returns true, else
 * returns false. 
 */

/*bool
move(int tile)
{
	int i , j;
	if (tile < 1 || tile > d * d - 1)
    		return false;
	for (i = 0; i < d; i++)
		for (j = 0; j < d; j++)
			if (board[i][j] == tile)
				goto FINDED;
	fprintf(stderr, "can not find tile\n");
	return false;
FINDED:
	if ((abs(i - y_pos) == 1 && j == x_pos) || (abs(j - x_pos) == 1 && i == y_pos)){
		_swap(x_pos, y_pos, j, i);
		x_pos = j;
		y_pos = i;
		return true;
	}
	return false;
	
}*/

bool
move(int tile)
{
	int x_pos_new, y_pos_new;
	bool r = false;

	switch (tile){
		case TOP:
			x_pos_new = x_pos;
			y_pos_new = y_pos - 1;
			break;
		case LEFT:
			x_pos_new = x_pos - 1;
			y_pos_new = y_pos;
			break;
		case RIGHT:
			x_pos_new = x_pos + 1;
			y_pos_new = y_pos;
			break;
		case DOWN:
			x_pos_new = x_pos;
			y_pos_new = y_pos + 1;
			break;
		default:
			x_pos_new = y_pos_new = -1;
			break;
	}

	if (x_pos_new >= 0 && x_pos_new < d && y_pos_new >= 0 && y_pos_new < d){
		board[y_pos][x_pos] = board[y_pos_new][x_pos_new];
		x_pos = x_pos_new;
		y_pos = y_pos_new;
		board[y_pos][x_pos] = 0;
		r = true;
	}
	return r;
}


			
				



/*
 * Returns true if game is won (i.e., board is in winning configuration), 
 * else false.
 */

bool
won(void)
{
	for (int i = 0; i < d; i++)
		for (int j = 0; j < d; j++){
			if (i == d - 1 && j == d - 1){
				if (board[i][j] != 0)
					return false;
			}else
				if (board[i][j] != d * i + j + 1)
					return false;
		}
	return true;
}

int
random4(int s)
{
	srand((unsigned int) (s  + time(NULL)));
	return rand() % 4;
}

bool
swap(int direct)
{
	int x, y;
REDO:
	switch (direct){
		case 0:
			x = x_pos;
			y = y_pos - 1;
			break;
		case 1:
			x = x_pos + 1;
			y = y_pos;
			break;
		case 2:
			x = x_pos;
			y = y_pos + 1;
			break;
		case 3:
			x = x_pos - 1;
			y = y_pos;
			break;
		default:
			fprintf(stderr, "wrong direct %d\n", direct);
			exit(1);
	}

	if ((x >= 0 && x < d) && (y >= 0 && y < d)){
		_swap(x_pos, y_pos, x, y);
		x_pos = x;
		y_pos = y;
	}else{
		direct = (direct + 1) % 4;
		goto REDO;
	}
	return true;

}

void
_swap(int x1, int y1, int x2, int y2)
{
	int t;
	t = board[y1][x1];
	board[y1][x1] = board[y2][x2];
	board[y2][x2] = t;
}


/*int
GetMove(void)
{
	char buf[32];
	char *end_pos;
	int l, n;
	if (god_mode_open == 0){
		if (fgets(buf, sizeof buf, stdin) == NULL){
			fprintf(stderr, "break game!\n");
			abort();
		}
		l = strlen(buf);
		if (buf[l - 1] == '\n')
		buf[l - 1] = '\0';
		if (strcmp(buf, "GOD") == 0){
			god_mode_open = 1;
			goto GOD_MODE;
		}
		n = strtol(buf, &end_pos, 10);
		if (*end_pos != '\0'){
			fprintf(stderr, "error input\n");
			n = 0;
		}
		return n;
	}
GOD_MODE:
	return 0;
}
*/


int
GetMove(void)
{
	char buf[4];
	int i, r = -1;

	if (tty_reset(STDIN_FILENO) != 0){
		fprintf(stderr, "reset fd failed\n");
		exit(-1);
	}
	if (tty_cbreak(STDIN_FILENO) != 0){
		fprintf(stderr, "can't go into cbreak mode\n");
		exit(-1);
	}
	while ( r == -1 ){
		i = read(STDIN_FILENO, buf, 1);
		if (buf[0] == '\033'){
			i = read(STDIN_FILENO, buf, 1);
			if( buf[0] == '['){
				i = read(STDIN_FILENO, buf, 1);
				switch (buf[0]){
					case 'A':
						r = TOP;
						break;
					case 'B':
						r = DOWN;
						break;
					case 'C':
						r = RIGHT;
						break;
					case 'D':
						r = LEFT;
						break;
					default:
						break;
				}
			}
		}
	}

	if (tty_reset(STDIN_FILENO) != 0){
		fprintf(stderr, "reset fd again failed\n");
		exit(-1);
	}

	return r;
}



