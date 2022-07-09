/*
 * ImageCrossLoop.c
 *
 *  Created on: 2022��5��25��
 *      Author: yue
 */

#include "ImageSpecial.h"
#include "PID.h"
#include "ICM20602.h"

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
 ** ��������: ʶ����ʮ�ֻػ�����
 ** ��    ��: ��
 ** �� �� ֵ: 0��û��ʶ��ʮ�ֻػ�����
 **          1��ʶ��ʮ�ֻػ�����
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CrossLoopEnd_L(void)
{
    for(uint8 row=10;row+1<40;row++)  //����ɨ
    {
        if(BinaryImage[row][30]==IMAGE_BLACK&&BinaryImage[row+1][30]==IMAGE_WHITE)
        {
            return 0;
        }
    }
    if(LostNum_LeftLine>110)    //��ֹ��δ����������
    {
        return 0;
    }
    if(LostNum_LeftLine>L_LOSTNUM&&LostNum_RightLine>L_LOSTNUM)  //���ұ߽������
    {
        if(fabsf(Bias)<1.5)
        {
            //������Ҵ���������һ������ʱʵ�ֳ���
            Bias=-10;
            systick_delay_ms(STM0,300);
            return 1;
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
                CrossLoopOverBegin_L(LeftLine, RightLine, InflectionL, InflectionR);
                num_2++;
            }
            break;
        }
        case 1: //С��ʶ��ʮ�ֻػ��ĳ��ڣ���ת����
        {
            if(CrossLoopEnd_L()==1&&flag_end==0)  //��һ�μ�⵽�ػ�����
            {
                StartIntegralAngle_Z(70);   //��������
                flag_end=1;
            }
            if(flag_end==1)  //�����Ѿ�����
            {
                if(icm_angle_z_flag==1) //������״̬
                {
                    flag_end=0;flag=3; //��ת��״̬3
                    break;
                }
            }
            break;
        }
    }
}
