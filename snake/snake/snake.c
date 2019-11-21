/*
 * snake.c
 *
 * Created: 11/20/2019 5:39:00 PM
 *  Author: William Blanc and Justin Crampton
 */ 

#include <avr/eeprom.h>	// For random init
#include <stdlib.h>
#include "led_matrix.h"
#include "snake.h"

// Global board matrix for location of elements
uint8_t board[BOARD_WIDTH][BOARD_WIDTH];

void placeFood()
{
	uint8_t food_row = rand() % BOARD_WIDTH;
	uint8_t food_col = rand() % BOARD_WIDTH;
	updateBoard(food_row, food_col, HIGH);
}

void updateBoard(uint8_t row, uint8_t col, uint8_t value){
	board[row][col] = value;
	if (value != LOW){
		setLED(row, col, HIGH);
	}
	else {
		setLED(row, col, LOW);
	}
}

// Initialize random to be random enough for game play
void random_init()
{
	// Obtain value from EEPROM
	uint8_t seed = eeprom_read_byte((uint8_t*)0x66); 
	// Set the random seed
	srand(seed);
	// Obtain random char value
	seed = rand() % 255;
	// Store seed for future use
	eeprom_write_byte((uint8_t*)0x66, seed);
};