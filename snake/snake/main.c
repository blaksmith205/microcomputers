/*
 * snake.c
 *
 * Created: 11/20/2019 3:07:24 PM
 * Author : William Blanc and Justin Crampton
 */ 

#define F_CPU 16000000UL	// Set CPU frequency to 16MHZ
#define BOARD_WIDTH 8
#define GAME_DELAY 1000		// The delay of the game updating time, in ms

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
uint8_t monitorButtons();

bool isRunning = true;

int main(void)
{
    setup();
	
 	// Create the snake
 	snake_cell *snake = start(-1, -1, &isRunning);
 
   	while (isRunning){
   		// Monitor the buttons on PIND
   		switch (monitorButtons()){
   			case UP:
   				moveSnake(snake, UP);
   				break;
   			case DOWN:
   				moveSnake(snake, DOWN);
   				break;
   			case LEFT:
   				moveSnake(snake, LEFT);
   				break;
   			case RIGHT:
   				moveSnake(snake, RIGHT);
   				break;
   		}
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
	Monitors all button pins. If a button is pressed, returns the direction.
	Otherwise, returns 0 
*/
uint8_t monitorButtons()
{
	if (MONITOR_BUTTON(BUTTON_PORT, UP_PIN)){
		return UP;
	}
	else if (MONITOR_BUTTON(BUTTON_PORT, DOWN_PIN)){
		return DOWN;
	}
	else if (MONITOR_BUTTON(BUTTON_PORT, LEFT_PIN)){
		return LEFT;
	}
	else if (MONITOR_BUTTON(BUTTON_PORT, RIGHT_PIN)){
		return RIGHT;
	}
	return 0;
}