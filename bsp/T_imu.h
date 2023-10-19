#ifndef __T_IMU_H
#define __T_IMU_H

#include "stdint.h"

#define TIMU_PALSTANCE_ID   0x001
#define TIMU_ANGLE_ID   	0x002

typedef struct
{
    float pitch;
    float yaw;
    float wy;
    float wz;
    float az;
    float kal_az;
} Taurus_imu_data_t;
typedef struct
{
	union{
		float pitch;
		uint8_t a[4];
	}recepitch_t;
	union{
		float yaw;
		uint8_t b[4];
	}receyaw_t;

}imu_rece_t;

extern imu_rece_t imu_rece;
void T_imu_calcu(uint32_t can_id, uint8_t* CAN_Rx_data);

#endif
