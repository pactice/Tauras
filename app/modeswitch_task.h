#ifndef __MODESWITCH__

#ifdef  __MODESW_TASK_GLOBALS
#define __MODESW_TASK_EXT
#else
#define __MODESW_TASK_EXT extern
#endif
#include "stm32f4xx.h"
#define MODESWITCH_PERIOD 5

typedef enum
{
    PROTECT_MODE,  		//保护模式
	TEST1_MODE,			//调试模式1
	TEST2_MODE,			//调试模式2
	TEST3_MODE,			//调试模式3
    REMOTER_MOVE_MODE, 	//底盘移动模式
	KEYBOARD_MODE,      //键盘模式
}ctrl_mode_e;

typedef enum
{
	GET_REMOTE_PRE,
	GET_REMOTE_ING,
	GET_REMOTE_STORE,
	GET_REMOTE_READY,
	GET_REMOTE_END,
}get_remote_e;


//功能模式
typedef enum
{
	MOVE_MODE,				//行走模式
	ONELICK_AIRGET,			//一键空接
	GET_GOLD_MODE,			//金矿石抓取模式			
	PICK_BLOCK_MODE,		//拾起地上矿石模式
	EXCHANGE_MODE,			//矿石兑换模式
	REMOTE_GET,					//遥控器空接
} func_mode_e;

__MODESW_TASK_EXT ctrl_mode_e global_ctrl_mode;
__MODESW_TASK_EXT ctrl_mode_e last_global_ctrl_mode;
__MODESW_TASK_EXT func_mode_e func_mode;
__MODESW_TASK_EXT get_remote_e get_remote;
__MODESW_TASK_EXT func_mode_e last_func_mode;
__MODESW_TASK_EXT uint8_t init_flag;
void modeswitch_task(void const *argu);
void unlock_init(void);
void switch_protect_mode(void);
void switch_remote_mode(void);
void switch_keyboard_mode(void);
void pid_init (void);
void Test1_Step_Switch(void);
#define __MODESWITCH__
#endif
