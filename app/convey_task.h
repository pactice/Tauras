#ifndef __CONVEY_TASK_H__
#define __CONVEY_TASK_H__
#ifdef  __CONVEY_TASK_GLOBALS
#define __CONVEY_TASK_EXT
#else
#define __CONVEY_TASK_EXT extern
#endif

#include "stm32f4xx_hal.h"

#define CONVEY_PERIOD 6

// #define EMPTY_CHECK  (convey.tof_data>400)
// #define BEHIND_CHECK (convey.tof_data>260)
// #define MID_CHECK	 (convey.tof_data>140)
// #define FRONT_CHECK	 (convey.tof_data<110)
// #define LAST_CHECK	 (convey.tof_data<370)

typedef enum
{
	CONVEY_UNKNOWN,			//δ֪̬
	CONVEY_CALIBRA,			//У׼
	CONVEY_KNOWN			//У׼���
}convey_init_e;

typedef enum
{
	CONVEY_STOP,	 //����
	CONVEY_STORE,	 //��
	CONVEY_RETRIEVE, //ȡ
	CONVEY_ROTATE,	 //��ת
	CONVEY_INIT,	 //��ʼ��
} convey_action_e;

typedef struct
{
	convey_init_e 	state;
	convey_action_e action;
	convey_action_e last_action;

	uint16_t 		tof_data;

	float 			cnt_ref; //����ֵ���pid���⻷Ŀ��ֵ
	float 			cnt_fdb; //����ֵ,��ֵ��С
	int16_t 		spd_ref;
	int16_t 		current;

	float 			offset; //����
}convey_t;



__CONVEY_TASK_EXT	convey_t convey;

void convey_task(void const *argu);
void convey_init(void);
void convey_init_step(void);
void Tof_Read_Data(uint8_t *Tof_Data);
#endif
