/*
 * Motor.h
 *
 *  Created on: 2021��12��19��
 *      Author: yue
 */

#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_

#include "zf_gtm_pwm.h"

//������Ƶ��������
#define LEFT_MOTOR_PIN1     ATOM0_CH4_P02_4
#define LEFT_MOTOR_PIN2     ATOM0_CH5_P02_5
#define RIGHT_MOTOR_PIN1    ATOM0_CH6_P02_6
#define RIGHT_MOTOR_PIN2    ATOM0_CH7_P02_7

void MotorCtrl(int16 left_speed,int16 right_speed);     //�������ҵ����ת��������ת

#endif /* CODE_MOTOR_H_ */
