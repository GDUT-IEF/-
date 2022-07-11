/*
 * ImageCircleIsland.c
 *
 *  Created on: 2022��5��25��
 *      Author: yue
 */

#include "ImageSpecial.h"
#include "PID.h"
#include "common.h"
#include "ICM20602.h"
#include "LED.h"
#include "zf_gpio.h"
#include "ImageProcess.h"
#include "zf_stm_systick.h"
#include <stdio.h>

/*
 *******************************************************************************************
 ** ��������: ʶ�𻷵���ڣ����
 ** ��    ��: LeftLine����������
 **          RightLine����������
 ** �� �� ֵ: 0��û��ʶ�𵽻���
 **          1��ʶ�𵽻������ڳ������
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CircleIslandBegin_L(int *LeftLine,int *RightLine)
{
    static uint8 flag=0;  //��������flag�������ڵ�һ�β����ж�

    if(flag==1) //�Ѿ������ߣ����ⲻ��Ԥ֪���Ŵ�ϲ���
    {
        for(int row=MT9V03X_H;row-1>0;row--)  //�������ϼ����߽���
        {
            if(LeftLine[row]==0&&LeftLine[row-1]!=0)    //���ж��߶���һ�в�����
            {
                Point StarPoint,EndPoint;   //���岹�ߵ������յ�
                EndPoint.Y=row;             //�յ㸳ֵ
                EndPoint.X=LeftLine[row];
                StarPoint.Y=MT9V03X_H-1;    //��㸳ֵ
                StarPoint.X=MT9V03X_W-1;
                FillingLine('R',StarPoint,EndPoint);    //����
                return 1;
            }
        }
        //���������forѭ����û�з��֣�����ʵʩ����
    }
    else if(BinaryImage[115][5]!=IMAGE_BLACK)   //����Լ������
    {
        if(LostNum_LeftLine>C_LOSTLINE)   //��߶��ߣ�������������
        {
            for(int row=MT9V03X_H;row-1>0;row--)  //�������ϼ����߽���
            {
                if(LeftLine[row]==0&&LeftLine[row-1]!=0)    //���ж��߶���һ�в�����
                {
                    //���������ֹ���뻷��������
                    for(int column=0;column+1<MT9V03X_W-1;column++) //����ɨ
                    {
                        if(BinaryImage[10][column]!=BinaryImage[10][column+1])
                        {
                            Point StarPoint,EndPoint;   //���岹�ߵ������յ�
                            EndPoint.Y=row;             //�յ㸳ֵ
                            EndPoint.X=LeftLine[row];
                            StarPoint.Y=MT9V03X_H-1;    //��㸳ֵ
                            StarPoint.X=MT9V03X_W-1;
                            FillingLine('R',StarPoint,EndPoint);    //����
                            flag=1; //�������߱��
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: �ڶ���ʶ�𻷵���ڣ���࣬����ֱ�к���
 ** ��    ��: LeftLine����������
 **          RightLine����������
 ** �� �� ֵ: 0��û��ʶ�𵽻���
 **          1��ʶ�𵽻������ڳ������
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CircleIslandOverBegin_L(int *LeftLine,int *RightLine)
{
    //������������
    for(int row=MT9V03X_H;row-1>0;row--)  //�������ϼ����߽���
    {
        if(LeftLine[row]==0&&LeftLine[row-1]!=0)    //���ж��߶���һ�в�����
        {
            //���������ֹ���뻷��������
            for(int column=0;column+1<MT9V03X_W-1;column++) //����ɨ
            {
                if(BinaryImage[10][column]!=BinaryImage[10][column+1])
                {
                    if(row-10>0)
                    {
                        row-=10;
                    }
                    Point StarPoint,EndPoint;   //���岹�ߵ������յ�
                    EndPoint.Y=row;             //�յ㸳ֵ
                    EndPoint.X=LeftLine[row];
                    StarPoint.Y=MT9V03X_H-1;    //��㸳ֵ
                    StarPoint.X=0;
                    FillingLine('L',StarPoint,EndPoint);    //����
                    return 1;
                }
            }
        }
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: ʶ�𻷵�������־���ڣ����
 ** ��    ��: InflectionL�����¹յ�
 **          InflectionR�����¹յ�
 ** �� �� ֵ: 0��û��ʶ�𵽻���
 **          1��ʶ�𵽻����������ڳ������
 ** ��    ��: WBN
 ** ע    �⣺�ú�������ʱӦȷ��С�����ڻ�����
 ********************************************************************************************
 */
uint8 CircleIslandEnd_L(Point InflectionL,Point InflectionR)
{
    static uint8 flag=0;
    if(flag==1) //�Ѿ�ʶ�𵽻������ڣ����������жϣ���֤���ߵ�������
    {
        if(InflectionR.X>MT9V03X_W/2&&InflectionR.Y>MT9V03X_H/2)    //�ҹյ���ʵ����
        {
            //�����ұ߽��߲���
            for(uint8 row=MT9V03X_H/2;row-1>0;row--)  //����ɨ
            {
                if(BinaryImage[row][MT9V03X_W/2]==IMAGE_WHITE&&BinaryImage[row-1][MT9V03X_W/2]==IMAGE_BLACK)
                {
                    //����
                    Point StartPoint;
                    StartPoint.X=RightLine[0];    //���Ϊ����е��ұ߽��
                    StartPoint.Y=0;
                    FillinLine_V2('R', InflectionR.Y, row, StartPoint, InflectionR);
                    break;
                }
            }
        }
        else    //�������ҹյ�
        {
            //ͼ�����½ǲ��ߵ������м䶥��
            for(uint8 row=MT9V03X_H-20;row-1>0;row--)  //����ɨ
            {
                if(BinaryImage[row][MT9V03X_W/2]==IMAGE_WHITE&&BinaryImage[row-1][MT9V03X_W/2]==IMAGE_BLACK)
                {
                    //����
                    Point StartPoint,EndPoint;
                    StartPoint.Y=MT9V03X_H-1;
                    StartPoint.X=MT9V03X_W-1;
                    EndPoint.Y=row;
                    EndPoint.X=MT9V03X_W/2;
                    FillingLine('R',StartPoint,EndPoint);
                    break;
                }
            }
        }
        return 1;
    }
    if(LostNum_LeftLine>55&&LostNum_RightLine>55&&fabsf(Bias)<1.5)  //Լ��������ʶ�𵽻�������
    {
        /*У������ʹ�ò�����������ǵķ�������*/
        if(InflectionR.X>MT9V03X_W/2&&InflectionR.Y>MT9V03X_H/2)    //�ҹյ���ʵ����
        {
            //�����ұ߽��߲���
            for(uint8 row=MT9V03X_H/2;row-1>0;row--)  //����ɨ
            {
                if(BinaryImage[row][MT9V03X_W/2]==IMAGE_WHITE&&BinaryImage[row-1][MT9V03X_W/2]==IMAGE_BLACK)
                {
                    //����
                    Point StartPoint;
                    StartPoint.X=RightLine[0];    //���Ϊ����е��ұ߽��
                    StartPoint.Y=0;
                    FillinLine_V2('R', InflectionR.Y, row, StartPoint, InflectionR);
                    break;
                }
            }
        }
        else    //�������ҹյ�
        {
            //ͼ�����½ǲ��ߵ������м䶥��
            for(uint8 row=MT9V03X_H/2;row-1>0;row--)  //����ɨ
            {
                if(BinaryImage[row][MT9V03X_W/2]==IMAGE_WHITE&&BinaryImage[row-1][MT9V03X_W/2]==IMAGE_BLACK)
                {
                    //����
                    Point StartPoint,EndPoint;
                    StartPoint.Y=MT9V03X_H-1;
                    StartPoint.X=MT9V03X_W-1;
                    EndPoint.Y=row;
                    EndPoint.X=MT9V03X_W/2;
                    FillingLine('R',StartPoint,EndPoint);
                    break;
                }
            }
        }
        flag=1; //�Ѿ�ʶ�𵽻�������flag
        return 1;
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: ʶ�𻷵����ڣ���࣬����ֱ��
 ** ��    ��: LeftLine����������
 **          RightLine����������
 **          InflectionL�����¹յ�
 **          InflectionR�����¹յ�
 ** �� �� ֵ: 0��û��ʶ��Ŀ��
 **          1��ʶ��Ŀ��
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CircleIslandExit_L(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    if(InflectionL.X!=0&&InflectionL.Y!=0&&InflectionL.Y>85)  //�ж�����һ���Ƿ������յ����Ҳ�ֱ�����ҳ��ӽӽ�����
    {
        float bias_right=Regression_Slope(119,0,RightLine);   //����ұ߽���б��
        if(fabsf(bias_right)<G_LINEBIAS)    //�ұ߽�Ϊֱ��
        {
            for(uint8 row=InflectionL.Y;row-1>0;row--)    //����յ㿪ʼ����ɨ
            {
                if(BinaryImage[row][InflectionL.X]==IMAGE_WHITE&&BinaryImage[row-1][InflectionL.X]==IMAGE_BLACK)
                {
                    uint8 row_f=row;
                    for(;row-1>0;row--)
                    {
                        if(BinaryImage[row][InflectionL.X]==IMAGE_BLACK&&BinaryImage[row-1][InflectionL.X]==IMAGE_WHITE)
                        {
                            row=(row+row_f)/2;
                            for(uint8 column=InflectionL.X;column<MT9V03X_W-1;column++)   //����ɨ
                            {
                                if(BinaryImage[row-1][column]==IMAGE_WHITE)
                                {
                                    /*���߲���*/
                                    Point end;
                                    end.Y=row-1;
                                    end.X=column;
                                    FillingLine('L', InflectionL, end);   //����
                                    return 1;
                                }
                            }
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: �ڶ���ʶ�𻷵����ڣ���࣬����ֱ��
 ** ��    ��: LeftLine����������
 **          RightLine����������
 ** �� �� ֵ: 0��û��ʶ��Ŀ��
 **          1��ʶ��Ŀ��
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CircleIslandOverExit_L(int *LeftLine,int *RightLine)
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
 ** ��������: ʶ�𻷵��в������
 ** ��    ��: LeftLine����������
 **          RightLine����������
 ** �� �� ֵ: 0��ʶ��Ŀ��
 **          1��û��ʶ��Ŀ��
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CircleIslandMid_L(int *LeftLine,int *RightLine)
{
    /*���������1.���������½��Ǻ�ɫ�����ڽ����в�ǰ���Բ����У��������������Ϊһ����������
     *          2.�������ںܿ���ĵط�ʱ�����½��ǰ�ɫ�ģ����Һ�δ�����в���ͼ������ƣ������Ψһ�����������ұߺڶ����ֵĸ߶Ȳ�ͬ
     *            ͨ��ʶ��ڰ��������ȷ���ڶ��߶ȣ��Ӷ�ȷ�����������*/

    if(BinaryImage[MT9V03X_H-1][0]==IMAGE_BLACK)    //����λ��Ϊ��
    {
        //�������for��ֹ�ڻ�������ʱ����Ϊ�����в�
        for(uint8 row=MT9V03X_H/2;row+1<MT9V03X_H-1;row++) //����ɨ
        {
            if(LeftLine[row]==0&&LeftLine[row+1]!=0)    //����-������
            {
                return 0;
            }
        }
        //�жϻ����в�������
        for(uint8 row=60;row-1>0;row--)  //����ɨ
        {
            if(LeftLine[row]!=0&&LeftLine[row-1]==0)    //������-����
            {
                for(;row-1>0;row--)    //��������ɨ
                {
                    if(LeftLine[row]==0&&LeftLine[row-1]!=0)    //����-������
                    {
                        return 1;
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
 ** ��������: �жϻ���Flag3�Ƿ���������
 ** ��    ��: LeftLine����������
 **           RightLine����������
 ** �� �� ֵ: 0��Flag3������
 **           1��Flag3�����������
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CircleIslandInside_L(void)
{
    if(LostNum_LeftLine>70)    //��߽ӽ�ȫ����
    {
        //������þ���ֵ���������
        if(BinaryImage[90][40]==IMAGE_WHITE&&BinaryImage[30][120]==IMAGE_BLACK)
        {
            return 1;
        }
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: ʶ���󻷵�
 ** ��    ��: LeftLine����������
 **          RightLine����������
 **          InflectionL�����¹յ�
 **          InflectionR�����¹յ�
 ** �� �� ֵ: 0����δ��ɻ���
 **          1����ɻ���
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CircleIslandIdentify_L(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    static uint8 flag,flag_exit,flag_in,flag_begin,flag_last_begin,flag_last2_begin,flag_end;
    switch(flag)
    {
        case 0: //��ʱС��δ���ﻷ������ʼ�жϻ������ڲ���·�Σ�������Ҫ����
        {
            if(flag_exit==0)    //��δʶ�𵽻�������
            {
                if(CircleIslandExit_L(LeftLine, RightLine, InflectionL, InflectionR)==1)    //ʶ�𵽻������ڲ���·��
                {
                    flag_exit=1;
                }
            }
            if(flag_exit==1)   //֮ǰ��ʶ�𵽻�������
            {
                if(CircleIslandMid_L(LeftLine, RightLine)==1)   //ʶ�𵽻����в�
                {
                    flag_exit=0;flag=1; //��ת��״̬1
                    gpio_set(LED_GREEN, 0);
                    break;
                }
                CircleIslandOverExit_L(LeftLine, RightLine);    //�ڶ���ʶ�𻷵����ڣ�����ֱ��
            }
            break;
        }
        case 1: //��ʱС�����ﻷ���в�����ʼ�жϻ�����ڲ�����뻷��������Ҫ����
        {
            if(CircleIslandBegin_L(LeftLine, RightLine)==1&&flag_in==0) //ʶ�𵽻������
            {
                StartIntegralAngle_Z(20);   //���������Ǹ����뻷
                flag_in=1;                  //�����ظ�����������
                gpio_set(LED_WHITE, 0);
            }
            if(flag_in==1)  //֮ǰ�Ѿ�ʶ�𵽻������
            {
                if(icm_angle_z_flag==1) //������ʶ���Ѿ��뻷
                {
                    flag_in=0;flag=2;   //��ת��״̬2
                    gpio_set(LED_YELLOW,0);
                    break;
                }
            }
            break;
        }
        case 2: //��ʱС���Ѿ��ڻ����У���ʼ�жϻ�������
        {
            if(CircleIslandEnd_L(InflectionL, InflectionR)==1&&flag_end==0)  //��һ�μ�⵽��������
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
        case 3: //��ʱС���Ѿ����������ǻ��ٴν���������ڣ���Ҫ����ֱ��
        {
            flag_begin=CircleIslandOverBegin_L(LeftLine, RightLine);     //ʶ�𻷵���ڲ��ߺ���
            if(flag_begin==0&&flag_last_begin==0&&flag_last2_begin==1)   //���ϴ�ʶ�𵽻�����ڶ������ζ�û��ʶ�𵽻������
            {
                flag=4;     //��ת��δ֪״̬��״̬������
                return 1;   //�˳�״̬��
            }
            flag_last2_begin=flag_last_begin;   //�������ϴε�״̬
            flag_last_begin=flag_begin;         //������һ�ε�״̬
            break;
        }
        default:break;
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: ʶ�𻷵���ڣ��Ҳ�
 ** ��    ��: LeftLine����������
 **           RightLine����������
 ** �� �� ֵ: 0��û��ʶ�𵽻���
 **           1��ʶ�𵽻������ڳ����Ҳ�
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CircleIslandBegin_R(int *LeftLine,int *RightLine)
{
    if(BinaryImage[115][154]==IMAGE_BLACK)    //��ֹ��ǰ���뻷��
    {
        return 0;
    }
    //����������ұ�
    if(LostNum_RightLine>C_LOSTLINE)   //�ұ߶��ߣ�����������ұ�
    {
        for(int row=MT9V03X_H;row-1>0;row--)  //�������ϼ���ұ߽���
        {
            if(RightLine[row]==MT9V03X_W-1&&RightLine[row-1]!=MT9V03X_W-1)    //���ж��߶���һ�в�����
            {
                //���������ֹ���뻷��������
                for(int column=MT9V03X_W-1;column-1>0;column--) //����ɨ
                {
                    if(BinaryImage[30][column]!=BinaryImage[30][column-1])
                    {
                        Point StarPoint,EndPoint;   //���岹�ߵ������յ�
                        EndPoint.Y=row;             //�յ㸳ֵ
                        EndPoint.X=RightLine[row];
                        StarPoint.Y=MT9V03X_H-1;    //��㸳ֵ
                        StarPoint.X=0;
                        FillingLine('L',StarPoint,EndPoint);    //����
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: �ڶ���ʶ�𻷵���ڣ��Ҳ࣬����ֱ��ͨ��
 ** ��    ��: LeftLine����������
 **           RightLine����������
 ** �� �� ֵ: 0��û��ʶ�𵽻���
 **           1��ʶ�𵽻������ڳ����Ҳ�
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CircleIslandOverBegin_R(int *LeftLine,int *RightLine)
{
    //����������ұ�
    for(int row=MT9V03X_H;row-1>0;row--)  //�������ϼ���ұ߽���
    {
        if(RightLine[row]==MT9V03X_W-1&&RightLine[row-1]!=MT9V03X_W-1)    //���ж��߶���һ�в�����
        {
            //���������ֹ���뻷��������
            for(int column=MT9V03X_W-1;column-1>0;column--) //����ɨ
            {
                if(BinaryImage[30][column]!=BinaryImage[30][column-1])
                {
                    if(row-10>0)
                    {
                        row-=10;
                    }
                    Point StarPoint,EndPoint;   //���岹�ߵ������յ�
                    EndPoint.Y=row;             //�յ㸳ֵ
                    EndPoint.X=RightLine[row];
                    StarPoint.Y=MT9V03X_H-1;    //��㸳ֵ
                    StarPoint.X=MT9V03X_W-1;
                    FillingLine('R',StarPoint,EndPoint);    //����
                    return 1;
                }
            }
        }
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: ʶ�𻷵����ڣ��Ҳ�
 ** ��    ��: InflectionL�����¹յ�
 **           InflectionR�����¹յ�
 ** �� �� ֵ: 0��û��ʶ�𵽻���
 **           1��ʶ�𵽻����������ڳ����Ҳ�
 ** ��    ��: WBN
 ** ע    �⣺�ú�������ʱӦȷ��С�����ڻ�����
 ********************************************************************************************
 */
uint8 CircleIslandEnd_R(Point InflectionL,Point InflectionR)
{
    static uint8 flag=0;
    if(flag==1) //�Ѿ�ʶ�𵽻������ڣ����������жϣ���֤���ߵ�������
    {
        if(InflectionL.X>MT9V03X_W/2&&InflectionL.Y<MT9V03X_H/2)    //��յ���ʵ����
        {
            //������߽��߲���
            for(uint8 row=MT9V03X_H/2;row-1>0;row--)  //����ɨ
            {
                if(BinaryImage[row][MT9V03X_W/2]==IMAGE_WHITE&&BinaryImage[row-1][MT9V03X_W/2]==IMAGE_BLACK)
                {
                    //����
                    Point StartPoint;
                    StartPoint.X=LeftLine[0];    //���Ϊ����е���߽��
                    StartPoint.Y=0;
                    FillinLine_V2('L', InflectionL.Y, row, StartPoint, InflectionL);
                    break;
                }
            }
        }
        else    //��������յ�
        {
            //ͼ�����½ǲ��ߵ������м䶥��
            for(uint8 row=MT9V03X_H-20;row-1>0;row--)  //����ɨ
            {
                if(BinaryImage[row][MT9V03X_W/2]==IMAGE_WHITE&&BinaryImage[row-1][MT9V03X_W/2]==IMAGE_BLACK)
                {
                    //����
                    Point StartPoint,EndPoint;
                    StartPoint.Y=MT9V03X_H-1;
                    StartPoint.X=0;
                    EndPoint.Y=row;
                    EndPoint.X=MT9V03X_W/2;
                    FillingLine('L',StartPoint,EndPoint);
                    break;
                }
            }
        }
        return 1;
    }
    if(LostNum_LeftLine>55&&LostNum_RightLine>55&&fabsf(Bias)<1.5)  //Լ��������ʶ�𵽻�������
    {
        /*У������ʹ�ò�����������ǵķ�������*/
        if(InflectionL.X>MT9V03X_W/2&&InflectionL.Y>MT9V03X_H/2)    //��յ���ʵ����
        {
            //������߽��߲���
            for(uint8 row=MT9V03X_H/2;row-1>0;row--)  //����ɨ
            {
                if(BinaryImage[row][MT9V03X_W/2]==IMAGE_WHITE&&BinaryImage[row-1][MT9V03X_W/2]==IMAGE_BLACK)
                {
                    //����
                    Point StartPoint;
                    StartPoint.X=LeftLine[0];    //���Ϊ����е���߽��
                    StartPoint.Y=0;
                    FillinLine_V2('L', InflectionL.Y, row, StartPoint, InflectionL);
                    break;
                }
            }
        }
        else    //��������յ�
        {
            //ͼ�����½ǲ��ߵ������м䶥��
            for(uint8 row=MT9V03X_H/2;row-1>0;row--)  //����ɨ
            {
                if(BinaryImage[row][MT9V03X_W/2]==IMAGE_WHITE&&BinaryImage[row-1][MT9V03X_W/2]==IMAGE_BLACK)
                {
                    //����
                    Point StartPoint,EndPoint;
                    StartPoint.Y=MT9V03X_H-1;
                    StartPoint.X=0;
                    EndPoint.Y=row;
                    EndPoint.X=MT9V03X_W/2;
                    FillingLine('L',StartPoint,EndPoint);
                    break;
                }
            }
        }
        flag=1; //�Ѿ�ʶ�𵽻�������flag
        return 1;
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: ʶ�𻷵����ڣ�����ֱ�У��Ҳ�
 ** ��    ��: LeftLine����������
 **          RightLine����������
 **          InflectionL�����¹յ�
 **          InflectionR�����¹յ�
 ** �� �� ֵ: 0��û��ʶ��Ŀ��
 **          1��ʶ��Ŀ��
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CircleIslandExit_R(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    if(InflectionR.X!=0&&InflectionR.Y!=0&&InflectionR.Y>85)  //�ж�����һ���Ƿ�����ҹյ������ֱ�������ҳ��ӽӽ�����
    {
        float bias_left=Regression_Slope(119,0,LeftLine);   //�����߽���б��
        if(fabsf(bias_left)<G_LINEBIAS)    //��߽�Ϊֱ��
        {
            for(int row=InflectionR.Y;row-1>0;row--)    //���ҹյ㿪ʼ����ɨ
            {
                if(BinaryImage[row][InflectionR.X]==IMAGE_WHITE&&BinaryImage[row-1][InflectionR.X]==IMAGE_BLACK)
                {
                    uint8 row_f=row;
                    for(;row-1>0;row--) //��������ɨ
                    {
                        if(BinaryImage[row][InflectionR.X]==IMAGE_BLACK&&BinaryImage[row-1][InflectionR.X]==IMAGE_WHITE)
                        {
                            row=(row_f+row)/2;
                            for(uint8 column=InflectionR.X;column>0;column--)   //����ɨ
                            {
                                if(BinaryImage[row][column]==IMAGE_WHITE)
                                {
                                    /*���߲���*/
                                    Point end;
                                    end.Y=row;
                                    end.X=column;
                                    FillingLine('R', InflectionR, end);   //����
                                    return 1;
                                }
                            }
                        }
                    }
                    return 0;
                }
            }
        }
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: �ڶ���ʶ�𻷵����ڣ�����ֱ�У��Ҳ�
 ** ��    ��: LeftLine����������
 **          RightLine����������
 ** �� �� ֵ: 0��û��ʶ��Ŀ��
 **          1��ʶ��Ŀ��
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CircleIslandOverExit_R(int *LeftLine,int *RightLine)
{
    float bias_left=Regression_Slope(119,0,LeftLine);   //�����߽���б��
    if(fabsf(bias_left)<G_LINEBIAS)    //��߽�Ϊֱ��
    {
        for(uint8 row=MT9V03X_H-20,column=MT9V03X_W-20;row-1>0;row--)    //����ɨ
        {
            if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)
            {
                uint8 row_f=row;
                for(;row-1>0;row--) //��������ɨ
                {
                    if(BinaryImage[row][column]==IMAGE_BLACK&&BinaryImage[row-1][column]==IMAGE_WHITE)
                    {
                        row=(row+row_f)/2;
                        for(;column>0;column--)   //����ɨ
                        {
                            if(BinaryImage[row][column]==IMAGE_WHITE)
                            {
                                /*���߲���*/
                                Point start,end;
                                end.Y=row-1;
                                end.X=column;
                                start.Y=MT9V03X_H-1;    //����
                                start.X=MT9V03X_W-1;
                                FillingLine('R', start, end);   //����
                                return 1;
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
 ** ��������: �Ǳ������в����Ҳ�
 ** ��    ��: LeftLine����������
 **          RightLine����������
 ** �� �� ֵ: 0��û��ʶ��Ŀ��
 **          1��ʶ��Ŀ��
 ** ��    ��: WBN
 ** ע    �⣺����·���Ի�����
 ********************************************************************************************
 */
uint8 CircleIslandMid_R(int *LeftLine,int *RightLine)
{
    if(LostNum_RightLine<35)    //��߽�Ϊֱ�����ұ߶���С��
    {
        if(BinaryImage[100][149]==IMAGE_BLACK)    //����λ��Ϊ��
        {
//            for(int cloum=0;cloum<MT9V03X_W-1;cloum++)
//            {
//                lcd_drawpoint(cloum,60,PURPLE);
//            }
            //�������for��ֹ�ڻ�������ʱ����Ϊ�����в�
            for(int row=60;row+1<MT9V03X_H-1;row++) //����ɨ
            {
                if(RightLine[row]==MT9V03X_W-1&&RightLine[row+1]!=MT9V03X_W-1)    //����-������
                {
                    return 0;
                }
            }
            for(int row=80;row-1>0;row--)  //����ɨ
            {
                if(RightLine[row]!=MT9V03X_W-1&&RightLine[row-1]==MT9V03X_W-1)    //������-����
                {
                    for(;row-1>0;row--)    //��������ɨ
                    {
                        if(RightLine[row]==MT9V03X_W-1&&RightLine[row-1]!=MT9V03X_W-1)    //����-������
                        {
                            return 1;
                        }
                    }
                    break;
                }
            }
        }
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: ʶ���Ѿ����뻷��
 ** ��    ��: LeftLine����������
 **          RightLine����������
 ** �� �� ֵ: 0��û��ʶ��Ŀ��
 **          1��ʶ��Ŀ��
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CircleIslandInside_R(void)
{
    if(LostNum_RightLine>70)    //�ұ߽ӽ�ȫ����
    {
        //������þ���ֵ���������
        if(BinaryImage[90][119]==IMAGE_WHITE&&BinaryImage[60][20]==IMAGE_BLACK)
        {
            return 1;
        }
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: ʶ���һ���
 ** ��    ��: LeftLine����������
 **           RightLine����������
 **           InflectionL�����¹յ�
 **           InflectionR�����¹յ�
 ** �� �� ֵ: 0����δ��ɻ���
 **           1����ɻ���
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CircleIslandIdentify_R(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    static uint8 flag,num_1,num_2,flag_begin,flag_last_begin,flag_last2_begin,flag_end,flag_in;
    switch(flag)
    {
        case 0: //��ʱС��δ���ﻷ������ʼ�жϻ������ڲ���·�Σ�������Ҫ����
        {
            if(CircleIslandExit_R(LeftLine, RightLine, InflectionL, InflectionR)==1)    //ʶ�𻷵����ڣ����в���
            {
                if(num_1<100)   //�޷�
                {
                    num_1++;    //ʶ��flag1��֡��++
                }
            }
            if(CircleIslandMid_R(LeftLine, RightLine)==1)    //ʶ�𵽻����в�
            {
                if(num_1>0) //�ڴ�֮ǰ��ʶ�𵽻�������
                {
                    num_1=0;flag=1; //��ת��״̬1
                }
            }
            if(num_1>0)   //û��ʶ�𵽳��ڣ�Ӧ�Ը�ѹ�����ڵ������Ҳ��Ҫ���ߣ����ȼ����
            {
                CircleIslandOverExit_R(LeftLine, RightLine);    //�ڶ���ʶ�𻷵����ڣ�����ֱ��
            }
            break;
        }
        case 1: //��ʱС�����ﻷ���в�����ʼ�жϻ�����ڲ�����뻷��������Ҫ����
        {
            if(CircleIslandBegin_R(LeftLine, RightLine)==1) //ʶ�𵽻������
            {
                if(num_1<100)   //�޷�
                {
                    num_1++;    //ʶ�𵽻�����ڵ�֡��++
                }
                if(flag_in==0)  //�����ظ�����������
                {
                    StartIntegralAngle_Z(20);   //���������Ǹ����뻷
                    flag_in=1;
                }
            }
            if(icm_angle_z_flag==1) //�������ж��뻷
            {
                num_1=0;num_2=0;flag=2; //��ת��״̬2
                break;
            }
            break;
        }
        case 2: //��ʱС���Ѿ��ڻ����У���ʼ�жϻ�������
        {
            if(CircleIslandEnd_R(InflectionL, InflectionR)==1&&flag_end==0)  //��һ�μ�⵽��������
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
        case 3: //��ʱС���Ѿ����������ǻ��ٴν���������ڣ���Ҫ����ֱ��
        {
            flag_begin=CircleIslandOverBegin_R(LeftLine, RightLine);
            if(flag_begin==0&&flag_last_begin==0&&flag_last2_begin==1)   //���ϴ�ʶ�𵽻�����ڶ������ζ�û��ʶ�𵽻������
            {
                flag=4;
                return 1;   //�˳�״̬��
            }
            flag_last2_begin=flag_last_begin;   //�������ϴε�״̬
            flag_last_begin=flag_begin;         //������һ�ε�״̬
            break;
        }
        default:break;
    }
    return 0;
}

