#define __CRADLE_TASK_GLOBALS
#include "cradle_task.h"
#include "bsp_can.h"
#include "DataScope_DP.h"
#include "cmsis_os.h"
#include "string.h"
#include "math_calcu.h"
#include "bsp_usart.h"
#include "pid.h"
int16_t yaw_frame_period = 1;

#define YAW_RADIO			(24.0f)
extern osMessageQId can2msqHandle;

void cradle_init()
{
	cradle.ctrl_mode = CRADLE_STOP;
	PID_struct_init(&pid_yaw_init_spd, POSITION_PID, 3000, 2000,
								15.0f,	0.0f,	0.0f	);	

	PID_struct_init(&pid_yaw_pos, POSITION_PID, 4000, 3000,
								35.0f,0.0f,	50.0f	);
	PID_struct_init(&pid_yaw_spd, POSITION_PID, 8000, 6000,
								5.5f,	0.0f,	0.0f	);

}

void cradle_task(void const *argu)
{
	uint32_t mode_wake_time = osKernelSysTick();
	for(;;)
	{
		
		cradle.yaw_cnt_fdb = (moto_yaw.total_ecd - cradle.yaw_offset )/YAW_RADIO;
		cradle.yaw_spd_fdb = moto_yaw.speed_rpm;

		switch(cradle.ctrl_mode)
		{
			case CRADLE_STOP:
			{
				 cradle.yaw_current =0;
			}break;
			case CRADLE_INIT:
			{

					cradle_init_step();
			}break;
			case CRADLE_KEYBOARD:
			{	

					cradle.yaw_spd_ref = pid_calc(&pid_yaw_pos,cradle.yaw_cnt_fdb,cradle.yaw_cnt_ref);
					cradle.yaw_current = pid_calc(&pid_yaw_spd,cradle.yaw_spd_fdb,cradle.yaw_spd_ref);
				
			}break;
			case CRADLE_BACK:
			{
				
				
			}break;
			case CRADLE_SETMID:
			{
			
			}break;
		}
		
		cradle.last_ctrl_mode = cradle.ctrl_mode;
		Can2_Queue.Current_U2.Current[1] = __REV16(cradle.yaw_current); 
		osDelayUntil(&mode_wake_time, CRADLE_PERIOD);
	}
}

void cradle_init_step(void)
{
		static uint16_t	yaw_init_count = 0;

			if(cradle.last_ctrl_mode != CRADLE_INIT)
	{ cradle.state = CRADLE_UNKNOWN; }
		switch(cradle.state)
		{
			case CRADLE_UNKNOWN:
			{
				memset(&cradle.yaw_current,0,sizeof(cradle.yaw_current));
				yaw_init_count++;
				if(yaw_init_count > 10)
				{
					cradle.state = CRADLE_YAWCALIBRA;
					yaw_init_count =0;
				}
			}
			break;
			case CRADLE_YAWCALIBRA:
			{
				cradle.yaw_current =pid_calc(&pid_yaw_init_spd,moto_yaw.speed_rpm,500);
				if(ABS(moto_yaw.speed_rpm) <20)
				{
					yaw_init_count ++;
					if(yaw_init_count > 50)
					{
						cradle.yaw_offset = moto_yaw.total_ecd;
						yaw_init_count = 0;
						cradle.state = CRADLE_CHCALIBRAEND;						
					}
				}
			}
			break;
			case CRADLE_CHCALIBRAEND:
			{
				cradle.yaw_cnt_ref = CRADLE_YAW_FRONT;				
				cradle.yaw_current =pid_calc(&pid_yaw_init_spd,moto_yaw.speed_rpm,0);
				cradle.yaw_offset = moto_yaw.total_ecd;
				cradle.state = CRADLE_KNOWN;
				
			}
			break;
			
			case CRADLE_KNOWN:
			{
				
				cradle.ctrl_mode = CRADLE_KEYBOARD;
				
			}
			break;
		}
			
}