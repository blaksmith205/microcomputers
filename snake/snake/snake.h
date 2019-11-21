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

// Directions
#define UP 0x01
#define DOWN 0x02
#define LEFT 0x04
#define RIGHT 0x08

// Snake related
/* Linked list snake cell structure. Only the head will contain the reference to the tail */
typedef struct snake_cell
{
	unsigned char row;
	unsigned char col;
	struct snake_cell *next;
	struct snake_cell *prev;	
}snake_cell;

snake_cell* createSnake(int8_t startRow, int8_t startCol);
snake_cell* start(uint8_t startRow, uint8_t startCol, bool *loopVar);
void grow(snake_cell *head);
void moveSnake(snake_cell *head, uint8_t direction);
void getAvailablePosition(int8_t *row, int8_t *col);
void getNextCell(uint8_t currentRow, uint8_t currentCol, uint8_t direction, uint8_t *nextRow, uint8_t *nextCol);
bool isAvailable(uint8_t row, uint8_t col);
bool isApple(uint8_t row, uint8_t col);
bool isTail(uint8_t row, uint8_t col, snake_cell *tail);
void placeFood();
void clearBoard();
void updateBoard(uint8_t row, uint8_t col, uint8_t value);
void updateBoardAndDisplay(uint8_t row, uint8_t col, uint8_t value);
void endGame();

// Misc
void randomInit();

#endif /* SNAKE_H_ */