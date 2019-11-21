/*
 * snake.c
 *
 * Created: 11/20/2019 5:39:00 PM
 *  Author: William Blanc and Justin Crampton
 */ 

#include <assert.h>		// For serious problems
#include <avr/eeprom.h>	// For random init
#include <stdbool.h>
#include <stdlib.h>
#include "led_matrix.h"
#include "snake.h"

// Global board matrix for location of elements
uint8_t board[BOARD_WIDTH][BOARD_WIDTH];
// Game loop tracker. Probably a better way to do this
bool *gameLoop;
// Snake related

/*
	Creates the head of the snake at a random location
*/
snake_cell* createSnake(int8_t startRow, int8_t startCol)
{
	// Allocate space
	snake_cell *head = (snake_cell *) malloc(sizeof(snake_cell));
	
	int8_t head_row, head_col;
	
	// Randomly generate position if -1 is given for row and col
	if (startRow == -1 && startCol == -1){
		getAvailablePosition(&head_row, &head_col);
	}
	else {
		head_row = startRow;
		head_col = startCol;
	}
	// Position should never be -1 when starting a game
	assert(head_row != -1 && head_col != -1);
	
	// Setup the head values
	head->row = head_row;
	head->col = head_col;
	head->next = NULL;
	head->prev = NULL;
	
	// Place the snake on the board
	updateBoardAndDisplay(head_row, head_col, HIGH);
	return head;
}

/* Creates the snake at desired location and keeps track of the game looping variable */
snake_cell* start(uint8_t startRow, uint8_t startCol, bool *loopVar)
{
	gameLoop = loopVar;
	return createSnake(startRow, startCol);
}

/* Grows the snake by 1 cell. Updates the tail pointer in head and updates the food*/
void grow(snake_cell *head)
{
	
}

/* 
	Moves the snake up. Performs necessary checks
*/
void moveSnake(snake_cell *head, uint8_t direction)
{
	if (head == NULL) return;
	snake_cell *tail = head->prev;
	uint8_t currentRow = head->row;
	uint8_t currentCol = head->col;
	uint8_t nextRow, nextCol;
	getNextCell(currentRow, currentCol, direction, &nextRow, &nextCol);
	
	// Check if the next cell is not available (contains apple or snake segment)
	if (!isAvailable(nextRow, nextCol)){
		// Check if the next cell contains the apple
		if (isApple(nextRow, nextCol)){
			// Grow the snake
			grow(head);
		}
		// Check if the next cell contains a body segment (not a tail)
		else if (!isTail(nextRow, nextCol, tail)){
			endGame();	// Signal the game to end
			return;
		}
	}
	// Otherwise, update snake's position if game hasn't ended
	head->row = nextRow;
	head->col = nextCol;
	
	// Update the head
	updateBoardAndDisplay(nextRow, nextCol, SNAKE_CELL);
	// Store old location of tail
	uint8_t oldTailRow, oldTailCol;
	// Update body
	// moveBody(head, currentRow, currentCol, &oldTailRow, &oldTailCol);
	// Clear the old position of the tail on the matrix.
	if (tail == NULL){
		updateBoardAndDisplay(currentRow, currentCol, LOW);
	}
	else {
		updateBoardAndDisplay(oldTailRow, oldTailCol, LOW);
	}
}

/*
	Sets the value of row and col to an available position on the board.
	
	If position can't be randomly determined 8 times, will set the row and col
	to the first available location on the board. If no available location,
	row and col are set to -1
*/
void getAvailablePosition(int8_t *_row, int8_t *_col)
{
	int8_t row = rand() % BOARD_WIDTH;
	int8_t col = rand() % BOARD_WIDTH;
	
	// Keeps looking for available row and col
	for (uint8_t i = 1; i < 8; i++){
		if (isAvailable(row,col)){
			// Update pointers
			*_row = row;
			*_col = col;
			return;
		}
		else {
			// Update position
			row = rand() % BOARD_WIDTH;
			col = rand() % BOARD_WIDTH;
		}
	}
	
	// Available random location wasn't found
	// Search for first free location
	for (int8_t row = 0; row < BOARD_WIDTH; row++){
		for (int8_t col = 0; col < BOARD_WIDTH; col++){
			if (isAvailable(row, col)){
				// Update pointers
				*_row = row;
				*_col = col;
				return;	
			}
		}
	}
	
	// No location was found. Set position to -1
	*_row = -1;
	*_col = -1;
}

/* 
	Sets the next cell from the current cell and desired direction. 
	Loops top <=> bottom and left <=> right. Keeps the row and col 
	board bounded.
*/
void getNextCell(uint8_t currentRow, uint8_t currentCol, uint8_t direction, uint8_t *nextRow, uint8_t *nextCol)
{
	switch (direction){
		case UP:
			// Loop to the bottom row
			if (currentRow == 0){
				*nextRow = BOARD_WIDTH - 1;
			}
			else {
				*nextRow = currentRow - 1;
			}
			*nextCol = currentCol;
			break;
		case DOWN:
			// Loop to the top row
			if (currentRow == BOARD_WIDTH - 1){
				*nextRow = 0;
			}
			else {
				*nextRow = currentRow + 1;
			}
			*nextCol = currentCol;
			break;
		case LEFT:
			// Loop to the right col
			if (currentCol == 0){
				*nextCol = BOARD_WIDTH - 1;
			}
			else {
				*nextCol = currentCol - 1;
			}
			*nextRow = currentRow;
			break;
		case RIGHT:
			// Loop to the left col
			if (currentCol == BOARD_WIDTH - 1){
				*nextCol = 0;
			}
			else {
				*nextCol = currentCol + 1;
			}
			*nextRow = currentRow;
			break;
	}
}

/* Check if the cell is available */
bool isAvailable(uint8_t row, uint8_t col)
{
	// Check range of row and col
	if (isBoardBounded(row) && isBoardBounded(col)){
		// Check if the board doesn't have an apple or snake segment
		return board[row][col] == 0;
	}
	return false;
}

/* Checks if the cell is the apple */
bool isApple(uint8_t row, uint8_t col)
{
	if (isBoardBounded(row) && isBoardBounded(col)){
		// Check if the cell is an apple
		return board[row][col] == APPLE;
	}
	return false;
}

/* Checks if the cell is the tail */
bool isTail(uint8_t row, uint8_t col, snake_cell *tail)
{
	if (tail != NULL && isBoardBounded(row) && isBoardBounded(col)){
		// Check if the cell is a snake segment
		if (board[row][col] == SNAKE_CELL){
			// Check if the cell matches the snake cell
			if (tail->row == row && tail->col == col)
				return true;
		}
	}
	return false;
}

void placeFood()
{
	int8_t food_row;
	int8_t food_col;
	getAvailablePosition(&food_row, &food_col);
	if (food_row != -1 && food_col != -1){
		updateBoard(food_row, food_col, APPLE);
	}
	else {
		endGame();
	}
}

/* Clears internal board state */
void clearBoard()
{
	for (int8_t row = 0; row < BOARD_WIDTH; row++){
		for (int8_t col = 0; col < BOARD_WIDTH; col++){
			board[row][col] = LOW;
		}
	}
}

/*  Updates the internal board state */
void updateBoard(uint8_t row, uint8_t col, uint8_t value)
{
	if (!isBoardBounded(row) || !isBoardBounded(col))
		return;
		
	board[row][col] = value;
}

/*
	Update the internal board state and the LED matrix.
	Convenience function for updateBoard and setLED
*/
void updateBoardAndDisplay(uint8_t row, uint8_t col, uint8_t value)
{
	if (!isBoardBounded(row) || !isBoardBounded(col))
		return;
		
	updateBoard(row, col, value);
	
	if (value != LOW){
		setLED(row, col, HIGH);
	}
	else {
		setLED(row, col, LOW);
	}
}

void endGame()
{
	*gameLoop = false;
}

// Misc related

/* Initialize random to be random enough for game play */
void randomInit()
{
	// Obtain value from EEPROM
	uint8_t seed = eeprom_read_byte((uint8_t*)0x66); 
	// Set the random seed
	srand(seed);
	// Obtain random char value
	seed = rand() % 255;
	// Store seed for future use
	eeprom_write_byte((uint8_t*)0x66, seed);
}
