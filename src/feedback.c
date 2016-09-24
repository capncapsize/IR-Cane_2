/*
 * feedback.c
 *
 *  Created on: Mar 10, 2016
 *      Author: daveng-2
 */

#define GRADIENT 0
#define CONSTANT 0
#include "feedback.h"

float distanceShortSensor(float signal)
{
    float distance;
    distance = 1/((signal-(280.1772794+CONSTANT))/(24831.67395+GRADIENT));
    return distance;
}

float distanceLongSensor(float signal)
{
	float distance;
	distance = 1/((signal - 1284.671904)/160460.5137);
	return distance;
}
