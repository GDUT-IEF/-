/*
 * PID.c
 *
 *  Created on: 2021��12��10��
 *      Author: yue
 */

#include "PID.h"
#include "Filter.h" //�˲��㷨
#include "Steer.h"  //���
#include "Motor.h"  //���
#include "SEEKFREE_18TFT.h"

SteerPID SteerK;            //���PID����
MotorPID MotorK_L,MotorK_R; //���PID����

/********************************************************************************************
 ** ��������: ����PID�����ĸ�ֵ��ʼ��
 ** ��    ��: SteerPID *SteerK
 **           MotorPID *MotorK
 ** �� �� ֵ: ��
 ** ��    ��: LJF
 *********************************************************************************************/
void PID_init(SteerPID *SteerK,MotorPID *MotorK_L,MotorPID *MotorK_R)
{
    SteerK->P=16.25;SteerK->I=0;SteerK->D=37.5;         //��ʼ�������PID����   //У������19.25 5   //����������14.25 30
    MotorK_L->P=180;MotorK_L->I=0.45;MotorK_L->D=0;     //��ʼ�������PID����   //У������80 0.5    //����������180 0.45
    MotorK_R->P=180;MotorK_R->I=0.45;MotorK_R->D=0;     //��ʼ�������PID����   //У������80 0.5    //����������180 0.45
}

/*
 *******************************************************************************************
 ** ��������: ����ƫ��������PWM
 ** ��    ��: float SlopeBias:    ��С�ع鷽�̵ó�����ƫ��
 **           SteerPID K:         ����PID�Ķ��PID����
 ** �� �� ֵ: �������PWM
 ** ��    ��: LJF
 ** ע    �⣺ƫ���Ǵ��������Ǵ��������߽����ٸ����Ǹ�������ƫ�
 **           ���س�ȥ��PWM���ܺ�С�������������PWM�������Ż��ǰ������ȥ����?
 **           Bias�����������ﴦ���Ǵ�����֮ǰ������������һ�������й�����
 ********************************************************************************************
 */
uint32 Steer_Position_PID(float bias,float slope,SteerPID K)
{
    static float last_bias_slope;
    float bias_slope=0;
    int PWM;
    //�����ں�
    slope=FirstOrderLagFilter_Slope(slope); //��б�ʵ�ͨ�˲�
//    bias_slope=BIAS_SLOPE_P*bias+(1-BIAS_SLOPE_P)*(3*slope);
    bias_slope=0.9*bias+slope;
//    printf("%f,%f\n",bias,slope);
    //PID����
    PWM=(int)(K.P*bias_slope+K.D*(bias_slope-last_bias_slope));
    //������һ��ƫ��
    last_bias_slope=FirstOrderLagFilter(bias_slope);   //����һ�ε�ƫ���ͨ�˲�

    return STEER_MID+PWM;
}

/*
 *******************************************************************************************
 ** ��������: �������ʽ�ٶ�PI�����������ݱ�������ֵ�����Ͻ���������ٶ�   //����
 ** ��    ��: left_encoder��������������ֵ
 **           left_target ������Ŀ���ٶ�
 **           K           : ���PID����
 ** �� �� ֵ: �������PWM
 ** ��    ��: WBN
 ** ע    �⣺ʹ������ʽPI���Ƶ��ŵ㣺
 **           1.��ʽ��û���ۼӵĹ��̣�������ֻ������β���ֵ�йأ������ײ���������
 **           2.����������������仯���������и��õ��ݴ�
 ********************************************************************************************
 */
int Speed_PI_Left(int16 left_encoder,int16 left_target,MotorPID K)
{
    static int bias,last_bias,PWM;

    bias=left_target-left_encoder;              //�����ǰƫ��
    PWM+=(int)(K.P*(bias-last_bias)+K.I*bias);  //����ʽPID�����ѽ��ֱ�ӵ�������һ�ε�PWM��
    last_bias=bias;           //������һ��ƫ��

    //PID����޷�����ֹ���ڵ����PID������ͬ�����µ�������ջٵ�����
    //ע�⣺��PID����޷���ᵼ����Ӧ����
//    if(PWM>MOTOR_PWM_MAX)
//    {
//        PWM=MOTOR_PWM_MAX;
//    }
//    else if(PWM<-MOTOR_PWM_MAX)
//    {
//        PWM=-MOTOR_PWM_MAX;
//    }

    return PWM;         //���ؿ���ֱ�Ӹ�ֵ�������PWM
}

/*
 *******************************************************************************************
 ** ��������: �������ʽ�ٶ�PI�����������ݱ�������ֵ�����Ͻ���������ٶ�   //�ҵ��
 ** ��    ��: left_encoder��������������ֵ
 **           left_target ������Ŀ���ٶ�
 **           K           : ���PID����
 ** �� �� ֵ: �������PWM
 ** ��    ��: WBN
 ** ע    �⣺ʹ������ʽPI���Ƶ��ŵ㣺
 **           1.��ʽ��û���ۼӵĹ��̣�������ֻ������β���ֵ�йأ������ײ���������
 **           2.����������������仯���������и��õ��ݴ�
 ********************************************************************************************
 */
int Speed_PI_Right(int16 right_encoder,int16 right_target,MotorPID K)
{
    static int bias,last_bias,PWM;

    bias=right_target-right_encoder;              //�����ǰƫ��
    PWM+=(int)(K.P*(bias-last_bias)+K.I*bias);  //����ʽPID�����ѽ��ֱ�ӵ�������һ�ε�PWM��
    last_bias=bias;           //������һ��ƫ��

    //PID����޷�����ֹ���ڵ����PID������ͬ�����µ�������ջٵ�����
    //ע�⣺��PID����޷���ᵼ����Ӧ����
//    if(PWM>MOTOR_PWM_MAX)
//    {
//        PWM=MOTOR_PWM_MAX;
//    }
//    else if(PWM<-MOTOR_PWM_MAX)
//    {
//        PWM=-MOTOR_PWM_MAX;
//    }

    return PWM;         //���ؿ���ֱ�Ӹ�ֵ�������PWM
}
