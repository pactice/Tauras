#define __UPLIFT_TASK_GLOBALS
#include "uplift_task.h"
#include "string.h"
#include "cmsis_os.h"
#include "pid.h"
#include "bsp_can.h"
#include "math_calcu.h"
#include "keyboard_handle.h"
#include "stretch_task.h"
#include "convey_task.h"
extern osMessageQId can1msqHandle;
#define uplift_ratio (256.0f)
#define uplift_height_ratio (37.5f)
void upliftup_init(void)
{
		//抬升的PID
	PID_struct_init(&pid_uplift_height[0], POSITION_PID, 3600, 1000,
								25.0f,	0.0f,	0.0f	); 
	PID_struct_init(&pid_uplift_height[1], POSITION_PID, 3600, 1000,
								25.0f,	0.0f,	0.0f	); 	
	PID_struct_init(&pid_uplift_spd[0], POSITION_PID, 15000, 8000,
								17.0f,	0.2f,	0.0f	); 
	PID_struct_init(&pid_uplift_spd[1], POSITION_PID, 15000, 8000,
								17.0f,	0.2f,	0.0f	);  
	uplift.state = UPLIFT_UNKNOWN;
	memset(uplift.height_offset, 0, sizeof(uplift.height_offset));

}

void uplift_task(void const *argu)
{
	uint32_t mode_wake_time = osKernelSysTick();
	for(;;)
	{
		taskENTER_CRITICAL();
		
		uplift.cnt_fdb[0]=(moto_uplift[0].total_ecd - uplift.height_offset[0])/uplift_ratio;			//左
		uplift.cnt_fdb[1]=(moto_uplift[1].total_ecd - uplift.height_offset[1])/uplift_ratio;			//右
		uplift.height_fdb[0] = uplift.cnt_fdb[0]/uplift_height_ratio;
		uplift.height_fdb[1] = uplift.cnt_fdb[1]/uplift_height_ratio;
		switch(uplift.action) 
		{
			case UPLIFT_STOP:
			{
				memset(uplift.current, 0, sizeof(uplift.current));//抬升电机停转
			}break;	
			case UPLIFT_AUTO://自动
			{
				if(uplift.cnt_ref > UPLIFT_MAX)
				{
					uplift.cnt_ref = UPLIFT_MAX;
				}

				
				uplift.cnt_unset_ref = -uplift.cnt_ref - 0;
				
				uplift.spd_ref[0] = pid_calc(&pid_uplift_height[0],uplift.cnt_fdb[0],uplift.cnt_ref); //高度环
				uplift.spd_ref[1] = pid_calc(&pid_uplift_height[1],uplift.cnt_fdb[1],uplift.cnt_unset_ref); //高度环

				uplift.current[0] = pid_calc(&pid_uplift_spd[0],moto_uplift[0].speed_rpm,uplift.spd_ref[0]); //速度环
				uplift.current[1] = pid_calc(&pid_uplift_spd[1],moto_uplift[1].speed_rpm,uplift.spd_ref[1]); //速度环	
				
			
			}break;
			case UPLIFT_FALL://下落
			{
				uplift_fall_step();
			}break;
			case UPLIFT_INIT://初始化
			{
				upliftup_init_step();
			}break;
		
		}
		uplift.last_action = uplift.action;
		Can1_Queue.Current_U1.Current[0] = __REV16(uplift.current[0]);
		Can1_Queue.Current_U1.Current[1] = __REV16(uplift.current[1]); 
		
		taskEXIT_CRITICAL();
		osDelayUntil(&mode_wake_time, UPLIFT_PERIOD);
	}
}

void uplift_fall_step(void)
{	
	static uint8_t uplift_free_times = 0;

	if(uplift.last_action != UPLIFT_FALL)		//第一次进入下落模式
	{
		uplift.fall_step = UPLIFTFALL_PID;
		uplift_free_times = 0;
	}
	switch(uplift.fall_step)
	{
		case UPLIFTFALL_PID:
		{
			if(fabs(uplift.cnt_fdb[0]) > 100)
			{
				uplift.cnt_ref = 100;
				uplift.cnt_unset_ref = -uplift.cnt_ref - 0;
				
				if(UPLIFT_ERROR0>1000)		{ pid_uplift_spd[0].iout=0; }	
				if(UPLIFT_ERROR1>1000)		{ pid_uplift_spd[1].iout=0; }
				
				uplift.spd_ref[0] = pid_calc(&pid_uplift_height[0],uplift.cnt_fdb[0],uplift.cnt_ref); //高度环
				uplift.current[0] = pid_calc(&pid_uplift_spd[0],moto_uplift[0].speed_rpm,uplift.spd_ref[0]); //速度环		

				uplift.spd_ref[1] = pid_calc(&pid_uplift_height[1],uplift.cnt_fdb[1],uplift.cnt_unset_ref); //高度环
				uplift.current[1] = pid_calc(&pid_uplift_spd[1],moto_uplift[1].speed_rpm,uplift.spd_ref[1]); //速度环				
				
				if(UPLIFT_ERROR0<50||UPLIFT_ERROR1<50)
				{
					uplift.fall_step = UPLIFTFALL_FREE;
				}							
			}else{ uplift.fall_step = UPLIFTFALL_FREE; }		 
		}break;

		case UPLIFTFALL_FREE:
		{
			memset(uplift.current, 0, sizeof(uplift.current));//抬升电机停转
			if(ABS(moto_uplift[0].speed_rpm)<80 && ABS(moto_uplift[1].speed_rpm)<80)
			{
				uplift_free_times ++;
				uplift.height_offset[0] = moto_uplift[0].total_ecd;
				uplift.height_offset[1] = moto_uplift[1].total_ecd;
				if(uplift_free_times>50)
				{
					uplift.cnt_ref = uplift.cnt_fdb[0];
					uplift_free_times = 0;
					uplift.action = UPLIFT_AUTO;						
				}				
			}else { uplift_free_times = 0; }	
		}break;
	}
}

uint8_t handle_lt_times1 = 0;
void upliftup_init_step(void)
{
	if(uplift.last_action != UPLIFT_INIT)
	{ 
		uplift.state = UPLIFT_UNKNOWN; 
	}
	switch(uplift.state)
	{
		case UPLIFT_UNKNOWN:
		{
			memset(uplift.current, 0, sizeof(uplift.current));//电流清零
			memset(uplift.height_offset, 0, sizeof(uplift.height_offset));//补偿值清零
			if(ABS(moto_uplift[0].speed_rpm)<50 && ABS(moto_uplift[1].speed_rpm)<50)
			{
				handle_lt_times1++;
				if(handle_lt_times1>30)
				{
					handle_lt_times1 = 0;
					uplift.state = UPLIFT_CALIBRA;
				}
			}else{handle_lt_times1=0;}//如果不能连续自加要求次数,自加清零
			}break;
		case UPLIFT_CALIBRA:
			{
				uplift.height_offset[0] = moto_uplift[0].total_ecd;
				uplift.height_offset[1] = moto_uplift[1].total_ecd;
				uplift.cnt_ref = UPLIFT_RESETHEIGHT;//死亡防止头在里面
				uplift.action = UPLIFT_AUTO;
				uplift.state = UPLIFT_KNOWN;
			
			
			}break;
		case UPLIFT_KNOWN :
			{		
				
			}break;
	}
	
}