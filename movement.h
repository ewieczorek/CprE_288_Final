/*
 * Ethan Wieczorek
 * Josh Rolles
 *
 * Created 1/25/17
 *
 *
 */

#ifndef MOVEMENT_H_
#include "open_interface.h";
#define MOVEMENT_H_

void move_forward(int centimeters);
void move_backward(int centimeters);
void turn_clockwise(int degrees);
void turn_counter_clockwise(int degrees);
void maneuver(oi_t *sensor_data);
void move(int centimeters);
void turn(int angle);


/*
	    90
		^
		|
180 <---+---> 0
		|
		v
	  270
*/

 int abs_angle;
 float abs_position_x;
 float abs_position_y;

#endif
