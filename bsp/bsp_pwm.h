#ifndef _BSP_PWM_
#define _BSP_PWM_

#include "stm32f4xx.h"
#include "tim.h"
#include "remote_msg.h"
#include "modeswitch_task.h"

#define PIT_ANGLE (rc.mouse.y)

#define ANGLE_1_SET(angle) __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,angle)
#define ANGLE_2_SET(angle) __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,angle)
#define ANGLE_3_SET(angle) __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,angle)
#define ANGLE_4_SET(angle) __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,angle)
#define ANGLE_5_SET(angle) __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,angle)
//改变一度的占空比 temp = 
//范围设置 300 —— 2600
#define MAX_ANGLE 1600
#define MID_ANGLE 1450 
#define MIN_ANGLE 1400
extern float angle;
void camera_init (void);
void camera_pit_set(void);
void camera_pit_reset(void);
#endif
