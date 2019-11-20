/*
 * LED_Matrix_test.c
 *
 * Created: 11/18/2019 4:20:13 PM
 * Author : William Blanc
 
 For Matrix:
	5v	->	VCC
	GND	->	GND
	12	->	DIN
	10	->	CS
	11	->	CLK
	
 LED matrix latches data when CS is low and following rising edge or 00 for spi mode (bit 3 and bit2)
 
 0x0FFF is all leds on
*/ 

#ifndef F_CPU
#define F_CPU 16000000UL // Set CPU frequency to 16MHZ
#endif

#include <avr/io.h>
#include <util/delay.h>

#define SET_BIT(port, bit)	port |= (1<<bit);
#define CLR_BIT(port, bit)	port &= ~(1<<bit);
#define LOW 0
#define HIGH 1

#define DIN_PIN 3	// Set MOSI (DIN) to bit 3 of PORTB (pin 11)
#define SCK_PIN	5	// Set Clock bit as bit 5 of PORTB (pin 13)
#define CS_PIN	2	// Set CS bit as bit 2 of PORTB (pin 10)

uint16_t row0 = 0x0800;
uint16_t row1 = 0x0700;
uint16_t row2 = 0x0600;
uint16_t row3 = 0x0500;
uint16_t row4 = 0x0400;
uint16_t row5 = 0x0300;
uint16_t row6 = 0x0200;
uint16_t row7 = 0x0100;

void spi_init(volatile uint8_t *DDR, uint8_t MOSI, uint8_t CLK, uint8_t CS);
void spi_transfer(uint16_t data);
void setIntensity(uint8_t intensity);

int main(void)
{
	SET_BIT(PORTB, CS_PIN); // Don't select matrix until transmission
	spi_init(&DDRB, DIN_PIN, SCK_PIN, CS_PIN);	// Initialize serial output pins on PORTB
	setIntensity(15);
	
	for (uint16_t data = 0x0100; data <= 0x08FF; data++){
		spi_transfer(data);
		_delay_ms(20);
	}

	SET_BIT(DDRB, 0);	// Signal program is done
	SET_BIT(PORTB, 0); 
	while(1);
	return 0;
}

void spi_init(volatile uint8_t *DDR, uint8_t MOSI, uint8_t CLK, uint8_t CS)
{
	*DDR = (1<<MOSI)|(1<<CLK)|(1<<CS);		//MOSI and SCK are output, CS to select matrix
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);	//enable SPI as master
	
	spi_transfer(0x0900);			//disable decoding for all segments
	spi_transfer(0x0B07);			//scan 8 8-segments
	
	// Clear all digits
	for(uint16_t data = 0x0100; data <=0x0800; data += 0x0100){
		spi_transfer(data);
	}
	
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

