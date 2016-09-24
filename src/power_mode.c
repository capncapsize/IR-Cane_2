/*
 * power_mode.c
 *
 *  Created on: Mar 21, 2016
 *      Author: Capsize
 */

#include "power_mode.h"


void sleepUnitlEvent(void){
	//__WFI();
	__ASM volatile ("nop");
}
