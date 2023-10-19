#define __MODESW_TASK_GLOBALS
#include "modeswitch_task.h"
#include "rotate_task.h"
#include "stretch_task.h"
#include "addstretch_task.h"
#include "chassis_task.h"
#include "bsp_can.h"
#include "keyboard_handle.h"
#include "remote_msg.h"
#include "cradle_task.h"
#include "math.h"
#include "stdlib.h"
#include "cmsis_os.h"
#include "cross_task.h"
#include "cradle_task.h"
#include "relay_task.h"
#include "addstretch_task.h"
#include "convey_task.h"
#include "uplift_task.h"
#include "spin_task.h"
#include "cross_task.h"
#include "bsp_pwm.h"
#include "bsp_judge.h"
#include "T_imu.h"
#include "math_calcu.h"
uint8_t flag_unlock = 0;//0未解锁 1已解锁
uint8_t init_flag = 0;
int remote_flag=0;
int last_rc2=0;
int remote_init=0;
void modeswitch_task(void const *argu)
{
	uint32_t mode_wake_time = osKernelSysTick();
	for(;;)
	{
		taskENTER_CRITICAL();
		if (!flag_unlock)   
		{ 	
			unlock_init();  
			
		}else{
			 switch(rc.sw1)
			 {
					case RC_UP:
					{
						 global_ctrl_mode = KEYBOARD_MODE;
						
						 if(last_global_ctrl_mode != KEYBOARD_MODE)
						 {
								entire_reset_handle();
							  func_mode = MOVE_MODE;
							  move_status = INIT_MOVE;
								chassis.ctrl_mode = CHASSIS_TURN;
								
						 }
						
						 keyboard_handle();
						 
						 last_global_ctrl_mode = KEYBOARD_MODE; 
					}break;
					case RC_MI:
					{
						
						global_ctrl_mode = PROTECT_MODE;
						remote_flag = 0;
						if(last_global_ctrl_mode != PROTECT_MODE)
						{
							Can_Init();
							pid_init();
								
						}
						
						switch_protect_mode();
						last_global_ctrl_mode = PROTECT_MODE; 
						
					}break;
					 case RC_DN:
					{
					  global_ctrl_mode = REMOTER_MOVE_MODE;
						chassis.ctrl_mode = CHASSIS_REMOTER;
						if(rc.sw2 == RC_DN&&remote_init==0)
						 {
								entire_reset_handle();
							  func_mode = MOVE_MODE;
							  move_status = INIT_MOVE;
							  remote_init = 1;
								
						 }
						 if(entire_init_flag>=1)
						 {
							switch_remote_mode();
						 }
						if(last_global_ctrl_mode != REMOTER_MOVE_MODE)
						{
							cradle.ctrl_mode = CRADLE_INIT;						
						}
						last_global_ctrl_mode = REMOTER_MOVE_MODE;
					
					}break;
			 }
			
		}	
		cradle.last_ctrl_mode = cradle.ctrl_mode;
		chassis.last_ctrl_mode = chassis.ctrl_mode;
		last_global_ctrl_mode = global_ctrl_mode;
	  last_func_mode = func_mode;//记录上次模式
		taskEXIT_CRITICAL();
		osDelayUntil(&mode_wake_time, MODESWITCH_PERIOD);
	}
}

/**
  * @brief  解锁函数
  * @param  void
  * @retval void
  */
void unlock_init(void)
{
	if (global_ctrl_mode == PROTECT_MODE && rc.sw1 == RC_MI &&rc.ch4==-660 && rc.ch3==660)
	{
		flag_unlock = 1;				
	}
}
/**
  * @brief  保护模式函数
  * @param  void
  * @retval void
  */
void switch_protect_mode(void)
{
	  PUMP_OFF;
		global_ctrl_mode = PROTECT_MODE;
		stretch.action =  STRETCH_STOP;
		rotate.action = ROTATE_STOP;
		addstretch.action = ADDSTRETCH_STOP;
		convey.action = CONVEY_STOP;
		cradle.ctrl_mode =  CRADLE_STOP;
		uplift.action =UPLIFT_STOP;
		spin.action = SPIN_STOP;
		cross.action = CROSS_STOP;
		chassis.ctrl_mode = CHASSIS_PROTECT;
		main_send.assist_action = ASSIST_STOP;	
    init_flag = 0;
    PUMP_STOP;
  
}
void pid_init (void)
{
	stretch_init();
	addstretch_init();
	rotate_init();
	cross_init();
	upliftup_init();
	spin_init();
	convey_init();
	cradle_init();
}
////测试自定义遥控器
//uint8_t last_rc_sw2 =9;
//void switch_remote_mode(void)
//{
//	if(rc.sw2 ==RC_DN)
//	{
//		rotate.angle_err = (imu_rece.recepitch_t.pitch - rotate.diy_angref_offset)*0.3;		
//		cross.angle_err = circle_error(cross.diy_angref_offset,imu_rece.receyaw_t.yaw,360)*0.3;
//		
//		if(entire_init_flag!=3)
//		{
//			rotate.diy_angref_offset = imu_rece.recepitch_t.pitch;
//			cross.diy_angref_offset = imu_rece.receyaw_t.yaw;
//			SET_HEIGHT(UPLIFT_MAX);
//			stretch.cnt_ref = STRETCH_MAX;
//			if(UPLIFT_CHECK && STRETCH_CHECK)
//			{
//				rotate.cnt_ref = ROTATE_SUCKUP;
//				if(ROTATE_CHECK)
//				{
//					cross.cnt_ref = CROSS_DNMID;
//					if(CROSS_CHECK)
//					{
//						entire_init_flag=3;
//					}
//				}
//			}
//		}
//		if(entire_init_flag==3)
//		{				
//		
//			rotate.angle_ref = rotate.angle_err+rotate.angle_fdb[0];
//			cross.angle_ref = cross.angle_err+cross.angle_fdb;
//			
//			cross.cnt_ref_eda =	data_limit((cross.angle_ref * 1.7),CROSS_EXCHAGERIGHT_LIM,CROSS_EXCHAGELEFT_LIM);
//			rotate.cnt_ref_eda = data_limit((rotate.angle_ref * 1.7),ROTATE_VERTICAL,ROTATE_EXCHANGE_UP);
//			
//			if(fabs(cross.cnt_ref_eda -cross.last_cnt_ref)>1)
//			{
//				cross.cnt_ref = cross.cnt_ref_eda;
//			}else
//			{
//				cross.cnt_ref = cross.last_cnt_ref;
//			}
//				if(fabs(rotate.cnt_ref_eda -rotate.last_cnt_ref)>1)
//			{
//				rotate.cnt_ref = rotate.cnt_ref_eda;
//			}else
//			{
//				rotate.cnt_ref = rotate.last_cnt_ref;
//			}
//						
//		  rotate.last_cnt_ref = rotate.cnt_ref;
//			cross.last_cnt_ref = cross.cnt_ref;
//		}

//	}

//}
//检录用
void switch_remote_mode(void)
{
	if(rc.sw2 == RC_MI&&remote_flag==0)
	{
		func_mode = REMOTE_GET;
		get_remote = GET_REMOTE_PRE;
		remote_flag =1;
		last_rc2 = RC_MI;
	}
	if(func_mode == REMOTE_GET)
	{
		switch(get_remote)
		{
			case GET_REMOTE_PRE:
			{
				uplift.action = UPLIFT_AUTO;
				SET_HEIGHT(UPLIFT_MAX);
				if(UPLIFT_CHECK)
				{
					addstretch.action = ADDSTRETCH_AUTO;
					addstretch.cnt_ref = ADDSTRETCH_PITCHENABLE;
					if(ADDSTRETCH_CHECK)
					{
						rotate.action = ROTATE_AUTO;
						rotate.cnt_ref = ROTATE_VERTICAL;
						if(ROTATE_CHECK)
						{
							cross.action = CROSS_AUTO;
							cross.cnt_ref = CROSS_MID;
							if(CROSS_CHECK)
							{
								stretch.action = STRETCH_AUTO; 
								stretch.cnt_ref = STRETCH_MAX;
								if(STRETCH_CHECK)
								{
									if(remote_flag == 1&& rc.sw2 != last_rc2)
									{
										remote_flag =2;
										last_rc2 = rc.sw2;
										get_remote = GET_REMOTE_ING;		
									}
								}
							}
						}
					}
				}
			
			}break;
			case GET_REMOTE_ING:
			{
				PUMP_ON;
				if(remote_flag == 2&& rc.sw2 != last_rc2)
				{
					remote_flag =3;
					last_rc2 = rc.sw2;
					get_remote = GET_REMOTE_STORE;		
				}
			}break;
			case GET_REMOTE_STORE:
			{
				stretch.cnt_ref = STRETCH_BLOCKABLE;
				addstretch.cnt_ref = ADDSTRETCH_PITCHENABLE;
				if(STRETCH_CHECK&&ADDSTRETCH_CHECK)
				{
					rotate.cnt_ref = ROTATE_VERTICALDN;
					if(ROTATE_CHECK)
					{
						//检测矿仓情况
						block_state_auto_refresh();	
						if(remote_flag == 3&& rc.sw2 != last_rc2)
						{
							remote_flag =4;
							last_rc2 = rc.sw2;
							get_remote = GET_REMOTE_READY;		
						}
					}
				}
			}break;
			case GET_REMOTE_READY:
			{
				PUMP_OFF;
				if(remote_flag == 4&& rc.sw2 != last_rc2)
						{
							remote_flag =5;
							last_rc2 = rc.sw2;
							get_remote = GET_REMOTE_END;		
						}
			}break;
			case GET_REMOTE_END:
			{
				rotate.cnt_ref = ROTATE_VERTICAL;
				if(ROTATE_CHECK)
				{
					uplift.action = UPLIFT_FALL;
				}
			}break;
		}
	}
}

