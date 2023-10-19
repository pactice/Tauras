#ifndef __DIY_REMOTE_H
#define __DIY_REMOTE_H
#include "main.h"
#define DIYREMOTE_PERIOD	1
//״̬
typedef enum
{
	READY,
	STOP,
	START,
}t265_state_e;

typedef struct{
	uint8_t	refresh_count;  
	t265_state_e t265_state;	 
	float 	ang_roll_offset;
	float 	ang_pitch_offset;
	float 	ang_yaw_offset;
	float 	pos_x_offset;
  float 	pos_y_offset;
	float 	pos_z_offset;
	
}diy265_t;
typedef struct{	
	float 	ang_roll;
	float 	ang_pitch;
	float 	ang_yaw;
	float 	pos_x;
  float 	pos_y;
	float 	pos_z;
	float   null;
	int16_t  end;
}diy_t265_t;



extern diy265_t diy_remote;
extern diy_t265_t diy_t265;

void diy_remote_init();
void t265_controller();
void t265_data_refresh();
void diy_receive_data(uint8_t *receive);
void diy_refresh_task(void const *argu);

#endif