/*
 * ICM20602.h
 *
 *  Created on: 2022��5��19��
 *      Author: yue
 */

#ifndef CODE_ICM20602_H_
#define CODE_ICM20602_H_

#include "common.h"
#include "Attitude.h"

extern float icm_target_angle_z;    //ICM����Z��Ŀ��Ƕ�
extern uint8 icm_angle_z_flag;      //ICM����Z��ﵽĿ��Ƕ�flag

int16 GetICM20602Gyro_Z(void);
float GetICM20602Angle_Z(uint8 flag);
void StartIntegralAngle_Z(float target_angle);
void GetICM20602Eulerian(void);

#endif /* CODE_ICM20602_H_ */
