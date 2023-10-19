#include "diy_remote.h"
#include "remote_msg.h"
#include "rotate_task.h"
#include "cross_task.h"
#include "bsp_judge.h"
#include "uplift_task.h"
#include "keyboard_handle.h"
#include "stretch_task.h"
#include "modeswitch_task.h"
#include "math_calcu.h"
#include "bsp_usart.h"
#include "string.h"
#include "T_imu.h"
#include "spin_task.h"
//²âÊÔ×Ô¶¨ÒåÒ£¿ØÆ÷
int8_t diy_remote_init_status=0;
diy_t265_t diy_t265;
diy265_t diy_remote;

void diy_remote265_init()
{
	diy_remote.ang_roll_offset = diy_t265.ang_roll;
	diy_remote.ang_yaw_offset = diy_t265.ang_yaw;
	diy_remote.ang_pitch_offset = diy_t265.ang_pitch;
	diy_remote.pos_x_offset = diy_t265.pos_x;
	diy_remote.pos_y_offset = diy_t265.pos_y; 
	diy_remote.pos_z_offset = diy_t265.pos_z;
}

void t265_data_refresh()
{
	rotate.angle_err = -circle_error(diy_remote.ang_pitch_offset,diy_t265.ang_pitch,360);	
	cross.angle_err = circle_error(diy_remote.ang_yaw_offset,diy_t265.ang_yaw,360);	
	spin.angle_err = circle_error(diy_remote.ang_roll_offset,diy_t265.ang_roll,360);
	uplift.z_err = diy_t265.pos_z - diy_remote.pos_z_offset ;
	stretch.x_err = diy_remote.pos_x_offset - diy_t265.pos_x ;
	
} 

void t265_controller()
{

	rotate.angle_ref =  data_limit((rotate.angle_err*0.1 + rotate.cnt_fdb[0]),ROTATE_VERTICAL,ROTATE_EXCHANGE_UP);
	if(fabs(rotate.angle_ref - rotate.last_cnt_ref)>1)
	{
		rotate.cnt_ref = rotate.angle_ref ;
	}
	else {
		rotate.cnt_ref = rotate.last_cnt_ref;
	}
	
	cross.angle_ref = data_limit((cross.angle_err*0.1 + cross.cnt_fdb),CROSS_EXCHAGERIGHT_LIM,CROSS_EXCHAGELEFT_LIM);
	if(fabs(cross.angle_ref - cross.last_cnt_ref)>1)
	{
		cross.cnt_ref = cross.angle_ref ;
	}
	else {
		cross.cnt_ref = cross.last_cnt_ref;
	}
	
	spin.angle_ref = data_limit((spin.angle_err*0.1 + spin.cnt_fdb ),360,0);
	if(fabs(spin.angle_ref - spin.last_cnt_ref)>5)
	{
		spin.cnt_ref = spin.angle_ref ;
	}
	else {
		spin.cnt_ref = spin.last_cnt_ref;
	}
	

	uplift.z_ref = data_limit(uplift.z_err*0.2	+ uplift.cnt_fdb[0],UPLIFT_MAX,900);
	if(fabs(uplift.z_ref - uplift.last_cnt_ref)>10)
	{
		uplift.cnt_ref = uplift.z_ref ;
	}
	else {
		uplift.cnt_ref = uplift.last_cnt_ref;
	}
	
	stretch.x_ref = data_limit(stretch.x_err*0.3 + stretch.cnt_fdb[0],STRETCH_MAX,INNER_STRETCH);
	if(fabs(stretch.x_ref - stretch.last_cnt_ref)>10)
	{
		stretch.cnt_ref = stretch.x_ref ;
	}
	else {
		stretch.cnt_ref = stretch.last_cnt_ref;
	}
	
	rotate.last_cnt_ref = rotate.cnt_ref;
	cross.last_cnt_ref = cross.cnt_ref;
	spin.last_cnt_ref = spin.cnt_ref;
	uplift.last_cnt_ref = uplift.cnt_ref;
	stretch.last_cnt_ref = stretch.cnt_ref;
}

int SET_265=0;
void diy_refresh_task(void const *argu)
{
	uint32_t mode_wake_time = osKernelSysTick();
	for(;;)
	{
		taskENTER_CRITICAL();
		if(diy_t265.null==0)
		{
			diy_remote265_init();
		}
		if(diy_t265.null==1)
		{
			t265_data_refresh();
			t265_controller();
		}
		
		taskEXIT_CRITICAL();
    osDelayUntil(&mode_wake_time,DIYREMOTE_PERIOD);
	}
	
}
