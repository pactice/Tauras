 #define __CHASSIS_TASK_GLOBALS
#include "chassis_task.h"
#include "math_calcu.h"
#include "remote_msg.h"
#include "cmsis_os.h"
#include "pid.h"
#include "keyboard_handle.h"
#include "cradle_task.h"
#include "keyboard_handle.h"
#include "modeswitch_task.h"
/**
  * @brief chassis_task
  * @param     
  * @attention  
	* @note  
  */
void chassis_task(void const *argu)
{ 
	uint32_t mode_wake_time = osKernelSysTick();

	
	for(;;)
	{
		taskENTER_CRITICAL();
			
		switch(chassis.ctrl_mode)
		{
			case CHASSIS_PROTECT:
			{
				chassis.vx = 0;
				chassis.vy = 0;
				chassis.vw = 0;
			}break;
			
			case CHASSIS_REMOTER:
			{
				chassis.vx = rc.ch4*5;
				chassis.vy = rc.ch3*(5);
				chassis.vw = -rc.ch1 * 5;

			}break;
			
			case CHASSIS_KEYBOARD:
			{
				
				cradle.yaw_cnt_ref = CRADLE_YAW_BACK;
				
				if (rc.kb.bit.CTRL) 					
				{	 
					chassis_speed = SPEED_SLOW;
					mouse_speed = 60;
				}   			
				else if (rc.kb.bit.SHIFT)		
				{	 
					chassis_speed = SPEED_FAST; 
					mouse_speed = 100;
				}  
				else{
						
					 chassis_speed = SPEED_NORMAL;
					 mouse_speed = 90;
				}
				
				chassis_ramp();
				chassis.vx = chassis_ws_ramp.out;
				chassis.vy = chassis_ad_ramp.out;
				chassis.vw = rc.mouse.x*mouse_speed;
			}break;
			
			case CHASSIS_STAY: //停在原地
			{	
				chassis.vx = 0;
				chassis.vy = 0;
				chassis.vw = 0;
			}break;
			case CHASSIS_TURN:
			{
				cradle.yaw_cnt_ref = CRADLE_YAW_FRONT; 
				
				if(func_mode == MOVE_MODE)
				{
				
					if (rc.kb.bit.CTRL) 							
					{	 
						chassis_speed = SPEED_SLOW; 		 
						mouse_speed = 60;
					}  		
					else if (rc.kb.bit.SHIFT)		
					{	 
						chassis_speed = SPEED_FAST; 	
						mouse_speed = 100;
					}   
					else
					{ 
						chassis_speed = SPEED_NORMAL; 		
							mouse_speed = 90;
					}
				}
				else{
						if(func_mode == EXCHANGE_MODE)
					{
						chassis_speed = SPEED_VERY_SLOW; 		
						mouse_speed = 60;
					}
					else
					{
						chassis_speed = SPEED_SLOW; 	
						mouse_speed = 60;			
					}  
				
				
				}
				chassis_ramp();
				chassis.vx = chassis_ws_ramp.out;
				chassis.vy = chassis_ad_ramp.out;
				chassis.vw = rc.mouse.x*(-mouse_speed);
			}break;
		}		
		
		taskEXIT_CRITICAL();
		osDelayUntil(&mode_wake_time, CHASSIS_PERIOD);
	}
}

/**
  * @brief          底盘斜坡函数
  * @author         
  * @param[in]      void
  * @retval         void
  */
void chassis_ramp()
{
  if(rc.kb.bit.W)
	{
	  ramp_calc(&chassis_ws_ramp,1.0f,30.0f, chassis_speed, 0.0f);			
	}
	else if(rc.kb.bit.S)
	{
		ramp_calc(&chassis_ws_ramp,1.0f,-30.0f, 0.0f, -chassis_speed);			
	}
	else
	{
		if(chassis_ws_ramp.out > 0)
		{
			ramp_calc(&chassis_ws_ramp,1.0f,-30.0f, chassis_speed, 0.0f);			
		}
		else if(chassis_ws_ramp.out < 0)
		{
			ramp_calc(&chassis_ws_ramp,1.0f,30.0f, 0.0f, -chassis_speed);	
		}
	}
	
	if(rc.kb.bit.D)
	{
	  ramp_calc(&chassis_ad_ramp,1.0f,30.0f, 1.2*chassis_speed, 0.0f);			
	}
	else if(rc.kb.bit.A)
	{
	  ramp_calc(&chassis_ad_ramp,1.0f,-30.0f, 0.0f, -1.2*chassis_speed);			
	}
	else
	{
		if(chassis_ad_ramp.out > 0)
		{
			ramp_calc(&chassis_ad_ramp,1.0f,-30.0f, 1.2*chassis_speed, 0.0f);			
		}
		else if(chassis_ad_ramp.out < 0)
		{
			ramp_calc(&chassis_ad_ramp,1.0f,30.0f, 0.0f, -1.2*chassis_speed);	
		}
	}
}
