/*
 * FuzzyPID.c
 *
 *  Created on: 2022��1��21��
 *  Author: 30516
 *  Effect: ����ģ��PID
 */

#include "FuzzyPID.h"

/********************************************************************************************
 ** ��������: ���ƫ�������ֵ��������
 ** ��    ��: float E: ƫ��/ƫ������궨�壨������������-3~3֮�䣩
 **           float Membership[2]: ���ڴ洢�������������������ȵ����飬M[0]�ǱȽ�С��ģ���Ӽ���������
 **           int Index[2]: �������飬���������������Membershipͬ��M[0]�ǱȽ�С��ģ���Ӽ�������ֵ
 ** �� �� ֵ: ��
 ** ��    ��: LJF
 *********************************************************************************************/
void ClacMembership(float E,float Membership[2],int Index[2])
{
    /*���������Ⱥ���*/
    if(E>=NB && E<=NM)
    {
        Index[0]=0;//NB
        Index[1]=1;//NM
        Membership[0]=NM-E;//������NB��������(NM-E)/(NM-NB)����Ϊ��ĸ����1
        Membership[1]=E-NB;//������NM��������(E-NB)/(NM-NB)
    }
    else if(E>=NM && E<=NS)
    {
        Index[0]=1;
        Index[1]=2;
        Membership[0]=NS-E;//������NM��������
        Membership[1]=E-NM;//������NS��������
    }
    else if(E>=NS && E<=ZO)
    {
        Index[0]=2;
        Index[1]=3;
        Membership[0]=ZO-E;//������NS��������
        Membership[1]=E-NS;//������ZO��������
    }
    else if(E>=ZO && E<=PS)
    {
        Index[0]=3;
        Index[1]=4;
        Membership[0]=PS-E;//������ZO��������
        Membership[1]=E-ZO;//������PS��������
    }
    else if(E>=PS && E<=PM)
    {
        Index[0]=4;
        Index[1]=5;
        Membership[0]=PM-E;//������PS��������
        Membership[1]=E-PS;//������PM��������
    }
    else if(E>=PM && E<=PB)
    {
        Index[0]=5;
        Index[1]=6;
        Membership[0]=PB-E;//������PS��������
        Membership[1]=E-PM;//������PM��������
    }
}

/********************************************************************************************
 ** ��������: ����ģ��������Լ����ķ�������
 ** ��    ��: int IndexE[2]: ƫ�����������
 **           float MSE[2]: ƫ�������������ֵ������������
 **           int IndexEC[2]: ƫ��仯�ʵ���������
 **           float MSEC[2]: ƫ��仯�ʶ�����������ֵ������������
 **           int type: 1��KP 2��KI 3��KD
 ** �� �� ֵ: K������K=K0+detaK*����ϵ��
 ** ��    ��: LJF
 *********************************************************************************************/
int  SolutionFuzzy(int IndexE[2],float MSE[2],int IndexEC[2],float MSEC[2],int type)
{
    int temp=0;
    switch(type)
    {
        case 1:
            //���ķ����
            temp=KPFuzzyRule[IndexE[0]][IndexEC[0]]*MSE[0]*MSEC[0]+
                 KPFuzzyRule[IndexE[0]][IndexEC[1]]*MSE[0]*MSEC[1]+
                 KPFuzzyRule[IndexE[1]][IndexEC[0]]*MSE[1]*MSEC[0]+
                 KPFuzzyRule[IndexE[1]][IndexEC[1]]*MSE[1]*MSEC[1];
            break;
        case 2:
            temp=KIFuzzyRule[IndexE[0]][IndexEC[0]]*MSE[0]*MSEC[0]+
                 KIFuzzyRule[IndexE[0]][IndexEC[1]]*MSE[0]*MSEC[1]+
                 KIFuzzyRule[IndexE[1]][IndexEC[0]]*MSE[1]*MSEC[0]+
                 KIFuzzyRule[IndexE[1]][IndexEC[1]]*MSE[1]*MSEC[1];
            break;
        case 3:
            temp=KDFuzzyRule[IndexE[0]][IndexEC[0]]*MSE[0]*MSEC[0]+
                 KDFuzzyRule[IndexE[0]][IndexEC[1]]*MSE[0]*MSEC[1]+
                 KDFuzzyRule[IndexE[1]][IndexEC[0]]*MSE[1]*MSEC[0]+
                 KDFuzzyRule[IndexE[1]][IndexEC[1]]*MSE[1]*MSEC[1];
            break;
        default:
            break;
    }
    return temp;
}

