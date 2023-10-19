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
	ADDSTRETCH_UNKNOWN,			//δ֪̬
	ADDSTRETCH_CALIBRA,			//У׼
	ADDSTRETCH_CALIBRA_END,	//���������ʼ��λ��
    ADDSTRETCH_WAIT,           //�ȴ���������
	ADDSTRETCH_KNOWN					//У׼���
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
	
	float           cnt_ref;//����ֵ���pid���⻷Ŀ��ֵ
	float           cnt_fdb[2];  //����ֵ,��ֵ��С
	int16_t					spd_ref[2]; //ֻʹ��һ�ߵĵ������
  int16_t         current[2]; 
	int16_t					ramp_input[2];
	int16_t 				ramp_set;
	float						offset[2];//����
}addstretch_t;

#define  			OUTER_ADDSTRETCH		-15
#define				ADDSTRETCH_PITCHDISABLE			-700//-700
#define				ADDSTRETCH_PITCHENABLE			-15
__ADDSTRETCH_TASK_EXT	addstretch_t addstretch;


void addstretch_task(void const *argu);
void addstretch_init(void);

void addstretch_init_step(void);

#endif
