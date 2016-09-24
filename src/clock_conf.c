/*
 * clock_conf.c
 *
 *  Created on: Feb 16, 2016
 *      Author: daveng-2
 */


#include "clock_conf.h"
void setsysclock24(void){

	// Enable HSE


}


int init_clock1(){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;


	// Output clock on MCO pin
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_AF_MCO;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//RCC_MCOConfig(RCC_MCO_SYSCLK);
	GPIOA->OSPEEDR = 0Xffffffff;

	RCC->CFGR &= ~0x00600000;
	RCC->CFGR |= 0x00600000;


	return 1;
}

int init_clock(){

    RCC->CFGR &= ~0xC0000000; //MCO2 resetted and set as SYSCLK
    RCC->CFGR &= ~0x38000000; //MCO2 prescaler resetted
    RCC->CFGR |= 0x00000000; //MCO2 prescaler = 4

	RCC->AHB1ENR |= 0x00000004; //GPIOC clock enable
	GPIOC->MODER |= 0x00080000; //GPIOC9 AM MCO2
	GPIOC->AFR[1] &= ~0x000000F0; //GPIOC9- AF
	GPIOC->OSPEEDR = 0xFFFFFFFF; //use gpio with high speed
	return 1;
}
