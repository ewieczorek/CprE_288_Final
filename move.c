#include "move.h"
#include "cliff.h"
#include "color.h"
#include "light.h"


void move(oi_t *sensor, int centimeters){
	if(centimeters == 0) return;

	int dist = 0;
	if(centimeters < 0)
		oi_setWheels(-300,-300);
	else
		oi_setWheels(300,300);

	while((centimeters < 0 && dist > centimeters) || (centimeters > 0 && dist < centimeters))
	{
		oi_update(sensor);
		dist += sensor->distance;
	}

	oi_setWheels(0,0);
}

void turn(oi_t *sensor, int degrees){
	if(degrees == 0) return;

	int angle = 0;
	if(degrees > 0)
		oi_setWheels(-200,200);
	else
		oi_setWheels(200,-200);

	while((degrees > 0 && angle > -degrees) || (degrees < 0 && angle < -degrees))
	{
		oi_update(sensor);
		angle += sensor->angle;
	}

	oi_setWheels(0,0);
}

void escape(oi_t *sensor)
{
	move(sensor,-15);

	turn(sensor,-90);
	move(sensor,25);

	turn(sensor,-90);
	move(sensor,15);
}

int checkSensors(){
	oi_t *sensorData = oi_alloc();
	oi_init(sensorData);

	int returner = 0;
	returner += checkCliffs(sensorData);
	returner += checkColors(sensorData);
	returner += checkLight(sensorData);

	return returner;
}
