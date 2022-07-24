/*
 * ImageGarageIn.c
 *
 *  Created on: 2022��7��22��
 *      Author: yue
 */

#include "ImageSpecial.h"
#include "ImageProcess.h"
#include <math.h>
#include "LED.h"
#include "zf_gpio.h"
#include "ICM20602.h"
#include "PID.h"
#include <math.h>
#include "Motor.h"

/********************************************************************************************
 ** ��������: ���ר����Bias���ɱ�������߸���
 ** ��    ��: starline:    ��ɢ�����ʼ��
 **           endline:     ��ɢ��Ľ�����
 **           *CentreLine����������
 ** �� �� ֵ: ƫ��Bias
 ** ��    ��: WBN
 *********************************************************************************************/
float DifferentBias_GarageIn(uint8 startline,uint8 endline,int *CentreLine)
{
    static float last_bias;
    float bias=0;
    uint8 rownum=0;//���ڼ������˶����е�ƫ��

    for(uint8 i=startline;i>endline;i--)
    {
        if(abs(CentreLine[i]-CentreLine[i+1])>MT9V03X_W/3)  //���߷���ͻ�䣬�����ۻ�
        {
            break;
        }
        bias+=(float)(MT9V03X_W/2-CentreLine[i]);  //�ۻ�ƫ�Mid-Centre�������Ҹ��������ڳ�ͷ����/�ң�Ӧ������/�ң�
        rownum++;
    }
    bias=bias/rownum/10;   //��ƫ���ֵ

    if(bias<0.5&&bias>-0.5) //�ֶμ�Ȩ
    {
        bias=bias*0.1;
    }
    else if(bias<-3||bias>3)
    {
        bias=bias*1.5;
    }

    if(bias==bias)  //bias����ֵ
    {
        last_bias=bias;
        return bias;
    }
    else
    {
        return last_bias;   //������󣬺��Դ˴μ��㣬������һ�ε�ֵ
    }
}

/********************************************************************************************
 ** ��������: ��������
 ** ��    ��: start_line�������ʼ��
 **           end_line����������
 ** �� �� ֵ: 1����⵽������
 **           0��û�м�⵽������
 ** ��    ��: WBN
 *********************************************************************************************/
uint8 ZebraCrossingSearch(uint8 start_line,uint8 end_line)
{
    uint8 num=0;
//    uint8 last_num=0; //����
    for(uint8 row=start_line;row-2>end_line;row-=2)    //����ɨ
    {
        num=0;
        for(uint8 column=0;column+1<MT9V03X_W-1;column++)   //����ɨ
        {
            if(BinaryImage[row][column]!=BinaryImage[row][column+1])    //�����
            {
                num++;
                if(num>10)
                {
                    return 1;
                }
//                //����
//                if(num>last_num)
//                {
//                    last_num=num;
//                }
            }
        }
    }
    return 0;
}

/********************************************************************************************
 ** ��������: ������⣬���øú����ؽ�����
 ** ��    ��: ��
 ** �� �� ֵ: 0��û��������
 **          1��������
 ** ��    ��: WBN
 *********************************************************************************************/
void GarageInBegin(void)
{
    Point StarPoint,EndPoint;
    uint8 row=MT9V03X_H-2,column=1,flag_1=0,flag_2=0;
    //Ѱ�Ҳ������
    if(BinaryImage[row][column]==IMAGE_BLACK)   //���½�Ϊ��
    {
        flag_2=1;   //�����ҹյ�
    }
    else                                        //���½�Ϊ��
    {
        for(;row-1>2*(MT9V03X_H/3);row--)   //����ɨ�����½�
        {
            //��-���������ͼ���·�����֮һ��
            if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)  //��-��
            {
                flag_2=1;
                break;
            }
        }
    }
    if(flag_2==1)  //������յ�
    {
        //�����Ϸ�Ѱ�ҹյ�
        for(;column+1<MT9V03X_W-1;column++)   //��ָ���ƶ����ײ����Ҷ�
        {
            if(BinaryImage[row][column+1]==IMAGE_WHITE)
            {
                break;
            }
        }
        while(column+1<MT9V03X_W-1&&row-1>0)  //����
        {
            if(BinaryImage[row][column+1]==IMAGE_BLACK) //�Һ�
            {
                column++;   //����
                flag_1=1;
            }
            if(BinaryImage[row-1][column]==IMAGE_BLACK) //�Ϻ�
            {
                row--;      //����
                flag_1=1;
            }
            if(flag_1==1)
            {
                flag_1=0;
                continue;
            }
            break;
        }
        //�ж��ǲ��������Ĺյ㣬�ų��Ѿ�������ֱ�������
        if(row>MT9V03X_H-5) //�����������
        {
            flag_2=0;   //�ٹյ�
        }
        StarPoint.Y=row;            //��㣺��յ��Ӧ�ұ߽�
        StarPoint.X=RightLine[row];
    }
    if(flag_2==0)   //���ﲻ��else����Ϊ�����if�л��ж�flag_2�Ľ�һ��ȷ��
    {
        StarPoint.Y=MT9V03X_H-2;    //��㣺���½�
        StarPoint.X=MT9V03X_W-2;
    }
    //Ѱ�Ҳ����յ�
    for(;row-1>0;row--) //����ɨ
    {
        if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)  //��-��
        {
            flag_2=0;   //��¼ָ���ƶ����
            for(;column+1<MT9V03X_W/3;column++)   //��ָ���ƶ����ײ����Ҷˣ�ֻ��ͼ��������֮һ��Ѱ��
            {
                if(BinaryImage[row][column+1]==IMAGE_BLACK)
                {
                    flag_2=1;   //�ƶ��ɹ�
                    break;
                }
            }
            if(flag_2==0)   //û���ƶ��ɹ���ֱ������Ѱ�ҹȵ׵Ĺ���
            {
                break;
            }
            while(column+1<MT9V03X_W-1&&row+1<MT9V03X_H-1)  //����
            {
                if(BinaryImage[row][column+1]==IMAGE_BLACK) //�Һ�
                {
                    column++;   //����
                    flag_1=1;
                }
                if(BinaryImage[row+1][column]==IMAGE_BLACK) //�º�
                {
                    row++;      //����
                    flag_1=1;
                }
                if(flag_1==1)
                {
                    flag_1=0;
                    continue;
                }
                break;
            }
            break;
        }
    }
    EndPoint.Y=row;     //�յ㣺�ȵ�
    EndPoint.X=column;
    if(EndPoint.Y>=StarPoint.Y||EndPoint.X>=StarPoint.X)    //�쳣
    {
        return;
    }
    //���������
    FillingLine('R', StarPoint, EndPoint);
//    //����������
    StarPoint.X=0;
    EndPoint.X=0;
    FillingLine('L', StarPoint, EndPoint);
    //��Bias
    Bias=DifferentBias_GarageIn(bias_startline,bias_endline,CentreLine); //��̬ǰհ����ƫ��
    bias_startline=95;bias_endline=50;                          //�ָ�Ĭ��ǰհ
    Garage_flag=1;
}

/********************************************************************************************
 ** ��������: ����Ƿ����
 ** ��    ��: ��
 ** �� �� ֵ: 0��δ���
 **          1�����
 ** ��    ��: WBN
 *********************************************************************************************/
uint8 GarageInEnd(void)
{
    //��򵥣�Bias<1
    Bias=DifferentBias(bias_startline,bias_endline,CentreLine); //��̬ǰհ����ƫ��
    if(fabs(Bias)<1)
    {
        return 1;
    }
    //������ͼ��������֮һ������
    for(uint8 row=MT9V03X_H-1;row>MT9V03X_H/3;row--)
    {
        if(BinaryImage[row][CentreLine[row]]==IMAGE_BLACK)
        {
            return 1;
        }
    }
    return 0;
}

/********************************************************************************************
 ** ��������: ���״̬��
 ** ��    ��: ��
 ** �� �� ֵ: 0��û��������
 **          1��������
 ** ��    ��: WBN
 *********************************************************************************************/
uint8 GarageInIdentify(void)
{
    static uint8 flag=0,flag_in=0;
    switch(flag)
    {
        case 0: //ʶ�������
        {
            if(ZebraCrossingSearch(MT9V03X_H/2+15+10, MT9V03X_H/2-15+10)==1)    //ʶ�𵽰�����
            {
                base_speed=150; //�������
                flag=1;
            }
            break;
        }
        case 1: //���߹���
        {
            GarageInBegin();        //���Ȳ���
            if(flag_in==1&&encoder_dis_flag==1&&icm_angle_z_flag==1) //���֡�������
            {
                gpio_set(LED_GREEN, 0);
                flag=2;
            }
            else if(flag_in==0)
            {
                EncoderDistance(1, 0.3, 0, 0);  //������������࣬�����������ѹ��
                StartIntegralAngle_Z(20);       //���������ǻ��֣���������ʶ�����
                flag_in=1;
                gpio_set(LED_YELLOW, 0);
            }
            break;
        }
        case 2: //�������
        {
            if(GarageInEnd()==1)    //ʶ���Ѿ����
            {
                flag=3; //״̬������
                return 1;
            }
            else
            {
                Bias=10;
                Garage_flag=1;
//                GarageInBegin();
            }
        }
    }
    return 0;
}
