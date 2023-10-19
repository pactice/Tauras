#include "bsp_pwm.h"
#include "math_calcu.h"
#include "stretch_task.h"

float angle_set = 0.0f;

uint16_t pwm_set = 1500;
float angle;
/**
 * 描述 PWM初始化
 * @date 2022-03-29
 * @param {void}
 * @returns {void}
 */
void camera_init (void)
{
//    ANGLE_1_SET(1550);
//    ANGLE_2_SET(1550);
		angle = MID_ANGLE;
    ANGLE_3_SET(MID_ANGLE);
    ANGLE_4_SET(MIN_ANGLE);
		ANGLE_5_SET(MID_ANGLE);
	
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);			//图传
		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);

}

/**
 * 描述 图传位置置中
 * @date 2022-03-29
 * @param {void}
 * @returns {void}
 */

void camera_pit_reset(void)
{
    ANGLE_3_SET(MID_ANGLE);
    ANGLE_4_SET(MID_ANGLE);
}

/**
 * 描述 控制舵机位置
 * @date 2022-03-29
 * @param {void}
 * @returns {void}
 */
void camera_pit_set(void)
{
    
		ANGLE_4_SET(angle); 
}
