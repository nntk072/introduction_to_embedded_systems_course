#ifndef ALIEN_NECESSARY_H_
#define ALIEN_NECESSARY_H_

// Before the game
void ship_appearance();

// In the game

// Moving the ship and the bullet and the point (alien)
void move_bullet();
void move_point();
void move_ship_left();
void move_ship_right();
void initial_bullet();

// Response to the collision
void hit_alien();
void hit_bullet();
void points_gain_func();
void points_lost_func();

// End the game
void printwin();
void printloss();

// Restart the game
void resetgame();
#endif
