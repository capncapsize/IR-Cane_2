/*
 * user_interface.c
 *
 *  Created on: Mar 11, 2016
 *      Author: Capsize
 */

#include "user_interface.h"

char bToggleState = 0;

/*GPIO PC9..6*/
int init_UI(void){
	 	GPIO_InitTypeDef GPIO_InitStruct;
	    EXTI_InitTypeDef EXTI_InitStruct;
	    NVIC_InitTypeDef NVIC_InitStruct;

	    /* Enable clock for GPIOC */
	    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	    /* Enable clock for SYSCFG */
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	    /* Set pin as input */
	    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	    GPIO_Init(GPIOC, &GPIO_InitStruct);

	    /* Tell system that you will use PB12 for EXTI_Line9 */
	    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource9);

	    /* PB12 is connected to EXTI_Line9 */
	    EXTI_InitStruct.EXTI_Line = EXTI_Line9;
	    /* Enable interrupt */
	    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	    /* Interrupt mode */
	    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	    /* Triggers on rising and falling edge */
	    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; //EXTI_Trigger_Rising_Falling;
	    /* Add to EXTI */
	    EXTI_Init(&EXTI_InitStruct);

	    /* Add IRQ vector to NVIC */
	    /* PB9 is connected to EXTI_Line9, which has EXTI9_5_IRQn vector */
	    NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
	    /* Set priority */
	    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	    /* Set sub priority */
	    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x01;
	    /* Enable interrupt */
	    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	    /* Add to NVIC */
	    NVIC_Init(&NVIC_InitStruct);

	    return 1;
}

void EXTI9_5_IRQHandler(void) {
    /* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line9) != RESET) {
        /* Do your stuff when PC9 is changed */
    	bToggleState ^= 0x1;
    	set_tim1_pulse(3 - 2*bToggleState);

    	//volatile int i;
    	//for(i=0;i<0x1fffff;i++);

    	EXTI_ClearITPendingBit(EXTI_Line9);
        /* Clear interrupt flag */

    }
}
