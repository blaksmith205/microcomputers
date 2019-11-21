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
// Snake related

/*
	Creates the head of the snake at a random location
*/
snake_cell* createSnake()
{
	// Allocate space
	snake_cell *head = (snake_cell *) malloc(sizeof(snake_cell));
	int8_t head_row, head_col;
	getAvailablePosition(&head_row, &head_col);
	
	// Position should never be -1 when starting a game
	assert(head_row != -1 && head_col != -1);
	
	// Setup the head values
	head->row = head_row;
	head->col = head_col;
	head->next = NULL;
	
	// Place the snake on the board
	updateBoard(head_row, head_col, HIGH);
	return head;
}

// Board/Game related
void endGame()
{
	
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
		else{
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

/* Check if the row,col is available */
bool isAvailable(uint8_t row, uint8_t col)
{
	// Check range of row and col
	if (isBoardBounded(row) && isBoardBounded(col)){
		// Check if the board doesn't have an apple or snake segment
		return board[row][col] == 0;
	}
	else{
		return false;
	}
}

void placeFood()
{
	int8_t food_row;
	int8_t food_col;
	getAvailablePosition(&food_row, &food_col);
	if (food_row != -1 && food_col != -1){
		updateBoard(food_row, food_col, HIGH);
	}
	else{
		endGame();
	}
}


void updateBoard(uint8_t row, uint8_t col, uint8_t value)
{
	if (!isBoardBounded(row) || !isBoardBounded(col))
		return;
		
	board[row][col] = value;
	if (value != LOW){
		setLED(row, col, HIGH);
	}
	else {
		setLED(row, col, LOW);
	}
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
