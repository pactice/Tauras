#ifndef __BSP_CIRCLE_H
#define __BSP_CIRCLE_H
#include "stm32f4xx.h"

#define  RINGBUFF_LEN          (500)     //»º³åÇø³¤¶È
#define  RINGBUFF_OK           1     		
#define  RINGBUFF_ERR          0   

typedef struct
{
		uint8_t *buffer;
    uint16_t *pHead;       
		uint16_t *pValidRead;
		uint16_t *pValidWrite;
    uint16_t *pTail;
    uint16_t Lenght;
}RingBuff_t;

extern RingBuff_t ringBuff;
void RingBuff_Init(RingBuff_t *ringbuff);
int isBufferFull(RingBuff_t *ringbuff);
int isBufferEmpty(RingBuff_t *ringbuff);
uint8_t writeBuffer(RingBuff_t *ringbuf, uint8_t *data,uint8_t length);
uint8_t readBuffer(RingBuff_t *ringbuf,uint8_t *data,uint8_t length);

#endif