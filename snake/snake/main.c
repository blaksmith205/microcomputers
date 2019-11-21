/*
 * snake.c
 *
 * Created: 11/20/2019 3:07:24 PM
 * Author : William Blanc and Justin Crampton
 */ 

#define F_CPU 16000000UL // Set CPU frequency to 16MHZ
#define BOARD_WIDTH 8

#define DIN_PIN 3	// Set MOSI (DIN) to bit 3 of PORTB (pin 11)
#define SCK_PIN	5	// Set Clock bit as bit 5 of PORTB (pin 13)
#define CS_PIN	2	// Set CS bit as bit 2 of PORTB (pin 10)

#include <avr/io.h>
#include <util/delay.h>
#include "led_matrix.h"
#include "snake.h"

void setup();

int main(void)
{
    setup();
	
	// Create the snake
	snake_cell *snake = createSnake();

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
	
	// Setup random generator
	randomInit();
	
	// Place the first food
	placeFood();
}
