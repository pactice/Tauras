#ifndef __BSP_USART_H__
#define __BSP_USART_H__

#include "usart.h"

#define 	DBUS_HUART    huart1
#define   JUDGE_HUART   huart2
#define		TOF_HUART			huart3

#define		RELAY_HUART		huart5
#define   DIYREMOTE_HUART		huart6

#define  DMA_DBUS_LEN					50
#define  DMA_JUDGE_LEN				200
#define  DMA_DIYREMOTE_LEN   	39//10
#define  DMA_TOF_LEN   				80


void user_uart_init(void);
void Relay_Read_Data();
void USER_UART_IRQHandler(UART_HandleTypeDef *huart);
void USER_UART_IDLECallback(UART_HandleTypeDef *huart);

#endif

