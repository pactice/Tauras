#define __ADDSTRETCH_TASK_GLOBALS
#include "addstretch_task.h"
#include "cmsis_os.h"
#include "math_calcu.h"
#include "bsp_can.h"
#include "math_calcu.h"
#include "string.h"
#include "stretch_task.h"
#include "pid.h"
#define addstretch_ratio (256.0f)
extern osMessageQId can1msqHandle;
extern osMessageQId can2msqHandle;
int16_t addstretch_frame_period = 4;
void addstretch_init()
{
	PID_struct_init(&pid_addstretch_init_spd[0], POSITION_PID, 4000, 1000,
								5.0f,	0.0f,	0.0f	);
		PID_struct_init(&pid_addstretch_init_spd[1], POSITION_PID, 4000, 1000,
								5.0f,	0.0f,	0.0f	);
	
	PID_struct_init(&pid_addstretch_pos[0], POSITION_PID, 8000, 0,
								18.0f,	0.0f,	0.0f	);
	PID_struct_init(&pid_addstretch_pos[1], POSITION_PID, 8000, 0,
								18.0f,	0.0f,	0.0f	);	
	PID_struct_init(&pid_addstretch_spd[0], POSITION_PID, 15000, 3000,
								7.0f,	0.05f,	0.0f	); 
	PID_struct_init(&pid_addstretch_spd[1], POSITION_PID, 15000, 3000,
								7.0f,	0.05f,	0.0f	);
}

void addstretch_task(void const *argu)
{
	uint32_t mode_wake_time = osKernelSysTick();
		
	for(;;)
	{
		taskENTER_CRITICAL();
		addstretch.cnt_fdb[0] = (moto_addstretch[0].total_ecd - addstretch.offset[0] )/addstretch_ratio;
		addstretch.cnt_fdb[1] = (moto_addstretch[1].total_ecd - addstretch.offset[1] )/addstretch_ratio;	
		switch(addstretch.action) 
		{
			case ADDSTRETCH_STOP:
			{
				memset(addstretch.current, 0, sizeof(addstretch.current));	//电机停转	
			}break;
			case ADDSTRETCH_AUTO:
			{
				if (fabs(addstretch.ramp_set - addstretch.cnt_ref) > addstretch_frame_period)
							{
								if (addstretch.ramp_set > addstretch.cnt_ref)
										addstretch.ramp_set -= addstretch_frame_period;
								else
										addstretch.ramp_set += addstretch_frame_period;
							}
							else 
							{ addstretch.ramp_set = addstretch.cnt_ref; }
				
				addstretch.spd_ref[0] = pid_calc(&pid_addstretch_pos[0],addstretch.cnt_fdb[0],addstretch.ramp_set); 
				addstretch.current[0] = pid_calc(&pid_addstretch_spd[0],moto_addstretch[0].speed_rpm,addstretch.spd_ref[0]);
				addstretch.spd_ref[1] = pid_calc(&pid_addstretch_pos[1],addstretch.cnt_fdb[1],-addstretch.ramp_set); 
				addstretch.current[1] = pid_calc(&pid_addstretch_spd[1],moto_addstretch[1].speed_rpm,addstretch.spd_ref[1]);	
					
			}break;
			case ADDSTRETCH_INIT:
			{
				addstretch_init_step();
			}break;
		}
		addstretch.last_action = addstretch.action;
		Can1_Queue.Current_U1.Current[2] = __REV16(addstretch.current[0]);
		Can2_Queue.Current_U1.Current[2] = __REV16(addstretch.current[1]); 
		osMessagePut(can1msqHandle,(uint32_t)&Can1_Queue,0);
		osMessagePut(can2msqHandle,(uint32_t)&Can2_Queue,0);		
		taskEXIT_CRITICAL();
		osDelayUntil(&mode_wake_time, ADDSTRETCH_PERIOD);
	}
}

void addstretch_init_step(void)
{
	static uint16_t addstretch_init_count=0;
	if(addstretch.last_action != ADDSTRETCH_INIT)
	{ addstretch.state = ADDSTRETCH_UNKNOWN; }
	
		switch(addstretch.state)
		{
			case ADDSTRETCH_UNKNOWN:
			{
				memset(addstretch.current, 0, sizeof(addstretch.current));
				addstretch_init_count ++;
				if( (addstretch_init_count > 10) )//&& (stretch.state == STRETCH_CALIBRA) )	//停转，并确保已伸出
				{
					addstretch.state = ADDSTRETCH_CALIBRA;
					addstretch_init_count = 0;
				}

			}break;
			case ADDSTRETCH_CALIBRA:
			{
				
				addstretch.current[0] = pid_calc(&pid_addstretch_init_spd[0],moto_addstretch[0].speed_rpm,1000);
				addstretch.current[1] = pid_calc(&pid_addstretch_init_spd[1],moto_addstretch[1].speed_rpm,-1000);
				if(ABS(moto_addstretch[0].speed_rpm)<5 && ABS(moto_addstretch[1].speed_rpm)<5)
					{
						addstretch_init_count++;
						if(addstretch_init_count > 50)
						{
							addstretch.state = ADDSTRETCH_CALIBRA_END;
							addstretch_init_count = 0;
						}
					}else{ addstretch_init_count = 0; }
			}break;
			case ADDSTRETCH_CALIBRA_END :
			{
				addstretch.current[0] = pid_calc(&pid_addstretch_init_spd[0],moto_addstretch[0].speed_rpm,0);
				addstretch.current[1] = pid_calc(&pid_addstretch_init_spd[1],moto_addstretch[1].speed_rpm,0);
						addstretch.offset[0] = moto_addstretch[0].total_ecd;
						addstretch.offset[1] = moto_addstretch[1].total_ecd;
						addstretch.state = ADDSTRETCH_KNOWN;
						
						addstretch_init_count = 0;

			}break;
			case ADDSTRETCH_KNOWN :
				{
					addstretch.cnt_ref = OUTER_ADDSTRETCH;
					addstretch.action = ADDSTRETCH_AUTO;
				}break;
				default :
					break;
			}
	
}