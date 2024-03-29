/*
* snake.c
*
* Created: 11/20/2019 3:07:24 PM
* Author : William Blanc and Justin Crampton
*/

#define F_CPU 16000000UL	// Set CPU frequency to 16MHZ
#define BOARD_WIDTH 8
#define GAME_DELAY 250		// The delay of the game updating time, in ms

#define DIN_PIN PORTB3	// Set MOSI (DIN) to bit 3 of PORTB (pin 11)
#define SCK_PIN	PORTB5	// Set Clock bit as bit 5 of PORTB (pin 13)
#define CS_PIN	PORTB2	// Set CS bit as bit 2 of PORTB (pin 10)

#define BUTTON_PORT	PIND	// The port the buttons are on
#define UP_PIN		PORTD7	// Set the up button pin to bit 7 of PORT D (pin 7)
#define DOWN_PIN	PORTD6	// Set the down button pin to bit 6 of PORT D (pin 6)
#define LEFT_PIN	PORTD5	// Set the left button pin to bit 5 of PORT D (pin 5)
#define RIGHT_PIN	PORTD4	// Set the right button pin to bit 4 of PORT D (pin 4)
#define MONITOR_BUTTON(portIn, buttonPin) (portIn & (1 << buttonPin))

#include <avr/io.h>
#include <util/delay.h>
#include "led_matrix.h"
#include "snake.h"
#include "digits.h"

void setup();
void setupButtons(volatile uint8_t *BUTTON_DDR);

uint8_t monitorButtons(uint8_t *newDirection);

bool isRunning = true;

int main(void)
{
	setup();
	
	// Create the snake
	snake_cell *snake = start(-1, -1, &isRunning);
	
	// Store the direction for continuous movement
	uint8_t direction = 0;
	
	// Wait for first user press
	while (monitorButtons(&direction) == 0);
	
	while (isRunning){
		// Move snake in determined direction
		moveSnake(snake, direction);
		// Check the buttons
		monitorButtons(&direction);
		// Delay the game
		_delay_ms(GAME_DELAY);
	}
	SET_BIT(DDRB, 0);	// Signal program is done
	SET_BIT(PORTB, 0);
	while(1);
	return 0;
}

void setup()
{
	// Setup matrix
	SET_BIT(PORTB, CS_PIN); // Don't select matrix until transmission
	spi_init(&DDRB, DIN_PIN, SCK_PIN, CS_PIN);	// Initialize serial output pins on PORTB
	setIntensity(15);
	
	// Setup buttons on DDRD
	setupButtons(&DDRD);
	
	// Setup random generator
	randomInit();
	
	// Place the first food
	placeFood();
}

void setupButtons(volatile uint8_t *BUTTON_DDR)
{
	// Enable pins on BUTTON_DDR to input
	CLR_BIT(*BUTTON_DDR, UP_PIN);
	CLR_BIT(*BUTTON_DDR, DOWN_PIN);
	CLR_BIT(*BUTTON_DDR, LEFT_PIN);
	CLR_BIT(*BUTTON_DDR, RIGHT_PIN);
}

/*
	Monitors all button pins. If a button is pressed, returns and updates the direction.
	If no button was pressed, 0 is returned and direction remains unchanged
*/
uint8_t monitorButtons(uint8_t *newDirection)
{
	if (MONITOR_BUTTON(BUTTON_PORT, UP_PIN)){
		// If already going DOWN, can't go UP instantly. Avoids death by self in most cases
		if (*newDirection == DOWN)
			return DOWN;
		*newDirection = UP;
		return UP;
	}
	else if (MONITOR_BUTTON(BUTTON_PORT, DOWN_PIN)){
		// If already going UP, can't go DOWN instantly. Avoids death by self in most cases
		if (*newDirection == UP)
			return UP;
		*newDirection = DOWN;
		return DOWN;
	}
	else if (MONITOR_BUTTON(BUTTON_PORT, LEFT_PIN)){
		// If already going RIGHT, can't go LEFT instantly. Avoids death by self in most cases
		if (*newDirection == RIGHT)
			return RIGHT;
		*newDirection = LEFT;
		return LEFT;
	}
	else if (MONITOR_BUTTON(BUTTON_PORT, RIGHT_PIN)){
		// If already going RIGHT, can't go LEFT instantly. Avoids death by self in most cases
		if (*newDirection == LEFT)
			return LEFT;
		*newDirection = RIGHT;
		return RIGHT;
	}
	return 0;
}