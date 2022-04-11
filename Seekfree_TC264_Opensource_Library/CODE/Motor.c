/*
 * Motor.c
 *
 *  Created on: 2021��12��19��
 *      Author: yue
 */

#include "Motor.h"
#include "Filter.h"         //�˲�
#include <stdio.h>
#include "PID.h"            //PID
#include "protocol.h"

int16 speed_l,speed_r;  //��������ٶ�Ŀ��ֵ��ȫ�ֱ���

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
    if(pwm_l>MOTOR_PWM_MAX)
        pwm_l=MOTOR_PWM_MAX;
    else if(pwm_l<-MOTOR_PWM_MAX)
        pwm_l=-MOTOR_PWM_MAX;
    if(pwm_r>MOTOR_PWM_MAX)
        pwm_r=MOTOR_PWM_MAX;
    else if(pwm_r<-MOTOR_PWM_MAX)
        pwm_r=-MOTOR_PWM_MAX;
    //�жϵ��������ת�������ٶȸ�ֵ
    //��������������壬ʹ��һ��IO�ڿ�������ת��һ��IO�����PWM�Ŀ��Ʒ�ʽ
//    if(pwm_l>=0)   //������ת
//    {
//        gpio_set(P02_6,0);
//        pwm_duty(LEFT_MOTOR_PIN1,pwm_l);
//    }
//    else                //������ת
//    {
//        gpio_set(P02_6,1);
//        pwm_duty(LEFT_MOTOR_PIN1,-pwm_l);
//    }
//    if(pwm_r>=0)  //�ҵ����ת
//    {
//        gpio_set(P02_7,0);
//        pwm_duty(RIGHT_MOTOR_PIN1,pwm_r);
//    }
//    else                //�ҵ����ת
//    {
//        gpio_set(P02_7,1);
//        pwm_duty(RIGHT_MOTOR_PIN1,-pwm_r);
//    }

    //���������������壬ʹ������PWM�ź�����һ���������������
    //����
    if(pwm_l>=0)   //��ת
    {
        pwm_duty(LEFT_MOTOR_PIN1,pwm_l);
        pwm_duty(LEFT_MOTOR_PIN2,0);
    }
    else                //��ת
    {
        pwm_duty(LEFT_MOTOR_PIN1,0);
        pwm_duty(LEFT_MOTOR_PIN2,-pwm_l);
    }
    //�ҵ��
    if(pwm_r>=0)  //��ת
    {
        pwm_duty(RIGHT_MOTOR_PIN1,0);
        pwm_duty(RIGHT_MOTOR_PIN2,pwm_r);
    }
    else                //��ת
    {
        pwm_duty(RIGHT_MOTOR_PIN1,-pwm_r);
        pwm_duty(RIGHT_MOTOR_PIN2,0);
    }
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

/*
*********************************************************************************************************
** �� �� ��: MotorCtrl
** ����˵��: ʹ������ʽPI�������������ҵ��ת��
** ��    ��: speed_l�������ٶ�
**           speed_r���ҵ���ٶ�
** �� �� ֵ: ��
*********************************************************************************************************
*/
void MotorCtrl(int16 speed_l,int16 speed_r)
{
    int16 encoder_l=0,encoder_r=0;   //���ҵ��������ֵ
    int pwm_l=0,pwm_r=0;             //���ҵ��PWM

    MotorEncoder(&encoder_l,&encoder_r);              //��ȡ���ҵ��������
    encoder_l=SecondOrderLagFilter_L(encoder_l);      //���׵�ͨ�˲�
    encoder_r=SecondOrderLagFilter_R(encoder_r);
    pwm_l=Speed_PI_Left(encoder_l,speed_l,MotorK);    //���ҵ��PID
    pwm_r=Speed_PI_Right(encoder_r,speed_r,MotorK);
    MotorSetPWM(pwm_l,pwm_r);                         //���PWM��ֵ

    //Ұ����λ������
//    int data_l=encoder_l,data_r=encoder_r;     //Ұ����λ��ֻ֧��int�����ݣ����������ǿ��ת��
//    set_computer_value(SEND_FACT_CMD, CURVES_CH1, &data_l, 1);      //�����������
//    set_computer_value(SEND_FACT_CMD, CURVES_CH2, &data_r, 1);      //�����ұ�����
}

/*
*********************************************************************************************************
** �� �� ��: MotorSetTarget
** ����˵��: �������ҵ���ٶȵ�Ŀ��ֵ
** ��    ��: target_l�������ٶ�Ŀ��ֵ
**           target_r���ҵ���ٶ�Ŀ��ֵ
** �� �� ֵ: ��
*********************************************************************************************************
*/
void MotorSetTarget(int16 target_l,int16 target_r)
{
    speed_l=target_l;
    speed_r=target_r;
}
