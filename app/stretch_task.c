#define __STRETCH_TASK_GLOBALS
#include "stretch_task.h"
#include "uplift_task.h"
#include "cmsis_os.h"
#include "math_calcu.h"
#include "bsp_can.h"
#include "string.h"
#include "pid.h"
#include "rotate_task.h"
#include "cross_task.h"
#include "modeswitch_task.h"
#include "keyboard_handle.h"
#define stretch_ratio (256.0f)
#define stretch_long_ratio (9.5f)
extern osMessageQId can2msqHandle;
extern osMessageQId can1msqHandle;
int16_t stretch_frame_period = 6;
uint8_t entire_init_flag;
void stretch_init()
{
	PID_struct_init(&pid_stretch_init_spd[0] , POSITION_PID, 4000, 1000,
								5.0f,	0.0f,	0.0f	);
PID_struct_init(&pid_stretch_init_spd[1] , POSITION_PID, 4000, 1000,
								5.0f,	0.0f,	0.0f	);
	PID_struct_init(&pid_stretch_pos[0], POSITION_PID, 6000, 0,
								15.0f,	0.0f,	0.0f	);
	PID_struct_init(&pid_stretch_pos[1], POSITION_PID, 6000, 0,
								15.0f,	0.0f,	0.0f	);	
	PID_struct_init(&pid_stretch_spd[0], POSITION_PID, 15000, 3000,
								7.0f,	0.05f,	0.0f	); 
	PID_struct_init(&pid_stretch_spd[1], POSITION_PID, 15000, 3000,
								7.0f,	0.05f,	0.0f	);

	stretch.state = STRETCH_UNKNOWN;
}

void stretch_task(void const *argu)
{
	uint32_t mode_wake_time = osKernelSysTick();
	for(;;)
	{
		taskENTER_CRITICAL();
		stretch.cnt_fdb[0] = (moto_stretch[0].total_ecd - stretch.offset[0])/stretch_ratio ;
		stretch.cnt_fdb[1] = (moto_stretch[1].total_ecd - stretch.offset[1])/stretch_ratio ;	
		stretch.long_fdb[0] = stretch.cnt_fdb[0]/stretch_long_ratio;
		stretch.long_fdb[1] = stretch.cnt_fdb[1]/stretch_long_ratio;
		switch(stretch.action) 
		{
			case STRETCH_STOP:
			{
				memset(stretch.current, 0, sizeof(stretch.current));	//电机停转	
			}break;	
			case STRETCH_AUTO:
			{
				if (fabs(stretch.ramp_set - stretch.cnt_ref) > stretch_frame_period)
							{
								if (stretch.ramp_set > stretch.cnt_ref)
										stretch.ramp_set -= stretch_frame_period;
								else
										stretch.ramp_set += stretch_frame_period;
							}
							else 
							{ stretch.ramp_set = stretch.cnt_ref; }
				
				stretch.spd_ref[0] = pid_calc(&pid_stretch_pos[0],stretch.cnt_fdb[0],stretch.ramp_set); 
				stretch.current[0] = pid_calc(&pid_stretch_spd[0],moto_stretch[0].speed_rpm,stretch.spd_ref[0]);
				stretch.spd_ref[1] = pid_calc(&pid_stretch_pos[1],stretch.cnt_fdb[1],-stretch.ramp_set); 
				stretch.current[1] = pid_calc(&pid_stretch_spd[1],moto_stretch[1].speed_rpm,stretch.spd_ref[1]);		

			}break;		
			case STRETCH_INIT:
			{
				stretch_init_step();
			}break;		
		}
		stretch.last_action = stretch.action;	
		//发送电流
		Can1_Queue.Current_U1.Current[3] = __REV16(stretch.current[0]);
		Can2_Queue.Current_U1.Current[3] = __REV16(stretch.current[1]); 
		osMessagePut(can2msqHandle,(uint32_t)&Can2_Queue,0);
		osMessagePut(can1msqHandle,(uint32_t)&Can1_Queue,0);
		
		taskEXIT_CRITICAL();
		osDelayUntil(&mode_wake_time, STRETCH_PERIOD);
	}
}
void stretch_init_step(void)
{
	static uint16_t stretch_init_count=0;	
	if(stretch.last_action != STRETCH_INIT)
	{ stretch.state = STRETCH_UNKNOWN; }
	if(uplift.state == UPLIFT_KNOWN && UPLIFT_CHECK)
	{
			switch(stretch.state)
			{
				case STRETCH_UNKNOWN:
				{
					stretch.current[0] = pid_calc(&pid_stretch_init_spd[0],moto_stretch[0].speed_rpm,-1000);
					//stretch.current[1] = pid_calc(&pid_stretch_init_spd[1],moto_stretch[0].speed_rpm,1000);
					stretch.current[1] = -stretch.current[0];
					//推出
					stretch_init_count++;
					if ( (stretch_init_count>20) && (ABS(moto_stretch[0].speed_rpm)<50) && (ABS(moto_stretch[1].speed_rpm)<50))
					{
						stretch.state = STRETCH_CALIBRA;
						stretch_init_count=0;
					}
				}break;
				case STRETCH_CALIBRA:
				{
					stretch.current[0] = pid_calc(&pid_stretch_init_spd[0],moto_stretch[0].speed_rpm,0);
					stretch.current[1] = -stretch.current[0];
					
					//推出
					stretch_init_count++;
					if( (stretch_init_count>10) /* && (cross.state == CROSS_KNOWN) && (rotate.state == ROTATE_KNOWN) */)
					{
	//					stretch.offset[0]=moto_stretch[0].total_ecd-486400-998400; //堵转后获得的相对编码值(+一个值可令零点后移)
	//					stretch.offset[1]=moto_stretch[1].total_ecd+486400+998400;		
						stretch.offset[0]=moto_stretch[0].total_ecd;
						stretch.offset[1]=moto_stretch[1].total_ecd;						
						stretch.state = STRETCH_CALIBRA_END;
						stretch_init_count=0;
					}
				}break;
				case STRETCH_CALIBRA_END :
				{
						
					if (rotate.state == ROTATE_KNOWN && cross.state == CROSS_KNOWN)
						{
							if ( ROTATE_CHECK &&CROSS_CHECK)
							{
								stretch.cnt_ref = STRETCH_BLOCKABLE;					
								stretch.state = STRETCH_KNOWN;
								stretch.action = STRETCH_AUTO;
								entire_init_flag =1;
							}
						}
				}break;	
				case STRETCH_KNOWN :
				{
					
				}break;
			
		}
	}
}
