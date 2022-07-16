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
#include "SEEKFREE_18TFT.h"

extern uint8 stop_flag;
int16 speed_l,speed_r;      //��������ٶ�Ŀ��ֵ��ȫ�ֱ���
uint8 encoder_dis_flag=2;   //���������flag������ֵ=2�������Լ�������⵼��int�����

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
    static uint8 flag;               //�������flag
    int16 encoder_l=0,encoder_r=0;   //���ҵ��������ֵ
    int pwm_l=0,pwm_r=0;             //���ҵ��PWM

    MotorEncoder(&encoder_l,&encoder_r);              //��ȡ���ҵ��������
    encoder_l=SecondOrderLagFilter_L(encoder_l);      //��ͨ�˲�
    encoder_r=SecondOrderLagFilter_R(encoder_r);
    pwm_l=Speed_PI_Left(encoder_l,speed_l,MotorK_L);    //���ҵ��PID
    pwm_r=Speed_PI_Right(encoder_r,speed_r,MotorK_R);

    //ͣ������
    if((encoder_l<10||encoder_r<10)&&stop_flag==1)
    {
        MotorSetPWM(0, 0);
        while(1);
    }

    MotorSetPWM(pwm_l,pwm_r);                   //���PWM��ֵ
    EncoderDistance(0,0,encoder_l,encoder_r);   //�ɼ�������ֵ�����ڱ��������

    //Ұ����λ������
//    int data_l=encoder_l,data_r=encoder_r;     //Ұ����λ��ֻ֧��int�����ݣ����������ǿ��ת��
//    set_computer_value(SEND_FACT_CMD, CURVES_CH1, &data_l, 1);      //�����������
//    set_computer_value(SEND_FACT_CMD, CURVES_CH2, &data_r, 1);      //�����ұ�����
    //VOFA��λ������
//    printf("%d,%d\n",encoder_l,encoder_r);  //�������ұ�����
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
    //�ٶ��޷�
    if(target_l>base_speed+SPEED_LIMIT)
    {
        target_l=base_speed+SPEED_LIMIT;
    }
    else if(target_l<base_speed-SPEED_LIMIT)
    {
        target_l=base_speed-SPEED_LIMIT;
    }
    if(target_r>base_speed+SPEED_LIMIT)
    {
        target_r=base_speed+SPEED_LIMIT;
    }
    else if(target_r<base_speed-SPEED_LIMIT)
    {
        target_r=base_speed-SPEED_LIMIT;
    }
    //�ٶȸ�ֵ
    speed_l=target_l;
    speed_r=target_r;
}

/*
*********************************************************************************************************
** �� �� ��: EncoderDistance
** ����˵��: ��������࣬�ú������ڱ������ж��б������ڲɼ���������ֵ��flag=0����
**           ���������б�����������Ŀ�����Ϳ�����⣬��������Ҫ�û��ֶ���ѯencoder_dis_flag==1?
**           �Ӷ�����Ƿ�ﵽĿ��ֵ
** ��    ��: flag��ѡ�������ܣ�0���ɼ����������ݣ�1������Ŀ��ֵ���Ӵ˴���ʼ��ࣩ
**           dis��Ŀ����룬��λm��ֻ��flag=1ʱ������������ã�
**           encoder_l���������ֵ��ֻ��flag=0ʱ������������ã�
**           encoder_r���ұ�����ֵ��ֻ��flag=0ʱ������������ã�
** �� �� ֵ: ��
*********************************************************************************************************
*/
void EncoderDistance(uint8 flag,float target_dis,int16 encoder_l,int16 encoder_r)
{
    static int encoder_nowsum,encoder_target;   //��ǰ������ֵ��Ŀ�������ֵ
    //�ɼ�����������
    if(flag==0&&encoder_dis_flag==0)    //��ȷ�����ҵ�ǰ���ڲ��״̬
    {
        int16 encoder=(encoder_l+encoder_r)/2;  //���ұ�������ֵ
        encoder_nowsum+=encoder;                //�������ۼ�
        if(encoder_nowsum>encoder_target)       //��ǰ����������Ŀ��ֵ
        {
            encoder_dis_flag=1; //���������flag��1
        }
    }
    //����Ŀ��ֵ
    else if(flag==1)
    {
        encoder_target=11731*target_dis;    //����Ŀ��������Ŀ�������ֵ
        encoder_dis_flag=0;                 //���������flag��0�������������ɼ�
        encoder_nowsum=0;                   //��ձ������ۻ�ֵ
    }
}
