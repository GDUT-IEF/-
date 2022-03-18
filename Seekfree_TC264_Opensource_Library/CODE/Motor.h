/*
 * Motor.h
 *
 *  Created on: 2021��12��19��
 *      Author: yue
 */

#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_

#include "zf_gtm_pwm.h"
#include "zf_gpt12.h"
#include "zf_gpio.h"

//������Ƶ����PWM����
#define LEFT_MOTOR_PIN1     ATOM0_CH4_P02_4
#define LEFT_MOTOR_PIN2     ATOM0_CH6_P02_6
#define RIGHT_MOTOR_PIN1    ATOM0_CH5_P02_5
#define RIGHT_MOTOR_PIN2    ATOM0_CH7_P02_7
//������Ʊ������Ķ�ʱ��
#define LEFT_ENCODER    GPT12_T2
#define RIGHT_ENCODER   GPT12_T6

void MotorSetPWM(int pwm_l,int pwm_r);             //�������ҵ����ת��������ת
void MotorEncoder(int16* left_encoder,int16* right_encoder);    //��ȡ���ҵ����������ֵ

#endif /* CODE_MOTOR_H_ */
