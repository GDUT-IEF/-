/*
 * PID.c
 *
 *  Created on: 2021��12��10��
 *      Author: yue
 */
#include "PID.h"

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
int Steer_Position_PID(float SlopeBias,SteerPID K)//���λ��ʽPID���ƣ����÷ֶ�ʽPID����
{
    static float LastSlopeBias;
    int PWM;
    PWM=K.P*SlopeBias+K.D*(SlopeBias-LastSlopeBias);
    LastSlopeBias=SlopeBias;
    return PWM;
}

