/*
 * FuzzyPID.h
 *
 *  Created on: 2022��1��21��
 *  Author: 30516
 *  Effect: ����ģ��PID
 */

#ifndef CODE_FUZZYPID_H_
#define CODE_FUZZYPID_H_

#include "common.h" //������������

//������ģ�����ĺ궨��{NB,NM,NS,ZO,PS,PM,PB},������������Ϊ������������ı�д
//******************ģ���Ӽ�******************************
#define NB -3
#define NM -2
#define NS -1
#define ZO  0
#define PS  1
#define PM  2
#define PB  3
//*******************************************************

//����ֵ�ĵ�λֵ��FuzzyArray[]*���=����ֵ����ֵ��Ҫ����ʹ�ã�(MAX-MIN)/8�õ�
/*******************��������궨��****************************/
//*******************************************************
#define BiasMembership 60 //ƫ��
#define BiasCMembership 10//ƫ������һ��ƫ��Ĳ�
#define SteerKP 1
#define SteerKd 1
#define MotorKP 1
#define MotorKI 1
//********************************************************

/*************************ģ������***********************/
uint8 KPFuzzyRule[7][7]={{PB,PB,PM,PM,PS,ZO,ZO},
                         {PB,PB,PM,PS,PS,ZO,NS},
                         {PM,PM,PM,PS,ZO,NS,NS},
                         {PM,PM,PS,ZO,NS,NM,NM},
                         {PS,PS,ZO,NS,NS,NM,NM},
                         {PS,ZO,NS,NM,NM,NM,NB},
                         {ZO,ZO,NM,NM,NM,NB,NB}};//KP������ģ�������
uint8 KIFuzzyRule[7][7]={{NB,NB,NM,NM,NS,ZO,ZO},
                         {NB,NB,NM,NS,NS,ZO,ZO},
                         {NB,NM,NS,NS,ZO,PS,PS},
                         {NM,NM,NS,ZO,PS,PM,PM},
                         {NM,NS,ZO,PS,PS,PM,PB},
                         {ZO,ZO,PS,PS,PM,PB,PB},
                         {ZO,ZO,PS,PM,PM,PB,PB}};//KI������ģ�������
uint8 KDFuzzyRule[7][7]={{PS,NS,NB,NB,NB,NM,PS},
                         {PS,NS,NB,NM,NM,NS,ZO},
                         {ZO,NS,NM,NM,NS,NS,ZO},
                         {ZO,NS,NS,NS,NS,NS,ZO},
                         {ZO,ZO,ZO,ZO,ZO,ZO,ZO},
                         {PB,NS,PS,PS,PS,PS,PB},
                         {PB,PM,PM,PM,PS,PS,PB}};//KD������ģ�������
/**********************************************************/

void ClacMembership(float E,float Membership[2],int Index[2]);//���ƫ�������ֵ��������
int  SolutionFuzzy(int IndexE[2],float MSE[2],int IndexEC[2],float MSEC[2],int type);//��ģ��

#endif /* CODE_FUZZYPID_H_ */