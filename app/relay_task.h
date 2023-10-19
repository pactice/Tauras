#ifndef __RELAY_TASK__
#define __RELAY_TASK__
#ifdef  __RELAY_TASK_GLOBALS
#define __RELAY_TASK_EXT
#else
#define __RELAY_TASK_EXT extern
#endif

#include "stm32f407xx.h"
#include "chassis_task.h"
#include "cradle_task.h"
typedef enum
{
	ASSIST_STOP,		//保护
	ASSIST_OFF,			//救援关闭
	HELP_ON,				//救援开启
	BARRIER_AIM,		//障碍块瞄准
	BARRIER_GET,		//障碍块获取
	BARRIER_SET,		//障碍块放置
	ASSIST_INIT,		//初始化	
} assist_action_e;//救援/障碍块机构目的

typedef enum
{
	OFF_MODE,
	ON_MODE,
} pump_mode_e;//模式

typedef __packed struct 
{ 
	pump_mode_e pump1:1;
	pump_mode_e pump2:1;
	pump_mode_e pump3:1;
	pump_mode_e pump4:1;		//气泵
} main_send_flag_t;

typedef struct//主板发送的信息
{
	uint8_t					frameheader;            //数据帧头
	main_send_flag_t 		flag;                   //标志位
	chassis_t				chassis;                //底盘数据
	assist_action_e  		assist_action;          //救援爪行为
	uint8_t					frametail;              //数据帧尾
} main_send_t;

__RELAY_TASK_EXT main_send_t main_send;

//气泵
#define PUMP0_ON 	main_send.flag.pump1 = ON_MODE
#define PUMP0_OFF 	main_send.flag.pump1 = OFF_MODE
#define PUMP1_ON	main_send.flag.pump2 = ON_MODE
#define PUMP1_OFF 	main_send.flag.pump2 = OFF_MODE
//电磁阀
#define PUMP2_ON 	main_send.flag.pump3 = ON_MODE
#define PUMP2_OFF 	main_send.flag.pump3 = OFF_MODE
#define PUMP3_ON	main_send.flag.pump4 = ON_MODE
#define PUMP3_OFF 	main_send.flag.pump4 = OFF_MODE

#define PUMP_ON		{PUMP0_ON;\
					PUMP3_ON;\
					PUMP2_OFF;\
					PUMP1_OFF;}

#define PUMP_OFF	{PUMP0_OFF;\
					PUMP3_OFF;\
					PUMP2_ON;\
					PUMP1_ON;}

#define PUMP_STOP   {PUMP0_OFF;\
					PUMP3_OFF;\
					PUMP2_OFF;\
					PUMP1_OFF;}


#define PUMP0_CHECK main_send.flag.pump1 == ON_MODE
#define PUMP1_CHECK main_send.flag.pump2 == ON_MODE
#define PUMP2_CHECK main_send.flag.pump3 == ON_MODE
#define PUMP3_CHECK main_send.flag.pump4 == ON_MODE

void relay_task(void const *argu);

#endif
