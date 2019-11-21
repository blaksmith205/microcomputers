/*
 * snake.h
 *
 * Created: 11/20/2019 5:38:45 PM
 *  Author: William Blanc and Justin Crampton
 */ 


#ifndef SNAKE_H_
#define SNAKE_H_

#define APPLE 'a'		// Symbol for apple
#define SNAKE_CELL 's'	// symbol for a snake segment

// Snake related
/* Linked list snake cell structure */
typedef struct snake_cell
{
	unsigned char row;
	unsigned char col;
	struct snake_cell *next;	
}snake_cell;

snake_cell* createSnake();

// Board/Game related
void endGame();
void getAvailablePosition(int8_t *row, int8_t *col);
bool isAvailable(uint8_t row, uint8_t col);
void placeFood();
void updateBoard(uint8_t row, uint8_t col, uint8_t value);

// Misc
void randomInit();

#endif /* SNAKE_H_ */