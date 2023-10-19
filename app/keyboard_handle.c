#define __KEYBOARD_HANDLE_GLOBALS
#include "keyboard_handle.h"
#include "rotate_task.h"
#include "stretch_task.h"
#include "string.h"
#include "math.h"
#include "stdlib.h"
#include "remote_msg.h"
#include "bsp_pwm.h"
#include "relay_task.h"
#include "addstretch_task.h"
#include "modeswitch_task.h"
#include "cross_task.h"
#include "uplift_task.h"
#include "bsp_gpio.h"
#include "convey_task.h"
#include "spin_task.h"
#include "bsp_can.h"
int switch_move_true =0;	//是否是在完成步骤后切换行走
/**
 * 描述	 整机复位函数
 * @date 2022-03-22
 * @param {void}
 * @returns {void}
 */
void entire_reset_handle(void)
{
	
	//relay
	PUMP_OFF;
	CLAW_GET = NonGET;
	FRONT_GET = NonGET;
	BACK_GET = NonGET;
	
	memset(&convey,0,sizeof(convey));
	memset(&uplift,0,sizeof(uplift));	
	memset(&rotate,0,sizeof(rotate));
	memset(&spin,0,sizeof(spin));	
	memset(&stretch,0,sizeof(stretch)); 
	memset(&addstretch,0,sizeof(addstretch));
	memset(&cross,0,sizeof(cross));

	
	//init
	uplift.action = UPLIFT_INIT;
	stretch.action = STRETCH_INIT;
	addstretch.action = ADDSTRETCH_INIT;
	rotate.action = ROTATE_INIT;
	cross.action = CROSS_INIT;
	main_send.assist_action = ASSIST_INIT;
	spin.action = SPIN_INIT;
	cradle.ctrl_mode = CRADLE_INIT;
	chassis.ctrl_mode =CHASSIS_TURN;
  move_rotate=UP;	
	//切换行走模式的标志位
	switch_move_true =0;
	entire_init_flag  =0;

}
void keyboard_handle(void)
{
	keyboard_mode_switch();	//键盘下模式选择
	keyboard_chassis_ctrl();//底盘速度控制
	cradle_mode_switch();//云台控制
	move_rotate_change();
	if(func_mode!=EXCHANGE_MODE)
	{
		block_state_set();//矿石位置手动处理
	}
	reset_spin();//重新初始化吸盘
	switch(func_mode)
	{
		
		case MOVE_MODE://行走
		{
			Switch_Move_Step();
			//help_action_set();			
		}break;
		case ONELICK_AIRGET://一键空接
		{
			Gold_one_click();
		}break;
		case GET_GOLD_MODE://取矿
		{
			Gold_Get_Step();
		}break;
		case PICK_BLOCK_MODE://地上矿
		{
			if(exchange_status != EXCHANGE||exchange_status != EXCHANGE_PRE)
			{
				block_pick_step();
			}
		}break;
		case EXCHANGE_MODE://兑换
		{
			block_exchange_step();
		}break;
		case REMOTE_GET:
		{
		
		}break;
	}
}
void switch_keyboard_mode(void)
{
	global_ctrl_mode = KEYBOARD_MODE;
}
/**
 * 描述	键盘模式选择
 * @date 2023-05-15
 * @param {void}
 * @returns {void}
 */
int back_flag =0;
void keyboard_mode_switch(void)
{
	if(rc.mouse.r)
	{
		if ( IF_Q &&(func_mode!=MOVE_MODE))
		{
			func_mode = MOVE_MODE;//移动模式
		
			//只有矿仓满了才把矿拿在手上
			if(CLAW_GET == GET)
			{
				move_status = CLAW_MOVE;
			}
			else{
				move_status = PRE_TO_MOVE;
			}
		
		}
		else if ( IF_E &&(func_mode!=ONELICK_AIRGET))//一键空接
		{
			func_mode = ONELICK_AIRGET;
			oneclick_status = ONECLICK_PRE;
		}
		else if ( IF_R &&(func_mode!=GET_GOLD_MODE))//取金矿
		{
			func_mode = GET_GOLD_MODE;
      gold_status = GOLD_PRE;
		}
		else if( IF_G &&(func_mode!=PICK_BLOCK_MODE))//地面矿
		{
			func_mode = PICK_BLOCK_MODE;
			block_pick = PICK_GOUNDPRE;
		}
		else if( IF_F &&(func_mode!=EXCHANGE_MODE))
		{
			func_mode = EXCHANGE_MODE;//兑换模式
			exchange_status = EXCHANGE_PRE;
			main_send.assist_action = ASSIST_OFF;
			
		}
	}
}

/**
  * @brief      机内矿石状态自动处理
  * @param[in]  none
  * @retval     none
  */
void block_state_auto_refresh(void)
{
	if(convey.tof_data>310)//全空
	{
		FRONT_GET = NonGET;
		BACK_GET = NonGET;
		convey.action = CONVEY_STOP;
	}else if(convey.tof_data>300)	//前空后有
	{
		FRONT_GET = NonGET;
		BACK_GET = GET;
		convey.action = CONVEY_STOP;
	}else if(convey.tof_data>200)		//中间有
	{
		convey.action = CONVEY_STORE;
	}else if(convey.tof_data<=200)	//前有
	{
		FRONT_GET = GET;
		convey.action = CONVEY_STORE;
	}
}
//机内矿石状态手动处理
void block_state_set()
{
	if (rc.kb.bit.X)
	{
		if (IF_C)
		{
			if (BACK_GET)
			{ BACK_GET = NonGET; }
			else { BACK_GET = GET; }
		}else if (IF_V)
		{
			if (FRONT_GET)
			{ FRONT_GET = NonGET; }
			else { FRONT_GET = GET; }
		}else if (IF_B)
		{
			if(CLAW_GET)
			{ 
				CLAW_GET = NonGET;
				main_send.flag.pump1 = OFF_MODE;
			}else { CLAW_GET = GET; }
		}
	}
	if(rc.mouse.r)
	{
		if (rc.kb.bit.Z)
		{
			block_state_auto_refresh();
			convey.action = CONVEY_STORE; 
		}else if (rc.kb.bit.C) 
		{ convey.action = CONVEY_RETRIEVE; }	
	}
}
/**
 * 描述	底盘模式选择
 * @date 2022-03-22
 * @param {void}
 * @returns {void}
 */

void keyboard_chassis_ctrl(void)
{
	
		if(func_mode == MOVE_MODE)
		{		
			if(IF_F)
			{
				if(chassis.last_ctrl_mode ==CHASSIS_TURN)
				{
					chassis.ctrl_mode = CHASSIS_KEYBOARD;		
				}
				else
			  {
					chassis.ctrl_mode =CHASSIS_TURN;				
				}
			}	
			
		}			
		else 
		{
			cradle.yaw_cnt_ref = CRADLE_YAW_FRONT;
			if(func_mode == EXCHANGE_MODE)
			{
				chassis_speed = SPEED_VERY_SLOW; 		chassis.ctrl_mode = CHASSIS_TURN;
				
			}
			else
			{	chassis_speed = SPEED_SLOW; 		chassis.ctrl_mode = CHASSIS_TURN;
									
			}   
		}
}
		
	



//重新写的行走模式
void Switch_Move_Step()
{
	if(last_func_mode != MOVE_MODE)
	{
		convey.action = CONVEY_STORE;
	}
	switch(move_status)
	{	
		//普通准备
		case PRE_TO_MOVE:	
		{	
			uplift.action = UPLIFT_AUTO;	
			PUMP_OFF;
			cross.cnt_ref = CROSS_MID;
			if(CROSS_CHECK)
			{
				SET_HEIGHT(SWITCHABLE_HEIGHT);//可带矿变换的高度
				if(UPLIFT_CHECK)
				{
					addstretch.cnt_ref = ADDSTRETCH_PITCHENABLE;	
					stretch.cnt_ref = STRETCH_MAX;
					if(ADDSTRETCH_CHECK&&STRETCH_CHECK)
					{
						move_status = TRUN_TO_MOVE;
					}
				}
			}
		}break;	
		case TRUN_TO_MOVE:
		{
			if(move_rotate==BACK)
			{
				rotate.cnt_ref = ROTATE_SUCKUP;			
			}else if(move_rotate==UP)
			{
				rotate.cnt_ref = ROTATE_VERTICAL;
			}
			
			if(ROTATE_CHECK)
			{
				block_state_auto_refresh();
				if(convey.tof_data>=285)
				{
					stretch.cnt_ref = STRETCH_WALKING;
					
				}else
				{
					stretch.cnt_ref = STRETCH_BLOCKABLE;
				}
				if(STRETCH_CHECK)
				{
					SET_HEIGHT(WALKING_HEIGHT);
					if(UPLIFT_CHECK)
					{
						move_status =	CPL_MOVE;
					}
				}
			}
		}break;
		
		case CLAW_MOVE:
		{
			cross.cnt_ref = CROSS_MID;
			if(CROSS_CHECK)
			{
				
				rotate.cnt_ref = RPTATE_GOLD_BACK;
				
				stretch.cnt_ref = STRETCH_OVERGOLD;
				SET_HEIGHT(WALKING_HEIGHT);
			}
		}break;
		
		case CPL_MOVE:
		{
			if(move_rotate==BACK)
			{
				rotate.cnt_ref = ROTATE_SUCKUP;			
			}else if(move_rotate==UP)
			{
				rotate.cnt_ref = ROTATE_VERTICAL;
			}
		}break;
			
		case INIT_MOVE:
		{
			if(entire_init_flag==1)
			{	
				if(convey.tof_data>=285)
				{
					stretch.cnt_ref = STRETCH_WALKING;
					
				}else
				{
					stretch.cnt_ref = STRETCH_BLOCKABLE;
				}
				
					uplift.cnt_ref = WALKING_HEIGHT;
					if(move_rotate==BACK)
					{
						rotate.cnt_ref = ROTATE_SUCKUP;					
					}else if(move_rotate==UP)
					{
						rotate.cnt_ref = ROTATE_VERTICAL;
					}
				
			}
		}break;
	}

}

//一键空接
int wait_pump_time=0;
int wait_back_time=0;
int wait_throw_time =0;
void Gold_one_click()
{
		switch(oneclick_status)
	{
			case ONECLICK_PRE:
			{
				uplift.action = UPLIFT_AUTO;	
				SET_HEIGHT(UPLIFT_MAX);
				if(UPLIFT_CHECK)
				{
					addstretch.cnt_ref = ADDSTRETCH_PITCHENABLE;
					if(ADDSTRETCH_CHECK)
					{
						rotate.cnt_ref = ROTATE_VERTICAL;
						if(ROTATE_CHECK)
						{
							cross.cnt_ref = CROSS_MID;
							if(CROSS_CHECK)
							{
								stretch.cnt_ref = STRETCH_MAX;
								if(STRETCH_CHECK)
								{
									PUMP_ON;
									oneclick_status = ONECLICK_ING;
								}
							}
						}
					}
				}
			}break;
			case ONECLICK_ING:
			{
				addstretch.cnt_ref = ADDSTRETCH_PITCHDISABLE;
				block_state_auto_refresh();
				//转矿
				 if (rc.kb.bit.Z)
				{ spin.cnt_ref += 2;}
				else if (rc.kb.bit.C)
				{ spin.cnt_ref -= 2;}
				if(IF_CTRL)
				{
					PUMP_OFF;
				}					
				if(IF_ML)
				{
					if(!FRONT_GET)
					{
						oneclick_status = ONECLICK_READY;
					}else{
						oneclick_status = ONCLICK_INCLAW;
					}
				}
			}break;
			case ONECLICK_READY:
			{
				if(IF_CTRL)
				{
					oneclick_status = ONECLICK_PRE;
				}
				addstretch.cnt_ref = ADDSTRETCH_PITCHENABLE;
				if(ADDSTRETCH_CHECK)
				{
					rotate.cnt_ref = ROTATE_VERTICALDN;
					if(ROTATE_CHECK)
					{
						stretch.cnt_ref = STRETCH_BLOCKABLE;
						
						 
						if(STRETCH_CHECK)
						{
								uplift.cnt_ref = UPLIFT_READYSTORE;
							if(UPLIFT_CHECK)
							{
								//转矿
								if (rc.kb.bit.C)
								{ 
									spin.cnt_ref -= 0.2f;
								}
								 else if (rc.kb.bit.Z)
								{ 
									spin.cnt_ref += 0.2f;
								}
								//检测矿仓情况
								block_state_auto_refresh();
								if(IF_ML)
								{
									CLAW_GET = GET;
									oneclick_status = ONECLICK_STORE;
								}
							}
						}
					}
				}
			}break;
			case ONECLICK_STORE:
			{
				uplift.cnt_ref = UPLIFT_STORE;
				if(UPLIFT_CHECK)
				{
					wait_pump_time++;
					if(wait_pump_time>50)
					{
						PUMP_OFF;
						CLAW_GET = NonGET;
						convey.action = CONVEY_STORE;					
						oneclick_status = ONECLICK_COMPLE;								
					}
				}
			}break;
			case ONECLICK_COMPLE:
			{
				if(rc.kb.bit.R)
				{
					oneclick_status = ONECLICK_PRE;
				}
				if(rc.kb.bit.X)
				{
					convey.action = CONVEY_STORE;	
					func_mode = MOVE_MODE;
				}
				if(rc.kb.bit.CTRL)
				{
					oneclick_status = ONECLICK_THROW;
				}
			}break;
			case ONECLICK_THROW:
			{
				stretch.cnt_ref = STRETCH_BLOCKABLE;
				if(STRETCH_CHECK)
				{
					SET_HEIGHT(UPLIFT_UPSUCK);
					PUMP_ON;
					wait_throw_time++;
				}
				if(wait_throw_time>250)
				{
					oneclick_status = ONECLICK_THROW_END;
				}
			}break;
			case ONECLICK_THROW_END:
			{
				wait_throw_time =0;
				SET_HEIGHT(UPLIFT_MAX);
				stretch.cnt_ref = STRETCH_MAX;
				if(UPLIFT_CHECK && STRETCH_CHECK)
				{
					PUMP_OFF;
					if(rc.kb.bit.R)
					{
						oneclick_status = ONECLICK_PRE;
					}
					if(rc.kb.bit.X)
					{
						func_mode = MOVE_MODE;
					}
					if(rc.kb.bit.CTRL)
					{
						oneclick_status = ONECLICK_THROW;
					}
				}
			}break;
			case ONCLICK_INCLAW:
			{
				addstretch.cnt_ref = ADDSTRETCH_PITCHENABLE;
				CLAW_GET=GET;
				stretch.cnt_ref = STRETCH_BLOCKABLE;
				if(STRETCH_CHECK)
				{
					uplift.action = UPLIFT_FALL;
				}
					break;
			}
	}
}

//金矿除了抬升其他不改
int gold_store_wait =0;
int gold_throw_wait =0;
void Gold_Get_Step()
{
	switch(gold_status)
	{
		case GOLD_PRE:
		{
			uplift.action = UPLIFT_AUTO;	
			gold_store_wait = 0;
			PUMP_OFF;
			CLAW_GET = NonGET;
			//取金矿抬升
			SET_HEIGHT(UPLIFT_GOLD)//要改高度			
			if (UPLIFT_CHECK)
			{
				stretch.cnt_ref = STRETCH_GOLD_REDY;
				addstretch.cnt_ref = ADDSTRETCH_PITCHENABLE;
				if(ADDSTRETCH_CHECK && STRETCH_CHECK)
				{
					rotate.cnt_ref = ROTATE_FRONT;
					if(ROTATE_CHECK)
					{
						cross.cnt_ref = CROSS_MID;
						if(CROSS_CHECK)
						{
							if(IF_ML)
							{
								PUMP_ON;
								stretch.cnt_ref = STRETCH_MAX;
								gold_status = GOLD_ING;
							}
						}
					}
				}
				
			}
		}break;
		case GOLD_ING:
		{
			CLAW_GET = GET;
			if(IF_X)
			{			
				gold_status = GOLD_CATCH_BACK;					
			}
		}break;
		case GOLD_CATCH_BACK:
		{
				SET_HEIGHT(UPLIFT_READYSTORE);
				if(UPLIFT_CHECK)
				{
					rotate.cnt_ref = RPTATE_GOLD_BACK;
					convey.action = CONVEY_STORE;
					block_state_auto_refresh();
					if(ROTATE_CHECK)
					{
						if(!FRONT_GET)
						{
							gold_status = GOLD_STORE;
							gold_store_wait =0;
						}
						else
						{
							gold_status = GOLD_IN_CLIP; 
						}
					}
					
				}	
		}break;
		case GOLD_IN_CLIP:
		{
			CLAW_GET = GET;
			SET_HEIGHT(UPLIFT_OVERGOLD);
			if(UPLIFT_CHECK)
			{
				stretch.cnt_ref = STRETCH_OVERGOLD;
				if(STRETCH_CHECK)
				{
					uplift.action = UPLIFT_FALL;
					CLAW_GET = GET;
					if(UPLIFT_CHECK)
					{
						gold_status = GOLD_END;
					}
				}
			}
		}break;
		case GOLD_STORE:
		{
			stretch.cnt_ref = STRETCH_STOREBLOCK;
			if(STRETCH_CHECK)
			{
				rotate.cnt_ref = ROTATE_VERTICALDN;
				if(ROTATE_CHECK)
				{
					SET_HEIGHT(UPLIFT_STORE);
					if(UPLIFT_CHECK)
					{
						//转矿
						 if (rc.kb.bit.Z)
						{ spin.cnt_ref += 2;}
						else if (rc.kb.bit.C)
						{ spin.cnt_ref -= 2;}
						
							if(IF_ML)
							{
								CLAW_GET = NonGET;
								PUMP_OFF;
								convey.action = CONVEY_STORE;
								CLAW_GET = NonGET;
								gold_status = GOLD_ISCONTINUE;
												
							}		
					}
				}
			}

		}break;
		case GOLD_ISCONTINUE:
		{
			if(rc.kb.bit.CTRL)
			{
				gold_status = GOLD_THROW;
			}
			if(IF_X)
			{				
				rotate.cnt_ref = ROTATE_VERTICAL; 
				if(ROTATE_CHECK)
				{
					gold_status = GOLD_END;
					
				}					
			}
			if(IF_R)
			{
					gold_status = GOLD_PRE;
			}
			 
		}break;
		case GOLD_THROW:
		{
			stretch.cnt_ref = STRETCH_BLOCKABLE;
			if(STRETCH_CHECK)
			{
				SET_HEIGHT(UPLIFT_UPSUCK);
				PUMP_ON;
				gold_throw_wait++;
			}
			if(gold_throw_wait>250)
			{
				gold_status = GOLD_THROW_END;
			}
			
		}break;
		case GOLD_THROW_END:
		{
			gold_throw_wait = 0 ;
			SET_HEIGHT(UPLIFT_MAX);
			stretch.cnt_ref = STRETCH_MAX;
			if(STRETCH_CHECK)
			{
				PUMP_OFF;
				if(IF_X)
				{				
					rotate.cnt_ref = ROTATE_VERTICAL; 
					if(ROTATE_CHECK)
					{
						gold_status = GOLD_END;
						
					}					
				}
				if(IF_R)
				{
						gold_status = GOLD_PRE;
				}
				if(rc.kb.bit.CTRL)
				{
					gold_status = GOLD_THROW;
				}					
			}
		}break;
		case GOLD_END://结束
		{
			
		}break;

	}
}
//除了抬升，其他不改
int ground_status = 0;
int init_time =0;
int pick_throw_wait =0;
void block_pick_step()
{
	
	switch (block_pick)
	{
		case PICK_GOUNDPRE:
		{
			uplift.action = UPLIFT_AUTO;	
			if(fabs(cross.cnt_fdb-CROSS_MID)>5)
			{
				rotate.cnt_ref = ROTATE_SUCKUP;
				if(ROTATE_CHECK)
				{
					init_time ++;
					if(init_time >20)
					{
						cross.cnt_ref = CROSS_MID;
						init_time =0;
					}
				}
			}
			else
			{
				ground_status = 0;
				PUMP_OFF;
				SET_HEIGHT(UPLIFT_PICKGROUNDPRE);
				if(UPLIFT_CHECK)
				{
					addstretch.cnt_ref = ADDSTRETCH_PITCHENABLE;
					stretch.cnt_ref = STRETCH_MAX;
					if(STRETCH_CHECK &&ADDSTRETCH_CHECK)
					{
						rotate.cnt_ref = ROTATE_VERTICALDN;
						if(ROTATE_CHECK)
						{
							block_pick = PICK_GET_ING;			
						}
					}
				}
			}
		}break;  
		case PICK_GET_ING:
		{				
			
			SET_HEIGHT(UPLIFT_GETGROUND);
			if(UPLIFT_CHECK)
			{
				
				block_pick = PICK_BLOCK;			
			}				
		}break;
		case PICK_BLOCK:
		{
			block_state_auto_refresh();
			CLAW_GET = GET;
			if(IF_ML)
			{
				PUMP_ON;	
				rotate.action = ROTATE_STOP;
				uplift.action = UPLIFT_FALL;
			}
			if(!FRONT_GET)
			{	
				if(IF_X)
					{
						PUMP_ON;	
						block_pick = PICK_BLOCK_CPL;							
					}
				}
			else{
				if(IF_X)
				{
					
					block_pick =  PICK_IN_CLAW;					
				}
			}
				
		}break;
		case PICK_BLOCK_CPL:
		{
			rotate.action = ROTATE_AUTO;
			uplift.action = UPLIFT_AUTO;	
			SET_HEIGHT(UPLIFT_REDYGOLD);
			rotate.cnt_ref =ROTATE_VERTICALDN;
			if(UPLIFT_CHECK&&ROTATE_CHECK)
			{
				stretch.cnt_ref = STRETCH_GROUNDSTORE;
				if(STRETCH_CHECK)
				{
					block_pick = PICK_STORE;
				}
			}
		}break;
		case PICK_STORE:
		{
			uplift.cnt_ref = UPLIFT_STORE;
			if(UPLIFT_CHECK)
			{
			//转矿
				 if (rc.kb.bit.Z)
				{ spin.cnt_ref += 0.2;}
				else if (rc.kb.bit.C)
				{ spin.cnt_ref -= 0.2;}
				
				if(IF_ML)
				{
						PUMP_OFF;
						convey.action = CONVEY_STORE;
						CLAW_GET = NonGET;
						block_pick = PICK_CPL;
				}
			}
		}break;
		case PICK_CPL:
		{
			if(rc.kb.bit.X)
				{				
					func_mode = MOVE_MODE;
				}
				if(rc.kb.bit.R)
				{
					block_pick = PICK_GOUNDPRE;
				}
				if(rc.kb.bit.CTRL)
				{
					block_pick = PICK_THROW;
				}
		}break;
		case PICK_THROW:
		{
			stretch.cnt_ref = STRETCH_BLOCKABLE;
			if(STRETCH_CHECK)
			{
					SET_HEIGHT(UPLIFT_UPSUCK);
					PUMP_ON;
					pick_throw_wait++;
			}
				if(pick_throw_wait>250)
				{
					block_pick = PICK_THROW_END;
				}
		}break;
		
		case PICK_THROW_END:
		{
			pick_throw_wait =0;
			SET_HEIGHT(UPLIFT_MAX);
			stretch.cnt_ref  = STRETCH_MAX;
			if(UPLIFT_CHECK && STRETCH_CHECK )
			{
				PUMP_OFF;
				if(rc.kb.bit.X)
				{				
					func_mode = MOVE_MODE;
				}
				if(rc.kb.bit.R)
				{
					block_pick = PICK_GOUNDPRE;
				}
				if(rc.kb.bit.CTRL)
				{
					block_pick = PICK_THROW;
				}
			}
		}break;
		case PICK_IN_CLAW:
		{
			rotate.action = ROTATE_AUTO;
			uplift.action = UPLIFT_AUTO;	
			SET_HEIGHT(UPLIFT_MAX);
			if(UPLIFT_CHECK)
			{
				rotate.cnt_ref = ROTATE_FRONT;
				if(ROTATE_CHECK)
				{
					stretch.cnt_ref = STRETCH_OVERGOLD;
					if(STRETCH_CHECK)
					{
						block_pick = PICK_INCLAW_MOVE;
					}
				}
			}
		}break;
		case PICK_INCLAW_MOVE:
		{
			CLAW_GET = GET;
			uplift.action = UPLIFT_FALL;
		}break;
		default :break;
	}
	
}
int add = 0;
int exchange_time1=0;
int exchange_time2=0;
void block_exchange_step()
{
	switch (exchange_status)
	{
		case EXCHANGE_PRE://准备取矿
		{
			uplift.action = UPLIFT_AUTO;	
				if(CLAW_GET == GET)//有矿在手上
			{
				spin.action = SPIN_INIT;
				convey.action = CONVEY_STORE;
				SET_HEIGHT(UPLIFT_MAX);
				if(UPLIFT_CHECK)
				{
					stretch.cnt_ref = STRETCH_MAX;
					if(STRETCH_CHECK)
					{																		
						rotate.cnt_ref = ROTATE_EXCHAGE;
						
						if(ROTATE_CHECK)
						{	
							cross.cnt_ref = CROSS_DNMID;
							if(CROSS_CHECK)
							{
								exchange_status = EXCHAGE_CLAWING;
							}						
						}					
					}
				}
			}
			else{
				uplift.action = UPLIFT_AUTO;	
				exchange_time1=0;			
				stretch.cnt_ref = STRETCH_EXCHAGEFRONT;
				if(STRETCH_CHECK)
				{
					cross.cnt_ref = CROSS_MID;
					if(CROSS_CHECK)
					{
						SET_HEIGHT(UPLIFT_EXCHAGEFRONT);		
						if(UPLIFT_CHECK)
						{
							convey.action = CONVEY_RETRIEVE;//把矿推到最前面		
							rotate.cnt_ref = ROTATE_VERTICAL;
								if(ROTATE_CHECK)
							{
								if(IF_F)//金矿
								{
									exchange_status = EXCHANGE_FRONT_READY;
									spin.action = SPIN_INIT;
								}
								else if(IF_R)//银矿
								{
									exchange_status = EXCHANGE_UP_READY;
									spin.action = SPIN_INIT;
								}
							}
						}
					}
				}
			}
		}break;
		case EXCHAGE_CLAWING:
		{
			spin.cnt_ref = SPIN_ROLL;
			exchange_status = EXCHANGE;
		}break;
		case EXCHANGE_FRONT_READY:
		{
			
				cross.cnt_ref =CROSS_DNMID;
				if(CROSS_CHECK)
				{	
					rotate.cnt_ref = ROTATE_FRONT;
					
					if(ROTATE_CHECK)
					{
							stretch.cnt_ref = STRETCH_EXCHAGEGETFRONT;			//现在伸的距离太多 宏定义用错了！！！！！
							PUMP_ON;
							convey.action = CONVEY_RETRIEVE;
							if(STRETCH_CHECK)
							{								
								exchange_time1 = 0;
								exchange_status = EXCHANGE_AIM_START;										
							}
						
					}
				}
		}break;
		
		 case EXCHANGE_UP_READY://从上面取矿
		 {
				SET_HEIGHT(UPLIFT_EXCHAGEUP);
				if(UPLIFT_CHECK)
				{
					rotate.cnt_ref = ROTATE_VERTICALDN;//吸盘向下打
					
					stretch.cnt_ref = STRETCH_EXCHAGEREADYUP;		//距离太小
					convey.action = CONVEY_RETRIEVE;
					if(STRETCH_CHECK && ROTATE_CHECK)
					{
						exchange_time1=0;
						exchange_time2=0;
						exchange_status = EXCHANGE_AIM_UP;
					}
				}
				
		 }break;
		 case EXCHANGE_AIM_START://吸前面矿之后开始对位
		 {	
			 convey.action = CONVEY_RETRIEVE;
				exchange_time1++;
				if(exchange_time1 >150)
			 {
				 exchange_time1 = 0;
				 exchange_status = EXCHANGE_AIM_OK;
			 }	
		 }break;
		 case EXCHANGE_AIM_OK:
		 {			
			 
				SET_HEIGHT(UPLIFT_GETFRONTBLOCK);
				if(UPLIFT_CHECK)
			{
				convey.action = CONVEY_STORE;
				stretch.cnt_ref = STRETCH_MAX;
				if(STRETCH_CHECK)
				{
					rotate.cnt_ref = ROTATE_EXCHAGE;
					if(ROTATE_CHECK)
					{
						exchange_status = EXCHANGE;
					}
				}
			}	
		 }break;

		 case EXCHANGE_AIM_UP://吸上面矿之后开始对位
		 {
			 
				SET_HEIGHT(UPLIFT_UPSUCK);
				if(UPLIFT_CHECK)
				{
					PUMP_ON;
					CLAW_GET = GET;
					exchange_time2++;//开气泵
					if(exchange_time2>150)
					{
						exchange_time2=0;
						exchange_status = EXCHANGE_UPTOEX;
					}
				}
		 }break;
		 case EXCHANGE_UPTOEX:
		 {
				SET_HEIGHT(UPLIFT_GETFRONTBLOCK);//吸到矿石，向上提出
			 if(UPLIFT_CHECK)
			 {
				 convey.action = CONVEY_STORE;
					stretch.cnt_ref = STRETCH_MAX;
					if(STRETCH_CHECK)
					{
						rotate.cnt_ref = ROTATE_EXCHAGE;//吸盘翻上去
						if(ROTATE_CHECK)
						{
							cross.cnt_ref = CROSS_DNMID;
							if(CROSS_CHECK)
							{
								spin.cnt_ref = SPIN_ROLL;
								exchange_status = EXCHANGE;
							}
						}
					}
			 }
		 }break;
		
		case EXCHANGE://自由调整 兑换
		{
			
			//按键控制
			if (rc.kb.bit.R)
			{
				add++;
				if(add>10)
				{
					if(rotate.cnt_ref >ROTATE_VERTICAL)//-15
					{
						rotate.cnt_ref =ROTATE_VERTICAL;
					}else 
					{
						rotate.cnt_ref +=1;
					}
					add =0;
				}
			} 
			if(rc.kb.bit.F)
			{
				if(rotate.cnt_ref < ROTATE_EXCHANGE_UP)//-220
				{
					rotate.cnt_ref =ROTATE_EXCHANGE_UP;
				}else 
				{
					rotate.cnt_ref -=0.5;
				}
			}	
			//YAW调整
			 if (rc.kb.bit.E)
			{
				if(cross.cnt_ref > CROSS_EXCHAGERIGHT_LIM)//-270
				{
					cross.cnt_ref = CROSS_EXCHAGERIGHT_LIM;
				}else
				{
					cross.cnt_ref +=0.5;
				}
			}
			if(rc.kb.bit.Q)
			{
				if(cross.cnt_ref < CROSS_EXCHAGELEFT_LIM)//-570
				{
					cross.cnt_ref = CROSS_EXCHAGELEFT_LIM;
				}else
				{
					cross.cnt_ref -=0.5;
				}
			}
			//滑轨前移 V后B前
			 if (rc.kb.bit.V)
			{
				if(stretch.cnt_ref > INNER_STRETCH)
				{
					stretch.cnt_ref = INNER_STRETCH;
				}else
				{
					stretch.cnt_ref ++;
				}
			}
			if(rc.kb.bit.B)
			{
				if(stretch.cnt_ref < STRETCH_MAX)
				{
					stretch.cnt_ref = STRETCH_MAX;
				}else
				{
					stretch.cnt_ref --;
				}
			}
			//高度调整
				if (rc.kb.bit.SHIFT)
			{
				if (uplift.cnt_ref < UPLIFT_MAX)
				{ uplift.cnt_ref += 2;}
				else
				{ uplift.cnt_ref = UPLIFT_MAX; }
			}
			else if (rc.kb.bit.CTRL)
			{
				if (uplift.cnt_ref > UPLIFT_EXCHAG_MIN)
				{ uplift.cnt_ref -= 2;}
				else 
				{uplift.cnt_ref = UPLIFT_EXCHAG_MIN;}
			}
			//吸盘调整
			 if (rc.kb.bit.C)
			{ spin.cnt_ref -= 0.5;}
			else if (rc.kb.bit.Z)
			{ spin.cnt_ref += 0.5;}
			
			if(IF_ML)//||imu_data.pump_off ==0
			{
				PUMP_OFF;
				CLAW_GET = NonGET;
				exchange_status  = EXCHANGE_BACK_COMPLETE;	
			}
		
		}break;
		 case EXCHANGE_BACK_COMPLETE:
		{
				if (rc.kb.bit.R)
			{
				add++;
				if(add>10)
				{
					if(rotate.cnt_ref >ROTATE_VERTICAL)//-15
					{
						rotate.cnt_ref =ROTATE_VERTICAL;
					}else 
					{
						rotate.cnt_ref +=1;
					}
					add =0;
				}
			} 
			if(rc.kb.bit.F)
			{
				if(rotate.cnt_ref < ROTATE_EXCHANGE_UP)//-220
				{
					rotate.cnt_ref =ROTATE_EXCHANGE_UP;
				}else 
				{
					rotate.cnt_ref -=0.5;
				}
			}	
			//YAW调整
			 if (rc.kb.bit.E)
			{
				if(cross.cnt_ref > CROSS_EXCHAGERIGHT_LIM)//-270
				{
					cross.cnt_ref = CROSS_EXCHAGERIGHT_LIM;
				}else
				{
					cross.cnt_ref +=0.5;
				}
			}
			if(rc.kb.bit.Q)
			{
				if(cross.cnt_ref < CROSS_EXCHAGELEFT_LIM)//-570
				{
					cross.cnt_ref = CROSS_EXCHAGELEFT_LIM;
				}else
				{
					cross.cnt_ref -=0.5;
				}
			}
			//滑轨前移 V后B前
			 if (rc.kb.bit.V)
			{
				if(stretch.cnt_ref > INNER_STRETCH)
				{
					stretch.cnt_ref = INNER_STRETCH;
				}else
				{
					stretch.cnt_ref ++;
				}
			}
			if(rc.kb.bit.B)
			{
				if(stretch.cnt_ref < STRETCH_MAX)
				{
					stretch.cnt_ref = STRETCH_MAX;
				}else
				{
					stretch.cnt_ref --;
				}
			}
			//高度调整
				if (rc.kb.bit.SHIFT)
			{
				if (uplift.cnt_ref < UPLIFT_MAX)
				{ uplift.cnt_ref += 2.0f;}
				else
				{ uplift.cnt_ref = UPLIFT_MAX; }
			}
			else if (rc.kb.bit.CTRL)
			{
				if (uplift.cnt_ref > UPLIFT_EXCHAG_MIN)
				{ uplift.cnt_ref -= 2.0f;}
				else 
				{uplift.cnt_ref = UPLIFT_EXCHAG_MIN;}
			}
			//吸盘调整
			 if (rc.kb.bit.C)
			{ spin.cnt_ref -= 0.5f;}
			else if (rc.kb.bit.Z)
			{ spin.cnt_ref += 0.5f;}
			
				if(IF_X)
			{				
				exchange_status = EXCHANGE_PRE;	
			}
			if(IF_B)
			{
				func_mode = MOVE_MODE;
			}	
		}break;
		default:
		break;
	}
}
void cradle_mode_switch()
{
	
	if(global_ctrl_mode == PROTECT_MODE)
	{
		ANGLE_4_SET(MIN_ANGLE);
	}else{
		if(rc.mouse.y)
		{
			angle = angle +rc.mouse.y*ANGLE_SCALE;
			if(angle > MAX_ANGLE)
			{
				angle = MAX_ANGLE;
			}
			if(angle < MIN_ANGLE)
			{
				angle = MIN_ANGLE;
			}
			ANGLE_4_SET(angle);
		}
	}
}
int key =0;
void reset_spin()
{
	if(IF_G)
	{
		moto_spin.round_cnt = 0;
		spin.offset = moto_spin.ecd - moto_spin.offset_ecd;
		spin.cnt_ref = 0;
		spin.ramp_set = 0;
	}
	
}
//遥控器兑换函数
void keyboard_control()
{
	if (rc.kb.bit.R)
			{
				add++;
				if(add>10)
				{
					if(rotate.cnt_ref >ROTATE_VERTICAL)//-15
					{
						rotate.cnt_ref =ROTATE_VERTICAL;
					}else 
					{
						rotate.cnt_ref +=1.0f;
					}
					add =0;
				}
			} 
			if(rc.kb.bit.F)
			{
				if(rotate.cnt_ref < ROTATE_EXCHANGE_UP)//-220
				{
					rotate.cnt_ref =ROTATE_EXCHANGE_UP;
				}else 
				{
					rotate.cnt_ref -=0.5f;
				}
			}	
			//YAW调整
			 if (rc.kb.bit.E)
			{
				if(cross.cnt_ref > CROSS_EXCHAGERIGHT_LIM)//-270
				{
					cross.cnt_ref = CROSS_EXCHAGERIGHT_LIM;
				}else
				{
					cross.cnt_ref +=0.5f;
				}
			}
			if(rc.kb.bit.Q)
			{
				if(cross.cnt_ref < CROSS_EXCHAGELEFT_LIM)//-570
				{
					cross.cnt_ref = CROSS_EXCHAGELEFT_LIM;
				}else
				{
					cross.cnt_ref -=0.5f;
				}
			}
			//滑轨前移 V后B前
			 if (rc.kb.bit.V)
			{
				if(stretch.cnt_ref > INNER_STRETCH)
				{
					stretch.cnt_ref = INNER_STRETCH;
				}else
				{
					stretch.cnt_ref ++;
				}
			}
			if(rc.kb.bit.B)
			{
				if(stretch.cnt_ref < STRETCH_MAX)
				{
					stretch.cnt_ref = STRETCH_MAX;
				}else
				{
					stretch.cnt_ref --;
				}
			}
			//高度调整
				if (rc.kb.bit.SHIFT)
			{
				if (uplift.cnt_ref < UPLIFT_MAX)
				{ uplift.cnt_ref += 2;}
				else
				{ uplift.cnt_ref = UPLIFT_MAX; }
			}
			else if (rc.kb.bit.CTRL)
			{
				if (uplift.cnt_ref > UPLIFT_EXCHAG_MIN)
				{ uplift.cnt_ref -= 2;}
				else 
				{uplift.cnt_ref = UPLIFT_EXCHAG_MIN;}
			}
			//吸盘调整
			 if (rc.kb.bit.C)
			{ spin.cnt_ref -= 0.5f;}
			else if (rc.kb.bit.Z)
			{ spin.cnt_ref += 0.5f;}

			if(IF_ML)
			{
				PUMP_OFF;
				CLAW_GET = NonGET;
				exchange_status  = EXCHANGE_BACK_COMPLETE;	
			}
		

}

void move_rotate_change()
{
	switch(move_rotate)
	{
		case UP:{
			if(IF_Q)
			{
				move_rotate = BACK;
			}
		}break;
		case BACK:{
			if(IF_Q)
			{
				move_rotate = UP;
			}
		
		}break;
		
	}
}