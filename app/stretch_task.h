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
	STRETCH_UNKNOWN,			//δ֪̬
	STRETCH_CALIBRA,			//У׼
	STRETCH_CALIBRA_END,	//���������ʼ��λ��
	STRETCH_KNOWN					//У׼���
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
	
	float           cnt_ref;//����ֵ���pid���⻷Ŀ��ֵ
	float           cnt_fdb[2];  //����ֵ,��ֵ��С
	int16_t					spd_ref[2]; //ֻʹ��һ�ߵĵ������
  int16_t         current[2]; 
	int16_t         ramp_input[2];
	int16_t  				ramp_set;
	float						long_ref;
	float						long_fdb[2];
	float						offset[2];//����
	float           x_err;
	float						x_ref;
	float           last_cnt_ref;
}stretch_t;

#define 	INNER_STRETCH			2900	//���������λ
#define   STRETCH_GOLD_REDY 200		//ȡ����Ԥ��

#define		STRETCH_STOREBLOCK 2200	//�����Ļ��վ���

#define   STRETCH_ONECLICKOVER	1500

#define		STRETCH_EXCHAGEFRONT 500
#define   STRETCH_EXCHAGEGETFRONT	1500
#define 	STRETCH_CLAWEXCHANGE	500 //�����п�ʱ׼���һ�
#define   STRETCH_EXCHAGEREADYUP 2200
//NEW
#define		STRETCH_WALKING   1800
#define   STRETCH_BLOCKABLE	1800		//����ģʽ�£��ܹ����������
#define		STRETCH_MAX				5			//���������λ����ס���
#define   STRETCH_OVERGOLD		1500 //������ˣ�ȡ��ͷ��
#define   STRETCH_GROUNDSTORE	2300//������Ҫ����һ��
__STRETCH_TASK_EXT	stretch_t stretch;
__STRETCH_TASK_EXT  uint8_t entire_init_flag;

void stretch_task(void const *argu);
void stretch_init(void);

void stretch_init_step(void);

#endif
