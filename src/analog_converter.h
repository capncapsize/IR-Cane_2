/*
 * analog_converter.h
 *
 *  Created on: Mar 1, 2016
 *      Author: daveng-2
 */

#ifndef ANALOG_CONVERTER_H_
#define ANALOG_CONVERTER_H_

#include "stm32f4xx_adc.h"

#include "serial_com.h"

volatile uint16_t ADCConvVal[2];

void init_ADC1_CH8(void);



#endif /* ANALOG_CONVERTER_H_ */
