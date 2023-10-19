/******************************简介*******************************************
采用了16位的列表过滤模式
每个can独立使用一个接受邮箱，即：
CAN1使用FIFO0，CAN2使用FIFO1。
此文件仅包含can接收函数，发送函数复制原来兵种的代码吧
*****************************使用说明*****************************************
				   直接调用Canx_Init函数即可
参数说明：
CAN_HandleTypeDef * hcan：CAN结构体

uint32_t  * id:需要的ID号数组，要以0xFFF结尾，比如你要读取ID为0x201的电机，则定义如下
uint32_t ID[] = {0x201,0xFFF};        注意要以0xFFF结尾！！！！
如果没有在数组中定义ID，即使外设挂载在总线上也会被过滤器滤除，导致读不到数据

void (*pFunc)(uint32_t ,uint8_t*):回调函数指针，需要自己定义一个回调函数
该回调函数返回类型为void 第一个参数为uint32_t的ID，第二个则是uint8_t的反馈数据
比如 void CAN1_CALLBACK(uint32_t ID,uint8_* CAN_RxData)
{
	   ………………;
}
具体内容要自己写，可以直接把之前代码复制过来，但是只需要复制数据处理部分就可以
不需要复制有关can的任何内容

使用范例：
uint32_t ID[] = {0x201,0xFFF};
void CAN1_CALLBACK(uint32_t ID,uint8_* CAN_RxData)
{
	switch(ID)
	{
		…………;
	}
}
Canx_Init(&hcan1,ID,CAN1_CALLBACK);
*******************************************************************************/
/* Private define ------------------------------------------------------------*/
#define __BSP_CAN_GLOBALS
/* Includes ------------------------------------------------------------------*/
#include "bsp_can.h"
#include "uplift_task.h"
#include "cross_task.h"
#include "convey_task.h"
#include "stretch_task.h"
#include "addstretch_task.h"
#include "rotate_task.h"
#include "spin_task.h"
#include "T_imu.h"
/* Private variables ---------------------------------------------------------*/
Can_RxBuff Can_RxData;

static void (*pCAN1_RxFifo0CpltCallback)(uint32_t, uint8_t *);
static void (*pCAN2_RxFifo1CpltCallback)(uint32_t, uint8_t *);

/* Private type --------------------------------------------------------------*/

/* Private function declarations ---------------------------------------------*/

/* function prototypes -------------------------------------------------------*/

/**
 * @brief  Initialize CAN Bus
 * @param  hcan: CANx created by CubeMX. id:an array of ListID. (*pFunc):USER_Callback_Func
 * @return None.
 */
void Canx_Init(CAN_HandleTypeDef *hcan, uint32_t *id, void (*pFunc)(uint32_t, uint8_t *))
{
	CAN_Filter_IDList_Config(hcan, id);
	HAL_CAN_Start(hcan);
	if (CAN1 == hcan->Instance)
	{
		HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
		__HAL_CAN_ENABLE_IT(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
		pCAN1_RxFifo0CpltCallback = pFunc;
	}
	else if (CAN2 == hcan->Instance)
	{
		HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO1_MSG_PENDING);
		__HAL_CAN_ENABLE_IT(hcan, CAN_IT_RX_FIFO1_MSG_PENDING);
		pCAN2_RxFifo1CpltCallback = pFunc;
	}
}
/**
 * @brief  Initialize CAN Filter
 * @param  hcan: CANx created by CubeMX. id:an array of ListID.
 * @return None.
 */
void CAN_Filter_IDList_Config(CAN_HandleTypeDef *hcan, uint32_t *ID)
{
	/* can filter config */
	CAN_FilterTypeDef can_filter;

	can_filter.FilterMode = CAN_FILTERMODE_IDLIST;
	can_filter.FilterScale = CAN_FILTERSCALE_16BIT;
	can_filter.SlaveStartFilterBank = 14;
	// Config IDList
	if (CAN1 == hcan->Instance)
		CAN1_IDList_Config(hcan, &can_filter, ID);
	else if (CAN2 == hcan->Instance)
		CAN2_IDList_Config(hcan, &can_filter, ID);
}
/**
 * @brief  Initialize Register about IDlist for CAN1
 * @param  hcan: CANx created by CubeMX. Can_Fliter: CAN filter configuration structure.id:an array of ListID.
 * @return None.
 */
void CAN1_IDList_Config(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *Can_Fliter, uint32_t *ID)
{
	uint8_t i = 0;
	while (ID[i] != 0xFFF)
	{
		switch (i % 4)
		{
		case 0:
			Can_Fliter->FilterIdHigh = ID[i] << 5;
			break;
		case 1:
			Can_Fliter->FilterIdLow = ID[i] << 5;
			break;
		case 2:
			Can_Fliter->FilterMaskIdHigh = ID[i] << 5;
			break;
		case 3:
		{
			Can_Fliter->FilterMaskIdLow = ID[i] << 5;
			CAN1_FilterEnd_Config(hcan, Can_Fliter, i);
			break;
		}
		}
		if (ID[i + 1] == 0xFFF)
		{
			CAN1_FilterEnd_Config(hcan, Can_Fliter, i);
		}
		i++;
	}
}
/**
 * @brief  Initialize Register about IDlist for CAN2
 * @param  hcan: CANx created by CubeMX. Can_Fliter: CAN filter configuration structure.id:an array for ListID.
 * @return None.
 */
void CAN2_IDList_Config(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *Can_Fliter, uint32_t *ID)
{
	uint8_t i = 0;
	while (ID[i] != 0xFFF)
	{
		switch (i % 4)
		{
		case 0:
			Can_Fliter->FilterIdHigh = ID[i] << 5;
			break;
		case 1:
			Can_Fliter->FilterIdLow = ID[i] << 5;
			break;
		case 2:
			Can_Fliter->FilterMaskIdHigh = ID[i] << 5;
			break;
		case 3:
		{
			Can_Fliter->FilterMaskIdLow = ID[i] << 5;
			CAN2_FilterEnd_Config(hcan, Can_Fliter, i);
			break;
		}
		}
		if (ID[i + 1] == 0xFFF)
		{
			CAN2_FilterEnd_Config(hcan, Can_Fliter, i);
		}
		i++;
	}
}
/**
 * @brief  Finish initialize register about IDlist for CAN1
 * @param  hcan: CANx created by CubeMX. Can_Fliter: CAN filter configuration structure. num: the index of ID array
 * @return None.
 */
void CAN1_FilterEnd_Config(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *Can_Fliter, uint8_t num)
{
	Can_Fliter->FilterBank = num / 4;
	Can_Fliter->FilterFIFOAssignment = CAN_FILTER_FIFO0;
	Can_Fliter->FilterActivation = CAN_FILTER_ENABLE;
	HAL_CAN_ConfigFilter(hcan, Can_Fliter);
}
/**
 * @brief  Finish initialize register about IDlist for CAN2
 * @param  hcan: CANx created by CubeMX. Can_Fliter: CAN filter configuration structure. num: the index of ID array
 * @return None.
 */
void CAN2_FilterEnd_Config(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *Can_Fliter, uint8_t num)
{
	Can_Fliter->FilterBank = num / 4 + 14;
	Can_Fliter->FilterFIFOAssignment = CAN_FILTER_FIFO1;
	Can_Fliter->FilterActivation = CAN_FILTER_ENABLE;
	HAL_CAN_ConfigFilter(hcan, Can_Fliter);
}

/*HAL库FIFO0中断*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if (hcan->Instance == CAN1)
	{
		if (HAL_CAN_GetRxMessage(hcan, CAN_FILTER_FIFO0, &Can_RxData.header, Can_RxData.data) == HAL_ERROR)
		{
		};
		pCAN1_RxFifo0CpltCallback(Can_RxData.header.StdId, Can_RxData.data);
	}
}

/*HAL库FIFO1中断*/
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if (hcan->Instance == CAN2)
	{
		if (HAL_CAN_GetRxMessage(hcan, CAN_FILTER_FIFO1, &Can_RxData.header, Can_RxData.data) == HAL_ERROR)
		{
		};
		pCAN2_RxFifo1CpltCallback(Can_RxData.header.StdId, Can_RxData.data);
	}
}

/**
 * @brief     get motor initialize offset value
 * @param     ptr: Pointer to a moto_measure_t structure
 * @retval    None
 * @attention this function should be called after system can init
 */
void get_moto_offset(moto_measure_t *ptr, uint8_t *CAN_Rx_data)
{
	ptr->ecd = (uint16_t)(CAN_Rx_data[0] << 8 | CAN_Rx_data[1]);
	ptr->offset_ecd = ptr->ecd;
	ptr->msg_cnt++;
}

/**
 * @brief     get motor rpm and calculate motor round_count/total_encoder/total_angle
 * @param     ptr: Pointer to a moto_measure_t structure
 * @attention this function should be called after get_moto_offset() function
 */
void encoder_data_handler(moto_measure_t *ptr, uint8_t *CAN_Rx_data)
{
	//转子转速
	ptr->speed_rpm = (int16_t)(CAN_Rx_data[2] << 8 | CAN_Rx_data[3]);
	ptr->given_current = (int16_t)(CAN_Rx_data[4] << 8 | CAN_Rx_data[5]);

	//机械角度
	ptr->last_ecd = ptr->ecd;
	ptr->ecd = (uint16_t)(CAN_Rx_data[0] << 8 | CAN_Rx_data[1]);

	//相对开机后的角度
	if (ptr->ecd - ptr->last_ecd > 4096)
	{
		ptr->round_cnt--;
		ptr->ecd_raw_rate = ptr->ecd - ptr->last_ecd - 8192;
	}
	else if (ptr->ecd - ptr->last_ecd < -4096)
	{
		ptr->round_cnt++;
		ptr->ecd_raw_rate = ptr->ecd - ptr->last_ecd + 8192;
	}
	else
	{
		ptr->ecd_raw_rate = ptr->ecd - ptr->last_ecd;
	}

	ptr->total_ecd = ptr->round_cnt * 8192 + ptr->ecd - ptr->offset_ecd;
}

void CAN1_CALLBACK_FUNC(uint32_t ID, uint8_t *CAN_RxData)
{
	switch (ID)
	{
		//抬升
		case CAN_UPLI_ID1:
		{
			moto_uplift[0].msg_cnt <= 50 ? get_moto_offset(&moto_uplift[0], CAN_RxData) : encoder_data_handler(&moto_uplift[0], CAN_RxData);
		}
		break;
		case CAN_UPLI_ID2:
		{
			moto_uplift[1].msg_cnt <= 50 ? get_moto_offset(&moto_uplift[1], CAN_RxData) : encoder_data_handler(&moto_uplift[1], CAN_RxData);
		}
		break;
		case CAN_ADDSTRE_S1_ID:
		{
			moto_addstretch[0].msg_cnt<= 50 ?get_moto_offset(&moto_addstretch[0], CAN_RxData) : encoder_data_handler(&moto_addstretch[0],CAN_RxData);
		}
		break;
		case CAN_STRE_S1_ID:
		{
			moto_stretch[0].msg_cnt<= 50 ?get_moto_offset(&moto_stretch[0], CAN_RxData) : encoder_data_handler(&moto_stretch[0],CAN_RxData);
		}
		break;
		//滚动
		case CAN_ROTA_R1_ID:
		{
			moto_rotate[0].msg_cnt <= 50 ?get_moto_offset(&moto_rotate[0], CAN_RxData) : encoder_data_handler(&moto_rotate[0],CAN_RxData);
		}
		break;
		case CAN_CONV_ID:
		{
			moto_convey.msg_cnt <= 50 ?get_moto_offset(&moto_convey, CAN_RxData) : encoder_data_handler(&moto_convey,CAN_RxData);
		}
		break;
	}
}

void CAN2_CALLBACK_FUNC(uint32_t ID, uint8_t *CAN_RxData)
{
	switch (ID)
	{
		case CAN_SPIN_S1_ID:
		{
			moto_spin.msg_cnt <= 50 ? get_moto_offset(&moto_spin, CAN_RxData) : encoder_data_handler(&moto_spin, CAN_RxData);
		}
		break;
		case CAN_CROSS_S1_ID:
		{
			moto_cross.msg_cnt <= 50 ? get_moto_offset(&moto_cross, CAN_RxData) : encoder_data_handler(&moto_cross, CAN_RxData);
		}
		break;
		case CAN_ADDSTRE_S2_ID:
		{
			moto_addstretch[1].msg_cnt <= 50 ? get_moto_offset(&moto_addstretch[1], CAN_RxData) : encoder_data_handler(&moto_addstretch[1], CAN_RxData);
		}
		break;
		case CAN_STRE_S2_ID:
		{
			moto_stretch[1].msg_cnt <= 50 ? get_moto_offset(&moto_stretch[1], CAN_RxData) : encoder_data_handler(&moto_stretch[1], CAN_RxData);
		}break;
		case CAN_ROTA_R2_ID:
		{
			moto_rotate[1].msg_cnt <= 50 ? get_moto_offset(&moto_rotate[1], CAN_RxData) : encoder_data_handler(&moto_rotate[1], CAN_RxData);
		}
		break;		
			case CRADLE_YAW_ID: //角度
    {
      moto_yaw.msg_cnt <= 50 ?get_moto_offset(&moto_yaw, CAN_RxData) : encoder_data_handler(&moto_yaw,CAN_RxData);    
    }
		break;
	}
}

/*****************************************  CAN Send  *******************************************/

HAL_StatusTypeDef Canx_SendCurrent(CAN_HandleTypeDef *hcan, uint32_t ID, uint8_t *data)
{
	if (hcan == NULL)
	{
		return HAL_ERROR;
	}
	CAN_TxHeaderTypeDef TxMessage;
	uint32_t Can_TX_MAILBOX0 = 0;

	TxMessage.DLC = 0x08;
	TxMessage.IDE = CAN_ID_STD;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.StdId = ID;

	HAL_CAN_AddTxMessage(hcan, &TxMessage, data, &Can_TX_MAILBOX0);
	return HAL_OK;
}

/*****************************************  Data Handle  *******************************************/
void Current_Switch_Queue(Current_Queue_t *Send_Queue, struction_e struction)
{
	switch (struction)
	{
		case STRUCTION_UPLIFT:
		{
			Send_Queue->Current_U1.Current[0] = __REV16(uplift.current[0]);
			Send_Queue->Current_U1.Current[1] = __REV16(uplift.current[1]);
		}break;
		case STRUCTION_CONVEY:
		{
			Send_Queue->Current_U2.Current[1] = __REV16(convey.current);

		}break;
		case STRUCTION_CROSS:
		{
			Send_Queue->Current_U1.Current[1] = __REV16(cross.current);
		}break;
		case STRUCTION_ROTATE:
		{
			Send_Queue->Current_U2.Current[0] = __REV16(rotate.current[0]);
			Send_Queue->Current_U2.Current[0] = __REV16(rotate.current[1]);
		}break;
		case STRUCTION_ADDSTRETCH:
		{
			Send_Queue->Current_U1.Current[2] = __REV16(addstretch. current[0]);
			Send_Queue->Current_U2.Current[1] = __REV16(addstretch.current[1]);
		}break;
		case STRUCTION_SPIN:
		{
			Send_Queue->Current_U1.Current[0] = __REV16(spin.current);
		}break;
		default:
			break;
	}

}

/*****************************************  CAN INIT  *******************************************/

void Can_Init (void)
{
	uint32_t ID_CAN1[] = {CAN_UPLI_ID1,CAN_UPLI_ID2,CAN_ADDSTRE_S1_ID,CAN_STRE_S1_ID,CAN_ROTA_R1_ID,CAN_CONV_ID	,0xFFF};
	uint32_t ID_CAN2[] = {CAN_SPIN_S1_ID,CAN_CROSS_S1_ID,CAN_ADDSTRE_S2_ID,CAN_STRE_S2_ID,CAN_ROTA_R2_ID,CRADLE_YAW_ID,0xFFF};
		
    Canx_Init(&hcan1,ID_CAN1,CAN1_CALLBACK_FUNC);
    Canx_Init(&hcan2,ID_CAN2,CAN2_CALLBACK_FUNC);
}
