#include "bsp_circle.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "string.h"
RingBuff_t ringBuff;

/**
* @brief  RingBuff_Init
* @param  RingBuff_t *ringbuff,uint16_t length
* @return void
* @note   ��ʼ��
*/
void RingBuff_Init(RingBuff_t *ringbuff)
{
		 ringbuff->Head = 0;
		 ringbuff->Tail = 0;
}
/**
* @brief  Write_RingBuff
* @param  uint8_t data
* @return FLASE/TRUE
* @note   �жϻ��λ������Ƿ�����
*/
int RingBuffer_IsFull(RingBuff_t *ringbuff)
{
		return ((ringbuff->Head+1)%RINGBUFF_LEN == ringbuff->Tail);
}
/**
* @brief  Write_RingBuff
* @param  uint8_t data
* @return FLASE/TRUE
* @note   �жϻ��λ������Ƿ�����
*/
int RingBuffer_IsEmpty(RingBuff_t *ringbuff)
{
		return ringbuff->Head == ringbuff->Tail;
}
/**
* @brief  Write_RingBuff
* @param  uint8_t data
* @return FLASE/TRUE
* @note   д��һ������
*/
int RingBuffer_Write(RingBuff_t *ringbuff,uint8_t data)
{
	
	ringbuff->buffer[ringbuff->Head] = data;
	ringbuff->Head = (ringbuff->Head + 1)%RINGBUFF_LEN;
	if(RingBuffer_IsFull(ringbuff))
	{
		return RINGBUFF_ERR;
	}
	return RINGBUFF_OK;
}
/**
* @brief  Write_RingBuff
* @param  uint8_t data
* @return FLASE/TRUE
* @note   ��ȡһ������
*/
int RingBuffer_Read(RingBuff_t *ringbuff,uint8_t *data)
{
	if(RingBuffer_IsEmpty(ringbuff))
	{
		return RINGBUFF_ERR;
	}
	*data = ringbuff->buffer[ringbuff->Tail];
	ringbuff->Tail = (ringbuff->Tail+1) % RINGBUFF_LEN; 
	return RINGBUFF_OK;
}
/**
* @brief  Write_RingBuff
* @param  uint8_t data
* @return FLASE/TRUE
* @note   ��ȡ������ʣ���С
*/
uint16_t RingBuffer_GetRemainSize(RingBuff_t *ringbuff)
{
	if(ringbuff->Head >= ringbuff->Tail)
	{
		return RINGBUFF_LEN - (ringbuff->Head - ringbuff->Tail);
	}
	else{
		return ringbuff->Tail - ringbuff->Head;
	}
}
/**
* @brief  Write_RingBuff
* @param  uint8_t data
* @return FLASE/TRUE
* @note   ��ȡ��������д���С
*/
uint16_t RingBuffer_GetWriteSize(RingBuff_t *ringbuff)
{
	if(ringbuff->Head >= ringbuff->Tail)
	{
		return ringbuff->Head - ringbuff->Tail;
	}
	else
	{
		return RINGBUFF_LEN - (ringbuff->Tail - ringbuff->Head - 1);
	}
}
/**
* @brief  Write_RingBuff
* @param  uint8_t data
* @return FLASE/TRUE
* @note   д��ָ����������
*/
int RingBuffer_WriteData(RingBuff_t *ringbuff,uint8_t* data,uint16_t length)
{
	
	for(uint16_t i=0;i<length;++i)
	{
		if(!RingBuffer_Write(ringbuff,data[i]))
		{
			return RINGBUFF_ERR;
		}
	}
	if(RingBuffer_GetRemainSize(ringbuff) < length)
	{
		return RINGBUFF_ERR;
	}
	return RINGBUFF_OK;
}
/**
* @brief  Write_RingBuff
* @param  uint8_t data
* @return FLASE/TRUE
* @note   ��ȡָ����������
*/
int RingBuffer_ReadData(RingBuff_t *ringbuff,uint8_t* data,uint16_t length)
{
	
	for(uint16_t i =0;i < length;++i)
	{
		if(!RingBuffer_Read(ringbuff,&data[i]))
		{
			return RINGBUFF_ERR;
		}
	}
	 if(RingBuffer_GetWriteSize(ringbuff) < length)
	{
		return RINGBUFF_ERR;
	}
	return RINGBUFF_OK;
}