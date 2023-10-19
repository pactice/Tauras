#define __CONVEY_TASK_GLOBALS
#include "convey_task.h"
#include "cmsis_os.h"
#include "bsp_can.h"
#include "pid.h"
#include "usart.h"
#define convey_ratio (256.0f)
uint8_t cmd[5] = {0x55,0xAA,0x81,0x00,0xFA};  //»ñÈ¡¾àÀëÖ¸Áî
extern osMessageQId can1msqHandle;


void convey_init()
{
	PID_struct_init(&pid_convey_stay_pos, POSITION_PID, 6000, 4000,
								20.0f,	0.0f,	0.0f	); 
	PID_struct_init(&pid_convey_stay_spd, POSITION_PID, 10000, 5000,
								6.0f,	0.05f,	0.0f	);  
	PID_struct_init(&pid_convey_store_spd, POSITION_PID, 8000, 5000,
								6.0f,	0.0f,	0.0f	);
	convey.state = CONVEY_UNKNOWN;   
}

void convey_task(void const *argu)
{ 
	uint32_t mode_wake_time = osKernelSysTick();
	for(;;)
	{
		static uint16_t convey_time1 = 0;
		static uint16_t convey_time2 = 0;
		taskENTER_CRITICAL();
		convey.cnt_fdb = (moto_convey.total_ecd - convey.offset)/convey_ratio;

		switch(convey.action) 
		{
			case CONVEY_STOP:
			{
				convey_time1 = 0;
				convey_time2 = 0;
				convey.current = 0;
			}break;	

			case CONVEY_STORE:
			{
				convey_time1++;
				convey.current = pid_calc(&pid_convey_store_spd,moto_convey.speed_rpm,-2000);
				if(convey_time1 >1000)
				{ convey.action = CONVEY_STOP; }
			}break;
			
			case CONVEY_RETRIEVE:
			{
				convey_time1++;
				convey.current= pid_calc(&pid_convey_store_spd,moto_convey.speed_rpm,2000);
				if(convey_time1 >800)
				{ convey.action = CONVEY_STOP; }				
			}break;
			case CONVEY_ROTATE:
			{

				convey.current = pid_calc(&pid_convey_store_spd,moto_convey.speed_rpm,2000);
	
				convey_time1++;
				if (convey_time1 >800)
				{
					convey.current = 0;
					convey_time2++;
					if (convey_time2 >500)
					{
						convey.action = CONVEY_STOP;
					}
				}
			}break;
			case CONVEY_INIT:
			{
				convey_init_step();
			}break;
		}
		convey.last_action = convey.action;
		Can1_Queue.Current_U2.Current[1] = __REV16(convey.current); 
		taskEXIT_CRITICAL();
		osDelayUntil(&mode_wake_time,CONVEY_PERIOD);
	}
}

void convey_init_step(void)
{
	if(convey.last_action != CONVEY_INIT)
	{ convey.state = CONVEY_UNKNOWN; }
	
	switch(convey.state)
	{
		case CONVEY_UNKNOWN:
		{
			moto_convey.round_cnt = 0;
			convey.cnt_ref = 0;
			convey.state = CONVEY_CALIBRA;
		}break;
		
		case CONVEY_CALIBRA:
		{
			convey.offset = moto_convey.total_ecd;
			convey.state = CONVEY_KNOWN;
			convey.action = CONVEY_STOP;
		}break;
		
		case CONVEY_KNOWN :
		{
		}break;	
	}		
}

void Tof_Read_Data(uint8_t *Tof_Data)
{
	uint16_t tof_dist=0;
	for(int i=28;i<40;i++)
	{
		if(Tof_Data[i]>=48&&Tof_Data[i]<=57)
		{
			tof_dist=tof_dist*10+(Tof_Data[i]-48);
		}			
	}
	convey.tof_data=tof_dist;
}


