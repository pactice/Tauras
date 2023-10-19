#include "math_calcu.h"
#include "remote_msg.h"
#define ABS(x)		((x>0)? (x): (-x))
ramp_function_source_t chassis_ws_ramp;
ramp_function_source_t chassis_ad_ramp;
ramp_function_source_t stretch_ramp[2];
ramp_function_source_t addstretch_ramp[2];
ramp_function_source_t rotate_ramp[2];
ramp_function_source_t cross_ramp;

/**
  * @brief          斜波函数计算，根据输入的值进行叠加， 输入单位为 /s 即一秒后增加输入的值
  * @author         RM
  * @param[in]      斜波函数结构体
  * @param[in]      输入值
  * @param[in]      滤波参数
  * @retval         返回空
  */
void ramp_calc(ramp_function_source_t *ramp_source_type, float frame_period, float input, float max, float min)
{
	ramp_source_type->max_value = max;
	ramp_source_type->min_value = min;
	ramp_source_type->frame_period = frame_period;

  ramp_source_type->input = input;

  ramp_source_type->out += ramp_source_type->input * ramp_source_type->frame_period;

  if (ramp_source_type->out > ramp_source_type->max_value)
  {
    ramp_source_type->out = ramp_source_type->max_value;
  }
  else if (ramp_source_type->out < ramp_source_type->min_value)
  {
    ramp_source_type->out = ramp_source_type->min_value;
  }
}

//绝对值
void abs_limit(float *a, float ABS_MAX,float offset)
{
	if(*a > ABS_MAX+offset)
			*a = ABS_MAX+offset;
	if(*a < -ABS_MAX+offset)
			*a = -ABS_MAX+offset;
}

/**
	*@func   float Circle_error(float set ,float get ,float circle_para)
	*@bref		环形数据计算偏差值
	*@param[in] set 设定值 get采样值 circle_para 一圈数值
	*@note	环形数据下，直接计算出PID中的偏差值
*/
float circle_error(float set ,float get ,float circle_para)
{
	float error;
	if(set > get)
	{
		if(set - get> circle_para/2)
			error = set - get - circle_para;
		else
			error = set - get;
	}
	else if(set < get)
	{
		if(set - get<-1*circle_para/2)
			error = set - get +circle_para;
		else
			error = set - get;
	}
	else	error = 0;

	return error;
}

float data_limit(float data,float max,float min)
{
	if (data > max)
	{
		data = max;
	}
	else if (data < min)
	{
		data = min;
	}
	return data;
}
//四元数数据解算
