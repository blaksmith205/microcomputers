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

// Initialize the rows to zero
uint16_t ROWS[] = {0x0800, 0x0700, 0x0600, 0x0500, 0x0400, 0x0300, 0x0200, 0x0100};

void spi_init(volatile uint8_t *DDR, uint8_t MOSI, uint8_t CLK, uint8_t CS)
{
	*DDR = (1<<MOSI)|(1<<CLK)|(1<<CS);		//MOSI and SCK are output, CS to select matrix
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);	//enable SPI as master
	
	spi_transfer(0x0900);			//disable decoding for all segments
	spi_transfer(0x0B07);			//scan 8 8-segments
	
	clearScreen();
	
	spi_transfer(0x0C01);			//turn on the matrix
}

void spi_transfer(uint16_t data)
{
	CLR_BIT(PORTB, CS_PIN);		// Select matrix
	SPDR = data >> 8;			//start CMD transmission
	while(!(SPSR & (1<<SPIF)));	//wait transfer finish
	SPDR = data & 0xFF;			//start DATA transmission
	while(!(SPSR & (1<<SPIF)));	//wait transfer finish
	SET_BIT(PORTB, CS_PIN)		//terminate the packet by
	_delay_ms(1);				// Wait a little bit
}

void setIntensity(uint8_t intensity)
{
	if (intensity < 0 || intensity > 15)
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
	if (row < 0 || row > 7)
		return;
	if (col < 0 || col > 7)
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
	if (row < 0 || row > 7) 
		return;
	// Clear all bits in the row in same transfer
	if (state == LOW)
		spi_transfer((8-row) << 8);
	// Set all bits in the row in same transfer
	else if (state == HIGH){
		spi_transfer(((8-row) << 8) | 0xFF);
	}
}

void setCol(uint8_t col, uint8_t state)
{
	if (col < 0 || col > 7)
		return;
	
	uint8_t _col;
	if (state == LOW){
		CLR_BIT(_col, col);
	}
	else if (state == HIGH){
		SET_BIT(_col, col);
	}
	else { 
		return;
	}
	
	for (uint16_t row = 0x0100; row <= 0x0800; row+=0x0100){
		spi_transfer(row | _col);
	}
}

void clearScreen()
{
		// Clear all digits
		for(uint16_t data = 0x0100; data <=0x0800; data += 0x0100){
			spi_transfer(data);
		}
}