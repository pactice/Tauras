#ifndef __STRETCH_TASK_H__
#define __STRETCH_TASK_H__
#ifdef  __STRETCH_TASK_GLOBALS
#define __STRETCH_TASK_EXT
#else
#define __STRETCH_TASK_EXT extern
#endif

#include "stm32f4xx_hal.h"

#define STRETCH_PERIOD 4
typedef enum
{	
	STRETCH_UNKNOWN,			//未知态
	STRETCH_CALIBRA,			//校准
	STRETCH_CALIBRA_END,	//机构到达初始化位置
	STRETCH_KNOWN					//校准完成
}stretch_init_e;	

typedef enum
{
	STRETCH_STOP,
	STRETCH_AUTO,
	STRETCH_INIT,
}stretch_action_e;

typedef struct
{	
	stretch_init_e		state;
	stretch_action_e  action;
	stretch_action_e  last_action;
	
	float           cnt_ref;//除于值后的pid用外环目标值
	float           cnt_fdb[2];  //除于值,让值变小
	int16_t					spd_ref[2]; //只使用一边的电机反馈
  int16_t         current[2]; 
	int16_t         ramp_input[2];
	int16_t  				ramp_set;
	float						long_ref;
	float						long_fdb[2];
	float						offset[2];//补偿
	float           x_err;
	float						x_ref;
	float           last_cnt_ref;
}stretch_t;

#define 	INNER_STRETCH			2900	//最里面的限位
#define   STRETCH_GOLD_REDY 200		//取金矿的预备

#define		STRETCH_STOREBLOCK 2200	//金矿存矿的回收距离

#define   STRETCH_ONECLICKOVER	1500

#define		STRETCH_EXCHAGEFRONT 500
#define   STRETCH_EXCHAGEGETFRONT	1500
#define 	STRETCH_CLAWEXCHANGE	500 //手里有矿时准备兑换
#define   STRETCH_EXCHAGEREADYUP 2200
//NEW
#define		STRETCH_WALKING   1800
#define   STRETCH_BLOCKABLE	1800		//行走模式下，能够存放两个矿
#define		STRETCH_MAX				5			//最外面的限位，顶住金矿
#define   STRETCH_OVERGOLD		1500 //矿仓满了，取在头上
#define   STRETCH_GROUNDSTORE	2300//存地面矿要多收一点
__STRETCH_TASK_EXT	stretch_t stretch;
__STRETCH_TASK_EXT  uint8_t entire_init_flag;

void stretch_task(void const *argu);
void stretch_init(void);

void stretch_init_step(void);

#endif
