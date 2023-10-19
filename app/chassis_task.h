#ifndef __CLASSIS_TASK_H__
#define __CLASSIS_TASK_H__
#ifdef  __CHASSIS_TASK_GLOBALS
#define __CHASSIS_TASK_EXT
#else
#define __CHASSIS_TASK_EXT extern
#endif

#include "stm32f4xx_hal.h"

#define CHASSIS_PERIOD 	10
#define KEYBOARD_SCALE_YAW	        ( -0.05f )
#define REMOTE_SCALE_YAW            ( -0.0005f )
typedef enum
{
    CHASSIS_PROTECT,
	CHASSIS_REMOTER,
	CHASSIS_KEYBOARD,
	CHASSIS_STAY,
    CHASSIS_TURN,
} chassis_mode_e;

typedef struct
{
  int16_t          vx; // forward/back
  int16_t          vy; // left/right
  int16_t          vw; // rotate
	
  float			   angle_del;

  chassis_mode_e  ctrl_mode;
	chassis_mode_e  last_ctrl_mode;
} chassis_t;

__CHASSIS_TASK_EXT chassis_t chassis;
__CHASSIS_TASK_EXT int16_t chassis_speed;
__CHASSIS_TASK_EXT int16_t mouse_speed;
__CHASSIS_TASK_EXT int8_t dir;

void chassis_task(void const *argu);

#endif
