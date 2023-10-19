#ifndef __ADDSTRETCH_TASK_H__
#define __ADDSTRETCH_TASK_H__
#ifdef  __ADDSTRETCH_TASK_GLOBALS
#define __ADDSTRETCH_TASK_EXT
#else
#define __ADDSTRETCH_TASK_EXT extern
#endif

#include "stm32f4xx_hal.h"

#define ADDSTRETCH_PERIOD 4
typedef enum
{	
	ADDSTRETCH_UNKNOWN,			//未知态
	ADDSTRETCH_CALIBRA,			//校准
	ADDSTRETCH_CALIBRA_END,	//机构到达初始化位置
    ADDSTRETCH_WAIT,           //等待其他机构
	ADDSTRETCH_KNOWN					//校准完成
}addstretch_init_e;	

typedef enum
{
	ADDSTRETCH_STOP,
	ADDSTRETCH_AUTO,
	ADDSTRETCH_INIT,
}addstretch_action_e;

#define ADD_MIN 				0

typedef struct
{	
	addstretch_init_e		state;
	addstretch_action_e  action;
	addstretch_action_e  last_action;
	
	float           cnt_ref;//除于值后的pid用外环目标值
	float           cnt_fdb[2];  //除于值,让值变小
	int16_t					spd_ref[2]; //只使用一边的电机反馈
  int16_t         current[2]; 
	int16_t					ramp_input[2];
	int16_t 				ramp_set;
	float						offset[2];//补偿
}addstretch_t;

#define  			OUTER_ADDSTRETCH		-15
#define				ADDSTRETCH_PITCHDISABLE			-700//-700
#define				ADDSTRETCH_PITCHENABLE			-15
__ADDSTRETCH_TASK_EXT	addstretch_t addstretch;


void addstretch_task(void const *argu);
void addstretch_init(void);

void addstretch_init_step(void);

#endif
