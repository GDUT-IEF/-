/*
 * PID.h
 *
 *  Created on: 2021��12��10��
 *      Author: yue
 */

#ifndef CODE_PID_H_
#define CODE_PID_H_

#include "common.h" //������������

//���PID���ƵĽṹ�壬���ڷֶο���PID��ͼ��ʶ������·��֮��PID�Ĳ����ͷ��ͱ仯
typedef struct SteerPID
{
    float P;
    float I;
    float D;
}SteerPID;
//���PID���ƵĽṹ��
typedef struct MotorPID
{
    float P;
    float I;
    float D;
}MotorPID;

extern SteerPID SteerK;     //�����ȫ�ֱ���
extern MotorPID MotorK_L,MotorK_R;     //�����ȫ�ֱ���
extern int16 base_speed;    //�����ٶȵ�ȫ�ֱ���
extern uint32 SteerPWM;     //���PWM

//���ﻹҪд��PID��ʼ���ĺ����������������
void PID_init(SteerPID *SteerK,MotorPID *MotorK_L,MotorPID *MotorK_R);    //����PID�����ĸ�ֵ��ʼ��
uint32 Steer_Position_PID(float SlopeBias,SteerPID K);                    //���λ��ʽPID���ƣ����÷ֶ�ʽPID����
int Speed_PI_Left(int16 left_encoder,int16 left_target,MotorPID K);       //�������ʽPI���� //����
int Speed_PI_Right(int16 right_encoder,int16 right_target,MotorPID K);    //�������ʽPI����  //�ҵ��

#endif /* CODE_PID_H_ */
