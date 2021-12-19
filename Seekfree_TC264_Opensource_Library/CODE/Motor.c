/*
 * Motor.c
 *
 *  Created on: 2021��12��19��
 *      Author: yue
 */

#include "Motor.h"

/*
*********************************************************************************************************
** �� �� ��: LeftMotorCtrl
** ����˵��: �������ҵ����ת��������ת
** ��    ��: left_speed����ֵ��������ת��
**           right_speed����ֵ�������ת��
** �� �� ֵ: ��
*********************************************************************************************************
*/
void MotorCtrl(int16 left_speed,int16 right_speed)
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
    //����
    if(left_speed>=0)   //��ת
    {
        pwm_duty(LEFT_MOTOR_PIN1,left_speed);
        pwm_duty(LEFT_MOTOR_PIN2,0);
    }
    else                //��ת
    {
        pwm_duty(LEFT_MOTOR_PIN1,0);
        pwm_duty(LEFT_MOTOR_PIN2,left_speed);
    }
    //�ҵ��
    if(right_speed>=0)  //��ת
    {
        pwm_duty(RIGHT_MOTOR_PIN1,right_speed);
        pwm_duty(RIGHT_MOTOR_PIN2,0);
    }
    else                //��ת
    {
        pwm_duty(RIGHT_MOTOR_PIN1,0);
        pwm_duty(RIGHT_MOTOR_PIN2,right_speed);
    }
}

