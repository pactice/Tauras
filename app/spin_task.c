#define __SPIN_TASK_GLOBALS
#include "spin_task.h"
#include "bsp_can.h"
#include "pid.h"
#include "string.h"
#include "cmsis_os.h"
#include "math.h"
extern osMessageQId can1msqHandle;
extern osMessageQId can2msqHandle;

#define	spin_frame_period	3
#define spin_ratio (256.0f)

void spin_init()
{
	PID_struct_init(&pid_spin_pos, POSITION_PID, 2000, 1000,
								15.0f,	0.0f,	0.0f	);
	PID_struct_init(&pid_spin_spd, POSITION_PID, 10000, 5000,
								6.0f,	0.2f,	0.0f	); 
	spin.state = SPIN_UNKNOWN;
}

void spin_task(void const *argu)
{
	uint32_t mode_wake_time = osKernelSysTick();
	for(;;)
	{
		taskENTER_CRITICAL();
			spin.cnt_fdb = (moto_spin.total_ecd - spin.offset)/spin_ratio;
			switch(spin.action) 
		{
			case SPIN_STOP:
			{
         spin.current = 0;
				 spin.cnt_ref = spin.cnt_fdb;
			}break;	
			
			case SPIN_AUTO:
			{
				if(fabs(spin.ramp_set-spin.cnt_ref)>spin_frame_period)
				{
					if (spin.ramp_set > spin.cnt_ref)
					{
						spin.ramp_set -= spin_frame_period;
					}
					else spin.ramp_set += spin_frame_period;
				}
				else spin.ramp_set = spin.cnt_ref;
				spin.spd_ref = pid_calc(&pid_spin_pos,spin.cnt_fdb,spin.ramp_set); 
				spin.current = pid_calc(&pid_spin_spd,moto_spin.speed_rpm,spin.spd_ref);			
			}break;

			case SPIN_INIT:
			{
				spin_init_step();
			}break;				
		}	
		spin.last_action = spin.action;
		Can2_Queue.Current_U1.Current[0] = __REV16(spin.current);
		taskEXIT_CRITICAL();
		osDelayUntil(&mode_wake_time,SPIN_PERIOD);
	}
}
void spin_init_step(void)
{
	if(spin.last_action != SPIN_INIT)
	{ spin.state = SPIN_UNKNOWN; }
	
	switch(spin.state)
	{
		case SPIN_UNKNOWN:
		{
			moto_spin.round_cnt = 0;
			spin.offset = moto_spin.ecd - moto_spin.offset_ecd;
			spin.cnt_ref = 0;
			spin.ramp_set = 0;
			spin.state = SPIN_CALIBRA;
		}break;
		
		case SPIN_CALIBRA:
		{
			spin.state = SPIN_KNOWN;
			spin.action = SPIN_AUTO;
		}break;
		
		case SPIN_KNOWN :
		{
		}break;	
	}		
}
