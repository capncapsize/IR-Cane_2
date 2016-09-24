/*
 * timer_conf.c
 *
 *  Created on: Feb 16, 2016
 *      Author: daveng-2
 */

#include "timer_conf.h"


char str[15];
char sc[6];



void add_measurement(float measurement);
float mean_measurment();
void MA_filter(float sample, float a);
float spike_mitigation();
int compare(const void * a, const void * b);


uint16_t current_count = 0;

/* Timer 1 */
uint16_t tim1_pulse = 0x2;
uint16_t tim1_period = 0x10;

uint16_t modulation = 0;
uint16_t DCGain = 0;


uint16_t i = 0;

/* Timer 2 */
uint16_t tim2_it_per_sec = 10;
uint16_t tim2_pulse = ((CNT_FREQ) / (1));
char noSignalPing = 0;
int distBufferLen = 5;
float distBuffer[5] = {};
float samples[5] = {};
float meanDist = 0;
float filterDist = 0;
float dist;




int init_TIM1(void){
	/* Init struct*/
	GPIO_InitTypeDef		GPIO_InitStructure;
	NVIC_InitTypeDef 		NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TBInitStructure;
	TIM_OCInitTypeDef 		TIM_OCInitStructure;

	/* Enable clock on peripherals */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* Enable TIM1-CH4 on PA11*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_TIM1);

	/* Enable compare interrupt on TIM1 */
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Set clock time base */
//	TIM_TBInitStructure.TIM_ClockDivision = 1;
//	TIM_TBInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TBInitStructure.TIM_Period = 65535;
//	TIM_TBInitStructure.TIM_Prescaler = TIM_PRESCALER;
//	TIM_TimeBaseInit(TIM1, &TIM_TBInitStructure);

	TIM_TBInitStructure.TIM_ClockDivision = 1;
	TIM_TBInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TBInitStructure.TIM_Period = 50;
	TIM_TBInitStructure.TIM_Prescaler = TIM_PRESCALER;
	TIM_TimeBaseInit(TIM1, &TIM_TBInitStructure);

	/* Set clock output channel 4 on PA11 audio out */
	//TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

	/* Set clock for modulation sine frequency step */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = 0x3;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);




	/* Disable OPM */
	//TIM1->CR1 &= ~(1 << 4);

	TIM_ITConfig(TIM1, TIM_IT_CC4, ENABLE);
	TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);
	//TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);

	TIM1->CR1 |= TIM_CR1_CEN;
	it_tim1_state(ENABLE);

	/* Start Waveform */
	TIM1->CCR4 = tim1_pulse;
	TIM1->ARR = tim1_period;
	DCGain = 2;

	return 1;
}


void TIM1_CC_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM1, TIM_IT_CC4) != RESET)             // Just a precaution (RESET = 0)
  {
    TIM_ClearITPendingBit(TIM1, TIM_IT_CC4);                  // Clear TIM1 Ch.1 flag
    //current_count = TIM_GetCapture1(TIM1);                   // Get current counter value
    //TIM_SetCompare4(TIM1, tim1_pulse+modulation);        // Set Output Compare 1 to the new value
   // TIM1->ARR = 3 + modulation;
   // modulation = (int)(2*sinTab(i));
  //  i++;
    TIM1->ARR = tim1_period + modulation;
    modulation = (int)(DCGain*sinTab(i));
    //i++;
  }
  if (TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET)             // Just a precaution (RESET = 0)
   {
     TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);                  // Clear TIM1 Ch.1 flag
     i++;
   }
}

int set_tim1_modulation(int gain){
	if(tim1_period-gain-1 > tim1_pulse){
	DCGain = gain + 1;
	return 1;
	}
	return 0;
}


int set_tim1_frequency(int Hz){
	if((CNT_FREQ/(Hz+1) > 2*tim1_pulse) && ((int)(CNT_FREQ/(Hz+1))-DCGain-1 > tim1_pulse)){
		tim1_period = (int)(CNT_FREQ/(Hz+1));
		return 1;
	}
	return 0;
}

int set_tim1_pulse(int ms){
	uint16_t tim1_it_per_sec;
	tim1_it_per_sec = 1000/ms;
	if((tim1_period > ((CNT_FREQ) / (tim1_it_per_sec))) && (tim1_period-DCGain-1 > ((CNT_FREQ) / (tim1_it_per_sec)))){
		tim1_pulse = ((CNT_FREQ) / (tim1_it_per_sec));
		TIM_SetCompare4(TIM1, tim1_pulse);
		return 1;
	}
	return 0;
}

void it_tim1_state(int state){
	if (state != DISABLE)
	  {
	    /* Enable the TIM Counter */
	    TIM1->CR1 |= TIM_CR1_CEN;
	  }
	  else
	  {
	    /* Disable the TIM Counter */
	    TIM1->CR1 &= (uint16_t)~TIM_CR1_CEN;
	  }
}


int init_TIM2(void){
	/* Init struct*/
	NVIC_InitTypeDef 		NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TBInitStructure;
	TIM_OCInitTypeDef 		TIM_OCInitStructure;

	/* Enable clock on peripherals */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* Enable compare interrupt on TIM2 */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Set clock time base */
	TIM_TBInitStructure.TIM_ClockDivision = 1;
	TIM_TBInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TBInitStructure.TIM_Period = 65535;
	TIM_TBInitStructure.TIM_Prescaler = TIM_PRESCALER;
	TIM_TimeBaseInit(TIM2, &TIM_TBInitStructure);

	/* Set clock OC*/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_Pulse = tim2_pulse;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Disable);

	/* Disable OPM */
	TIM2->CR1 &= ~(1 << 4);

	TIM2->CR1 &= ~(TIM_CR1_CEN);
	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);

	TIM2->CR1 |= TIM_CR1_CEN;

	set_tim2_pulse(40);


	return 1;
}

void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)             // Just a precaution (RESET = 0)
  {
	/* Start ADC1 Software Conversion */
	//ADC_SoftwareStartConv(ADC1);
	//while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
//	uint16_t in = 3800 - ADC_GetConversionValue(ADC1);
//	double lenVal = (double)in/66.666f + 20;
//	gcvt(lenVal, 5, sc);
//	snprintf(str,15, "%s cm\r",sc);

	//sprintf(str, "%i %i\r", ADCConvVal[0], ADCConvVal[1]);
	//serial_send_data(&str[0]);
	//sprintf(str, "SR%i:LR%i\r",(int)distanceLongSensor(ADCConvVal[0]),(int)distanceLongSensor(ADCConvVal[1]));
	//serial_send_data(&str[0]);

	//ADCConvVal[0] = ADCConvVal[0] - 450;
	if((ADCConvVal[0] > 588)){
		/*Reset Timer*/
		if(noSignalPing){
			TIM1->SR |= TIM_IT_CC4;
			TIM1->CNT = 0x0;
		}
		noSignalPing = 0;
		set_tim1_modulation(0);
		dist = distanceShortSensor(ADCConvVal[0]);
		add_measurement(dist);
		set_tim1_frequency(85 - (int)(filterDist*0.25));
		sprintf(str, "SR%i:%i\r",(int)filterDist, (int)dist);
		//sprintf(str, "%i\r", ADCConvVal[0]);
		serial_send_data(&str[0]);
	}
	else if((ADCConvVal[1] > 1700)){
		/*Reset Timer*/
		if(noSignalPing){
			TIM1->SR |= TIM_IT_CC4;
			TIM1->CNT = 0x0;
		}
		noSignalPing = 0;
		set_tim1_modulation(0);
		dist = distanceLongSensor(ADCConvVal[1]);
		add_measurement(dist);
		set_tim1_frequency(85 - (int)(filterDist*0.25));
		sprintf(str, "LR%i:%i\r",(int)filterDist,(int)dist);
		//sprintf(str, "%i\r", ADCConvVal[1]);
		serial_send_data(&str[0]);
	}else{
		set_tim1_modulation(0);
		set_tim1_frequency(1);
		noSignalPing = 1;
	}

	//sprintf(str, "%i:%i:%i:%i\r",(int)distBuffer[0],(int)distBuffer[1],(int)distBuffer[2],(int)distBuffer[3]);
	//serial_send_data(&str[0]);

    TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);                  // Clear TIM1 Ch.1 flag
    current_count = TIM_GetCapture1(TIM2);                   // Get current counter value
    TIM_SetCompare1(TIM2, current_count + tim2_pulse);        // Set Output Compare 1 to the new value
  }
}

void set_tim2_pulse(int ms){
	tim2_it_per_sec = 1000/ms;
	tim2_pulse = ((CNT_FREQ) / (tim2_it_per_sec));
}

void add_measurement(float measurement){
	int i;
	float sample;
	for(i=distBufferLen-1; i>0; i--){
		distBuffer[i] = distBuffer[i-1];
	}
	distBuffer[0] = measurement;
	//meanDist = mean_measurment();
	sample = spike_mitigation();
	MA_filter(sample, 0.9);

}

float mean_measurment(){
	int i;
	float sum = 0;
	for(i=0; i<distBufferLen; i++){
		sum = sum + distBuffer[i];
	}
	return sum/10;
}

void MA_filter(float sample, float a){
	filterDist = a*sample + (1-a)*filterDist;
}

float spike_mitigation(){
	int i;
	for(i=0;i<distBufferLen;i++){
		samples[i] = distBuffer[i];
	}

	qsort(samples, distBufferLen, sizeof(float), compare);

	return samples[2];

}

int compare(const void * a, const void * b){
 return ( *(int*)a - *(int*)b );
}
