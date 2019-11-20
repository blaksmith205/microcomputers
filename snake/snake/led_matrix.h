/*
 * led_matrix.h
 *
 * Created: 11/20/2019 3:08:25 PM
 *  Author: William Blanc
 */ 


#ifndef LED_MATRIX_H_
#define LED_MATRIX_H_
#ifndef F_CPU
#define F_CPU 16000000UL // Set CPU frequency to 16MHZ
#endif

#include <avr/io.h>
#include <util/delay.h>

#define SET_BIT(port, bit)	port |= (1<<bit);
#define CLR_BIT(port, bit)	port &= ~(1<<bit);
#define LOW 0
#define HIGH 1

void spi_init(volatile uint8_t *DDR, uint8_t MOSI, uint8_t CLK, uint8_t CS);
void spi_transfer(uint16_t data);
void setIntensity(uint8_t intensity);
void setCell(uint8_t row, uint8_t col, uint8_t state);
#endif /* LED_MATRIX_H_ */