#ifndef __CROSS_TASK_H__
#define __CROSS_TASK_H__
#ifdef  __CROSS_TASK_GLOBALS
#define __CROSS_TASK_EXT
#else
#define __CROSS_TASK_EXT extern
#endif

#include "stm32f4xx.h"

#define CROSS_PERIOD 4


typedef enum
{
	CROSS_UNKNOWN,			//未知态
	CROSS_CALIBRA,			//校准
	CROSS_CALIBRAEND,
	CROSS_KNOWN				//校准完成
}cross_init_e;

typedef enum
{
	CROSS_STOP,             //保护模式
	CROSS_AUTO,             //运行模式
	CROSS_INIT,             //初始化模式
	CROSS_TEST,
}cross_action_e;

typedef struct
{	
	cross_init_e			state;
	cross_action_e		    action;
	cross_action_e		    last_action;
	
	float			cnt_ref;//除于值后的pid用外环目标值    即降低精度的目标值
	float			cnt_fdb;  //除于值,让值变小	           在电机反馈基础上降低精度的反馈值
	float	    cnt_ref_eda;
	float     last_cnt_ref;
	float     angle_ref;
	float 		angle_fdb;
	float 		angle_err;
	float 		diy_angref_offset;
	int16_t		    spd_ref;
	int16_t		    current;
	int16_t    ramp_input;
	int16_t    ramp_set;
	float			offset;//补偿
}cross_t;

#define 		CROSS_DNMID			-423
#define 		CROSS_MID   			-115		
#define			CROSS_ANG_MID			-246
#define     CROSS_EXCHAGERIGHT_LIM	-270
#define			CROSS_EXCHAGELEFT_LIM		-570
__CROSS_TASK_EXT cross_t cross;
void cross_init(void);
void cross_task(void const *argu);
void cross_init_step(void);
#endif
