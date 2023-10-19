#ifndef __CAN_DRIVE__


#ifdef  __BSP_CAN_GLOBALS
#define __BSP_CAN_EXT
#else
#define __BSP_CAN_EXT extern
#endif

#include "can.h"

/* CAN send and receive ID */
typedef enum
{                                           
	//CAN1                        车头朝前时右
	CAN_UPLI_ID1    		=  	  0x201,//
	CAN_UPLI_ID2    		= 		0x202,//
	CAN_ADDSTRE_S1_ID		= 	  0x203,//
	CAN_STRE_S1_ID			= 	  0x204,//
	CAN_ROTA_R1_ID			=	  	0x205,//	
	CAN_CONV_ID					=	  	0x206,//
	
	//CAN2
	CAN_SPIN_S1_ID			=	  0x201,//左 	
	CAN_CROSS_S1_ID			=	  0x202,//          
	CAN_ADDSTRE_S2_ID		=	  0x203,//	
	CAN_STRE_S2_ID			=	 	0x204,//
	CAN_ROTA_R2_ID			=	  0x205,//
	CRADLE_YAW_ID   		= 	0x206,
} can_msg_id_e;

typedef struct	Can_RxMessage
{
	CAN_RxHeaderTypeDef header;
	uint8_t             data[8];
}Can_RxBuff;

/* can receive motor parameter structure */
typedef struct
{
  uint16_t ecd;
  uint16_t last_ecd;
  
  int16_t  speed_rpm;
  int16_t  given_current;

  int32_t  round_cnt;
  int32_t  total_ecd;
	
  uint16_t offset_ecd;
  uint8_t	 msg_cnt;
  
  int32_t  ecd_raw_rate;

} moto_measure_t;

//发送电流队列结构体
typedef struct 
{
  union 
  {
    int16_t Current[4];
    uint8_t SendCurrent[8];
  }Current_U1;

  union 
  {
    int16_t Current[4];
    uint8_t SendCurrent[8];
  }Current_U2;

}Current_Queue_t;

//机械结构分类
typedef enum
{
  STRUCTION_UPLIFT,
  STRUCTION_ADDSTRETCH,
  STRUCTION_ROTATE,
  STRUCTION_SPIN,
  STRUCTION_CROSS,
  STRUCTION_CONVEY,
} struction_e;

__BSP_CAN_EXT moto_measure_t moto_uplift[2];
__BSP_CAN_EXT moto_measure_t moto_stretch[2];
__BSP_CAN_EXT moto_measure_t moto_addstretch[2];
__BSP_CAN_EXT moto_measure_t moto_rotate[2];
__BSP_CAN_EXT moto_measure_t moto_cross;
__BSP_CAN_EXT moto_measure_t moto_spin;
__BSP_CAN_EXT moto_measure_t moto_convey;
__BSP_CAN_EXT moto_measure_t moto_yaw;
__BSP_CAN_EXT Current_Queue_t Can1_Queue;
__BSP_CAN_EXT Current_Queue_t Can2_Queue;

__BSP_CAN_EXT Can_RxBuff Can_RxData;

void Canx_Init(CAN_HandleTypeDef * hcan , uint32_t  * id,void (*pFunc)(uint32_t ,uint8_t*));
void CAN_Filter_IDList_Config(CAN_HandleTypeDef * hcan, uint32_t  * ID);
void CAN1_FilterEnd_Config(CAN_HandleTypeDef * hcan,CAN_FilterTypeDef  *Can_Fliter,uint8_t num);
void CAN2_FilterEnd_Config(CAN_HandleTypeDef * hcan,CAN_FilterTypeDef  *Can_Fliter,uint8_t num);
void CAN1_IDList_Config(CAN_HandleTypeDef * hcan,CAN_FilterTypeDef  *Can_Fliter,uint32_t  * ID);
void CAN2_IDList_Config(CAN_HandleTypeDef * hcan,CAN_FilterTypeDef  *Can_Fliter,uint32_t  * ID);
void get_moto_offset(moto_measure_t* ptr, uint8_t * CAN_Rx_data);
void encoder_data_handler(moto_measure_t* ptr, uint8_t * CAN_Rx_data);
void Current_Switch_Queue(Current_Queue_t *Send_Queue, struction_e struction);
void CAN1_CALLBACK_FUNC(uint32_t ID,uint8_t* CAN_RxData);
void CAN2_CALLBACK_FUNC(uint32_t ID,uint8_t* CAN_RxData);
HAL_StatusTypeDef Canx_SendCurrent(CAN_HandleTypeDef *hcan, uint32_t ID, uint8_t *data);
void Can_Init (void);
#define __CAN_DRIVE__
#endif
