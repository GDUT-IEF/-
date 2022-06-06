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
#define STEER_MID   738*2                 //�������ֵ
#define STEER_LEFT  802*2                 //����������ֵ   //���ݴ�һ��ͷ��˿������
#define STEER_RIGHT 675*2                 //������Ҵ���ֵ   //���ݴ�һ��ͷ��˿������

void SteerCtrl(uint32 pwm);     //���ƶ��

#endif /* CODE_STEER_H_ */
