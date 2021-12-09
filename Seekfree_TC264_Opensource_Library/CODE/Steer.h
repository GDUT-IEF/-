/*
 * Steer.h
 *
 *  Created on: 2021��12��9��
 *      Author: yue
 */

#include "headfile.h"       //��ɵķ�װ��

#ifndef CODE_STEER_H_
#define CODE_STEER_H_

#define STEER_PIN   ATOM1_CH1_P33_9     //������PWM����
#define STEER_MID   750                 //�������ֵ
#define STEER_LEFT  840                 //����������ֵ
#define STEER_RIGHT 680                 //������Ҵ���ֵ

void SteerCtrl(uint16 pwm);     //���ƶ��

#endif /* CODE_STEER_H_ */
