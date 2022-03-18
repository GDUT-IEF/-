/*
 * Motor.c
 *
 *  Created on: 2021��12��19��
 *      Author: yue
 */

#include "Motor.h"
#include "Filter.h" //�˲�

/*
*********************************************************************************************************
** �� �� ��: MotorSetPWM
** ����˵��: �������ҵ����ת��������ת
** ��    ��: left_speed����ֵ��������ת��
**           right_speed����ֵ�������ת��
** �� �� ֵ: ��
*********************************************************************************************************
*/
void MotorSetPWM(int pwm_l,int pwm_r)
{
    //�޷�����
    if(pwm_l>GTM_ATOM0_PWM_DUTY_MAX)
        pwm_l=GTM_ATOM0_PWM_DUTY_MAX;
    else if(pwm_l<-GTM_ATOM0_PWM_DUTY_MAX)
        pwm_l=-GTM_ATOM0_PWM_DUTY_MAX;
    if(pwm_r>GTM_ATOM0_PWM_DUTY_MAX)
        pwm_r=GTM_ATOM0_PWM_DUTY_MAX;
    else if(pwm_r<-GTM_ATOM0_PWM_DUTY_MAX)
        pwm_r=-GTM_ATOM0_PWM_DUTY_MAX;
    //�жϵ��������ת�������ٶȸ�ֵ
    if(pwm_l>=0)   //������ת
    {
        gpio_set(P02_6,0);
        pwm_duty(LEFT_MOTOR_PIN1,pwm_l);
    }
    else                //������ת
    {
        gpio_set(P02_6,1);
        pwm_duty(LEFT_MOTOR_PIN1,-pwm_l);
    }
    if(pwm_r>=0)  //�ҵ����ת
    {
        gpio_set(P02_7,0);
        pwm_duty(RIGHT_MOTOR_PIN1,pwm_r);
    }
    else                //�ҵ����ת
    {
        gpio_set(P02_7,1);
        pwm_duty(RIGHT_MOTOR_PIN1,-pwm_r);
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
//    *left_encoder=FirstOrderLagFilter(*left_encoder);   //�˲�
    gpt12_clear(LEFT_ENCODER);

    //�ұ�����
    *right_encoder = -gpt12_get(RIGHT_ENCODER);
//    *right_encoder=FirstOrderLagFilter(*right_encoder); //�˲�
    gpt12_clear(RIGHT_ENCODER);
}

