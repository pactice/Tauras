#ifndef __UPLIFT_TASK_H__
#define __UPLIFT_TASK_H__
#ifdef  __UPLIFT_TASK_GLOBALS
#define __UPLIFT_TASK_EXT
#else
#define __UPLIFT_TASK_EXT extern
#endif

#include "stm32f4xx_hal.h"

typedef enum
{
	UPLIFT_UNKNOWN,		//未知态
	UPLIFT_CALIBRA,		//校准中
	UPLIFT_KNOWN		//校准完成
} uplift_init_e;		//初始化过程

typedef enum
{
	UPLIFTFALL_PID,	
	UPLIFTFALL_FREE,
} uplift_fall_e;	    //下落状态

typedef enum
{
	UPLIFT_STOP,
	UPLIFT_AUTO,
	UPLIFT_FALL,
	UPLIFT_INIT,
} uplift_action_e;      //运行状态

typedef struct
{
    uplift_init_e		state;
	uplift_action_e		action;
    uplift_action_e   	last_action;
	
	uplift_fall_e       fall_step;
		
	int             	current[2];
	int16_t						ramp_input[2];
	int					spd_ref[2];
	float               cnt_ref;         //除于值后的pid用外环目标值
	float				cnt_unset_ref;	 	//未补偿的目标值
	float				cnt_add;
	float               cnt_fdb[2];      	//除于值,让值变小
	float				height_ref;
	float       height_fdb[2];
	float				height_offset[2];//高度补偿值	
	float       z_err;
	float       z_ref;
	float       last_cnt_ref;
} uplift_t;

__UPLIFT_TASK_EXT uplift_t uplift;

#define UPLIFT_PERIOD 5

#define UPLIFT_GETFRONTBLOCK	4200
#define UPLIFT_EXCHAGEUP		4100	//从上面取矿的高度

#define UPLIFT_UPSUCK				3400	//从上面取矿顶住矿石
#define SET_HEIGHT(x) 				uplift.cnt_ref = x;
	
#define SWITCHABLE_HEIGHT   	5050  		//可带矿变换机构的高度
#define WALKING_HEIGHT				900				//行走模式下的高度
#define UPLIFT_MAX		     	  5050 			//最大限高5050
#define UPLIFT_READYSTORE		  5000			//取矿后回收高度
#define UPLIFT_GROUND					1000			//取地面矿，初始抬升
#define UPLIFT_GOLD		    		4500 			//取银矿高度，可能要再高 4500
#define UPLIFT_REDYGOLD	   	 	4500	    //鹊厣稀矿后预备收矿的高度 可能要再高
#define UPLIFT_EXCHAG_MIN  		2000			//兑换时的最低高度
#define UPLIFT_PICKGROUNDPRE 	4000			//取地下矿预备，避免吸盘在矿仓里面
#define UPLIFT_GETGROUND			300		      //取地面矿
#define UPLIFT_GROUNDOVER			1700 				//地面矿时矿仓满了
#define UPLIFT_EXCHAGEFRONT 	2000			//兑换金矿
#define UPLIFT_OVERGOLD				4000					//矿仓满了，要先抬升再收前移，取金矿的时候矿在手上
#define UPLIFT_RESETHEIGHT    2500				//死亡防止头在里面
#define UPLIFT_STORE		  		4500				//放下高度
void uplift_task(void const *argu);
void upliftup_init(void);
void uplift_fall_step(void);
void upliftup_init_step(void);
#endif
