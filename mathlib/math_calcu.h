/** 
  * @file     math_calcu.h
  * @version  v2.0
  * @date     July,6th 2019
	*
  * @brief    数学计算函数,斜波函数，
	*
  *	@author   Fatmouse
  *
  */
#ifndef _MATH_CALCU_H_
#define _MATH_CALCU_H_

#include "math.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

#define ABS(x)		((x>0)? (x): (-x))

typedef __packed struct
{
    float input;        //输入数据
    float out;          //滤波输出的数据
    float num[1];       //滤波参数
    float frame_period; //滤波的时间间隔 单位 s
} first_order_filter_type_t;

typedef struct
{
    float input;        //输入数据
    float out;          //输出数据
    float min_value;    //限幅最小值
    float max_value;    //限幅最大值
    float frame_period; //时间间隔
} ramp_function_source_t;

extern ramp_function_source_t chassis_ws_ramp;
extern ramp_function_source_t chassis_ad_ramp;
extern ramp_function_source_t stretch_ramp[2];
extern ramp_function_source_t rotate_ramp[2];
extern ramp_function_source_t cross_ramp;
extern ramp_function_source_t addstretch_ramp[2];

void chassis_ramp(void);
void ramp_calc(ramp_function_source_t *ramp_source_type, float frame_period, float input, float max, float min);

float circle_error(float set ,float get ,float circle_para);
void abs_limit(float *a, float ABS_MAX,float offset);
float data_limit(float data,float max,float min);

#endif
