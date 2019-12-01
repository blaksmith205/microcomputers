/*
* digits.c
*
* Created: 11/21/2019 5:44:19 PM
*  Author: William Blanc and Justin Crampton
*/
#include <stdlib.h>
#include "led_matrix.h"
#include "digits.h"

/* Displays the score onto the matrix */
void showScore(uint8_t score)
{
	if (!isBounded(score, 0, 99))
	score %= 99;	// Force score to be displayable with only 2 digits
	uint8_t rightValue = score % 10;	// Git the ones digit of the score
	// Remove ones digit
	score -= (score % 10);
	uint8_t leftValue = score / 10 ;	// Get the tens digit of the score
	
	// Obtain the individual digits
	uint16_t *leftValues = getLeftDigit(leftValue);
	uint16_t *rightValues = getRightDigit(rightValue);
	uint16_t combinedValue;
	for (uint8_t row = 0; row < BOARD_WIDTH; row++){
		combinedValue = leftValues[row] | rightValues[row];
		// Show the value
		spi_transfer(combinedValue);
		_delay_ms(1);	// Dont overload the matrix
	}
	
	// free the memory obtained for the digits
	free(leftValues);
	free(rightValues);
}

/*
	Maps the digit to the array of values to send to the matrix.
	Digits are 5x3.
	Spacing: upper row, lower 2 rows, middle 2 columns
	Left digit will show in columns 0, 1, 2 for rows 1,2,3,4,5
*/
uint16_t* getLeftDigit(uint8_t value)
{
	// Allocate size of 8 uint16_t
	uint16_t *values = malloc(8 * sizeof(uint16_t));
	values[0] = 0x0800;	// Clear top row
	values[6] = 0x0200; // Clear second to last row
	values[7] = 0x0100; // Clear last row
	switch (value){
		case 0:
			SET_ROW_L(values, 1, lll)	// ***.....
			SET_ROW_L(values, 2, l_l)	// * *.....
			SET_ROW_L(values, 3, l_l)	// * *.....
			SET_ROW_L(values, 4, l_l)	// * *.....
			SET_ROW_L(values, 5, lll)	// ***.....
			break;
		case 1:
			SET_ROW_L(values, 1, _l_)	//  * .....
			SET_ROW_L(values, 2, ll_)	// ** .....
			SET_ROW_L(values, 3, _l_)	//  * .....
			SET_ROW_L(values, 4, _l_)	//  * .....
			SET_ROW_L(values, 5, lll)	// ***.....
			break;
		case 2:
			SET_ROW_L(values, 1, ll_)	// ** .....
			SET_ROW_L(values, 2, __l)	//   *.....
			SET_ROW_L(values, 3, lll)	// ***.....
			SET_ROW_L(values, 4, l__)	// *  .....
			SET_ROW_L(values, 5, lll)	// ***.....
			break;
		case 3:
			SET_ROW_L(values, 1, lll)	// ***.....
			SET_ROW_L(values, 2, __l)	//   *.....
			SET_ROW_L(values, 3, lll)	// ***.....
			SET_ROW_L(values, 4, __l)	//   *.....
			SET_ROW_L(values, 5, lll)	// ***.....
			break;
		case 4:
			SET_ROW_L(values, 1, l_l)	// * *.....
			SET_ROW_L(values, 2, l_l)	// * *.....
			SET_ROW_L(values, 3, lll)	// ***.....
			SET_ROW_L(values, 4, __l)	//   *.....
			SET_ROW_L(values, 5, __l)	//   *.....
			break;
		case 5:
			SET_ROW_L(values, 1, lll)	// ***.....
			SET_ROW_L(values, 2, l__)	// *  .....
			SET_ROW_L(values, 3, lll)	// ***.....
			SET_ROW_L(values, 4, __l)	//   *.....
			SET_ROW_L(values, 5, lll)	// ***.....
			break;
		case 6:
			SET_ROW_L(values, 1, lll)	// ***.....
			SET_ROW_L(values, 2, l__)	// *  .....
			SET_ROW_L(values, 3, lll)	// ***.....
			SET_ROW_L(values, 4, l_l)	// * *.....
			SET_ROW_L(values, 5, lll)	// ***.....
			break;
		case 7:
			SET_ROW_L(values, 1, lll)	// ***.....
			SET_ROW_L(values, 2, __l)	//   *.....
			SET_ROW_L(values, 3, __l)	//   *.....
			SET_ROW_L(values, 4, __l)	//   *.....
			SET_ROW_L(values, 5, __l)	//   *.....
			break;
		case 8:
			SET_ROW_L(values, 1, lll)	// ***.....
			SET_ROW_L(values, 2, l_l)	// * *.....
			SET_ROW_L(values, 3, lll)	// ***.....
			SET_ROW_L(values, 4, l_l)	// * *.....
			SET_ROW_L(values, 5, lll)	// ***.....
			break;
		case 9:
			SET_ROW_L(values, 1, lll)	// ***.....
			SET_ROW_L(values, 2, l_l)	// * *.....
			SET_ROW_L(values, 3, lll)	// ***.....
			SET_ROW_L(values, 4, __l)	//   *.....
			SET_ROW_L(values, 5, __l)	//   *.....
			break;
	}
	return values;
}

/*
	Maps the digit to the array of values to send to the matrix.
	Digits are 5x3.
	Spacing: upper row, lower 2 rows, middle 2 columns
	Right digit will show in columns 5, 6, 7 for rows 1,2,3,4,5
*/
uint16_t* getRightDigit(uint8_t value)
{
	// Allocate size of 8 uint16_t
	uint16_t *values = malloc(8 * sizeof(uint16_t));
	values[0] = 0x0800;	// Clear top row
	values[6] = 0x0200; // Clear second to last row
	values[7] = 0x0100; // Clear last row
	switch (value){
		case 0:
			SET_ROW_R(values, 1, lll)	// .....***
			SET_ROW_R(values, 2, l_l)	// .....* *
			SET_ROW_R(values, 3, l_l)	// .....* *
			SET_ROW_R(values, 4, l_l)	// .....* *
			SET_ROW_R(values, 5, lll)	// .....***
			break;
		case 1:
			SET_ROW_R(values, 1, _l_)	// ..... *
			SET_ROW_R(values, 2, ll_)	// .....**
			SET_ROW_R(values, 3, _l_)	// ..... *
			SET_ROW_R(values, 4, _l_)	// ..... *
			SET_ROW_R(values, 5, lll)	// .....***
			break;
		case 2:
			SET_ROW_R(values, 1, ll_)	// .....**
			SET_ROW_R(values, 2, __l)	// .....  *
			SET_ROW_R(values, 3, lll)	// .....***
			SET_ROW_R(values, 4, l__)	// .....*
			SET_ROW_R(values, 5, lll)	// .....***
			break;
		case 3:
			SET_ROW_R(values, 1, lll)	// .....***
			SET_ROW_R(values, 2, __l)	// .....  *
			SET_ROW_R(values, 3, lll)	// .....***
			SET_ROW_R(values, 4, __l)	// .....  *
			SET_ROW_R(values, 5, lll)	// .....***
			break;
		case 4:
			SET_ROW_R(values, 1, l_l)	// .....* *
			SET_ROW_R(values, 2, l_l)	// .....* *
			SET_ROW_R(values, 3, lll)	// .....***
			SET_ROW_R(values, 4, __l)	// .....  *
			SET_ROW_R(values, 5, __l)	// .....  *
			break;
		case 5:
			SET_ROW_R(values, 1, lll)	// .....***
			SET_ROW_R(values, 2, l__)	// .....*
			SET_ROW_R(values, 3, lll)	// .....***
			SET_ROW_R(values, 4, __l)	// .....  *
			SET_ROW_R(values, 5, lll)	// .....***
			break;
		case 6:
			SET_ROW_R(values, 1, lll)	// .....***
			SET_ROW_R(values, 2, l__)	// .....*
			SET_ROW_R(values, 3, lll)	// .....***
			SET_ROW_R(values, 4, l_l)	// .....* *
			SET_ROW_R(values, 5, lll)	// .....***
			break;
		case 7:
			SET_ROW_R(values, 1, lll)	// .....***
			SET_ROW_R(values, 2, __l)	// .....  *
			SET_ROW_R(values, 3, __l)	// .....  *
			SET_ROW_R(values, 4, __l)	// .....  *
			SET_ROW_R(values, 5, __l)	// .....  *
			break;
		case 8:
			SET_ROW_R(values, 1, lll)	// .....***
			SET_ROW_R(values, 2, l_l)	// .....* *
			SET_ROW_R(values, 3, lll)	// .....***
			SET_ROW_R(values, 4, l_l)	// .....* *
			SET_ROW_R(values, 5, lll)	// .....***
			break;
		case 9:
			SET_ROW_R(values, 1, lll)	// .....***
			SET_ROW_R(values, 2, l_l)	// .....* *
			SET_ROW_R(values, 3, lll)	// .....***
			SET_ROW_R(values, 4, __l)	// .....  *
			SET_ROW_R(values, 5, __l)	// .....  *
			break;
	}
	return values;
}

/*
	Creates the digit in specific row
*/
uint16_t createDigitRow(uint8_t row, uint8_t value, uint8_t side)
{
	if (!isBounded(row, 0, BOARD_WIDTH))
		return 0;
	if (!isBounded(value, 0, 10))
		value %= 10;	// Force value to be a digit
	uint16_t data = (8 - row) << 8;
	data |= (value << side);
	return data | (value << side);
}