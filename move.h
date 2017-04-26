#include "open_interface.h"


void move(oi_t *sensor, int centimeters);

void turn(oi_t *sensor, int degrees);
void escape(oi_t *sensor);

/*
	    90
		^
		|
180 <---+---> 0
		|
		v
	  270

 int abs_angle;
 float abs_position_x;
 float abs_position_y;
*/

