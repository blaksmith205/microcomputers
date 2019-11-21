/*
 * snake.h
 *
 * Created: 11/20/2019 5:38:45 PM
 *  Author: William Blanc and Justin Crampton
 */ 


#ifndef SNAKE_H_
#define SNAKE_H_

#define BOARD_WIDTH 8	// Width of the square board
#define APPLE 'a'		// Symbol for apple
#define SNAKE_CELL 's'	// symbol for a snake segment

// Snake game related
// Linked list snake cell structure
typedef struct snake_cell
{
	unsigned char row;
	unsigned char col;
	struct snake_cell *next;	
}snake_cell;

void placeFood();
void updateBoard(uint8_t row, uint8_t col, uint8_t value);

// Misc
void random_init();
#endif /* SNAKE_H_ */