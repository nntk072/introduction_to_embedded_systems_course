/*
 * alien_necessary.c
 *
 *  Created on: Apr 7, 2023
 *      Author: Long Nguyen
 */
#include "alien_neccesary.h"
#include "Pixel.h"

// Ship appearance
extern uint8_t x;
extern uint8_t y;
extern uint8_t r;
extern uint8_t g;
extern uint8_t b;

// Alien (Point) appearance
extern uint8_t alien_x;
extern uint8_t alien_y;
extern uint8_t alien_r;
extern uint8_t alien_g;
extern uint8_t alien_b;
extern uint8_t left;
extern uint8_t right;

// Ship bullet
extern uint8_t bullet_x;
extern uint8_t bullet_y;
extern uint8_t bullet_exist;
extern uint8_t bullet_r;
extern uint8_t bullet_g;
extern uint8_t bullet_b;

// Points gain colour
extern uint8_t point_r_gain;
extern uint8_t point_g_gain;
extern uint8_t point_b_gain;

// Points lost colour
extern uint8_t point_r_lost;
extern uint8_t point_g_lost;
extern uint8_t point_b_lost;


// Status points
extern uint8_t points_gain;
extern uint8_t points_lost;
extern uint8_t game_status;
extern uint32_t count;

void ship_appearance()
{
	SetPixel(x + 1, y, r, g, b);
	SetPixel(x, y, r, g, b);
	SetPixel(x - 1, y, r, g, b);
	SetPixel(x, y - 1, r, g, b);
}

// In the game

// Moving the ship and the bullet and the point (alien)
void initial_bullet()
{
	bullet_exist = 1;
	bullet_x = x;
	bullet_y = y - 2;
	SetPixel(bullet_x, bullet_y, bullet_r, bullet_g, bullet_b);
}

void move_bullet()
{
	SetPixel(bullet_x, bullet_y, 0, 0, 0);
	if (bullet_y == 0)
	{
		SetPixel(bullet_x, bullet_y, 0, 0, 0);
		bullet_exist = 0;
	}
	bullet_y -= 1;
	SetPixel(bullet_x, bullet_y, bullet_r, bullet_g, bullet_b);
}

void move_point()
{
	// Delete old point
	SetPixel(alien_x, alien_y, 0, 0, 0);

	// Consider the coordinate of the point by using if functions
	// touching right side
	if (alien_x == 6 && left == 0 && right == 1)
	{
		left = 1;
		right = 0;
		alien_y += 1;
	}
	else if (alien_x == 0 && right == 0 && left == 1)
	{
		// touching left side
		left = 0;
		right = 1;
		alien_y += 1;
	}
	else if (right == 1)
	{
		// moving right
		alien_x += 1;
	}
	else if (left == 1)
	{
		// moving left
		alien_x -= 1;
	}
	// Draw new point
	SetPixel(alien_x, alien_y, alien_r, alien_g, alien_b);
}

void move_ship_left()
{
	// Delete old ship
	if (x > 0)
	{
		SetPixel(x + 1, y, 0, 0, 0);
		SetPixel(x, y, 0, 0, 0);
		SetPixel(x - 1, y, 0, 0, 0);
		SetPixel(x, y - 1, 0, 0, 0);
		x -= 1;
	}
}

void move_ship_right()
{
	// Consider the maximum coordinate of the ship
	if (x <= 5)
	{
		// Delete old ship
		SetPixel(x + 1, y, 0, 0, 0);
		SetPixel(x, y, 0, 0, 0);
		SetPixel(x - 1, y, 0, 0, 0);
		SetPixel(x, y - 1, 0, 0, 0);

		// Draw new ship
		x += 1;
	}
}

void points_gain_func()
{
	points_gain += 1;
	if (points_gain == 1)
	{
		SetPixel(7, 0, point_r_gain, point_g_gain, point_b_gain);
	}
	else if (points_gain == 2)
	{
		SetPixel(7, 1, point_r_gain, point_g_gain, point_b_gain);
	}
	else if (points_gain == 3)
	{
		SetPixel(7, 2, point_r_gain, point_g_gain, point_b_gain);
	}
}
void points_lost_func()
{
	points_lost += 1;
	if (points_lost == 1)
	{
		SetPixel(7, 3, point_r_lost, point_g_lost, point_b_lost);
	}
	else if (points_lost == 2)
	{
		SetPixel(7, 4, point_r_lost, point_g_lost, point_b_lost);
	}
	else if (points_lost == 3)
	{
		SetPixel(7, 5, point_r_lost, point_g_lost, point_b_lost);
	}
}
// Response to the collision
void hit_alien()
{
	// Delete the point
	SetPixel(alien_x, alien_y, r, g, b);
	// Initial the point
	count = 0;
	alien_x = 0;
	alien_y = 0;
	right = 1;
	left = 0;
	// Plus the points lost
	points_lost_func();
}

void hit_bullet()
{
	// Delete the bullet
	count = 0;
	alien_x = 0;
	alien_y = 0;
	right = 1;
	left = 0;
	SetPixel(bullet_x, bullet_y, 0, 0, 0);
	bullet_exist = 0;
	// Plus the points gain
	points_gain_func();
}

// End the game
void printwin()
{
	// Delete the matrix
	game_status = 0;
	for (int x = 0; x < 7; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			SetPixel(x, y, 0, 0, 0);
		}
	}
	SetPixel(7, 6, 0, 0, 0);
	SetPixel(7, 7, 0, 0, 0);

	// Print happy face on the matrix
	// Eyes
	SetPixel(1, 1, 0, 255, 0);
	SetPixel(0, 2, 0, 255, 0);
	SetPixel(2, 2, 0, 255, 0);
	SetPixel(5, 1, 0, 255, 0);
	SetPixel(4, 2, 0, 255, 0);
	SetPixel(6, 2, 0, 255, 0);

	// Nose
	SetPixel(3, 4, 0, 255, 0);
	SetPixel(3, 5, 0, 255, 0);

	// Mouth
	SetPixel(0, 5, 0, 255, 0);
	SetPixel(6, 5, 0, 255, 0);
	SetPixel(1, 6, 0, 255, 0);
	SetPixel(5, 6, 0, 255, 0);
	SetPixel(2, 7, 0, 255, 0);
	SetPixel(3, 7, 0, 255, 0);
	SetPixel(4, 7, 0, 255, 0);
}
void printloss()
{
	// Delete the matrix
	game_status = 0;
	for (int x = 0; x < 7; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			SetPixel(x, y, 0, 0, 0);
		}
	}
	SetPixel(7, 6, 0, 0, 0);
	SetPixel(7, 7, 0, 0, 0);

	// Print sad face on the matrix
	// Eyes
	SetPixel(0, 1, 255, 0, 0);
	SetPixel(2, 1, 255, 0, 0);
	SetPixel(1, 2, 255, 0, 0);

	SetPixel(4, 1, 255, 0, 0);
	SetPixel(6, 1, 255, 0, 0);
	SetPixel(5, 2, 255, 0, 0);

	// Nose
	SetPixel(3, 3, 255, 0, 0);

	// Mouth

	SetPixel(2, 5, 255, 0, 0);
	SetPixel(3, 5, 255, 0, 0);
	SetPixel(4, 5, 255, 0, 0);

	SetPixel(1, 6, 255, 0, 0);
	SetPixel(5, 6, 255, 0, 0);

	SetPixel(0, 7, 255, 0, 0);
	SetPixel(6, 7, 255, 0, 0);
}

// Restart the game
void resetgame()
{
	// Set the matrix to nothing
	for (int x = 0; x < 8; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			SetPixel(x, y, 0, 0, 0);
		}
	}

	game_status = 1;

	// Points coordinates
	alien_x = 0;
	alien_y = 0;
	left = 0;
	right = 1;

	// Alien appearance coordinates
	x = 1;
	y = 7;

	// Alien bullet
	bullet_x = 0;
	bullet_y = 0;

	// Status points
	points_gain = 0;
	points_lost = 0;
}
