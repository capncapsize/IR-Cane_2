/**
*****************************************************************************
**
**  File        : main.c
**
**  Abstract    : main function.
**
**  Functions   : main
**
**  Environment : Atollic TrueSTUDIO(R)
**                STMicroelectronics STM32F4xx Standard Peripherals Library
**
**  Distribution: The file is distributed "as is", without any warranty
**                of any kind.
**
**  (c)Copyright Atollic AB.
**  You may use this file as-is or modify it according to the needs of your
**  project. This file may only be built (assembled or compiled and linked)
**  using the Atollic TrueSTUDIO(R) product. The use of this file together
**  with other tools than Atollic TrueSTUDIO(R) is not permitted.
**
*****************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include "stdio.h"
#include "stdlib.h"

#include "serial_com.h"
#include "clock_conf.h"
#include "timer_conf.h"
#include "analog_converter.h"
#include "user_interface.h"



void sys_init(void);

/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/

int main(void)
{
  __disable_irq();
  sys_init();
  __enable_irq();


  serial_send_data("System: OK!\r");

  volatile i = 0;

  while (1){
	  //i++;
	  __WFI();
	  //serial_get_data();
  }
  return 0;
}


void sys_init(void){

	  RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOARST;    // Reset GPIOA
	  RCC->AHB1RSTR = 0;                         // Exit reset state

	  /*Set system clock and configure RCC*/
	  init_clock();
	  /* Init serial com */
	  init_USART2();
	  /* Init LED blink timer */
	  init_TIM1();
	  /* Init ADC sample */
	  init_TIM2();
	  /* Init ADC1 */
	  init_ADC1_CH8();
	  /* Init peripheral button */
	  //init_UI();
}



