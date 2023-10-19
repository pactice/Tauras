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
    PROTECT_MODE,  		//����ģʽ
	TEST1_MODE,			//����ģʽ1
	TEST2_MODE,			//����ģʽ2
	TEST3_MODE,			//����ģʽ3
    REMOTER_MOVE_MODE, 	//�����ƶ�ģʽ
	KEYBOARD_MODE,      //����ģʽ
}ctrl_mode_e;

typedef enum
{
	GET_REMOTE_PRE,
	GET_REMOTE_ING,
	GET_REMOTE_STORE,
	GET_REMOTE_READY,
	GET_REMOTE_END,
}get_remote_e;


//����ģʽ
typedef enum
{
	MOVE_MODE,				//����ģʽ
	ONELICK_AIRGET,			//һ���ս�
	GET_GOLD_MODE,			//���ʯץȡģʽ			
	PICK_BLOCK_MODE,		//ʰ����Ͽ�ʯģʽ
	EXCHANGE_MODE,			//��ʯ�һ�ģʽ
	REMOTE_GET,					//ң�����ս�
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
