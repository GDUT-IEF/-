/*
 * ImageCrossLoop.c
 *
 *  Created on: 2022��5��25��
 *      Author: yue
 */

#include "ImageSpecial.h"
#include "PID.h"
#include "ICM20602.h"
#include <stdio.h>
#include "SEEKFREE_18TFT.h"
#include "ImageProcess.h"

/*
 *******************************************************************************************
 ** ��������: ʶ����ʮ�ֻػ����
 ** ��    ��: LeftLine����������
 **           RightLine����������
 **           InflectionL�����¹յ�
 **           InflectionR�����¹յ�
 ** �� �� ֵ: 0��û��ʶ��ʮ�ֻػ����
 **          1��ʶ��ʮ�ֻػ����
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CrossLoopBegin_L(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    if(InflectionL.X!=0&&InflectionL.Y!=0&&InflectionR.X==0&&InflectionR.Y==0)    //������յ��Ҳ������ҹյ�
    {
        uint8 row_up=0;
        for(uint8 row=InflectionL.Y+2,column=InflectionL.X-2;row-1>0;row--)  //��յ�����ɨ
        {
            if(BinaryImage[row][column]==IMAGE_BLACK&&BinaryImage[row-1][column]==IMAGE_WHITE)  //��-��
            {
                for(;row-1>0;row--)   //��������ɨ
                {
                    if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)  //��-��
                    {
                        row_up=row; //���油�ߵ��յ�Y����
                        for(;row-1>0;row--)   //��������ɨ
                        {
                            if(BinaryImage[row][column]==IMAGE_BLACK&&BinaryImage[row-1][column]==IMAGE_WHITE)  //��-��
                            {
                                for(;row-1;row--)   //��������ɨ
                                {
                                    if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)  //��-��
                                    {
                                        for(row_up-=5;column<MT9V03X_W;column++)
                                        {
                                            if(BinaryImage[row_up][column]==IMAGE_WHITE)
                                            {
                                                Point end;
                                                end.Y=row_up;
                                                end.X=column;
                                                FillingLine('L', InflectionL, end); //����
                                                return 1;
                                            }
                                        }
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }
    }
    if(LostNum_LeftLine>70&&LostNum_RightLine<35)   //�޹յ㵫���Ҷ��߷���
    {
        float right_bias=0;
        right_bias=Regression_Slope(110, 60, RightLine);    //���ұ���б��
        if(fabsf(right_bias)>0.6)   //��ֹ�����������
        {
            return 0;
        }
        for(uint8 row=0;row<MT9V03X_H-1;row++)  //����ɨ
        {
            if(BinaryImage[row][20]==IMAGE_BLACK&&BinaryImage[row+1][20]==IMAGE_WHITE)  //��-��
            {
                for(;row<MT9V03X_H-1;row++) //��������ɨ
                {
                    if(BinaryImage[row][20]==IMAGE_WHITE&&BinaryImage[row+1][20]==IMAGE_BLACK)  //��-��
                    {
                        for(;row<MT9V03X_H-1;row++) //��������ɨ
                        {
                            if(BinaryImage[row][20]==IMAGE_BLACK&&BinaryImage[row+1][20]==IMAGE_WHITE)  //��-��
                            {
                                //Ѱ�Ҳ��ߵ�
                                for(uint8 row=100;row-1>0;row--)    //����ɨ
                                {
                                    if(LeftLine[row]==0&&LeftLine[row-1]!=0)
                                    {
                                        if(row-5>0)
                                        {
                                            row-=5;
                                        }
                                        Point start,end;
                                        start.Y=119;
                                        start.X=LeftLine[row];
                                        end.Y=row;
                                        end.X=LeftLine[row]+1;  //���ܲ���ֱ���ߣ�����ƫ��
                                        FillingLine('L', start, end);   //����
                                        return 1;
                                    }
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: �ڶ���ʶ����ʮ�ֻػ����
 ** ��    ��: LeftLine����������
 **           RightLine����������
 **           InflectionL�����¹յ�
 **           InflectionR�����¹յ�
 ** �� �� ֵ: 0��û��ʶ��ʮ�ֻػ����
 **          1��ʶ��ʮ�ֻػ����
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CrossLoopOverBegin_L(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    float bias_right=Regression_Slope(119,0,RightLine);   //����ұ߽���б��
    if(fabsf(bias_right)<G_LINEBIAS)    //�ұ߽�Ϊֱ��
    {
        for(uint8 row=MT9V03X_H-30,column=20;row-1>0;row--)    //����ɨ
        {
            if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)
            {
                uint8 row_f=row;
                for(;row-1>0;row--)
                {
                    if(BinaryImage[row][column]==IMAGE_BLACK&&BinaryImage[row-1][column]==IMAGE_WHITE)
                    {
                        uint8 row_s=row;
                        for(;row-1>0;row--)
                        {
                            if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)
                            {
                                row=(row_f+row_s)/2;
                                for(;column+1<MT9V03X_W;column++)   //����ɨ
                                {
                                    if(BinaryImage[row][column]==IMAGE_BLACK&&BinaryImage[row][column+1]==IMAGE_WHITE)
                                    {
                                        /*���߲���*/
                                        Point end,start;
                                        end.Y=row;
                                        end.X=column;
                                        start.Y=MT9V03X_H-2;
                                        start.X=1;
                                        FillingLine('L', start, end);   //����
                                        return 1;
                                    }
                                }
                                return 0;
                            }
                        }
                        return 0;
                    }
                }
                return 0;
            }
        }
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: ʶ����ʮ�ֻػ�����
 ** ��    ��: InflectionL�����¹յ�
 **           InflectionR�����¹յ�
 ** �� �� ֵ: 0��û��ʶ��ʮ�ֻػ�����
 **          1��ʶ��ʮ�ֻػ�����
 ** ��    ��: WBN
 ** ע    �⣺û�жԳ����ڻ���������ʻ����������������������ڻ�������̫�����⣨�ҹղ���ѹ�ǣ���
 **           ���ǳ������⻷��ʻ����ѹ�Ƿ��ա�Ԥ�ƴ�������ͨ���ҹյ��λ�����жϼ������
 ********************************************************************************************
 */
uint8 CrossLoopEnd_L(Point InflectionL,Point InflectionR)
{
    static uint8 flag=0;    //��������flag��������һ�β����ж�

    if(flag==1) //�Ѿ������ߣ����ⲻ��Ԥ֪���Ŵ�ϲ���
    {
        for(uint8 row=MT9V03X_H-1;row-1>0;row--)  //�м�������ɨ
        {
            if(BinaryImage[row][MT9V03X_W/2]==IMAGE_WHITE&&BinaryImage[row-1][MT9V03X_W/2]==IMAGE_BLACK)
            {
                uint8 EndX=0;
                if(row>70)  //���ݱƽ��߽�̶����������߽����̶�
                {
                    EndX=MT9V03X_W-1;
                }
                else
                {
                    EndX=MT9V03X_W/2;
                }
                //�����ߣ���ת
                Point StartPoint,EndPoint;
                StartPoint.Y=MT9V03X_H-1;   //��㣺����е���߽��
                StartPoint.X=LeftLine[MT9V03X_H-1];
                EndPoint.Y=row;             //�յ㣺�ֽ���
                EndPoint.X=EndX;
                FillingLine('L', StartPoint, EndPoint);
                Bias=DifferentBias(StartPoint.Y,EndPoint.Y,CentreLine);
                return 1;
            }
        }
        //���������forѭ����û�з��֣�����ʵʩ����
    }
    else if(LostNum_LeftLine>55&&LostNum_RightLine>55&&fabsf(Bias)<1.5) //�����жϵ�Լ������
    {
        for(uint8 row=MT9V03X_H/2;row-1>0;row--)  //�м�������ɨ
        {
            if(BinaryImage[row][MT9V03X_W/2]==IMAGE_WHITE&&BinaryImage[row-1][MT9V03X_W/2]==IMAGE_BLACK)
            {
                //�����ߣ���ת
                Point StartPoint,EndPoint;
                StartPoint.Y=MT9V03X_H-1;   //��㣺����е���߽��
                StartPoint.X=LeftLine[MT9V03X_H-1];
                EndPoint.Y=row;             //�յ㣺�ֽ����е�
                EndPoint.X=MT9V03X_W/2;
                FillingLine('L', StartPoint, EndPoint);
                flag=1; //�������߱��
                Bias=DifferentBias(StartPoint.Y,EndPoint.Y,CentreLine);
                return 1;
            }
        }
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: ��ʮ�ֻػ�״̬��
 ** ��    ��: ��
 ** �� �� ֵ: 0��û�����ʮ�ֻػ�
 **          1�����ʮ�ֻػ�
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CrossLoopIdentify_L(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    static uint8 flag,flag_end,num_1,num_2;

    switch(flag)
    {
        case 0: //С��ʶ��ʮ�ֻػ�����ڣ����в���ֱ��
        {
            if(CrossLoopBegin_L(LeftLine, RightLine, InflectionL, InflectionR)==1)  //ʶ�𵽻ػ���ڣ�����ֱ��
            {
                if(num_1<100) //num�޷�
                {
                    num_1++;
                }
            }
            else if(num_1>0) //û��ʶ����ڵ�֮ǰ��ʶ�𵽣���Ӧ��ѹ����ڵ����
            {
                CrossLoopOverBegin_L(LeftLine, RightLine, InflectionL, InflectionR);    //�ڶ���ʶ��ػ���ڣ�����ֱ��
                num_2++;
            }
//            if(num_2>/*ĳ����ֵ*/)  //�����Ѿ�����ػ�
//            {
//                flag=1; //��ת��״̬1
//                break;
//            }
            break;
        }
        case 1: //С��ʶ��ʮ�ֻػ��ĳ��ڣ���ת����
        {
            if(CrossLoopEnd_L(InflectionL, InflectionR)==1&&flag_end==0)  //��һ�μ�⵽�ػ�����
            {
                StartIntegralAngle_Z(70);   //��������
                flag_end=1;
            }
            if(flag_end==1)  //�����Ѿ�����
            {
                if(icm_angle_z_flag==1) //������״̬
                {
                    flag_end=0;flag=2; //��ת��״̬2
                    return 1;
                    break;
                }
            }
            break;
        }
    }
    return 0;
}
