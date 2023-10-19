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
	ROTATE_UNKNOWN,					//δ֪̬
	ROTATE_CALIBRA,					//У׼
	ROTATE_CALIBRA_END,	    //���������ʼ��λ��
	ROTATE_KNOWN							//У׼���
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
	
	float          	  	cnt_ref;//����ֵ���pid���⻷Ŀ��ֵ
	float               cnt_ref_eda;
    float               last_continue_ref;//λ�øı���ɺ�Ż�ı�ı���
    float               last_cnt_ref;  //�ϴε�Ŀ��ֵ
    
	float             	cnt_fdb[2];  //����ֵ,��ֵ��С	
	float 					angle_err;
	float  					diy_angref_offset;
	float						angle_fdb[2];
	float           angle_ref;
	int16_t			  	spd_ref[2];
  	int16_t         	current[2];
	int16_t 			 ramp_input[2];
	float				offset[2];//����
    float               ramp_set;	
}rotate_t;

#define 	ROTATE_VERTICAL			-15 //����ֱ ���̶���ǰ
#define   ROTATE_SUCKUP				-164	//��ˮƽ ���̶�����
#define 	ROTATE_ANG_SUCKUP   -96
#define 	ROTATE_EXCHANGE_UP	-200	//�һ���λ

#define		ROTATE_VERTICALDN		-320	//����ֱ ���̶����
#define		ROTATE_FRONT				-474	//��ˮƽ ���̶�����
#define   ROTATE_EXCHAGE			-90				
#define		RPTATE_GOLD_BACK		-573 //������̧һ��PITCH


__ROTATE_TASK_EXT	rotate_t rotate;

void rotate_task(void const *argu);
void rotate_init(void);

void rotate_init_step(void);

#endif
