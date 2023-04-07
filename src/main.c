/******************************************************************************
 *
 * Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 ******************************************************************************/

/*
 *
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

// Main program for exercise

//****************************************************
// By default, every output used in this exercise is 0
//****************************************************
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "sleep.h"
#include "xgpiops.h"
#include "xttcps.h"
#include "xscugic.h"
#include "xparameters.h"
#include "Pixel.h"
#include "Interrupt_setup.h"
#include "alien_neccesary.h"

//********************************************************************
//***************TRY TO READ COMMENTS*********************************
//********************************************************************

//***Hint: Use sleep(x)  or usleep(x) if you want some delays.****
//***To call assembler code found in blinker.S, call it using: blinker();***

// Comment this if you want to disable all interrupts
#define enable_interrupts

/***************************************************************************************
Name: Long Nguyen

Led-matrix driver		Game		    Assembler
	[x]					[x]					[x]

Brief description:
In this game, we will try to hit the point by the alien. The alien will move from left to right and vice versa.
The player will use the button to shoot the alien.

If the player hit the alien, the player will gain 1 point.
If the player miss the alien, the player will lose 1 point.

The game will end when the player gain 3 points or lose 3 points.
The player will win if the player gain 3 points and lose if the player lose 3 points.

The player will use the button to shoot the alien.
Button LD0: Reset the game
Button LD1: Move the ship to the right
Button LD2: Shoot the alien (point)
Button LD3: Move the ship to the left
Switch SW0: Modify the game's speed
Switch SW1: Pause the game
*****************************************************************************************/

// Alien points
volatile uint8_t alien_x = 0;
volatile uint8_t alien_y = 0;
volatile uint8_t alien_r = 100;
volatile uint8_t alien_g = 100;
volatile uint8_t alien_b = 100;

// Alien appearance
volatile uint8_t x = 1;
volatile uint8_t y = 7;
volatile uint8_t r = 0;
volatile uint8_t g = 255;
volatile uint8_t b = 0;

// Alien bullet
volatile uint8_t bullet_x = 0;
volatile uint8_t bullet_y = 0;
volatile uint8_t bullet_exist = 0;
volatile uint8_t bullet_r = 0;
volatile uint8_t bullet_g = 0;
volatile uint8_t bullet_b = 255;
volatile uint8_t left = 0;
volatile uint8_t right = 1;

// Points gain colour
volatile uint8_t point_r_gain = 0;
volatile uint8_t point_g_gain = 255;
volatile uint8_t point_b_gain = 0;

// Points lost colour
volatile uint8_t point_r_lost = 255;
volatile uint8_t point_g_lost = 000;
volatile uint8_t point_b_lost = 000;

// Status
volatile uint8_t points_gain = 0;
volatile uint8_t points_lost = 0;
volatile uint8_t game_status = 1;

// Tick handler
volatile uint8_t chnl = 0;
// Speed of the game
volatile uint32_t freq = 5;
volatile uint32_t speed = 3;
volatile uint32_t count = 0;

int main()
{
	//**DO NOT REMOVE THIS****
	init_platform();
	//************************

#ifdef enable_interrupts
	init_interrupts();
#endif
	// setup screen
	setup();

	Xil_ExceptionEnable();
	// Try to avoid writing any code in the main loop.
	while (1)
	{
	}
	cleanup_platform();
	return 0;
}

// Timer interrupt handler for led matrix update. Frequency is 800 Hz
void TickHandler(void *CallBackRef)
{
	// Don't remove this
	uint32_t StatusEvent;

	// Exceptions must be disabled when updating screen
	Xil_ExceptionDisable();

	if (chnl > 7)
		chnl = 0;

	//****Write code here ****
	// Close all channel
	open_line(100);

	// open and run channel
	run(chnl);
	open_line(chnl);
	chnl++;

	//****END OF OWN CODE*****************

	//*********clear timer interrupt status. DO NOT REMOVE********
	StatusEvent = XTtcPs_GetInterruptStatus((XTtcPs *)CallBackRef);
	XTtcPs_ClearInterruptStatus((XTtcPs *)CallBackRef, StatusEvent);
	//*************************************************************
	// enable exceptions
	Xil_ExceptionEnable();
}

// Timer interrupt for moving alien, shooting... Frequency is 10 Hz by default
void TickHandler1(void *CallBackRef)
{

	// Don't remove this
	uint32_t StatusEvent;

	//****Write code here ****
	// first appearance of alien points
	if (game_status == 1)
	{
		// Create the alien (point)
		SetPixel(alien_x, alien_y, alien_r, alien_g, alien_b);
		// Create the ship
		ship_appearance();
		// check if it reach the end of matrix and change direction
		if (alien_x == 0 && alien_y == 7 && left == 1 && right == 0)
		{
			SetPixel(alien_x, alien_y, 0, 0, 0);
			alien_x = 0;
			alien_y = 0;
			right = 1;
			left = 0;
		}
		else
		{
			// check if it touch the ship on the top
			if (alien_x == x && alien_y == y - 1)
			{
				hit_alien();
			}
			// check if it touch the ship on the right
			else if (alien_x == x + 1 && alien_y == y)
			{
				hit_alien();
			}
			// check if it touch the ship on the left
			else if (alien_x == x - 1 && alien_y == y)
			{
				hit_alien();
			}
			// change the speed of the points according to the speed
			if (count == speed)
			{
				move_point();
				count = 0;
			}
			else
				count += 1;
		}
	}

	// check if player click "hit" button and create the bullet
	if (bullet_exist == 1)
	{
		move_bullet();
		// check if the bullet touch the alien
		if (alien_x == bullet_x && alien_y == bullet_y)
		{
			hit_bullet();
		}
	}

	// check if the game is over
	if (points_gain == 3 || points_lost == 3)
	{
		// check if the player won
		if (points_gain == 3)
		{
			printwin();
		}
		// check if the player lost
		if (points_lost == 3)
		{
			printloss();
		}
	}
	// Assembler function to update the light on the top of the button
	blinker();

	//****END OF OWN CODE*****************
	// clear timer interrupt status. DO NOT REMOVE
	StatusEvent = XTtcPs_GetInterruptStatus((XTtcPs *)CallBackRef);
	XTtcPs_ClearInterruptStatus((XTtcPs *)CallBackRef, StatusEvent);
}

// Interrupt handler for switches and buttons.
// Reading Status will tell which button or switch was used
// Bank information is useless in this exercise
void ButtonHandler(void *CallBackRef, u32 Bank, u32 Status)
{
	//****Write code here ****
	// Hint: Status==0x01 ->btn0, Status==0x02->btn1, Status==0x04->btn2, Status==0x08-> btn3, Status==0x10->SW0, Status==0x20 -> SW1

	// If true, btn0 was used to trigger interrupt
	if (Status == 0x01)
	{
		resetgame();
	}
	// BTN1 is clicked
	else if (Status == 0x02)
	{
		if (game_status == 1)
			move_ship_right();
	}
	// BTN2 is clicked
	else if (Status == 0x04)
	{
		// In case the player want to shoot only when the bullet is not exist, uncomment the following line and comment the next line
		// if (bullet_exist == 0 && game_status == 1) 
		if (game_status == 1)
			initial_bullet();
	}
	// BTN3 is clicked
	else if (Status == 0x08)
	{
		if (game_status == 1)
			move_ship_left();
	}
	// SW0 position is changed. 0xE000A068 address needs to be read if you want to know in which position slider is
	else if (Status == 0x10)
	{
		// Change the speed of the point and the game
		if (game_status == 1)
		{
			if (freq == 5)
			{
				freq = 10;
				change_freq(freq);
			}
			else
			{
				freq = 5;
				change_freq(freq);
			}
		}
	}
	// SW1 position is changed. 0xE000A068 address needs to be read if you want to know in which position slider is
	else if (Status == 0x20)
	{
		// Pause the game and resume it
		if (game_status == 0)
		{
			game_status = 1;
		}
		else if (game_status == 1)
		{
			game_status = 0;
		}
	}

	//****END OF OWN CODE*****************
}
