#ifndef __ROTATE_TASK_H__
#define __ROTATE_TASK_H__
#ifdef  __ROTATE_TASK_GLOBALS
#define __ROTATE_TASK_EXT
#else
#define __ROTATE_TASK_EXT extern
#endif

#include "stm32f4xx_hal.h"

#define ROTATE_PERIOD 4

typedef enum
{
	ROTATE_UNKNOWN,					//未知态
	ROTATE_CALIBRA,					//校准
	ROTATE_CALIBRA_END,	    //机构到达初始化位置
	ROTATE_KNOWN							//校准完成
}rotate_init_e;

typedef enum
{
	ROTATE_STOP,
	ROTATE_AUTO,
	ROTATE_UP,
	ROTATE_DOWN,
    ROTATE_STAY,
	ROTATE_INIT,
}rotate_action_t;

typedef struct
{	
	rotate_init_e	  	state;
	rotate_action_t   	action;
	rotate_action_t   	last_action;	
	
	float          	  	cnt_ref;//除于值后的pid用外环目标值
	float               cnt_ref_eda;
    float               last_continue_ref;//位置改变完成后才会改变的变量
    float               last_cnt_ref;  //上次的目标值
    
	float             	cnt_fdb[2];  //除于值,让值变小	
	float 					angle_err;
	float  					diy_angref_offset;
	float						angle_fdb[2];
	float           angle_ref;
	int16_t			  	spd_ref[2];
  	int16_t         	current[2];
	int16_t 			 ramp_input[2];
	float				offset[2];//补偿
    float               ramp_set;	
}rotate_t;

#define 	ROTATE_VERTICAL			-15 //板竖直 吸盘端向前
#define   ROTATE_SUCKUP				-164	//板水平 吸盘端向上
#define 	ROTATE_ANG_SUCKUP   -96
#define 	ROTATE_EXCHANGE_UP	-200	//兑换限位

#define		ROTATE_VERTICALDN		-320	//板竖直 吸盘端向后
#define		ROTATE_FRONT				-474	//板水平 吸盘端向下
#define   ROTATE_EXCHAGE			-90				
#define		RPTATE_GOLD_BACK		-573 //金矿回来抬一下PITCH


__ROTATE_TASK_EXT	rotate_t rotate;

void rotate_task(void const *argu);
void rotate_init(void);

void rotate_init_step(void);

#endif
