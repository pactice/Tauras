#ifndef __BSP_CIRCLE_H
#define __BSP_CIRCLE_H
#include "stm32f4xx.h"

#define  RINGBUFF_LEN          (500)     //»º³åÇø³¤¶È
#define  RINGBUFF_OK           1     		
#define  RINGBUFF_ERR          0   

typedef struct
{
		uint8_t buffer[RINGBUFF_LEN];
    uint16_t Head;       
    uint16_t Tail;
    uint16_t Length;
}RingBuff_t;

extern RingBuff_t ringBuff;

void RingBuff_Init(RingBuff_t *ringbuff);
int RingBuffer_IsFull(RingBuff_t *ringbuff);
int RingBuffer_IsEmpty(RingBuff_t *ringbuff);
int RingBuffer_Write(RingBuff_t *ringbuff,uint8_t data);
int RingBuffer_Read(RingBuff_t *ringbuff,uint8_t *data);
uint16_t RingBuffer_GetRemainSize(RingBuff_t *ringbuff);
uint16_t RingBuffer_GetWriteSize(RingBuff_t *ringbuff);
int RingBuffer_WriteData(RingBuff_t *ringbuff,uint8_t* data,uint16_t length);
int RingBuffer_ReadData(RingBuff_t *ringbuff,uint8_t* data,uint16_t length);



#endif