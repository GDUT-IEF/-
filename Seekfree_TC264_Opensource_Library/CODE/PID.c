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
    SteerK->P=15.25;SteerK->I=0;SteerK->D=30;           //��ʼ�������PID����   //У������19.25 5   //����������14.25 30
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
uint32 Steer_Position_PID(float SlopeBias,SteerPID K)//���λ��ʽPID���ƣ����÷ֶ�ʽPID����
{
    static float LastSlopeBias;
    int PWM;
    PWM=(int)(K.P*SlopeBias+K.D*(SlopeBias-LastSlopeBias));
    LastSlopeBias=FirstOrderLagFilter(SlopeBias);   //һ�׵�ͨ�˲�
    return STEER_MID+PWM;//����б�ʵķ�ΧΪ[-5,5]����������PWM�ķ�ΧΪ[850,680]����ȥ��ֵ֮�����ӳ�䵽[-85,85]���ڴ˶�Ӧ�����Է���ֵӦ�ø����ټ���ֵ��KP�Ȳ²�Ϊ17
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
    static int Bias,Last_Bias,PWM,Last_2_Bias;

    Bias=left_target-left_encoder;              //�����ǰƫ��
    PWM+=(int)(K.P*(Bias-Last_Bias)+K.I*Bias+K.D*(Bias-2*Last_Bias+Last_2_Bias));  //����ʽPID�����ѽ��ֱ�ӵ�������һ�ε�PWM��

    Last_2_Bias=Last_Bias;    //������һ��ƫ��
    Last_Bias=Bias;           //������һ��ƫ��

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
    static int Bias,Last_Bias,PWM,Last_2_Bias;

    Bias=right_target-right_encoder;              //�����ǰƫ��
    PWM+=(int)(K.P*(Bias-Last_Bias)+K.I*Bias+K.D*(Bias-2*Last_Bias+Last_2_Bias));  //����ʽPID�����ѽ��ֱ�ӵ�������һ�ε�PWM��

    Last_2_Bias=Last_Bias;    //������һ��ƫ��
    Last_Bias=Bias;           //������һ��ƫ��

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
