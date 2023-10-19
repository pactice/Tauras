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
	CROSS_UNKNOWN,			//δ֪̬
	CROSS_CALIBRA,			//У׼
	CROSS_CALIBRAEND,
	CROSS_KNOWN				//У׼���
}cross_init_e;

typedef enum
{
	CROSS_STOP,             //����ģʽ
	CROSS_AUTO,             //����ģʽ
	CROSS_INIT,             //��ʼ��ģʽ
	CROSS_TEST,
}cross_action_e;

typedef struct
{	
	cross_init_e			state;
	cross_action_e		    action;
	cross_action_e		    last_action;
	
	float			cnt_ref;//����ֵ���pid���⻷Ŀ��ֵ    �����;��ȵ�Ŀ��ֵ
	float			cnt_fdb;  //����ֵ,��ֵ��С	           �ڵ�����������Ͻ��;��ȵķ���ֵ
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
	float			offset;//����
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
