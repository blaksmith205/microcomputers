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
#include "digits.h"

// Global board matrix for location of elements
uint8_t board[BOARD_WIDTH][BOARD_WIDTH];
// Game loop tracker. Probably a better way to do this
bool *gameLoop;
// Snake related
uint8_t score;

/*
	Creates a snake segment at a random location if row and col are -1.
	Otherwise, creates the snake segment at the desired cell.
*/
snake_cell* createSnakeSegment(int8_t startRow, int8_t startCol, snake_cell *prev)
{
	// Allocate space
	snake_cell *segment = malloc(sizeof(snake_cell));
	
	int8_t segment_row, segment_col;
	
	// Randomly generate position if -1 is given for row or col
	if (startRow == -1 || startCol == -1){
		getAvailablePosition(&segment_row, &segment_col);
	}
	else {
		segment_row = startRow;
		segment_col = startCol;
	}
	// Position should never be -1 when starting a game
	assert(segment_row != -1 && segment_col != -1);
	
	// Setup the head values
	segment->row = segment_row;
	segment->col = segment_col;
	segment->next = NULL;
	segment->prev = prev;
	
	// Place the snake on the board
	updateBoardAndDisplay(segment_row, segment_col, SNAKE_CELL);
	return segment;
}

/* Creates the snake at desired location and keeps track of the game looping variable */
snake_cell* start(uint8_t startRow, uint8_t startCol, bool *loopVar)
{
	gameLoop = loopVar;
	return createSnakeSegment(startRow, startCol, NULL);
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
	bool grown = false;
	getNextCell(currentRow, currentCol, direction, &nextRow, &nextCol);
	
	// Check if the next cell is not available (contains apple or snake segment)
	if (!isAvailable(nextRow, nextCol)){
		// Check if the next cell contains the apple
		if (isApple(nextRow, nextCol)){
			// Grow the snake. Use position of tail if not null because whole snake moves
			if (tail == NULL){
				eatFood(head, currentRow, currentCol);
				grown = true;
			}
			else {
				eatFood(head, tail->row, tail->col);
				grown = true;
			}
		}
		// Check if the next cell contains a body segment (not a tail)
		else if (!isTail(nextRow, nextCol, tail)){
			endGame();	// Signal the game to end
			return;
		}
	}
	uint8_t prevTailRow, prevTailCol;
	// Move the body if game hasn't ended
	moveBody(head, &prevTailRow, &prevTailCol);
	
	// update snake's head position
	head->row = nextRow;
	head->col = nextCol;
	
	// If the snake does not have a tail and it did not grow, clear the current position
	if (tail == NULL && !grown){
		updateBoardAndDisplay(currentRow, currentCol, LOW);
	}
	// If the snake has a tail and it did not grow, clear the tails previous position
	else if (tail != NULL && !grown){
		updateBoardAndDisplay(prevTailRow, prevTailCol, LOW);
	}
	
	/*
		If the snake has no tail and it did grow, the current position becomes a new snake segment
		(tail == NULL && grown) -> do nothing
		if the snake has a tail and it grew, the tails old position becomes a new snake segment
		(tail != NULL && grown) -> do nothing
	*/
	
	// Update the head
	updateBoardAndDisplay(nextRow, nextCol, SNAKE_CELL);
}

/* Increases the length of the snake, and changes the food location */
void eatFood(snake_cell *head, uint8_t currentRow, uint8_t currentCol)
{
	if (head == NULL) return;
	
	// Updates the snake and the board
	growSnake(head, currentRow, currentCol);
	
	// Set the new apple position
	placeFood();
	
	// update score
	score++;
}

/*
	Moves the body by updating every body segment with the cell of the previous segment.
	Head hasn't moved yet. Should be able to set the next segment to the cell of previous
*/
void moveBody(snake_cell *head, uint8_t *tailOldRow, uint8_t *tailOldCol)
{
	if (head == NULL) return;

	uint8_t prevRow = head->row;
	uint8_t prevCol = head->col;
	uint8_t currentRow, currentCol;
	
	// If there is no tail, the previous position of the head is returned as tailOldpos
	snake_cell *current_segment = head->next;
	while (current_segment != NULL){
		// Store the current position
		currentRow = current_segment->row;
		currentCol = current_segment->col;
		
		// Update the segments position
		current_segment->row = prevRow;
		current_segment->col = prevCol;
		
		// Update the prev position
		prevRow = currentRow;
		prevCol = currentCol;
		
		// Continue to next segment
		current_segment = current_segment->next;
	}
	
	*tailOldRow = prevRow;
	*tailOldCol = prevCol;
}

/* Grows the snake by 1 cell. Updates the tail pointer in head */
void growSnake(snake_cell *head, uint8_t currentRow, uint8_t currentCol)
{
	if (head == NULL) return;
	snake_cell *tail = head->prev;
	// Create a new snake segment
	snake_cell *segment;
	
	// Use the current tail to update the tail. Avoid traversing the linked list
	if (tail != NULL){
		// Create the new segment with current tail as prev
		segment = createSnakeSegment(currentRow, currentCol, tail);
		// Update the old tail to point to new tail
		tail->next = segment;
	}
	else {
		// if no tail, then only have 1 cell, the head.
		segment = createSnakeSegment(currentRow, currentCol, head);
		// Update head to point to next segment
		head->next = segment;
	}
	// Regardless, update the head to point to tail
	head->prev = segment;
}

/* Updates the food position and the board */
void updateFood(uint8_t newRow, uint8_t newCol)
{
	if (!isBoardBounded(newRow) || !isBoardBounded(newCol))
		return;
	
	// Old food position turns into a snake_cell. Only place new food
	updateBoardAndDisplay(newRow, newCol, APPLE);
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
		return board[row][col] == LOW;
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
		updateBoardAndDisplay(food_row, food_col, APPLE);
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
	
	// Wait 4 seconds
	_delay_ms(5000);
	
	setRow(7, LOW);
	_delay_ms(100);
	// Show bottom row as 'progress' bar
	for (uint8_t col = 0; col < BOARD_WIDTH; col++){
		setLED(7, col, HIGH);
		_delay_ms(200);
	}
	
	// Show the score
	showScore(score);
	
	// Light up the bottom row
	setRow(7, HIGH);
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
