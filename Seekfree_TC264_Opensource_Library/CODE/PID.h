/*
 * PID.h
 *
 *  Created on: 2021��12��10��
 *      Author: yue
 */

#ifndef CODE_PID_H_
#define CODE_PID_H_

//���PID���ƵĽṹ�壬���ڷֶο���PID��ͼ��ʶ������·��֮��PID�Ĳ����ͷ��ͱ仯
typedef struct SteerPID
{
    int P;
    int D;
}SteerPID;

//���ﻹҪд��PID��ʼ���ĺ����������������
int Steer_Position_PID(float SlopeBias,SteerPID K);//���λ��ʽPID���ƣ����÷ֶ�ʽPID����

#endif /* CODE_PID_H_ */
