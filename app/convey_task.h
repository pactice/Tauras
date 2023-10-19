#ifndef __CONVEY_TASK_H__
#define __CONVEY_TASK_H__
#ifdef  __CONVEY_TASK_GLOBALS
#define __CONVEY_TASK_EXT
#else
#define __CONVEY_TASK_EXT extern
#endif

#include "stm32f4xx_hal.h"

#define CONVEY_PERIOD 6

// #define EMPTY_CHECK  (convey.tof_data>400)
// #define BEHIND_CHECK (convey.tof_data>260)
// #define MID_CHECK	 (convey.tof_data>140)
// #define FRONT_CHECK	 (convey.tof_data<110)
// #define LAST_CHECK	 (convey.tof_data<370)

typedef enum
{
	CONVEY_UNKNOWN,			//未知态
	CONVEY_CALIBRA,			//校准
	CONVEY_KNOWN			//校准完成
}convey_init_e;

typedef enum
{
	CONVEY_STOP,	 //保护
	CONVEY_STORE,	 //存
	CONVEY_RETRIEVE, //取
	CONVEY_ROTATE,	 //旋转
	CONVEY_INIT,	 //初始化
} convey_action_e;

typedef struct
{
	convey_init_e 	state;
	convey_action_e action;
	convey_action_e last_action;

	uint16_t 		tof_data;

	float 			cnt_ref; //除于值后的pid用外环目标值
	float 			cnt_fdb; //除于值,让值变小
	int16_t 		spd_ref;
	int16_t 		current;

	float 			offset; //补偿
}convey_t;



__CONVEY_TASK_EXT	convey_t convey;

void convey_task(void const *argu);
void convey_init(void);
void convey_init_step(void);
void Tof_Read_Data(uint8_t *Tof_Data);
#endif
