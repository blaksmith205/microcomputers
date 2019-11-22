/*
 * digits.h
 *
 * Created: 11/21/2019 5:41:21 PM
 *  Author: William Blanc and Justin Crampton
 */ 

#ifndef DIGITS_H_
#define DIGITS_H_

#define LEFT_SIDE  0	// Left side of the screen
#define RIGHT_SIDE 5	// Right side of the screen. Will be used to left shift bits

// Shortcuts for different 3 bit configuration. l is on, _ is off
#define l__ 1
#define _l_ 2
#define ll_ 3
#define __l 4
#define l_l 5
#define _ll 6
#define lll 7

void showScore(uint8_t score);
uint16_t* getLeftDigit(uint8_t value); 
uint16_t* getRightDigit(uint8_t value); 
uint16_t createDigitRow(uint8_t row, uint8_t value, uint8_t side);

#define SET_ROW_L(arr, row, bit_config) arr[row] = createDigitRow(row, bit_config, LEFT_SIDE);
#define SET_ROW_R(arr, row, bit_config) arr[row] = createDigitRow(row, bit_config, RIGHT_SIDE);
#endif /* DIGITS_H_ */