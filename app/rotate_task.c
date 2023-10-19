#define __ROTATE_TASK_GLOBALS
#include "string.h"
#include "cmsis_os.h"
#include "rotate_task.h"
#include "bsp_can.h"
#include "math_calcu.h"
#include "pid.h"
#include "uplift_task.h"
#include "stretch_task.h"
#include "addstretch_task.h"
#include "keyboard_handle.h"
#include "cross_task.h"
// -4 -354
extern osMessageQId can2msqHandle;
extern osMessageQId can1msqHandle;
#define rotate_ratio (256.0f)
#define rotate_angle_ratio  (1.7f)
float rotate_frame_period = 1;
float rotateblock_frame_period = 1;
void rotate_init()
{
	rotate.state = ROTATE_UNKNOWN;   //初始化夹取机构为未知状态

	//旋转夹取电机的PID
	PID_struct_init(&pid_rotate_init_spd[0], POSITION_PID, 8000, 7000,
								5.0f,	0.0f,	0.0f	);
	PID_struct_init(&pid_rotate_init_spd[1], POSITION_PID, 8000, 7000,
								5.0f,	0.0f,	0.0f	);		
	
	PID_struct_init(&pid_rotate_pos[0], POSITION_PID, 5000, 4000,
								14.0f,	0.0f,	0.0f	); 
	PID_struct_init(&pid_rotate_pos[1], POSITION_PID, 5000, 4000,
								14.0f,	0.0f,	0.0f	);	
	PID_struct_init(&pid_rotate_spd[0], POSITION_PID, 5000, 4000,
								5.0f,	0.5f,	0.0f	); 
	PID_struct_init(&pid_rotate_spd[1], POSITION_PID, 5000, 4000,
								5.0f,	0.5f,	0.0f	);
  
    //ROTATE_AUTO_BOLCK
	PID_struct_init(&pid_rotate_block_pos[0], POSITION_PID, 7000, 6000,
								13.0f,	0.0f,	0.0f	); 
	PID_struct_init(&pid_rotate_block_pos[1], POSITION_PID, 7000, 6000,
								13.0f,	0.0f,	0.0f	);	
	PID_struct_init(&pid_rotate_block_spd[0], POSITION_PID,  6000, 5000,
								5.0f,	0.5f,	0.0f	); 
	PID_struct_init(&pid_rotate_block_spd[1], POSITION_PID,  6000, 5000,
								5.0f,	0.5f,	0.0f	);
}

void rotate_task(void const *argu)
{
	uint32_t mode_wake_time = osKernelSysTick();
	for(;;)
	{
		taskENTER_CRITICAL();	
		rotate.cnt_fdb[0] = (moto_rotate[0].total_ecd - rotate.offset[0])/rotate_ratio;
		rotate.cnt_fdb[1] = (moto_rotate[1].total_ecd - rotate.offset[1])/rotate_ratio;
		rotate.angle_fdb[0] = rotate.cnt_fdb[0]/rotate_angle_ratio;
		rotate.angle_fdb[1] = rotate.cnt_fdb[1]/rotate_angle_ratio;
		switch(rotate.action) 
		{
			case ROTATE_STOP:
			{
				pid_rotate_spd[0].iout = 0;
				pid_rotate_spd[1].iout = 0;
				memset(rotate.current, 0, sizeof(rotate.current));
			}break;	
			
			case ROTATE_AUTO: 
			{
				if(addstretch.cnt_ref==ADDSTRETCH_PITCHENABLE)
				{
				  if(ADDSTRETCH_CHECK)
					{				
//						if(CLAW_GET ==NonGET)
//						{
							if (fabs(rotate.ramp_set - rotate.cnt_ref) > rotate_frame_period)
							{
								if (rotate.ramp_set > rotate.cnt_ref)
										rotate.ramp_set -= rotate_frame_period;
								else
										rotate.ramp_set += rotate_frame_period;
							}
							else 
							{ rotate.ramp_set = rotate.cnt_ref; }
							
							rotate.spd_ref[0] = pid_calc(&pid_rotate_pos[0],rotate.cnt_fdb[0],rotate.ramp_set); 
							rotate.current[0] = pid_calc(&pid_rotate_spd[0],moto_rotate[0].speed_rpm,rotate.spd_ref[0]);

							rotate.spd_ref[1] = pid_calc(&pid_rotate_pos[1],rotate.cnt_fdb[1],-rotate.ramp_set); 
							rotate.current[1] = pid_calc(&pid_rotate_spd[1],moto_rotate[1].speed_rpm,rotate.spd_ref[1]);		
//						}
//						else if(CLAW_GET == GET)
//						{
//							if (fabs(rotate.ramp_set - rotate.cnt_ref) > rotateblock_frame_period)
//							{
//								if (rotate.ramp_set > rotate.cnt_ref)
//										rotate.ramp_set -= rotateblock_frame_period;
//								else
//										rotate.ramp_set += rotateblock_frame_period;
//							}
//							else 
//							{ rotate.ramp_set = rotate.cnt_ref; }
//							
//							rotate.spd_ref[0] = pid_calc(&pid_rotate_block_pos[0],rotate.cnt_fdb[0],rotate.ramp_set); 
//							rotate.current[0] = pid_calc(&pid_rotate_block_spd[0],moto_rotate[0].speed_rpm,rotate.spd_ref[0]);

//							rotate.spd_ref[1] = pid_calc(&pid_rotate_block_pos[1],rotate.cnt_fdb[1],-rotate.ramp_set); 
//							rotate.current[1] = pid_calc(&pid_rotate_block_spd[1],moto_rotate[1].speed_rpm,rotate.spd_ref[1]);		
//						}
												
					}
				}
			}break;
			
			case ROTATE_UP:   
			{
					memset(rotate.current, 0, sizeof(rotate.current));
			}break;
			
			case ROTATE_DOWN:
			{
					memset(rotate.current, 0, sizeof(rotate.current));
			}break;
			
			case ROTATE_STAY:
			{
					memset(rotate.current, 0, sizeof(rotate.current));
			}break;
			
			case ROTATE_INIT:
			{
				rotate_init_step();
			}break;		
		}
			
		rotate.last_cnt_ref = rotate.cnt_ref;
		rotate.last_action = rotate.action;
	  Can1_Queue.Current_U2.Current[0] = __REV16(rotate.current[0]);
		Can2_Queue.Current_U2.Current[0] = __REV16(rotate.current[1]); 
		osMessagePut(can1msqHandle,(uint32_t)&Can1_Queue,0);
		osMessagePut(can2msqHandle,(uint32_t)&Can2_Queue,0);
		taskEXIT_CRITICAL();
		osDelayUntil(&mode_wake_time, ROTATE_PERIOD);
	}
}

void rotate_init_step(void)
{
	static uint8_t rotate_init_count=0;
	if(rotate.last_action != ROTATE_INIT)
	{ rotate.state = ROTATE_UNKNOWN; }
	if(CROSS_CHECK&&cross.state == CROSS_KNOWN)
	{
		
		switch(rotate.state)
		{
			case ROTATE_UNKNOWN:
			{
				memset(rotate.current, 0, sizeof(rotate.current));
				rotate_init_count ++;
				if( (rotate_init_count > 10) )
				{
					rotate.state = ROTATE_CALIBRA;
					rotate_init_count = 0;
				}
			}break;
			
			case ROTATE_CALIBRA:
			{
				rotate.current[0] = pid_calc(&pid_rotate_init_spd[0],moto_rotate[0].speed_rpm,1000);
				rotate.current[1] = pid_calc(&pid_rotate_init_spd[1],moto_rotate[1].speed_rpm,-1000);
				if(ABS(moto_rotate[0].speed_rpm)<5 && ABS(moto_rotate[1].speed_rpm)<5)
					{
						rotate_init_count++;
						if(rotate_init_count > 20)
						{
							rotate.current[0] = pid_calc(&pid_rotate_init_spd[0],moto_rotate[0].speed_rpm,0);
							rotate.current[1] = pid_calc(&pid_rotate_init_spd[1],moto_rotate[1].speed_rpm,0);
							rotate.state = ROTATE_CALIBRA_END;
							rotate_init_count = 0;
						}
					}else{ rotate_init_count = 0; }
					
			}break;
			
			case ROTATE_CALIBRA_END :
				{
						rotate.offset[0] = moto_rotate[0].total_ecd;
						rotate.offset[1] = moto_rotate[1].total_ecd;
		//                rotate.cnt_ref = AIR_ROTATE;
						rotate.state = ROTATE_KNOWN;
						rotate_init_count = 0;
				}break;		
			case ROTATE_KNOWN :
				{
					rotate.cnt_ref = ROTATE_VERTICAL;
					rotate.action = ROTATE_AUTO;
				}break;	
			default :
					break;			
		}
	
	}
}

