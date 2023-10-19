/******************************************************************************
/// @brief
/// @copyright Copyright (c) 2017 <dji-innovations, Corp. RM Dept.>
/// @license MIT License
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction,including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense,and/or sell
/// copies of the Software, and to permit persons to whom the Software is furnished
/// to do so,subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
/// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
*******************************************************************************/
#ifndef __pid_H
#define __pid_H

#ifdef  __PID_GLOBALS
#define __PID_EXT
#else
#define __PID_EXT extern
#endif
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

enum{
    LLAST	= 0,
    LAST 	= 1,
    NOW 	= 2,
    
    POSITION_PID,
    DELTA_PID,
};
typedef struct __pid_t
{
    float p;
    float i;
    float d;
    
    float set[3];				//目标值,包含NOW， LAST， LLAST上上次
    float get[3];				//测量值
    float err[3];				//误差
	
    float d_error;
    float pout;							//p输出
    float iout;							//i输出
    float dout;							//d输出
    
    float pos_out;						//本次位置式输出
    float last_pos_out;				//上次输出
    float delta_u;						//本次增量值
    float delta_out;					//本次增量式输出 = last_delta_out + delta_u
    float last_delta_out;
    
	  float max_err;
	  float deadband;				//err < deadband return
    uint32_t pid_mode;
    uint32_t MaxOutput;				//输出限幅
    uint32_t IntegralLimit;		//积分限幅
    
    void (*f_param_init)(struct __pid_t *pid,  //PID参数初始化
                    uint32_t pid_mode,
                    uint32_t maxOutput,
                    uint32_t integralLimit,
                    float p,
                    float i,
                    float d);
    void (*f_pid_reset)(struct __pid_t *pid, float p, float i, float d);		//pid三个参数修改

}pid_t;

//抬升
__PID_EXT pid_t pid_uplift_height[2];
__PID_EXT pid_t pid_uplift_spd[2];
//前yaw轴
__PID_EXT pid_t pid_cross_pos;
__PID_EXT pid_t pid_cross_init_spd;
__PID_EXT pid_t pid_cross_spd;
__PID_EXT pid_t pid_crosspos_block;
__PID_EXT pid_t pid_crossspd_block;
//前移
__PID_EXT pid_t pid_stretch_pos[2];
__PID_EXT pid_t pid_stretch_spd[2];
__PID_EXT pid_t pid_stretch_init_spd[2];
//前伸
__PID_EXT pid_t pid_addstretch_pos[2];
__PID_EXT pid_t pid_addstretch_spd[2];
__PID_EXT pid_t pid_addstretch_init_spd[2];
//滚动
__PID_EXT pid_t pid_rotate_init_spd[2];
__PID_EXT pid_t pid_rotate_pos[2];
__PID_EXT pid_t pid_rotate_spd[2];
__PID_EXT pid_t pid_rotate_block_pos[2];
__PID_EXT pid_t pid_rotate_block_spd[2];
__PID_EXT pid_t pid_rotate_stay_pos[2];
__PID_EXT pid_t pid_rotate_stay_spd[2];
//传送带
__PID_EXT pid_t pid_convey_stay_pos;
__PID_EXT pid_t pid_convey_stay_spd;
__PID_EXT pid_t pid_convey_store_spd;
//吸盘
__PID_EXT pid_t pid_spin_pos;
__PID_EXT pid_t pid_spin_spd;
//云台
__PID_EXT pid_t pid_yaw_init_spd;
__PID_EXT pid_t pid_yaw_spd;
__PID_EXT pid_t pid_yaw_pos;

void PID_struct_init(
    pid_t* pid,
    uint32_t mode,
    uint32_t maxout,
    uint32_t intergral_limit,
    
    float 	kp, 
    float 	ki, 
    float 	kd);
    
float pid_calc(pid_t* pid, float fdb, float ref);

#endif

