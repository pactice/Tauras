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
* @note   初始化
*/
void RingBuff_Init(RingBuff_t *ringbuff)
{
		if(NULL == ringbuff->pHead)
	{
		 ringbuff->pHead = (uint16_t *)malloc(RINGBUFF_LEN  * sizeof(uint16_t));
	}
	
	memset(ringbuff->pHead, 0 , sizeof(RINGBUFF_LEN));
	
	ringbuff->pValidRead = ringbuff->pHead;
	ringbuff->pValidWrite = ringbuff->pHead;
	ringbuff->pTail = ringbuff->pHead + RINGBUFF_LEN;  

}

/**
* @brief  Write_RingBuff
* @param  uint8_t data
* @return FLASE/TRUE
* @note   写入数据
*/
uint8_t writeBuffer(RingBuff_t *ringbuf, uint8_t *data,uint8_t length) {
   if(NULL == ringbuf->pHead)
	{
		
		return RINGBUFF_ERR;
	}

	if(length > ringbuf->pTail - ringbuf->pHead)
	{
		return RINGBUFF_ERR;
	}
	
	if(ringbuf->pValidWrite + length > ringbuf->pTail)
	{
		int PreLen = ringbuf->pTail - ringbuf->pValidWrite;
		int LastLen = length - PreLen;
		memcpy(ringbuf->pValidWrite, data, PreLen);	
		memcpy(ringbuf->pHead, data + PreLen, LastLen);
		
		ringbuf->pValidWrite = ringbuf->pHead + LastLen;	
	}
	else
	{
		memcpy(ringbuf->pValidWrite, data, length);
		ringbuf->pValidWrite += length;
	}
	return RINGBUFF_OK;

}
/**
* @brief  Read_RingBuff
* @param  RingBuff_t *ringbuf,uint8_t *rData
* @return FLASE/TRUE
* @note   读取
*/
uint8_t readBuffer(RingBuff_t *ringbuf,uint8_t *data,uint8_t length) {
 
    if(NULL == ringbuf->pHead)
	{
		
		return RINGBUFF_ERR;
	}
	if(length > ringbuf->pTail - ringbuf->pHead)
	{
		
		return RINGBUFF_ERR;
	}

	if(ringbuf->pValidRead + length > ringbuf->pTail)
	{
		int PreLen = ringbuf->pTail - ringbuf->pValidRead;
		int LastLen = length - PreLen;
		memcpy(data, ringbuf->pValidRead, PreLen);
		memcpy(data + PreLen, ringbuf->pHead, LastLen);
		
		ringbuf->pValidRead = ringbuf->pHead + LastLen;
	}
	else
	{
		memcpy(data, ringbuf->pValidRead, length);
		ringbuf->pValidRead += length;
		return RINGBUFF_OK;
	}
	
	
}
