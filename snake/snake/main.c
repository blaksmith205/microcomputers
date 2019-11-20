/*
 * snake.c
 *
 * Created: 11/20/2019 3:07:24 PM
 * Author : William Blanc and Justin Crampton
 */ 
#ifndef F_CPU
#define F_CPU 16000000UL // Set CPU frequency to 16MHZ
#endif

#define DIN_PIN 3	// Set MOSI (DIN) to bit 3 of PORTB (pin 11)
#define SCK_PIN	5	// Set Clock bit as bit 5 of PORTB (pin 13)
#define CS_PIN	2	// Set CS bit as bit 2 of PORTB (pin 10)

#include <avr/io.h>
#include <util/delay.h>
#include "led_matrix.h"

extern uint16_t ROWS[8];

int main(void)
{
    SET_BIT(PORTB, CS_PIN); // Don't select matrix until transmission
    spi_init(&DDRB, DIN_PIN, SCK_PIN, CS_PIN);	// Initialize serial output pins on PORTB
    setIntensity(15);
    
    // Set individual cells
    setCell(0,0, HIGH);
    setCell(0,7, HIGH);
    setCell(5,3, HIGH);
    setCell(7,7, HIGH);
    setCell(7,2, HIGH);
    
    SET_BIT(DDRB, 0);	// Signal program is done
    SET_BIT(PORTB, 0);
    while(1);
    return 0;
}

