/*
 * serial_com.c
 *
 *  Created on: Feb 9, 2016
 *      Author: daveng-2
 */

#include "serial_com.h"


int power(int base, int ex);

static block dequeue(block *queue);
static void enqueue(block p, block *queue);
int length_of_cmd(block *queue);
static void flushTo(block *Squeue, block *Dqueue);
static void initialize(void);

#define NBLOCKS        25


struct data_block {
    char c;
    block next;
};

struct data_block blocks[NBLOCKS];

struct data_block initp;

block freeQ   = blocks;
block TxQ  = NULL;
block RxQ  = NULL;
block current = &initp;

int initialized = 0;
int RxQ_ready = 0;
int RxQ_flush = 0;

char Serr[] = "SynErr: ";
char Lerr[] = "OverflowErr\r";
char Perr[] = "Param limit\r";
char status[] = "status ";
char* on = "on\r";
char* off = "off\r";
char* blink = "blink";
char* freq = "freq";
char* modl = "modl";

int init_USART2(void){

	  /* Enable clock on peripherals */
	  RCC->APB1ENR |= RCC_APB1Periph_USART2;
	  RCC->AHB1ENR |= RCC_AHB1Periph_GPIOA;

	  /* Set output mode to AF */
	  GPIOA->MODER |= GPIO_Mode_AF << (GPIO_PinSource2*2);
	  GPIOA->MODER |= GPIO_Mode_AF << (GPIO_PinSource3*2);

	  /* Output push pull mode */
	  GPIOA->OTYPER |= GPIO_OType_PP << (GPIO_PinSource2*1);
	  GPIOA->OTYPER |= GPIO_OType_PP << (GPIO_PinSource3*1);

	  /* Set pull-up mode */
	  GPIOA->PUPDR |= GPIO_PuPd_UP << (GPIO_PinSource2*2);
	  GPIOA->PUPDR |= GPIO_PuPd_UP << (GPIO_PinSource3*2);

	  /* Pin clock set */
	  GPIOA->OSPEEDR |= GPIO_Speed_2MHz << (GPIO_PinSource2*2);
	  GPIOA->OSPEEDR |= GPIO_Speed_2MHz << (GPIO_PinSource3*2);


	  /* Activate AF USART2_TX on PA2*/
	  GPIOA->AFR[0] &= ~(0xf << (GPIO_PinSource2*4));
	  GPIOA->AFR[0] |= GPIO_AF_USART2 << (GPIO_PinSource2*4);

	  /* Activate AF USART2_RX on PA3*/
	  GPIOA->AFR[0] &= ~(0xf << (GPIO_PinSource3*4));
	  GPIOA->AFR[0] |= GPIO_AF_USART2 << (GPIO_PinSource3*4);

	  /* Initiate USART */
	  USART2->CR1 |= USART_WordLength_8b | USART_Parity_No | (USART_Mode_Tx|USART_Mode_Rx);
	  USART2->CR2 |= USART_StopBits_1;
	  USART2->CR3 |= USART_HardwareFlowControl_None;

	  /*Set baudrate to 19200*/
	  USART2->BRR |= 84000000/(4*19200);

	  /* Enable Usart2 */
	  USART2->CR1 |= USART_CR1_UE;


	  USART2->CR1 |= USART_CR1_RXNEIE;	//RX interrupt
	  //USART2->CR1 |= USART_CR1_TXEIE;	//TX interrupt

	  NVIC_InitTypeDef NVIC_InitStruct;
	  NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	  NVIC_Init(&NVIC_InitStruct);

	return 0;
}

void USART2_IRQHandler(void){

	if(USART_GetITStatus(USART2, USART_IT_RXNE)){
		char c = (USART2->DR & (uint16_t)0x01FF);
		if(freeQ == NULL){
			RxQ_flush = 1;
		}
		block ptr = dequeue(&freeQ);
		ptr->c = c;
		enqueue(ptr, &RxQ);
		if(c == '\r'){
			RxQ_ready = 1;
		}
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
	if(USART_GetITStatus(USART2, USART_IT_TXE)){
		block s = dequeue(&TxQ);
		if(s == NULL){
			USART2->CR1 &= (~USART_FLAG_TXE);
		}else{
			USART2->DR = (s->c & (uint16_t)0x01FF);
		}
		enqueue(s, &freeQ);
		USART_ClearITPendingBit(USART2, USART_IT_TXE);
	}
}

static void initialize(void) {
    int i;
    for (i=0; i<NBLOCKS-1; i++)
        blocks[i].next = &blocks[i+1];
    blocks[NBLOCKS-1].next = NULL;

    initialized = 1;
}

static void enqueue(block p, block *queue) {
    p->next = NULL;
    if (*queue == NULL) {
        *queue = p;
    } else {
        block q = *queue;
        while (q->next)
            q = q->next;
        q->next = p;
    }
}

static block dequeue(block *queue) {
    block p = *queue;
    if (*queue) {
        *queue = (*queue)->next;
    } else {
        return NULL;
    }
    return p;
}

static void flushTo(block *Squeue, block *Dqueue){
	while(*Squeue != NULL){
		enqueue(dequeue(Squeue), Dqueue);
	}
}

int length_of_cmd(block *queue){
	int len = 0;
	block q = *queue;
	while (q != NULL){
		len++;
		if(q->c == '\r'){
			break;
		}
		q = q->next;
	}
	return len;
}

int serial_send_data(char* c){
	if(initialized == 0){
		initialize();
	}
	int i;
	for(i=0; *c != '\0'; i++){
		block ptr = dequeue(&freeQ);
		ptr->c = (*c & (uint16_t)0x01FF);
		enqueue(ptr, &TxQ);
		c++;
	}
	USART2->CR1 |= USART_FLAG_TXE;
	return (0);
}

int serial_get_data(){
	if(RxQ_ready == 0){
		if(RxQ_flush == 1){
			flushTo(&RxQ, &freeQ);
			RxQ_flush = 0;
			serial_send_data(&Lerr[0]);
		}
		return -1;
	}
	RxQ_ready = 0;
	block ptr;

	int len = length_of_cmd(&RxQ);
	char cmd_dat[len+1];
	char* cmd = &cmd_dat[0];
	volatile int arg = 0;
	int parse_error = 1;
	int pm = 0;
	int i;

	/* Ignore white-spaces */
	for(i=0; i<len; i++){
		if((RxQ->c == ' ') || (RxQ->c == '\t')){
			enqueue(dequeue(&RxQ), &freeQ);
		}else{
			break;
		}
	}
	/* Set the pointer at cmd start */
	cmd = &cmd_dat[i];

	/* Add data from buffer to string */
	for(i=i; i<len; i++){
		ptr = dequeue(&RxQ);
		if(ptr->c == ' '){
			pm = i+1;
		}
		cmd_dat[i] = ptr->c;
		enqueue(ptr, &freeQ);
	}
	cmd_dat[len] = '\0';

	//Echo
	//serial_send_data(cmd);


	/* Parse command */
	if(strcmp(cmd, on) == 0){
		it_tim1_state(ENABLE);
		serial_send_data(&status[0]);
		serial_send_data(on);
		parse_error = 0;
	}else if(strcmp(cmd, off) == 0){
		it_tim1_state(DISABLE);
		serial_send_data(&status[0]);
		serial_send_data(off);
		parse_error = 0;
	}else if(strncmp(cmd, blink, 5) == 0){
		for(i=pm; i<len; i++){
			if(cmd_dat[i] == '\r'){
				break;
			}
			arg += (cmd_dat[i]-48)*power(10,(len-i-2));
		}
		if((arg > 0) & (arg <= 1000)){
			it_tim1_state(ENABLE);
			if(!set_tim1_pulse(arg)){
				serial_send_data(&Perr[0]);
			}else{
				serial_send_data(&status[0]);
				serial_send_data(cmd);
			}
			parse_error = 0;
		}
	}else if(strncmp(cmd, freq, 4) == 0){
		for(i=pm; i<len; i++){
			if(cmd_dat[i] == '\r'){
				break;
			}
			arg += (cmd_dat[i]-48)*power(10,(len-i-2));
		}
		if((arg > 0) & (arg <= 1000)){
			it_tim1_state(ENABLE);
			if(!set_tim1_frequency(arg)){
				serial_send_data(&Perr[0]);
			}else{
				serial_send_data(&status[0]);
				serial_send_data(cmd);
			}
			parse_error = 0;
		}
	}else if(strncmp(cmd, modl, 4) == 0){
		for(i=pm; i<len; i++){
			if(cmd_dat[i] == '\r'){
				break;
			}
			arg += (cmd_dat[i]-48)*power(10,(len-i-2));
		}
		if((arg >= 0) & (arg <= 100)){
			it_tim1_state(ENABLE);
			if(!set_tim1_modulation(arg)){
				serial_send_data(&Perr[0]);
			}else{
				serial_send_data(&status[0]);
				serial_send_data(cmd);
			}
			parse_error = 0;
		}
	}

	if(parse_error){
		serial_send_data(&Serr[0]);
		serial_send_data(cmd);
	}
	return 0;
}

int power(int base, int ex){
	if(ex <= 0){
		return 1;
	}
	return base * power(base, ex - 1);
}


