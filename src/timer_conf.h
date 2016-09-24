/*
 * timer_conf.h
 *
 *  Created on: Feb 16, 2016
 *      Author: daveng-2
 */

#ifndef TIMER_CONF_H_
#define TIMER_CONF_H_

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include <stdio.h>
#include <stdlib.h>
#include "serial_com.h"
#include "analog_converter.h"
#include "math_standard.h"
#include "feedback.h"
#include "power_mode.h"

#define   APB1_FREQ        84000000                           // Clock driving TIM1
#define   CNT_FREQ         2000                           // TIM1 counter clock (prescaled APB1)
//#define   IT_PER_SEC       1                              // Interrupts per second
//#define   TIM1_PULSE       ((CNT_FREQ) / (IT_PER_SEC))        // Output compare reg value
#define   TIM_PRESCALER    (((APB1_FREQ) / (CNT_FREQ))-1)


int init_TIM1(void);
int set_tim1_modulation(int gain);
int set_tim1_frequency(int Hz);
int set_tim1_pulse(int ms);
void it_tim1_state(int state);

int init_TIM2(void);
void set_tim2_pulse(int ms);

#endif /* TIMER_CONF_H_ */
