/*
 * Motor.c
 *
 *  Created on: 2021��12��19��
 *      Author: yue
 */

#include "Motor.h"

/*
*********************************************************************************************************
** �� �� ��: MotorCtrl
** ����˵��: �������ҵ����ת��������ת
** ��    ��: left_speed����ֵ��������ת��
**           right_speed����ֵ�������ת��
** �� �� ֵ: ��
*********************************************************************************************************
*/
void MotorCtrl(int left_speed,int right_speed)
{
    //�޷�����
    if(left_speed>GTM_ATOM0_PWM_DUTY_MAX)
        left_speed=GTM_ATOM0_PWM_DUTY_MAX;
    else if(left_speed<-GTM_ATOM0_PWM_DUTY_MAX)
        left_speed=-GTM_ATOM0_PWM_DUTY_MAX;
    if(right_speed>GTM_ATOM0_PWM_DUTY_MAX)
        right_speed=GTM_ATOM0_PWM_DUTY_MAX;
    else if(right_speed<-GTM_ATOM0_PWM_DUTY_MAX)
        right_speed=-GTM_ATOM0_PWM_DUTY_MAX;
    //�жϵ��������ת�������ٶȸ�ֵ
    if(left_speed>=0)   //������ת
    {
        gpio_set(P02_6,1);
        pwm_duty(LEFT_MOTOR_PIN1,left_speed);
    }
    else                //������ת
    {
        gpio_set(P02_6,0);
        pwm_duty(LEFT_MOTOR_PIN1,-left_speed);
    }
    if(right_speed>=0)  //�ҵ����ת
    {
        gpio_set(P02_7,0);
        pwm_duty(RIGHT_MOTOR_PIN1,right_speed);
    }
    else                //�ҵ����ת
    {
        gpio_set(P02_7,1);
        pwm_duty(RIGHT_MOTOR_PIN1,-right_speed);
    }

    //����ע�͵���ʹ������PWM�ź�����һ���������������
//    //����
//    if(left_speed>=0)   //��ת
//    {
//        pwm_duty(LEFT_MOTOR_PIN1,0);
//        pwm_duty(LEFT_MOTOR_PIN2,left_speed);
//    }
//    else                //��ת
//    {
//        pwm_duty(LEFT_MOTOR_PIN1,-left_speed);
//        pwm_duty(LEFT_MOTOR_PIN2,0);
//    }
//    //�ҵ��
//    if(right_speed>=0)  //��ת
//    {
//        pwm_duty(RIGHT_MOTOR_PIN1,right_speed);
//        pwm_duty(RIGHT_MOTOR_PIN2,0);
//    }
//    else                //��ת
//    {
//        pwm_duty(RIGHT_MOTOR_PIN1,0);
//        pwm_duty(RIGHT_MOTOR_PIN2,-right_speed);
//    }
}

/*
*********************************************************************************************************
** �� �� ��: MotorEncoder
** ����˵��: ��ȡ���ҵ���ı�������ֵ
** ��    ��: left_encoder��������������ֵ
**           right_encoder���ҵ����������ֵ
** �� �� ֵ: ��
** ע    �⣺���ڱ�������Է��ã���ǰ�˶�ʱ���ұ�����ȡ������
*********************************************************************************************************
*/
void MotorEncoder(int16* left_encoder,int16* right_encoder)
{
    //�������
    *left_encoder = gpt12_get(LEFT_ENCODER);
    gpt12_clear(LEFT_ENCODER);
    //�ұ�����
    *right_encoder = -gpt12_get(RIGHT_ENCODER);
    gpt12_clear(RIGHT_ENCODER);
}

