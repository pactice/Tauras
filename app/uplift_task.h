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
	UPLIFT_UNKNOWN,		//δ֪̬
	UPLIFT_CALIBRA,		//У׼��
	UPLIFT_KNOWN		//У׼���
} uplift_init_e;		//��ʼ������

typedef enum
{
	UPLIFTFALL_PID,	
	UPLIFTFALL_FREE,
} uplift_fall_e;	    //����״̬

typedef enum
{
	UPLIFT_STOP,
	UPLIFT_AUTO,
	UPLIFT_FALL,
	UPLIFT_INIT,
} uplift_action_e;      //����״̬

typedef struct
{
    uplift_init_e		state;
	uplift_action_e		action;
    uplift_action_e   	last_action;
	
	uplift_fall_e       fall_step;
		
	int             	current[2];
	int16_t						ramp_input[2];
	int					spd_ref[2];
	float               cnt_ref;         //����ֵ���pid���⻷Ŀ��ֵ
	float				cnt_unset_ref;	 	//δ������Ŀ��ֵ
	float				cnt_add;
	float               cnt_fdb[2];      	//����ֵ,��ֵ��С
	float				height_ref;
	float       height_fdb[2];
	float				height_offset[2];//�߶Ȳ���ֵ	
	float       z_err;
	float       z_ref;
	float       last_cnt_ref;
} uplift_t;

__UPLIFT_TASK_EXT uplift_t uplift;

#define UPLIFT_PERIOD 5

#define UPLIFT_GETFRONTBLOCK	4200
#define UPLIFT_EXCHAGEUP		4100	//������ȡ��ĸ߶�

#define UPLIFT_UPSUCK				3400	//������ȡ��ס��ʯ
#define SET_HEIGHT(x) 				uplift.cnt_ref = x;
	
#define SWITCHABLE_HEIGHT   	5050  		//�ɴ���任�����ĸ߶�
#define WALKING_HEIGHT				900				//����ģʽ�µĸ߶�
#define UPLIFT_MAX		     	  5050 			//����޸�5050
#define UPLIFT_READYSTORE		  5000			//ȡ�����ո߶�
#define UPLIFT_GROUND					1000			//ȡ����󣬳�ʼ̧��
#define UPLIFT_GOLD		    		4500 			//ȡ����߶ȣ�����Ҫ�ٸ� 4500
#define UPLIFT_REDYGOLD	   	 	4500	    //ȵ��ϡ���Ԥ���տ�ĸ߶� ����Ҫ�ٸ�
#define UPLIFT_EXCHAG_MIN  		2000			//�һ�ʱ����͸߶�
#define UPLIFT_PICKGROUNDPRE 	4000			//ȡ���¿�Ԥ�������������ڿ������
#define UPLIFT_GETGROUND			300		      //ȡ�����
#define UPLIFT_GROUNDOVER			1700 				//�����ʱ�������
#define UPLIFT_EXCHAGEFRONT 	2000			//�һ����
#define UPLIFT_OVERGOLD				4000					//������ˣ�Ҫ��̧������ǰ�ƣ�ȡ����ʱ���������
#define UPLIFT_RESETHEIGHT    2500				//������ֹͷ������
#define UPLIFT_STORE		  		4500				//���¸߶�
void uplift_task(void const *argu);
void upliftup_init(void);
void uplift_fall_step(void);
void upliftup_init_step(void);
#endif
