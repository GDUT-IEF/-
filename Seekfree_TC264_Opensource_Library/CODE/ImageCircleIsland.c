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
#include "Motor.h"

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
    uint8 row=MT9V03X_H-1,column=0,flag_1=0;;
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
    uint8 start_row=row;//��¼��������Y���꣨������Bias��
    //Ѱ�Ҳ����յ�
    for(row=MT9V03X_H/2,column=MT9V03X_W/5;row-1>0;row--)    //����ɨ���������֮һ
    {
        if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)  //��-��
        {
            //�����·�Ѱ�ҹȵ�
            for(;column+1<MT9V03X_W-1;column++)   //��ָ���ƶ����ײ����Ҷ�
            {
                if(BinaryImage[row][column+1]==IMAGE_BLACK)
                {
                    break;
                }
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
    //������߽�
    StarPoint.X=column;
    FillingLine('L', StarPoint, EndPoint);
    //���������Bias����ֹ����Ԫ�ظ���
    if(row>bias_endline)        //�����յ����ǰհ�յ�
    {
        bias_endline=row;
    }
    if(start_row<bias_startline)//����������ǰհ���
    {
        bias_startline=start_row;
    }
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
uint8 CircleIslandOverBegin_L(int *LeftLine)
{
    //�ж��Ƿ���ȫ���������������������·������ںڶ�
    if(LostNum_LeftLine<50) //����������
    {
        uint8 flag=0;
        if(BinaryImage[MT9V03X_H-5][4]==IMAGE_BLACK)    //���½�Ϊ��
        {
            for(uint8 row=MT9V03X_H-5,column=4;row-2>0;row--)   //����ɨ
            {
                if(BinaryImage[row][column]==IMAGE_BLACK&&BinaryImage[row-1][column]==IMAGE_WHITE&&BinaryImage[row-2][column]==IMAGE_WHITE)    //��-��-�ף�����żȻ���ֵİ׵����У�
                {
                    for(;row-1>0;row--) //��������ɨ
                    {
                        if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)    //��-��
                        {
                            flag=1; //���ںڶ�
                            break;
                        }
                    }
                    break;
                }
            }
        }
        if(flag==0)
        {
            return 0;
        }
    }
    //��ʼ����
    Point StarPoint,EndPoint;
    uint8 row=MT9V03X_H-1,column=0,flag_1=0;
    //Ѱ�Ҳ������
    StarPoint.Y=row;    //��㣺��߽粻���ߴ�or���½�
    StarPoint.X=LeftLine[row];
    //Ѱ�Ҳ����յ�
    for(row=MT9V03X_H/2,column=MT9V03X_W/5;row-1>0;row--)    //����ɨ���������֮һ
    {
        if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)  //��-��
        {
            //�����·�Ѱ�ҹȵ�
            for(;column+1<MT9V03X_W-1;column++)   //��ָ���ƶ����ײ����Ҷ�
            {
                if(BinaryImage[row][column+1]==IMAGE_BLACK)
                {
                    break;
                }
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
    //���Ʋ����յ�
    if(row<MT9V03X_H/4||row>MT9V03X_H/2)
    {
        return 0;
    }
    else if(column>MT9V03X_W/2)
    {
        return 0;
    }
    EndPoint.Y=row;     //�յ㣺�ȵ�
    EndPoint.X=column;
    //������ֱ��
    FillingLine('L', StarPoint, EndPoint);

    return 1;
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
uint8 CircleIslandEnd_L(uint8 status)
{
    static uint8 flag=0;    //��֤���ߵ������ԣ������ڵ�һ���ж�
    //��������
    if(status==1)
    {
        flag=0;
        return 0;
    }

    if(LostNum_LeftLine<90||flag==1)  //����Լ������������������
    {
        Point StarPoint,EndPoint;
        uint8 row=MT9V03X_H-2,column=MT9V03X_W-2,flag_1=0,flag_2=0;
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
        if(flag_2==1)   //�����ҹյ�
        {
            //�����Ϸ�Ѱ�ҹյ�
            for(;column-1>0;column--)   //��ָ���ƶ����ײ������
            {
                if(BinaryImage[row][column-1]==IMAGE_WHITE)
                {
                    break;
                }
            }
            while(column-1>0&&row-1>0)  //����
            {
                if(BinaryImage[row][column-1]==IMAGE_BLACK) //���
                {
                    column--;   //����
                    flag_1=1;
                }
                if(BinaryImage[row-1][column]==IMAGE_BLACK&&flag_1==0) //�Ϻڣ����ڹյ����߽��߽ϻ������������ƺ����ƣ�
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
        }
        else            //�������ҹյ�
        {
            row=MT9V03X_H-2;
        }
        StarPoint.Y=row;    //��㣺�ҹյ�or���½�
        StarPoint.X=column;
        uint8 start_row=row;
//        //Ѱ�Ҳ����յ�
//        column=1;           //�յ�X����ȡ��߽磨���ڻ�������û����ʮ�ֻػ�һ����ֱ��ѹ�����⣬����������Լ򵥴���
//        for(;row-1>0;row--) //����ɨ
//        {
//            if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)  //��-��
//            {
//                EndPoint.Y=row;     //�յ㣺�ϱ߽��
//                EndPoint.X=column;
//                break;
//            }
//        }
        //Ѱ�Ҳ����յ�V2
        column=1;
        if(BinaryImage[MT9V03X_H-2][1]==IMAGE_BLACK)   //���´��ںڶ�
        {
            row=MT9V03X_H-2;   //��������Ϊ���½�
            for(;column+1<MT9V03X_W-1;column++) //����ɨ
            {
                if(BinaryImage[row][column]==IMAGE_BLACK&&BinaryImage[row][column+1]==IMAGE_WHITE)  //��-��
                {
                    break;  //���������յ��X����
                }
            }
        }
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
        //���������Bias����ֹ����Ԫ�ظ���
        if(row>bias_endline)        //�����յ����ǰհ�յ�
        {
            bias_endline=row;
        }
        if(start_row<bias_startline)//����������ǰհ���
        {
            bias_startline=start_row;
        }
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
uint8 CircleIslandExit_L(Point InflectionL,uint8 status)
{
    static uint8 flag=0;    //��������flag
    //��������
    if(status==1)
    {
        flag=0;
        return 0;
    }

    if((InflectionL.X!=0&&InflectionL.Y>50)||flag==1)  //����Լ������or������������״̬
    {
        //��ֹ��ͨ�������
        if(flag==0) //����������״̬
        {
            uint8 flag_exit=0;
            //�жϹյ��Ϸ��Ƿ��кڶ�
            for(uint8 row=InflectionL.Y,column=InflectionL.X;row-1>0;row--)  //�յ㴦������ɨ
            {
                if(BinaryImage[row-1][column]==IMAGE_WHITE&&BinaryImage[row][column]==IMAGE_BLACK)  //��-�ڣ��ڶ��±߽磩
                {
                    for(;row-1>0;row--)
                    {
                        if(BinaryImage[row-1][column]==IMAGE_BLACK&&BinaryImage[row][column]==IMAGE_WHITE)  //��-�ף��ڶ��ϱ߽磩
                        {
                            for(;row-1>0;row--)
                            {
                                if(BinaryImage[row-1][column]==IMAGE_WHITE&&BinaryImage[row][column]==IMAGE_BLACK)  //��-�ڣ��ڶ��⣩
                                {
                                    //�жϺڶ��Ҳ��Ƿ���ֱ��
                                    float slope_right=Regression_Slope(InflectionL.Y, row, RightLine);
                                    lcd_showfloat(0, 0, slope_right, 1, 2);
                                    if(fabs(slope_right)<1)
                                    {
                                        flag_exit=1;    //ȷ�ϻ���
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
            if(flag_exit==0)    //û��ȷ��
            {
                return 0;
            }
        }

        uint8 row=MT9V03X_H-5,column=4,flag_1=0;
        Point StarPoint,EndPoint;
        //Ѱ�Ҳ������
        if(BinaryImage[row][column]==IMAGE_BLACK)   //���½�Ϊ�ڣ�������յ㣩
        {
            for(;column+1<MT9V03X_W-1;column++)   //��ָ���ƶ����ײ����Ҷ�
            {
                if(BinaryImage[row][column+1]==IMAGE_WHITE)
                {
                    break;
                }
            }
            while(column+1<MT9V03X_W-1&&row-1>0)  //�����Ϸ�Ѱ��
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
            StarPoint.Y=row;    //��㣺��յ�
            StarPoint.X=column;
        }
        else
        {
            StarPoint.Y=row;    //��㣺���½�
            StarPoint.X=column;
            column=MT9V03X_W/4; //ΪѰ���Ϸ��ڶ�������X����Ϊ���ķ�֮һ��
        }

        //Ѱ�Ҳ����յ�
        for(;row-1>0;row--)    //����ɨ
        {
            if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)  //��-��
            {
                uint8 row_f=row;
                for(;row-1>0;row--) //��������ɨ
                {
                    if(BinaryImage[row][column]==IMAGE_BLACK&&BinaryImage[row-1][column]==IMAGE_WHITE)  //��-��
                    {
                        uint8 row_s=row;
                        for(;row-1>0;row--) //��������ɨ
                        {
                            if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)  //��-��
                            {
                                row=(row_f+row_s)/2;
                                for(;column+1<MT9V03X_W;column++)   //����ɨ
                                {
                                    if(BinaryImage[row][column]==IMAGE_BLACK&&BinaryImage[row][column+1]==IMAGE_WHITE)  //��-��
                                    {
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
        //������ֱ��
        EndPoint.Y=row; //�յ㣺�ڶ��ұ߽��
        EndPoint.X=column;
        FillingLine('L', StarPoint, EndPoint);
        flag=1;
        return 1;
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
uint8 CircleIslandMid_L(void)
{
    uint8 flag=0;  //�����в�Լ������
    if(BinaryImage[MT9V03X_H-5][4]==IMAGE_BLACK||BinaryImage[MT9V03X_H-3][2]==IMAGE_BLACK)    //�������������Ϊ��
    {
        flag=1; //����Լ������
        //����ĳ����ֹ��Exit�����У�������Բ����ֱ�ǽ��紦�ĺڿ飬������һ���ڶ�
        uint8 column=2,row=MT9V03X_H-3,flag_1=0;    //Ѱ�Һڶ�����X����
        for(;column+1>MT9V03X_W-1;column++)   //��ָ���ƶ����ײ����Ҷ�
        {
            if(BinaryImage[row][column+1]==IMAGE_WHITE)
            {
                break;
            }
        }
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
        //����ķ���ֻ�������󻷵���ǰ��û������Բ�����ţ��������һ�������������
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
//        //�����ǲο��һ����ķ���д����һ�ֿ��Ա���Զ�����ŵķ���������ʦ�������������ſ���������һ�֣�����Ҫ��ϲ�࣬��Ϊ��Exit���ܿ�������
//        for(;column-1>0;column--) //����ɨ���ڶ����е�
//        {
//            if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row][column-1]==IMAGE_WHITE)    //�ȵ��Ҳ�Ϊ�ף�������Exit��Բ����ֱ���ļн�Ϊ�ڶ�
//            {
//                flag=0;
//                break;
//            }
//        }
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
//        //����ĳ����ֹ��Exit�����У���Բ����ֱ�����紦����Ϊ�ڶ������滹��һ�������ĺڶ�����ʵ��û�кܱ�Ҫ����Ϊ�����Լ���Ѿ����ϣ�
//        if(flag==1) //�������������������������Լ��
//        {
//            for(;row-1>0;row--) //�ڶ��е㴦����ɨ����߽�
//            {
//                if(BinaryImage[row][1]==IMAGE_BLACK&&BinaryImage[row-1][1]==IMAGE_WHITE)    //��-��
//                {
//                    for(;row-1>0;row--) //��������ɨ
//                    {
//                        if(BinaryImage[row][1]==IMAGE_WHITE&&BinaryImage[row-1][1]==IMAGE_BLACK)    //��-��
//                        {
//                            for(;row-1>0;row--) //��������ɨ
//                            {
//                                if(BinaryImage[row][1]==IMAGE_BLACK&&BinaryImage[row-1][1]==IMAGE_WHITE)    //��-��
//                                {
//                                    for(;row-1>0;row--) //��������ɨ
//                                    {
//                                        if(BinaryImage[row][1]==IMAGE_WHITE&&BinaryImage[row-1][1]==IMAGE_BLACK)    //��-��
//                                        {
//                                            flag=0; //������Լ������
//                                            break;
//                                        }
//                                    }
//                                    break;
//                                }
//                            }
//                            break;
//                        }
//                    }
//                    break;
//                }
//            }
//        }
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
uint8 CircleIslandIdentify_L(int *LeftLine,Point InflectionL)
{
    static uint8 flag,flag_exit,flag_in,flag_begin,flag_last_begin,flag_last2_begin,flag_end;
    switch(flag)
    {
        case 0: //��ʱС��δ���ﻷ������ʼ�жϻ������ڲ���·�Σ�������Ҫ����
        {
            if(flag_exit==1&&encoder_dis_flag==1)        //֮ǰ��ʶ�𵽻�������
            {
                if(CircleIslandMid_L()==1)   //ʶ�𵽻����в�
                {
                    flag_exit=0;flag=1; //��ת��״̬1
                    break;
                }
            }
            if(CircleIslandExit_L(InflectionL,0)==1&&flag_exit==0)  //ʶ�𵽻���Exit��Ϊ���������в���������
            {
                EncoderDistance(1, 0.3, 0, 0);  //������ࣺ0.3m
                flag_exit=1;
            }
            break;
        }
        case 1: //��ʱС�����ﻷ���в�����ʼ�жϻ�����ڲ�����뻷��������Ҫ����
        {
            if(CircleIslandBegin_L()==1&&flag_in==0) //ʶ�𵽻������
            {
                StartIntegralAngle_Z(30);   //���������Ǹ����뻷
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
            if(flag_in==1&&icm_angle_z_flag==1) //������ʶ���Ѿ��뻷
            {
                flag_in=0;flag=3;   //��ת��״̬3
                break;
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
            if(CircleIslandEnd_L(0)==1&&flag_end==0)  //��һ�μ�⵽��������
            {
                StartIntegralAngle_Z(60);   //��������
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
            flag_begin=CircleIslandOverBegin_L(LeftLine);     //ʶ�𻷵���ڲ��ߺ���
            if(flag_begin==0&&flag_last_begin==0&&flag_last2_begin==1)   //���ϴ�ʶ�𵽻�����ڶ������ζ�û��ʶ�𵽻������
            {
                flag_begin=0;flag_last_begin=0;flag_last2_begin=0;flag=0;     //��ת��״̬0���ȴ���������
                //��������
                CircleIslandExit_L(InflectionL, 1);
                CircleIslandEnd_L(1);
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
uint8 CircleIslandBegin_R(void)
{
    Point StarPoint,EndPoint;
    uint8 row=MT9V03X_H-1,column=MT9V03X_W-1,flag_1=0;
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
    for(row=3*(MT9V03X_H/4),column=4*(MT9V03X_W/5);row-1>0;row--)    //����ɨ���������֮һ��
    {
        if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)  //��-��
        {
            //�����·�Ѱ�ҹȵ�
            for(;column-1>0;column--)   //��ָ���ƶ����ײ������
            {
                if(BinaryImage[row][column-1]==IMAGE_BLACK)
                {
                    break;
                }
            }
            while(column-1>0&&row+1<MT9V03X_H-1)  //����
            {
                if(BinaryImage[row][column-1]==IMAGE_BLACK) //���
                {
                    column--;   //����
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
    //���������Bias����ֹ����Ԫ�ظ���
    if(row>bias_endline)        //�����յ����ǰհ�յ�
    {
        bias_endline=row;
    }
    if(start_row<bias_startline)//����������ǰհ���
    {
        bias_startline=start_row;
    }
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
uint8 CircleIslandOverBegin_R(int *RightLine)
{
    //�ж��Ƿ���ȫ���������������������·������ںڶ�
    if(LostNum_RightLine<50) //����������
    {
        uint8 flag=0;
        if(BinaryImage[MT9V03X_H-5][MT9V03X_W-5]==IMAGE_BLACK)    //���½�Ϊ��
        {
            for(uint8 row=MT9V03X_H-5,column=MT9V03X_W-5;row-2>0;row--)   //����ɨ
            {
                if(BinaryImage[row][column]==IMAGE_BLACK&&BinaryImage[row-1][column]==IMAGE_WHITE&&BinaryImage[row-2][column]==IMAGE_WHITE)    //��-��-�ף�����żȻ���ֵİ׵����У�
                {
                    for(;row-1>0;row--) //��������ɨ
                    {
                        if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)    //��-��
                        {
                            flag=1; //���ںڶ�
                            break;
                        }
                    }
                    break;
                }
            }
        }
        if(flag==0)
        {
            return 0;
        }
    }
    //��ʼ����
    Point StarPoint,EndPoint;
    uint8 row=MT9V03X_H-1,column=MT9V03X_W-1,flag_1=0;
    //Ѱ�Ҳ������
    StarPoint.Y=row;    //��㣺�ұ߽粻���ߴ�or���½�
    StarPoint.X=RightLine[row];
    //Ѱ�Ҳ����յ�
    for(row=3*(MT9V03X_H/4),column=4*(MT9V03X_W/5);row-1>0;row--)    //����ɨ���������֮һ��
    {
        if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)  //��-��
        {
            //�����·�Ѱ�ҹȵ�
            for(;column-1>0;column--)   //��ָ���ƶ����ײ������
            {
                if(BinaryImage[row][column-1]==IMAGE_BLACK)
                {
                    break;
                }
            }
            while(column-1>0&&row+1<MT9V03X_H-1)  //����
            {
                if(BinaryImage[row][column-1]==IMAGE_BLACK) //���
                {
                    column--;   //����
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
    //���Ʋ����յ�
    if(row<MT9V03X_H/4||row>MT9V03X_H/2)
    {
        return 0;
    }
    else if(column<MT9V03X_W/2)
    {
        return 0;
    }
    EndPoint.Y=row;     //�յ㣺�ȵ�
    EndPoint.X=column;
    //������ֱ��
    FillingLine('R', StarPoint, EndPoint);

    return 1;
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
uint8 CircleIslandEnd_R(uint8 status)
{
    static uint8 flag=0;    //��֤���ߵ������ԣ������ڵ�һ���ж�
    //��������
    if(status==1)
    {
        flag=0;
        return 0;
    }

    if(LostNum_RightLine<90||flag==1)  //����Լ������������������
    {
        Point StarPoint,EndPoint;
        uint8 row=MT9V03X_H-2,column=1,flag_1=0,flag_2=0;
        //Ѱ�Ҳ������
        if(BinaryImage[row][column]==IMAGE_BLACK)   //���½�Ϊ�ڣ����ܴ�����յ㣩
        {
            flag_2=1;
        }
        else
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
        if(flag_2==1)   //������յ�
        {
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
                if(BinaryImage[row-1][column]==IMAGE_BLACK&&flag_1==0) //�Ϻڣ����ڹյ���ұ߽��߽ϻ������������ƺ����ƣ�
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
        }
        else            //��������յ�
        {
            row=MT9V03X_H-2;
        }
        StarPoint.Y=row;    //��㣺��յ�or���½�
        StarPoint.X=column;
        uint8 start_row=row;
//        //Ѱ�Ҳ����յ�
//        column=MT9V03X_W-2;           //�յ�X����ȡ�ұ߽磨���ڻ�������û����ʮ�ֻػ�һ����ֱ��ѹ�����⣬����������Լ򵥴���
//        for(;row-1>0;row--) //����ɨ
//        {
//            if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)  //��-��
//            {
//                EndPoint.Y=row;     //�յ㣺�ϱ߽��
//                EndPoint.X=column;
//                break;
//            }
//        }
        //Ѱ�Ҳ����յ�V2
        column=MT9V03X_W-2;
        if(BinaryImage[MT9V03X_H-2][MT9V03X_W-2]==IMAGE_BLACK)   //���´��ںڶ�
        {
            row=MT9V03X_H-2;   //��������Ϊ���½�
            for(;column-1>0;column--) //����ɨ
            {
                if(BinaryImage[row][column]==IMAGE_BLACK&&BinaryImage[row][column-1]==IMAGE_WHITE)  //��-��
                {
                    break;  //���������յ��X����
                }
            }
        }
        for(;row-1>0;row--) //����ɨ
        {
            if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)  //��-��
            {
                break;
            }
        }
        EndPoint.Y=row;     //�յ㣺�ϱ߽��
        EndPoint.X=column;
        //��������ת����
        FillingLine('L', StarPoint, EndPoint);
        flag=1; //�������߱�־
        //���������Bias����ֹ����Ԫ�ظ���
        if(row>bias_endline)        //�����յ����ǰհ�յ�
        {
            bias_endline=row;
        }
        if(start_row<bias_startline)//����������ǰհ���
        {
            bias_startline=start_row;
        }
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
uint8 CircleIslandExit_R(Point InflectionR,uint8 status)
{
    static uint8 flag=0;    //��������flag
    //��������
    if(status==1)
    {
        flag=0;
        return 0;
    }

    if((InflectionR.X!=0&&InflectionR.Y>50)||flag==1)  //����Լ������or������������״̬
    {
        //��ֹ��ͨ�������
        if(flag==0) //����������״̬
        {
            uint8 flag_exit=0;
            //�жϹյ��Ϸ��Ƿ��кڶ�
            for(uint8 row=InflectionR.Y,column=InflectionR.X;row-1>0;row--)  //�յ㴦������ɨ
            {
                if(BinaryImage[row-1][column]==IMAGE_WHITE&&BinaryImage[row][column]==IMAGE_BLACK)  //��-�ڣ��ڶ��±߽磩
                {
                    for(;row-1>0;row--)
                    {
                        if(BinaryImage[row-1][column]==IMAGE_BLACK&&BinaryImage[row][column]==IMAGE_WHITE)  //��-�ף��ڶ��ϱ߽磩
                        {
                            for(;row-1>0;row--)
                            {
                                if(BinaryImage[row-1][column]==IMAGE_WHITE&&BinaryImage[row][column]==IMAGE_BLACK)  //��-�ڣ��ڶ��⣩
                                {
                                    //�жϺڶ��Ҳ��Ƿ���ֱ��
                                    float slope_left=Regression_Slope(InflectionR.Y, row, LeftLine);
                                    if(fabs(slope_left)<1)
                                    {
                                        flag_exit=1;    //ȷ�ϻ���
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
            if(flag_exit==0)    //û��ȷ��
            {
                return 0;
            }
        }
        uint8 row=MT9V03X_H-5,column=MT9V03X_W-5,flag_1=0;
        Point StarPoint,EndPoint;
        //Ѱ�Ҳ������
        if(BinaryImage[row][column]==IMAGE_BLACK)   //���½�Ϊ�ڣ������ҹյ㣩
        {
            for(;column-1>0;column--)   //��ָ���ƶ����ײ������
            {
                if(BinaryImage[row][column-1]==IMAGE_WHITE)
                {
                    break;
                }
            }
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
            StarPoint.Y=row;    //��㣺�ҹյ�
            StarPoint.X=column;
        }
        else
        {
            StarPoint.Y=row;    //��㣺���½�
            StarPoint.X=column;
            column=3*(MT9V03X_W/4); //ΪѰ���Ϸ��ڶ�������X����Ϊ���ķ�֮һ��
        }
        //Ѱ�Ҳ����յ�
        for(;row-1>0;row--)    //����ɨ��Ѱ�Һڶ��±߽�
        {
            if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)  //��-��
            {
                uint8 row_f=row;    //�ڶ��±߽�
                for(;row-1>0;row--) //��������ɨ��Ѱ�Һڶ��ϱ߽�
                {
                    if(BinaryImage[row][column]==IMAGE_BLACK&&BinaryImage[row-1][column]==IMAGE_WHITE)  //��-��
                    {
                        uint8 row_s=row;    //�ڶ��ϱ߽�
                        for(;row-1>0;row--) //��������ɨ���Ժڶ�������֤
                        {
                            if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)  //��-��
                            {
                                row=(row_f+row_s)/2;    //�ڶ��е�Y����
                                for(;column-1>0;column--)   //����ɨ��Ѱ�Һڶ���߽�
                                {
                                    if(BinaryImage[row][column]==IMAGE_BLACK&&BinaryImage[row][column-1]==IMAGE_WHITE)  //��-��
                                    {
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
        //������ֱ��
        EndPoint.Y=row; //�յ㣺�ڶ���߽��
        EndPoint.X=column;
        FillingLine('R', StarPoint, EndPoint);
        flag=1;
        return 1;
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
uint8 CircleIslandMid_R(void)
{
    uint8 flag=0;  //�����в�Լ������
    if(BinaryImage[MT9V03X_H-5][MT9V03X_W-5]==IMAGE_BLACK||BinaryImage[MT9V03X_H-3][MT9V03X_W-3]==IMAGE_BLACK)    //�������������Ϊ��
    {
        flag=1; //����Լ������
        //����ĳ����ֹ��Exit�����У�������Բ����ֱ�ǽ��紦�ĺڿ飬������һ���ڶ���Ѱ�Һڶ�
        uint8 column=MT9V03X_W-3,row=MT9V03X_H-3,flag_1=0;
        for(;column-1<0;column--)   //��ָ���ƶ����ײ������
        {
            if(BinaryImage[row][column-1]==IMAGE_WHITE)
            {
                break;
            }
        }
        while(column-1>0&&row-1>0)
        {
            if(BinaryImage[row][column-1]==IMAGE_BLACK)    //���
            {
                column--;   //����
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
        //����Ϊ�˷�ֹԶ��ʮ�ֻ��ĸ��ţ����󻷵���ͬ����������һ���жϷ���
        for(;column+1<MT9V03X_W-1;column++) //����ɨ���ڶ����е�
        {
            if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row][column+1]==IMAGE_WHITE)    //�ȵ��Ҳ�Ϊ�ף�������Exit��Բ����ֱ���ļн�Ϊ�ڶ�
            {
                flag=0;
                break;
            }
        }
    }
    else    //�ж��Ƿ����㳵�ӿ����������ұ���һ���ӽ����м�ģ���С�ĺڶ���
    {
        uint8 row=MT9V03X_H-1,column=MT9V03X_W-1;
        for(;row-1>0;row--)  //����ɨ���ұ߽�
        {
            if(BinaryImage[row][MT9V03X_W-2]==IMAGE_WHITE&&BinaryImage[row-1][MT9V03X_W-2]==IMAGE_BLACK)    //��-�ڣ��ڶ��±߽磩
            {
                uint8 row_low=row;  //��¼�ڶ��±߽�
                for(;row-1>0;row--)   //��������ɨ
                {
                    if(BinaryImage[row][MT9V03X_W-2]==IMAGE_BLACK&&BinaryImage[row-1][MT9V03X_W-2]==IMAGE_WHITE)    //��-�ף��ڶ��ϱ߽磩
                    {
                        row=(row+row_low)/2;    //������ڶ��е�Y����
                        if(row>50&&row<70)      //�ڶ�λ���м�λ��
                        {
                            for(;column-1>0;column--)   //����ɨ���ڶ��е�
                            {
                                if(BinaryImage[row][column]==IMAGE_BLACK&&BinaryImage[row][column-1]==IMAGE_WHITE)  //��-�ף��ڶ���߽磩
                                {
                                    if(column>3*(MT9V03X_W/4))  //�ڶ���߽�λ��ͼ���Ҳ�
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
//        //����Ϊ�˷�ֹԶ��ʮ�ֻ��ĸ��ţ����󻷵���ͬ����������һ���жϷ���
//        for(;column+1<MT9V03X_W-1;column++) //����ɨ���ڶ����е�
//        {
//            if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row][column+1]==IMAGE_WHITE)    //�ȵ��Ҳ�Ϊ�ף�������Exit��Բ����ֱ���ļн�Ϊ�ڶ�
//            {
//                flag=0;
//                break;
//            }
//        }
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
uint8 CircleIslandIdentify_R(int *RightLine,Point InflectionR)
{
    static uint8 flag,flag_exit,flag_in,flag_begin,flag_last_begin,flag_last2_begin,flag_end;
    switch(flag)
    {
        case 0: //��ʱС��δ���ﻷ������ʼ�жϻ������ڲ���·�Σ�������Ҫ����
        {
            if(flag_exit==1&&encoder_dis_flag==1)   //��ʶ�𵽻��������Ҳ�����
            {
                if(CircleIslandMid_R()==1)   //ʶ�𵽻����в�
                {
                    flag_exit=0;flag=1; //��ת��״̬1
                    break;
                }
            }
            if(CircleIslandExit_R(InflectionR,0)==1&&flag_exit==0)  //ʶ�𵽻���Exit��Ϊ���������в���������
            {
                EncoderDistance(1, 0.3, 0, 0);  //������ࣺ0.3m
                flag_exit=1;
            }
            break;
        }
        case 1: //��ʱС�����ﻷ���в�����ʼ�жϻ�����ڲ�����뻷��������Ҫ����
        {
            if(CircleIslandBegin_R()==1&&flag_in==0) //ʶ�𵽻������
            {
                StartIntegralAngle_Z(30);   //���������Ǹ����뻷
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
            if(flag_in==1&&icm_angle_z_flag==1) //������ʶ���Ѿ��뻷
            {
                flag_in=0;flag=3;   //��ת��״̬3
                break;
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
            if(CircleIslandEnd_R(0)==1&&flag_end==0)  //��һ�μ�⵽��������
            {
                StartIntegralAngle_Z(60);   //��������
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
            flag_begin=CircleIslandOverBegin_R(RightLine);     //ʶ�𻷵���ڲ��ߺ���
            if(flag_begin==0&&flag_last_begin==0&&flag_last2_begin==1)   //���ϴ�ʶ�𵽻�����ڶ������ζ�û��ʶ�𵽻������
            {
                flag_begin=0;flag_last_begin=0;flag_last2_begin=0;flag=0;     //��ת��״̬0���ȴ���������
                //��������
                CircleIslandExit_R(InflectionR, 1);
                CircleIslandEnd_R(1);
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
