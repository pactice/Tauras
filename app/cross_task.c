#define __CROSS_TASK_GLOBALS
#include "cross_task.h"
#include "cmsis_os.h"
#include "pid.h"
#include "addstretch_task.h"
#include "bsp_can.h"
#include "math_calcu.h"
#include "keyboard_handle.h"
#include "rotate_task.h"
#include "stretch_task.h"
#include "keyboard_handle.h"
#define cross_radio (256.0f)
#define cross_angle_radio (1.7f)
extern osMessageQId can2msqHandle;
int16_t cross_frame_period = 2;
//-354  -2
void cross_init(void)
{
	PID_struct_init(&pid_cross_pos, POSITION_PID, 8000, 5000,
								16.0f,	0.0f,	0.0f	);
	PID_struct_init(&pid_cross_spd, POSITION_PID, 3000, 5000,
								14.0f,	0.0f,	6.0f	); 
	PID_struct_init(&pid_crosspos_block, POSITION_PID, 8000, 5000,
								25.0f,	0.0f,	0.0f	);
	PID_struct_init(&pid_crossspd_block, POSITION_PID, 6000, 5000,
								14.0f,	0.0f,	0.0f	); 
	cross.state = CROSS_UNKNOWN;
}

void cross_task(void const *argu)
{
	uint32_t mode_wake_time = osKernelSysTick();
	for(;;)
	{
		 taskENTER_CRITICAL();
		 cross.cnt_fdb = (moto_cross.total_ecd - cross.offset)/cross_radio;
		 cross.angle_fdb = cross.cnt_fdb/cross_angle_radio;
     switch(cross.action)
		 {
				case CROSS_STOP:
				{
						//电流清零
						cross.current = 0;
						//积分清零
						pid_cross_pos.iout = 0;
						pid_cross_spd.iout = 0;
				}break;
				case CROSS_AUTO:
				{
						if (fabs(cross.ramp_set - cross.cnt_ref) > cross_frame_period)
							{
								if (cross.ramp_set > cross.cnt_ref)
										cross.ramp_set -= cross_frame_period;
								else
										cross.ramp_set += cross_frame_period;
							}
							else 
							{ cross.ramp_set = cross.cnt_ref; }
												
							if(CLAW_GET ==NonGET)
						{
							cross.spd_ref = pid_calc(&pid_cross_pos,cross.cnt_fdb,cross.ramp_set);
							cross.current = pid_calc(&pid_cross_spd,moto_cross.speed_rpm,cross.spd_ref);
						}
						else if(CLAW_GET == GET)
						{
							cross.spd_ref = pid_calc(&pid_crosspos_block,cross.cnt_fdb,cross.ramp_set);
							cross.current = pid_calc(&pid_crossspd_block,moto_cross.speed_rpm,cross.spd_ref);	
						}	
						
				}break;
				case CROSS_INIT:
				{
						cross_init_step();
				}break;
				default:break;       
		 }			 
		 cross.last_action = cross.action;
		 Can2_Queue.Current_U1.Current[1] = __REV16(cross.current); 
		 osMessagePut(can2msqHandle,(uint32_t)&Can2_Queue,0);
		 taskEXIT_CRITICAL();
     osDelayUntil(&mode_wake_time,CROSS_PERIOD);
	}
}

/**
 * 描述	初始化横移机构
 * @date 2022-02-13
 * @returns {any}
 */
void cross_init_step()
{
	static uint8_t cross_init_times = 0;

	if ( cross.last_action != CROSS_INIT )		//防止重复初始化
	{ cross.state = CROSS_UNKNOWN; }

    if(addstretch.state == ADDSTRETCH_KNOWN && stretch.state == STRETCH_CALIBRA_END&&ADDSTRETCH_CHECK)
    {
		
			switch (cross.state)
			{
				case (CROSS_UNKNOWN):
				{
					 cross.current = 0;
					 cross_init_times ++;
					 if ( (cross_init_times > 10) )
					 {
							 cross.state = CROSS_CALIBRA;
							 cross_init_times = 0;
					 }
				}break;
				case (CROSS_CALIBRA):
				{
						cross.current = pid_calc(&pid_cross_spd,moto_cross.speed_rpm,800);	//赋予一定速度
						pid_cross_spd.iout = 0;
						if ( ABS(moto_cross.speed_rpm)<50 )
						{
								cross_init_times ++;
								if (cross_init_times > 20)
								{
									cross.current = pid_calc(&pid_cross_spd,moto_cross.speed_rpm,0);	//赋予一定速度
										cross.state = CROSS_CALIBRAEND;
								}
						}
						else { cross_init_times = 0; }
				}break;
				case (CROSS_CALIBRAEND):
				{
						cross.offset = moto_cross.total_ecd;			
            
						cross.state = CROSS_KNOWN;
						cross_init_times = 0;
				}break;
					case (CROSS_KNOWN):             
					{
						cross.cnt_ref = CROSS_MID ;							
						cross.action = CROSS_AUTO;
					}
					default:
							break;
			}
		
	}
}
