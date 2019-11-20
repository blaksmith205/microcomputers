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

#define SPI_DDR DDRB	// Set SPI DDR register as DDRB
#define DIN_PIN 3	// Set MOSI (DIN) to bit 3 of PORTB (pin 11)
#define SCK_PIN	5	// Set Clock bit as bit 5 of PORTB (pin 13)
#define CS_PIN	2	// Set CS bit as bit 2 of PORTB (pin 10)

uint8_t spi_write(char data)
{
	// Start transmission
	SPDR = data;
	// Wait for transmission to complete. SPIF = spi flag (bit 7)
	while(!(SPSR & (1 << SPIF)));
	return SPDR;
}

void spi_write_int(int data){
	CLR_BIT(PORTB, CS_PIN); // Select the matrix
	spi_write(data >> 8); // Write higher bits
	spi_write(data & 0xFF); // Write lower bits
	SET_BIT(PORTB, CS_PIN); // De-select the matrix
}

void spi_init(uint8_t mosi_bit, uint8_t clk_bit, uint8_t cs_bit)
{
	// Set MOSI, SCK output, and CS as output, all others input
	SPI_DDR = (1 << mosi_bit) | (1 << clk_bit) | (1 << cs_bit);
	// Enable SPI, Master, set clock to high when idle, set clock rate F_CPU/16 or 1MHz, and send MSB first (DORD 0 by default)
	SPCR = (1<<SPIF) | (1 << SPE) | (1 << MSTR) | (1 << SPR0);
	_delay_ms(10); // Small delay for consistency
	// Get Matrix out of shutdown mode by sending 0xXCX1 from datasheet
	spi_write_int(0x0C01);
	_delay_ms(10);
	// Set scan limit to all 8 digits (rows) by sending 0xXBX7 from datasheet
	spi_write_int(0x0B07);
	_delay_ms(10);
	// Clear all rows
	for(uint16_t data = 0x0100; data <= 0x0800; data+=0x0100){
		spi_write_int(data);
		_delay_ms(10);
	}
}

int main(void)
{
	SET_BIT(PORTB, CS_PIN); // Don't select matrix until transmission
	spi_init(DIN_PIN, SCK_PIN, CS_PIN);	// Initialize serial output pins
	
	for (uint16_t data = 0x0100; data <= 0x08FF; data+= ){
		spi_write_int(data);
		_delay_ms(20);
	}

	SET_BIT(DDRB, 0);	// Signal program is done
	SET_BIT(PORTB, 0); 
	while(1);
	return 0;
}

