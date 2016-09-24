/*
 * serial_com.h
 *
 *  Created on: Feb 9, 2016
 *      Author: daveng-2
 */

#ifndef SERIAL_COM_H_
#define SERIAL_COM_H_

#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include "timer_conf.h"
#include "power_mode.h"

#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
//
//
//struct test_struct
//{
//	char data;
//    struct test_struct *next;
//};

typedef struct data_block *block;

int init_USART2(void);
int serial_send_data(char* c);
int serial_get_data();

//struct test_struct* create_list(char data);
//struct test_struct* add_to_list(char data, bool add_to_end);
//char get_head();

//struct test_struct* create_list(char data, struct test_struct* head, struct test_struct* curr);
//struct test_struct* add_to_list(char data, bool add_to_end, struct test_struct* head, struct test_struct* curr);
//char get_head(struct test_struct* head);

#endif /* SERIAL_COM_H_ */
