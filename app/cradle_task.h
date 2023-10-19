#ifndef __CRADLE_TASK_H__
#define __CRADLE_TASK_H__
#ifdef  __CRADLE_TASK_GLOBALS
#define __CRADLE_TASK_EXT
#else
#define __CRADLE_TASK_EXT extern
#endif

#include "stm32f4xx_hal.h"

#define CRADLE_PERIOD 	5

/* cradle parameter structure */
typedef enum
{
	CRADLE_UNKNOWN,		//未知态
	CRADLE_YAWCALIBRA,		//校准中
	CRADLE_CHCALIBRAEND,//校准后
	CRADLE_KNOWN		//校准完成
} cradle_init_e;		//初始化过程

typedef enum
{
	CRADLE_STOP,
	CRADLE_INIT,
	CRADLE_SETMID,
	CRADLE_BACK,
	CRADLE_KEYBOARD,
} cradle_mode_e;

#define PARALLEL_PITCH_POSITION 100
#define DOWNMINE_PITCH_POSITION 100
#define PARALLEL_YAW_POSITION 100
#define DOWNMINE_YAW_POSITION 100

typedef struct
{
	int16_t           yaw_cnt_ref; 
	cradle_mode_e			ctrl_mode;
	cradle_mode_e			last_ctrl_mode;
} main_cradle_t;

typedef struct
{
	int16_t           yaw_cnt_ref; 
	int16_t						yaw_spd_ref;
	int16_t           yaw_cnt_fdb; 
	int16_t						yaw_spd_fdb;
	float 						yaw_offset;
	int         			yaw_current;
	int16_t						yaw_mecd_err;
	int16_t           ramp_set;
	cradle_mode_e  		ctrl_mode;
	cradle_mode_e			last_ctrl_mode;
	cradle_init_e			state;
	
}cradle_t; 
__CRADLE_TASK_EXT cradle_t cradle;
__CRADLE_TASK_EXT main_cradle_t main_cradle;
#define CRADLE_YAW_MIN			-242
#define CRADLE_YAW_MAX			-0
#define CRADLE_YAW_FRONT			-74
#define CRADLE_YAW_BACK			-242
	
void cradle_task(void const *argu);
void cradle_init(void);
void cradle_init_step(void);
#endif