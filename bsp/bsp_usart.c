#include "bsp_usart.h"
#include "remote_msg.h"
#include "string.h"
#include "dma.h"
#include "convey_task.h"
#include "bsp_judge.h"
#include "diy_remote.h"
#include "bsp_judgesend.h"
#include "T_imu.h"
#include "bsp_circle.h"
uint8_t dma_dbus_buf[DMA_DBUS_LEN];				
uint8_t	dma_judge_buf[DMA_JUDGE_LEN];
uint8_t dma_tof_buf[DMA_TOF_LEN];
uint8_t dma_diy_buf[DMA_DIYREMOTE_LEN];
uint8_t circle_diy_buf[DMA_DIYREMOTE_LEN];
/**
  * @brief 串口空闲中断回调函数
  * @param UART_HandleTypeDef *huart
  * @retval 无
  * @attention DBUS串口有 一个问题  需要把it.c中cube自动生成的中断函数注释掉，否则只能接收到一个字节。
  */
void USER_UART_IDLECallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance== USART1)			//DBUS串口  
	{
		rc_callback_handler(&rc,dma_dbus_buf);
		memset(&dma_dbus_buf,0,sizeof(dma_dbus_buf)); 
		HAL_UART_Receive_DMA(huart, dma_dbus_buf, DMA_DBUS_LEN);
	}
	else if(huart->Instance== USART2)	//JUDGE串口
	{
		Judge_Read_Data(dma_judge_buf);
		memset(&dma_judge_buf,0,sizeof(dma_judge_buf)); 
		HAL_UART_Receive_DMA(huart, dma_judge_buf, DMA_JUDGE_LEN);
	}
	else if(huart->Instance== USART3)	//TOF
	{
		diy_remote.refresh_count = 255;
		Tof_Read_Data(dma_tof_buf);
		memset(&dma_tof_buf,0,sizeof(dma_tof_buf)); 
		HAL_UART_Receive_DMA(huart, dma_tof_buf, DMA_TOF_LEN);
	}
else if(huart->Instance== USART6)	//自定义控制器调试串口
	{
		
		 writeBuffer(&ringBuff,dma_diy_buf,DMA_DIYREMOTE_LEN);
		readBuffer(&ringBuff,circle_diy_buf,DMA_DIYREMOTE_LEN);
		diy_Read_Data(circle_diy_buf);
		memset(&dma_diy_buf,0,sizeof(dma_diy_buf)); 
		HAL_UART_Receive_IT(&huart6,dma_diy_buf,DMA_DIYREMOTE_LEN);
// 		HAL_UART_Receive_DMA(huart, dma_diy_buf, DMA_DIYREMOTE_LEN);
	}
}

/**
  * @brief 串口空闲中断   注：需在it.c中每个串口的中断中调用该函数
  * @param UART_HandleTypeDef *huart
  * @retval 无
  */
void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
	if(RESET != __HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE)) //判断是否是空闲中断
	{
		__HAL_UART_CLEAR_IDLEFLAG(huart);                     //清除空闲中断标志（否则会一直不断进入中断）
		HAL_UART_DMAStop(huart);							  //停止本次DMA运输
		USER_UART_IDLECallback(huart);                        //调用串口功能回调函数
	}
}


/**
* @brief 串口初始化:使能串口空闲中断,开启串口DMA接收
* @param  无
* @retval 无
*/
void user_uart_init()
{
	__HAL_UART_CLEAR_IDLEFLAG(&DBUS_HUART);
	__HAL_UART_ENABLE_IT(&DBUS_HUART, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&DBUS_HUART, dma_dbus_buf, DMA_DBUS_LEN);
	
	__HAL_UART_CLEAR_IDLEFLAG(&JUDGE_HUART);
	__HAL_UART_ENABLE_IT(&JUDGE_HUART, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&JUDGE_HUART, dma_judge_buf, DMA_JUDGE_LEN);

	__HAL_UART_CLEAR_IDLEFLAG(&DIYREMOTE_HUART);
//	__HAL_UART_ENABLE_IT(&DIYREMOTE_HUART, UART_IT_IDLE);
	HAL_UART_Receive_IT(&DIYREMOTE_HUART,dma_diy_buf,DMA_DIYREMOTE_LEN);
//	HAL_UART_Receive_DMA(&DIYREMOTE_HUART, dma_diy_buf, DMA_DIYREMOTE_LEN);
	
	__HAL_UART_CLEAR_IDLEFLAG(&TOF_HUART);
	__HAL_UART_ENABLE_IT(&TOF_HUART, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&TOF_HUART, dma_tof_buf, DMA_DIYREMOTE_LEN);
}
