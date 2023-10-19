#ifndef __KEYBOARD_HANDLE__

#define __KEYBOARD_HANDLE__

#ifdef __KEYBOARD_HANDLE_GLOBALS
#define __KEYBOARD_HANDLE_EXT 
#else
#define __KEYBOARD_HANDLE_EXT extern
#endif

#include "stm32f4xx.h"

typedef enum
{
	PRE_TO_MOVE,		//�л�����׼��
	TRUN_TO_MOVE,		//�����л�����ģʽ
	CPL_MOVE,				//����л�
	INIT_MOVE,			//��ʼ������״̬
	CLAW_MOVE,
} move_enter_e;
typedef enum
{
	UP,
	BACK,
} move_rotate_e;//����״̬

typedef enum
{
	GOLD_PRE,//ȡ���׼��
	GOLD_ING,//����ȡ��
	GOLD_CATCH_BACK,//ȡ��󷵻�
	GOLD_IN_CLIP,		//�����צ��
	GOLD_STORE,			//����ڲֿ�
	GOLD_ISCONTINUE,		//Ԥ����
	GOLD_THROW,//�ѿ��ӵ�
	GOLD_THROW_END,
	GOLD_END,
} gold_get_e;

typedef enum
{
   ONECLICK_PRE,
	 ONECLICK_ING,
	 ONECLICK_READY,
	 ONECLICK_STORE,
	 ONECLICK_COMPLE,
	ONECLICK_THROW,
	ONECLICK_THROW_END,
	ONCLICK_INCLAW,
} oneclick_gold_e;

typedef enum
{
	PICK_GOUNDPRE,//Ԥ����
	PICK_GET_ING,//����ȡ���¿�
	PICK_BLOCK,//����ȡ���¿�
	PICK_BLOCK_CPL,//
	PICK_STORE,//���
	PICK_THROW,//�ӵ���
	PICK_THROW_END,
	PICK_CPL,//���
	PICK_IN_CLAW,//���������צ����
	PICK_INCLAW_MOVE,//�����ƶ�
} block_pick_e;

typedef enum
{
  EXCHANGE_PRE,
	EXCHAGE_CLAWING,
	EXCHANGE_FRONT_READY,   //����ǰ��
	EXCHANGE_AIM_START,	   //��ǰ���֮��ʼ��λ
	EXCHANGE_AIM_OK,
	
  EXCHANGE_UP_READY,      //��������
	EXCHANGE_AIM_UP,	   //�������֮��ʼ��λ
	EXCHANGE_UPTOEX,	
	
	EXCHANGE,			   //�һ�
	
	EXCHANGE_CHOICE,	   //�һ��������
	
	EXCHANGE_BACK_COMPLETE, //�һ����
	
	
} block_exchange_e;

typedef enum
{
	SWITCH_READY,
	SWITCH_BACK_IN_STORE,
	SWITCH_CONVEY_BLOCK,
	SWITCH_DIR,
	SWITCH_BACK_IN_CLAW,
	SWITCH_GET_BLOCK,
	SWITCH_END,
} switch_block_dir_e;

//״̬
typedef enum
{
	NonGET,			//����û��
	GET,				//�����п�
}block_store_e;

//����״̬
typedef enum
{
	step_one,
    step_two,
}test_mode_e;

__KEYBOARD_HANDLE_EXT switch_block_dir_e block_dir_switch;
__KEYBOARD_HANDLE_EXT block_exchange_e exchange_status;
__KEYBOARD_HANDLE_EXT oneclick_gold_e		oneclick_status;
__KEYBOARD_HANDLE_EXT gold_get_e gold_status;
__KEYBOARD_HANDLE_EXT block_pick_e block_pick;
__KEYBOARD_HANDLE_EXT move_enter_e move_status;
__KEYBOARD_HANDLE_EXT move_rotate_e move_rotate;
__KEYBOARD_HANDLE_EXT block_store_e block_state[3];
__KEYBOARD_HANDLE_EXT test_mode_e test_status;
__KEYBOARD_HANDLE_EXT uint16_t super_status;	//����״̬ (���� ��Ӧλ�ÿ�ʯ��ץȡ)

//�����ٶ�
#define SPEED_VERY_SLOW	500
#define SPEED_SLOW		1200
#define SPEED_NORMAL	5000
#define	SPEED_FAST		8000

//��ֵ
#define PUMP_SWITCH_TIME 100
#define ROTATE_THRESHOLD 10
#define CROSS_THRESHOLD  20
#define STRETCH_THRESHOLD 50
#define ADDSTRETCH_THRESHOLD 20
#define UPLIFT_THRESHOLD 50

//��ֵ
#define UPLIFT_ERROR0 	(fabs(uplift.cnt_fdb[0] - uplift.cnt_ref))
#define UPLIFT_ERROR1 	(fabs(uplift.cnt_fdb[1] - uplift.cnt_unset_ref))
#define ADDSTRETCH_ERROR0 	(fabs(addstretch.cnt_fdb[0] - addstretch.cnt_ref)) 
#define ADDSTRETCH_ERROR1 	(fabs(addstretch.cnt_fdb[1] + addstretch.cnt_ref)) 
#define STRETCH_ERROR0 	(fabs(stretch.cnt_fdb[0] - stretch.cnt_ref)) 
#define STRETCH_ERROR1 	(fabs(stretch.cnt_fdb[1] + stretch.cnt_ref))
#define ROTATE_ERROR0 	(fabs(rotate.cnt_fdb[0] - rotate.cnt_ref))
#define ROTATE_ERROR1 	(fabs(rotate.cnt_fdb[1] + rotate.cnt_ref))
#define CROSS_ERROR		(fabs(cross.cnt_fdb - cross.cnt_ref))

//У��
#define UPLIFT_CHECK0  (UPLIFT_ERROR0 < UPLIFT_THRESHOLD)
#define UPLIFT_CHECK1  (UPLIFT_ERROR1 < UPLIFT_THRESHOLD)
#define UPLIFT_CHECK   (UPLIFT_CHECK0 && UPLIFT_CHECK1 )

#define STRETCH_CHECK0 (STRETCH_ERROR0 < STRETCH_THRESHOLD)
#define STRETCH_CHECK1 (STRETCH_ERROR1 < STRETCH_THRESHOLD)
#define STRETCH_CHECK  (STRETCH_CHECK0 && STRETCH_CHECK1 )

#define ADDSTRETCH_CHECK0 (ADDSTRETCH_ERROR0 < ADDSTRETCH_THRESHOLD)
#define ADDSTRETCH_CHECK1 (ADDSTRETCH_ERROR1 < ADDSTRETCH_THRESHOLD)
#define ADDSTRETCH_CHECK  (ADDSTRETCH_CHECK0 && ADDSTRETCH_CHECK1 )

#define ROTATE_CHECK0  (ROTATE_ERROR0 < ROTATE_THRESHOLD)
#define ROTATE_CHECK1  (ROTATE_ERROR1 < ROTATE_THRESHOLD)
//#define ROTATE_CHECK   (ROTATE_CHECK0 && ROTATE_CHECK1)
#define ROTATE_CHECK   (ROTATE_CHECK1)
#define CROSS_CHECK	   (CROSS_ERROR < CROSS_THRESHOLD)

//����
#define CLAW_GET  block_state[0]
#define FRONT_GET block_state[1]
#define BACK_GET  block_state[2] 
#define ANGLE_SCALE -0.05
void entire_reset_handle(void);
void keyboard_mode_switch(void);
void keyboard_handle(void);
void keyboard_chassis_ctrl (void);
void Switch_Move_Step (void);
void reset_handle(void);
void block_state_set(void);
void help_action_set(void);
void Gold_Get_Step(void);
void Gold_one_click(void);
void block_state_auto_refresh(void);
void block_pick_step(void);
void block_exchange_step(void);
void block_switch_dir(void);
void switch_glod_state(void);
void cradle_mode_switch(void);
void reset_spin(void);
void keyboard_control(void);
void diyremote_control(void);
void move_rotate_change(void);
#endif
