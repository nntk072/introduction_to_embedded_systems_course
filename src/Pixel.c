/*
 * Pixel.c
 *
 *  Created on: 6/4/2023
 *      Author: Long Nguyen
 */

#include "Pixel.h"

// Table for pixel dots.
//				 dots[X][Y][COLOR]
volatile uint8_t dots[8][8][3] = {0};

// Control signals in page 12/21 Exercise guide
#define RSTn 0x01
#define BIT1 (0x01 << 1)
#define SB (0x01 << 2)
#define SCK (0x01 << 3)
#define SDA (0x01 << 4)

// Here the setup operations for the LED matrix will be performed
void setup()
{
	// Resetting the screen at start is a MUST to operation (Set RSTn to 1).
	// Set the control and channel registers to zero
	control = 0;
	channel = 0;

	control |= RSTn;
	usleep(500);
	control &= ~RSTn;
	usleep(500);

	control |= RSTn;
	usleep(500);

	control |= SDA;

	// Write code that sets 6-bit values in register of DM163 chip. Recommended that every bit in that register is set to 1. 6-bits and 24 "bytes", so some kind of loop structure could be nice.
	// 24*6 bits needs to be transmitted
	for (int i = 0; i < 24; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			// Set SDA to 1
			control |= SDA;
			// Set SCK to 0
			control &= ~SCK;
			// Set SCK to 1
			control |= SCK;
		}
	}
	// Final thing in this function is to set SB-bit to 1 to enable transmission to 8-bit register.
	control |= SB;
}

// Change value of one pixel at led matrix. This function is only used for changing values of dots array
void SetPixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b)
{
	// Hint: you can invert Y-axis quite easily with 7-y
	dots[x][y][0] = b;
	// Write rest of two lines of code required to make this function work properly (green and red colors to array).
	dots[x][y][1] = g;
	dots[x][y][2] = r;
}

// Put new data to led matrix. Hint: This function is supposed to send 24-bytes and parameter x is for channel x-coordinate.
void run(uint8_t x)
{

	// Write code that writes data to led matrix driver (8-bit data). Use values from dots array
	// Hint: use nested loops (loops inside loops)
	// Hint2: loop iterations are 8,3,8 (pixels,colour,8-bit data)
	for (uint8_t y = 0; y < 8; y++)
	{
		for (uint8_t color = 0; color < 3; color++)
		{
			// Read dots array to some temporally variable. This temporally variable is used in sending data
			uint8_t temp_variable = dots[x][y][color];
			for (uint8_t byte_count = 0; byte_count < 8; byte_count++)
			{
				if (temp_variable & 0x80)
					control |= SDA;
				else
					control &= ~SDA;
				control &= ~SCK;
				temp_variable <<= 1;
				control |= SCK;
			}
		}
	}
	latch();
}

// Latch signal. See colorsshield.pdf or DM163.pdf in project folder on how latching works
void latch()
{
	control |= BIT1;
	control &= ~BIT1;
}

// Set one line (channel) as active, one at a time.
void open_line(uint8_t x)
{
	switch (x)
	{
	case 0:
		channel |= 0x01;
		break;
	case 1:
		channel |= 0x02;
		break;
	case 2:
		channel |= 0x04;
		break;
	case 3:
		channel |= 0x08;
		break;
	case 4:
		channel |= 0x10;
		break;
	case 5:
		channel |= 0x20;
		break;
	case 6:
		channel |= 0x40;
		break;
	case 7:
		channel |= 0x80;
		break;
	default:
		channel = 0;
	}
}