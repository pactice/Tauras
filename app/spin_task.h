#ifndef __SPIN_TASK_H__
#define __SPIN_TASK_H__
#ifdef  __SPIN_TASK_GLOBALS
#define __SPIN_TASK_EXT
#else
#define __SPIN_TASK_EXT extern
#endif

#include "stm32f4xx_hal.h"

#define SPIN_PERIOD 4

#define PITCH_OFFSET 400

#define SPIN_ROLL -587

typedef enum
{
	SPIN_UNKNOWN,			//未知态
	SPIN_CALIBRA,			//校准
	SPIN_KNOWN				//校准完成
}spin_init_e;	

typedef enum
{
	SPIN_STOP,
	SPIN_AUTO,
	SPIN_INIT,
}spin_action_e;

typedef struct
{	
	spin_init_e			state;
	spin_action_e		action;
	spin_action_e		last_action;
	
	float 		ramp_set;
	float			cnt_ref;//除于值后的pid用外环目标值
	float     last_cnt_ref;
	float			cnt_fdb;  //除于值,让值变小	
	int16_t		spd_ref;
	int16_t		current;
	float			cnt_ref_eda;
	float			angle_ref;
	float			diy_angref_offset;
	float     angle_err;
	float     angle_fdb;
	float			offset;//补偿	
}spin_t;

__SPIN_TASK_EXT	spin_t spin;

void spin_task(void const *argu);
void spin_init(void);

void spin_init_step(void);
#endif
