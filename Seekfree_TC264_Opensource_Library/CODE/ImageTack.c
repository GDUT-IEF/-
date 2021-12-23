/*
 * ImageTack.c
 *  Created on: 2021��12��22��
 *  Author: 30516
 *  Effect: ���ڴ�Ŵ�ͼ������ȡѭ��Ԫ�صĺ���
 */

#include "ImageTack.h"

/*
 *******************************************************************************************
 ** ��������: ������������������ɢ�ĵ����ָ����һ�εĻع鷽�̣����������Իع鷽����ƫ��
 ** ��    ��: int starline:    ��ɢ�����ʼ��
 **           int endline:     ��ɢ��Ľ�����
 **           int *CentreLine����������
 ** �� �� ֵ: ��ϳ����Ļع鷽�̵�б�ʵĵ���
 ** ��    ��: LJF
 ** ע    �⣺��Ϊƫ����int���͵�����return��ʱ�������ƫ��С��45��Ҳ����б��С��1��ʱ����ܻ���Ϊ����ȷʵ�����0
 ********************************************************************************************
 */
float Regression_Slope(int startline,int endline,int *CentreLine)
{
    //Y=BX+A
    int i=0,SumX=0,SumY=0,SumLines=0;
    float SumUp=0,SumDown=0,avrX=0,avrY=0,Bias=0;
    SumLines=endline-startline;   // startline Ϊ��ʼ�У� //endline ������ //SumLines

    for(i=startline;i<endline;i++)
    {
        SumY+=i;                      //Y�����������
        SumX+=CentreLine[i];         //X�����������
    }
    avrX=(float)(SumX/SumLines);     //X��ƽ��ֵ
    avrY=(float)(SumY/SumLines);     //Y��ƽ��ֵ

    for(i=startline;i<endline;i++)
    {
        SumUp+=(CentreLine[i]-avrY)*(i-avrX);
        SumDown+=(i-avrX)*(i-avrX);
    }
    if(SumDown==0)//б�ʲ����ڵ�ʱ������ƫ����0
        Bias=0;
    else
        //B=(int)(SumUp/SumDown);б��
        Bias=SumDown/SumUp;//����Ҫ������Y��ļн�������б�ʵĵ�������������
    //A=(SumY-B*SumX)/SumLines;  //�ؾ�
    return Bias;
}


