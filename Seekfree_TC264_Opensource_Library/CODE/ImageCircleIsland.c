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
#include "ImageTack.h"

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
uint8 CircleIslandBegin_L(void)
{
    Point StarPoint,EndPoint;
    uint8 row=MT9V03X_H-1,column=0;
    //��һ�β��ߣ�
    //Ѱ�Ҳ������
    if(BinaryImage[MT9V03X_H-10][1]==IMAGE_BLACK&&BinaryImage[MT9V03X_H-20][9]==IMAGE_BLACK)    //���½Ǵ��ںڶ�
    {
        for(;row>0;row--)   //����ɨ���ұ߽�
        {
            if(RightLine[row]!=MT9V03X_W-1) //������
            {
                break;
            }
        }
    }
    StarPoint.Y=row;    //��㣺�ұ߽粻���ߴ�or���½�
    StarPoint.X=RightLine[row];
    //Ѱ�Ҳ����յ�
    for(row=MT9V03X_H-1,column=MT9V03X_W/3;row-1>0;row--)    //����ɨ����������֮һ
    {
        if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)  //��-��
        {
            uint8 flag_down=0;  //�߽���λ�ã�1����߽磻2���ұ߽磩
            if(BinaryImage[row][column+1]==IMAGE_BLACK)      //�ұ�Ϊ��
            {
                flag_down=1;    //��߽�
            }
            else if(BinaryImage[row][column-1]==IMAGE_BLACK) //���Ϊ��
            {
                flag_down=2;    //�ұ߽�
            }
            else    //X����û������������ϣ�Ĭ������߽磬�ֶ��Ƚ�
            {
                for(;column<MT9V03X_H-1;column++)   //����ɨ
                {
                    if(BinaryImage[row][column]==IMAGE_BLACK)
                    {
                        flag_down=1;    //��߽�
                        break;
                    }
                }
             }
            switch(flag_down)
            {
                case 1: //��߽磬����Ѱ�ҹȵ�
                {
                    uint8 flag_1=0;
                    while(column+1<MT9V03X_W-1&&row+1<MT9V03X_H-1)
                    {
                        if(BinaryImage[row][column+1]==IMAGE_BLACK) //�Һ�
                        {
                            column++;   //����
                            flag_1=1;
                        }
                        if(BinaryImage[row+1][column]==IMAGE_BLACK)    //�º�
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
                case 2: //�ұ߽磬����Ѱ�ҹȵ�
                {
                    uint8 flag_2=0;
                    while(column-1>0&&row+1<MT9V03X_H-1)
                    {
                        if(BinaryImage[row][column-1]==IMAGE_BLACK) //���
                        {
                            column--;   //����
                            flag_2=1;
                        }
                        if(BinaryImage[row+1][column]==IMAGE_BLACK) //�º�
                        {
                            row++;      //����
                            flag_2=1;
                        }
                        if(flag_2==1)
                        {
                            flag_2=0;
                            continue;
                        }
                        break;
                    }
                    break;
                }
                default:    //����������޷��б�߽��λ��
                {
                    EndPoint.Y=MT9V03X_H/2;     //�յ㣺��Ϊͼ������ĵ�
                    EndPoint.X=MT9V03X_W/2;
                }
            }
            EndPoint.Y=row;     //�յ㣺�ȵ�
            EndPoint.X=column;
            break;
        }
    }
    //����������뻷
    FillingLine('R', StarPoint, EndPoint);
    //�ڶ��β��ߣ�
    //Ѱ�Ҳ������
    StarPoint.Y=row;    //��㣺��һ�β��ߵ��յ�
    StarPoint.X=column;
    //Ѱ�Ҳ����յ�
    for(column=1;row-1>0;row--)
    {
        if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)  //��-��
        {
            break;
        }
    }
    EndPoint.Y=row;     //�յ㣺��߹ȶ�
    EndPoint.X=column;
    //����������뻷
    FillingLine('R', StarPoint, EndPoint);

    return 1;
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
 ********************************************************************************************
 */
uint8 CircleIslandEnd_L(Point InflectionL,Point InflectionR)
{
    static uint8 flag=0;    //��֤���ߵ������ԣ������ڵ�һ���ж�
    if((LostNum_LeftLine<90&&fabsf(Bias)<4)||flag==1)  //����Լ������������������
    {
        Point StarPoint,EndPoint;
        //Ѱ�Ҳ������
        uint8 row=MT9V03X_H-2,column=MT9V03X_W-2,flag_1;
        if(BinaryImage[row][column]==IMAGE_BLACK)   //���½�Ϊ�ڣ����ܴ����ҹյ㣩
        {
            while(column-1>0&&row-1>0)  //�����Ϸ�Ѱ��
            {
                if(BinaryImage[row][column-1]==IMAGE_BLACK) //���
                {
                    column--;   //����
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
            flag_1=1;   //����ҵ����ҹյ㣬Ϊ�������յ��ṩ��������
        }
        StarPoint.Y=row;    //��㣺�ҹյ�or���½�
        StarPoint.X=column;
        //Ѱ�Ҳ����յ�
        column=1;           //�յ�X����ȡ��߽磨���ڻ�������û����ʮ�ֻػ�һ����ֱ��ѹ�����⣬����������Լ򵥴���
        for(;row-1>0;row--) //����ɨ
        {
            if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)  //��-��
            {
                EndPoint.Y=row;     //�յ㣺�ϱ߽��
                EndPoint.X=column;
                break;
            }
        }
        //��������ת����
        FillingLine('R', StarPoint, EndPoint);
        flag=1; //�������߱�־
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
    uint8 flag=0;  //�����в�Լ������
    if(BinaryImage[MT9V03X_H-5][4]==IMAGE_BLACK||BinaryImage[MT9V03X_H-3][2]==IMAGE_BLACK)    //�������������Ϊ��
    {
        flag=1; //����Լ������
        //����ĳ����ֹ��Exit�����У�������Բ����ֱ�ǽ��紦�ĺڿ飬������һ���ڶ�
        uint8 column=2,row=MT9V03X_H-3,flag_1=0;    //Ѱ�Һڶ�����X����
        while(column+1<MT9V03X_W-1&&row-1>0)
        {
            if(BinaryImage[row][column+1]==IMAGE_BLACK)    //�Һ�
            {
                column++;   //����
                flag_1=1;
            }
            if(BinaryImage[row-1][column]==IMAGE_BLACK)    //�Ϻ�
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
        for(;row-1>0;row--)    //����ɨ����߽�
        {
            if(BinaryImage[row][column]==IMAGE_BLACK&&BinaryImage[row-1][column]==IMAGE_WHITE)    //��-��
            {
                for(;row-1>0;row--) //��������ɨ
                {
                    if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)    //��-��
                    {
                        for(;row-1>0;row--) //��������ɨ
                        {
                            if(BinaryImage[row][column]==IMAGE_BLACK&&BinaryImage[row-1][column]==IMAGE_WHITE)    //��-��
                            {
                                flag=0; //������Լ������
                            }
                        }
                    }
                }
                break;
            }
        }
    }
    else    //�ж��Ƿ����㳵�ӿ��ҵ�����������һ���ӽ����м�ģ���С�ĺڶ���
    {
        uint8 row=MT9V03X_H-1;
        for(;row-1>0;row--)  //����ɨ����߽�
        {
            if(BinaryImage[row][1]==IMAGE_WHITE&&BinaryImage[row-1][1]==IMAGE_BLACK)    //��-�ڣ��ڶ��±߽磩
            {
                uint8 row_low=row;  //��¼�ڶ��±߽�
                for(;row-1>0;row--)   //��������ɨ
                {
                    if(BinaryImage[row][1]==IMAGE_BLACK&&BinaryImage[row-1][1]==IMAGE_WHITE)    //��-�ף��ڶ��ϱ߽磩
                    {
                        row=(row+row_low)/2;    //������ڶ��е�Y����
                        if(row>50&&row<70)      //�ڶ�λ���м�λ��
                        {
                            for(uint8 column=0;column+1<MT9V03X_W-1;column++)   //����ɨ���ڶ��е�
                            {
                                if(BinaryImage[row][column]==IMAGE_BLACK&&BinaryImage[row][column+1]==IMAGE_WHITE)  //��-�ף��ڶ��ұ߽磩
                                {
                                    if(column<MT9V03X_W/4)  //�ڶ��ұ߽�λ��ͼ����
                                    {
                                        flag=1; //����Լ������
                                    }
                                    break;
                                }
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }
        //����ĳ����ֹ��Exit�����У���Բ����ֱ�����紦����Ϊ�ڶ������滹��һ�������ĺڶ�
        if(flag==1) //�������������������������Լ��
        {
            for(;row-1>0;row--) //�ڶ��е㴦����ɨ����߽�
            {
                if(BinaryImage[row][1]==IMAGE_BLACK&&BinaryImage[row-1][1]==IMAGE_WHITE)    //��-��
                {
                    for(;row-1>0;row--) //��������ɨ
                    {
                        if(BinaryImage[row][1]==IMAGE_WHITE&&BinaryImage[row-1][1]==IMAGE_BLACK)    //��-��
                        {
                            for(;row-1>0;row--) //��������ɨ
                            {
                                if(BinaryImage[row][1]==IMAGE_BLACK&&BinaryImage[row-1][1]==IMAGE_WHITE)    //��-��
                                {
                                    for(;row-1>0;row--) //��������ɨ
                                    {
                                        if(BinaryImage[row][1]==IMAGE_WHITE&&BinaryImage[row-1][1]==IMAGE_BLACK)    //��-��
                                        {
                                            flag=0; //������Լ������
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
    }
    //����Լ�������µĽ�һ���ж�
    if(flag==1)
    {
        return 1;
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
            if(flag_exit==1)        //֮ǰ��ʶ�𵽻�������
            {
                if(CircleIslandMid_L(LeftLine, RightLine)==1)   //ʶ�𵽻����в�
                {
                    flag_exit=0;flag=1; //��ת��״̬1
                    break;
                }
                CircleIslandOverExit_L(LeftLine, RightLine);    //�ڶ���ʶ�𻷵����ڣ�����ֱ��
            }
            else if(flag_exit==0)    //��δʶ�𵽻�������
            {
                if(CircleIslandExit_L(LeftLine, RightLine, InflectionL, InflectionR)==1)    //ʶ�𵽻������ڲ���·��
                {
                    flag_exit=1;
                }
            }
            break;
        }
        case 1: //��ʱС�����ﻷ���в�����ʼ�жϻ�����ڲ�����뻷��������Ҫ����
        {
            if(CircleIslandBegin_L()==1&&flag_in==0) //ʶ�𵽻������
            {
                StartIntegralAngle_Z(45);   //���������Ǹ����뻷
                flag_in=1;                  //�����ظ�����������
            }
            if(flag_in==1)  //֮ǰ�Ѿ�ʶ�𵽻������
            {
                if(icm_angle_z_flag==1) //������ʶ���Ѿ��뻷
                {
                    flag_in=0;flag=2;   //��ת��״̬2
                    break;
                }
            }
            break;
        }
        case 2: //��ʱС���ڻ��У�����Ѱ��
        {
            if(flag_in==1)
            {
                if(icm_angle_z_flag==1) //������ʶ���Ѿ��뻷
                {
                    flag_in=0;flag=3;   //��ת��״̬3
                    break;
                }
            }
            if(flag_in==0)
            {
                StartIntegralAngle_Z(180);  //���������Ǹ�������
                flag_in=1;                  //�����ظ�����������
            }
            break;
        }
        case 3: //��ʱС���Ѿ��ӽ��������ڣ���ʼ�жϻ�������
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
                    flag_end=0;flag=4; //��ת��״̬4
                    break;
                }
            }
            break;
        }
        case 4: //��ʱС���Ѿ����������ǻ��ٴν���������ڣ���Ҫ����ֱ��
        {
            flag_begin=CircleIslandOverBegin_L(LeftLine, RightLine);     //ʶ�𻷵���ڲ��ߺ���
            if(flag_begin==0&&flag_last_begin==0&&flag_last2_begin==1)   //���ϴ�ʶ�𵽻�����ڶ������ζ�û��ʶ�𵽻������
            {
                flag=5;     //��ת��δ֪״̬��״̬������
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
 ** ע    �⣺������󻷵���������������ʹ�õ�ɨ�߷�ʽ��Ĭ������Ѱ�����ߣ������󻷵���ʹ��������Ҳ��
 ********************************************************************************************
 */
uint8 CircleIslandBegin_R()
{
    Point StarPoint,EndPoint;
    uint8 row=MT9V03X_H-1,column=MT9V03X_W-1;
    //��һ�β��ߣ�
    //Ѱ�Ҳ������
    if(BinaryImage[MT9V03X_H-10][MT9V03X_W-2]==IMAGE_BLACK&&BinaryImage[MT9V03X_H-20][MT9V03X_W-10]==IMAGE_BLACK)    //���½Ǵ��ںڶ�
    {
        for(;row>0;row--)   //����ɨ����߽�
        {
            if(LeftLine[row]!=0) //������
            {
                break;
            }
        }
    }
    StarPoint.Y=row;    //��㣺
    StarPoint.X=LeftLine[row];
    uint8 start_row=row;//��¼��������Y���꣨������Bias��
    //Ѱ�Ҳ����յ�
    for(row=MT9V03X_H-1,column=3*(MT9V03X_W/4);row-1>0;row--)    //����ɨ����������֮һ��
    {
        if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)  //��-��
        {
            uint8 flag_down=0;  //�߽���λ�ã�1����߽磻2���ұ߽磩
            if(BinaryImage[row][column+1]==IMAGE_BLACK)      //�ұ�Ϊ��
            {
                flag_down=1;    //��߽�
            }
            else if(BinaryImage[row][column-1]==IMAGE_BLACK) //���Ϊ��
            {
                flag_down=2;    //�ұ߽�
            }
            else    //X����û������������ϣ�Ĭ��Ϊ�ұ߽磬�ֶ��Ƚ�
            {
                for(;column>0;column--)   //����ɨ
                {
                    if(BinaryImage[row][column]==IMAGE_BLACK)
                    {
                        flag_down=2;    //�ұ߽�
                        break;
                    }
                }
            }
            switch(flag_down)
            {
                case 1: //��߽磬����Ѱ�ҹȵ�
                {
                    uint8 flag_1=0;
                    while(column+1<MT9V03X_W-1&&row+1<MT9V03X_H-1)
                    {
                        if(BinaryImage[row][column+1]==IMAGE_BLACK) //�Һ�
                        {
                            column++;   //����
                            flag_1=1;
                        }
                        if(BinaryImage[row+1][column]==IMAGE_BLACK)    //�º�
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
                case 2: //�ұ߽磬����Ѱ�ҹȵ�
                {
                    uint8 flag_2=0;
                    while(column-1>0&&row+1<MT9V03X_H-1)    //����
                    {
                        if(BinaryImage[row][column-1]==IMAGE_BLACK) //���
                        {
                            column--;   //����
                            flag_2=1;
                        }
                        if(BinaryImage[row+1][column]==IMAGE_BLACK) //�º�
                        {
                            row++;      //����
                            flag_2=1;
                        }
                        if(flag_2==1)
                        {
                            flag_2=0;
                            continue;
                        }
                        break;
                    }
                    break;
                }
                default:    //����������޷��б�߽��λ��
                {
                    EndPoint.Y=MT9V03X_H/2;     //�յ㣺��Ϊͼ������ĵ�
                    EndPoint.X=MT9V03X_W/2;
                }
            }
            EndPoint.Y=row;     //�յ㣺�ȵ�
            EndPoint.X=column;
            break;
        }
    }
    //�������ҹ��뻷
    FillingLine('L', StarPoint, EndPoint);
    //�ڶ��β��ߣ�
    //Ѱ�Ҳ������
    StarPoint.Y=row;    //��㣺����һ�β��ߵ��յ���Ϊ���
    StarPoint.X=column;
    //Ѱ�Ҳ����յ�
    for(column=MT9V03X_W-2;row-1>0;row--)   //����ɨ���������
    {
        if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)  //��-��
        {
            break;
        }
    }
    EndPoint.Y=row;     //�յ㣺�ҹȶ�
    EndPoint.X=column;
    //�������ҹ��뻷
    FillingLine('L', StarPoint, EndPoint);
    //�����ұ߽�
    StarPoint.X=column;
    FillingLine('R', StarPoint, EndPoint);
    //���������Bias
    Bias=DifferentBias(start_row,row,CentreLine);//�����⴦��ʱ��ƫ�����
    CircleIsland_flag=1;
    return 1;
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
    static uint8 flag=1,num_1,num_2,flag_begin,flag_last_begin,flag_last2_begin,flag_end,flag_in;
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
            if(CircleIslandBegin_R()==1) //ʶ�𵽻������
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
                gpio_set(LED_WHITE, 0);
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

