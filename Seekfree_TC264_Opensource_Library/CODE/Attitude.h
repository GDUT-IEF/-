//
// Created by yue on 2022/5/11.
//

#ifndef ICM20602_ATTITUDE_H
#define ICM20602_ATTITUDE_H

#include "common.h"

typedef struct{ //��Ԫ��
    float q0;
    float q1;
    float q2;
    float q3;
}quaterInfo_t;

typedef struct{ //ŷ����
    float pitch;
    float roll;
    float yaw;
}eulerianAngles_t;

typedef struct  //������У׼ֵ
{
    int16 x;
    int16 y;
    int16 z;
}GyroOffset;

extern float values[6];
extern eulerianAngles_t eulerAngle;

void GyroOffsetInit(void);
void IMUGetValues(float * values);
void IMU_quaterToEulerianAngles(void);

#endif //ICM20602_ATTITUDE_H
