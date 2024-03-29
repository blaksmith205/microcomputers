/*
* led.c
*
* Created: 11/20/2019 3:08:04 PM
*  Author: William Blanc
*/

#include <avr/io.h>
#include <util/delay.h>
#include "led_matrix.h"

#ifndef DIN_PIN
#define DIN_PIN 3	// Set MOSI (DIN) to bit 3 of PORTB (pin 11)
#endif
#ifndef SCK_PIN
#define SCK_PIN	5	// Set Clock bit as bit 5 of PORTB (pin 13)
#endif
#ifndef CS_PIN
#define CS_PIN	2	// Set CS bit as bit 2 of PORTB (pin 10)
#endif

/* Initialize the rows to zero */
uint16_t ROWS[] = {0x0800, 0x0700, 0x0600, 0x0500, 0x0400, 0x0300, 0x0200, 0x0100};

void spi_init(volatile uint8_t *DDR, uint8_t MOSI, uint8_t CLK, uint8_t CS)
{
	*DDR = (1<<MOSI)|(1<<CLK)|(1<<CS);		//MOSI and SCK are output, CS to select matrix
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);	//enable SPI as master
	
	spi_transfer(0x0900);	//disable decoding for all segments
	spi_transfer(0x0B07);	//scan 8 8-segments
	
	clearScreen();
	
	spi_transfer(0x0C01);	//turn on the matrix
}

void spi_transfer(uint16_t data)
{
	CLR_BIT(PORTB, CS_PIN);		// Select matrix
	SPDR = data >> 8;			// start CMD transmission
	while(!(SPSR & (1<<SPIF)));	// wait for transfer to finish
	SPDR = data & 0xFF;			// start DATA transmission
	while(!(SPSR & (1<<SPIF)));	// wait for transfer to finish
	SET_BIT(PORTB, CS_PIN)		// De-select matrix
	_delay_ms(1);				// Wait a little bit
}

void setIntensity(uint8_t intensity)
{
	if (!isBounded(intensity, 0, 16))
		return;
	spi_transfer(0x0A00 | intensity);
}

/*
	Sets the LED in desired cell
	row: 0 to 7; upper left is 0
	col: 0 to 7; upper left is 0
*/
void setLED(uint8_t row, uint8_t col, uint8_t state)
{
	if (!isBoardBounded(row) || !isBoardBounded(col))
		return;
	
	uint16_t data = ROWS[row];
	
	// Turn off the cell
	if (state == LOW){
		CLR_BIT(data, col);
		spi_transfer(data);
	}
	// Turn on the cell
	if (state == HIGH){
		SET_BIT(data, col);
		spi_transfer(data);
	}
	ROWS[row] = data;
}

void setRow(uint8_t row, uint8_t state)
{
	if (!isBoardBounded(row))
		return;
	uint16_t newRow;
	// Clear all bits in the row in same transfer
	if (state == LOW){
		newRow = (8-row) << 8;
		spi_transfer(newRow);
	}
	// Set all bits in the row in same transfer
	else if (state == HIGH){
		newRow = ((8-row) << 8) | 0xFF;
		spi_transfer(newRow);
	}
	// Update state of row
	ROWS[row] = newRow;
}

void setCol(uint8_t col, uint8_t state)
{
	if (!isBoardBounded(col))
		return;
	
	for (uint8_t row = 0; row < BOARD_WIDTH; row++){
		setLED(row, col, state);
	}
}

void clearScreen()
{
	// Clear all digits
	for (uint16_t data = 0x0100; data <=0x0800; data += 0x0100){
		spi_transfer(data);
	}
}

/*
	Checks if a value is within a certain range.
	Lower bound is inclusive, Upper bound is exclusive
*/
bool isBounded(uint8_t value, uint8_t lowerBound, uint8_t upperBound)
{
	return value >= lowerBound || value < upperBound;
}

/* Convenience function for row and col bound checking */
bool isBoardBounded(uint8_t value)
{
	return isBounded(value, 0, BOARD_WIDTH);
}