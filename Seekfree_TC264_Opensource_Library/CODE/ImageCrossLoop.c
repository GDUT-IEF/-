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
#include "Motor.h"
#include "LED.h"

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
uint8 CrossLoopBegin_L(Point InflectionL,uint8 status)
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
        //���Ʋ����յ㣺�����Ѿ��ӽ��뻷����Ҫ���ߵ����
        if(row<MT9V03X_H/6)
        {
            return 0;
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
uint8 CrossLoopEnd_L(uint8 status)
{
    static uint8 flag=0;    //��������flag��������һ�β����ж�
    //��������
    if(status==1)
    {
        flag=0;
        return 0;
    }

    if((LostNum_LeftLine<90&&fabsf(Bias)<4)||flag==1) //����Լ������������������
    {
        Point StarPoint,EndPoint;
        uint8 row=MT9V03X_H-2,column=1,flag_1=0,flag_2=0;
        //Ѱ�Ҳ������
        if(BinaryImage[row][column]==IMAGE_BLACK)   //���½�Ϊ��
        {
            flag_2=1;
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
        if(flag_2==1)   //������յ�
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
        }
        else    //��������յ�
        {
            row=MT9V03X_H-2;
        }
        StarPoint.Y=row;    //��㣺��յ�or���½�
        StarPoint.X=column;
        uint8 start_row=row;
        //Ѱ�Ҳ����յ�
        row=MT9V03X_H-2;column=MT9V03X_W-2;flag_1=0;
        if(BinaryImage[row][column]==IMAGE_BLACK)       //���½�Ϊ�ڣ������ҹյ㣩
        {
            for(;column-1>0;column--)   //��ָ���ƶ����ײ������
            {
                if(BinaryImage[row][column-1]==IMAGE_WHITE)
                {
                    break;
                }
            }
            while(column-2>0&&row-2>0)  //�����Ϸ�Ѱ���ҹյ�
            {
                if(BinaryImage[row][column-1]==IMAGE_BLACK||BinaryImage[row][column-2]==IMAGE_BLACK) //���
                {
                    column--;   //����
                    flag_1=1;
                }
                if(BinaryImage[row-1][column]==IMAGE_BLACK||BinaryImage[row-2][column]==IMAGE_BLACK) //�Ϻ�
                {
                    row--;      //����
                    flag_1=1;
                }
                if(flag_1==1)
                {
                    flag_1=0;
                    continue;
                }
                break;  //̽��û���ƶ�
            }
            //ͨ�����Է�����������յ�
            Point point_1,point_2;
            point_1.Y=MT9V03X_H-2;
            point_1.X=RightLine[MT9V03X_H-2];
            point_2.Y=row;
            point_2.X=column;
            for(;row-1>0;row--) //����ɨ���ҹյ�
            {
                if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)   //��-��
                {
                    break;  //�����յ�Y����
                }
            }
            column=SlopeUntie_X(point_1, point_2, row); //�����յ�X����
            if(column<StarPoint.X)
            {
                column=(uint8)StarPoint.X;
            }
        }
        else    //�������ҹյ�
        {
            row=2*(MT9V03X_H/3);//��ֹ�·�С�ڶ��ĸ���
            for(;row-1>0;row--) //����ɨ
            {
                if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)   //��-��
                {
                    break;
                }
            }
        }
        EndPoint.Y=row;     //�յ㣺�ҹյ��Ϸ��߽紦or�ұ߽��Ϸ�
        EndPoint.X=column;
        //��������ת����
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
        flag=1; //�������߱�־
        return 1;
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
uint8 CrossLoopIdentify_L(Point InflectionL)
{
    static uint8 flag,flag_in,flag_end;
    switch(flag)
    {
        case 0: //С��ʶ��ʮ�ֻػ�����ڣ����в���ֱ��
        {
            if(CrossLoopBegin_L(InflectionL,0)==1&&flag_in==0)  //��һ��ʶ�𵽻ػ����
            {
                StartIntegralAngle_Z(20);   //���������Ǹ����뻷
                flag_in=1;                  //�����ظ���������
            }
            if(flag_in==1&&icm_angle_z_flag==1)  //������ʶ���Ѿ��뻷
            {
                flag_in=0;flag=1;   //��ת��״̬1
                break;
            }
            break;
        }
        case 1: //С���ڻ�������Ѱ��
        {
            if(flag_in==1&&icm_angle_z_flag==1) //������ʶ���Ѿ��ӽ�����
            {
                flag_in=0;flag=2;   //��ת��״̬2
                break;
             }
            if(flag_in==0)
            {
                StartIntegralAngle_Z(200);  //���������Ǹ�������
                flag_in=1;                  //�����ظ�����������
            }
            break;
        }
        case 2: //С��ʶ��ʮ�ֻػ��ĳ��ڣ���ת����
        {
            if(CrossLoopEnd_L(0)==1&&flag_end==0)  //��һ�μ�⵽�ػ�����
            {
                StartIntegralAngle_Z(30);   //��������
                flag_end=1;                 //�����ظ���������
            }
            if(flag_end==1&&icm_angle_z_flag==1)  //������ʶ���Ѿ�����
            {
                flag_end=0;flag=0;  //��ת��״̬0���ȴ���������
                //��������
                CrossLoopBegin_L(InflectionL,1);
                CrossLoopEnd_L(1);
                return 1;
            }
            break;
        }
    }
    return 0;
}

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
 ** ע    �⣺�ú����ͻ���Exit������һ���ģ�Ψһ������������Բ��ߵ��յ����������ƣ���Ӧʮ�ֻػ��뻷
 **           ��������������Ѿ�����Ҫ���ߵĻػ���ڴ����ߵ����뻷ʧ��
 **           8.15
 ********************************************************************************************
 */
uint8 CrossLoopBegin_R(Point InflectionR,uint8 status)
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
        //���Ʋ����յ㣺�����Ѿ��ӽ��뻷����Ҫ���ߵ����
        if(row<MT9V03X_H/6)
        {
            return 0;
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
 ** ��������: ʶ����ʮ�ֻػ�����
 ** ��    ��: InflectionL�����¹յ�
 **           InflectionR�����¹յ�
 ** �� �� ֵ: 0��û��ʶ��ʮ�ֻػ�����
 **          1��ʶ��ʮ�ֻػ�����
 ** ��    ��: WBN
 ** ע    �⣺û�жԳ����ڻ���������ʻ����������������������ڻ�������̫�����⣨��ղ���ѹ�ǣ���
 **           ���ǳ������⻷��ʻ����ѹ�Ƿ��ա�Ԥ�ƴ�������ͨ����յ��λ�����жϼ������
 ********************************************************************************************
 */
uint8 CrossLoopEnd_R(uint8 status)
{
    static uint8 flag=0;    //��������flag��������һ�β����ж�
    //��������
    if(status==1)
    {
        flag=0;
        return 0;
    }

    if((LostNum_RightLine<90&&fabsf(Bias)<1.5)||flag==1) //����Լ������������������
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
            //�����Ϸ�Ѱ�ҹȵ�
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
        }
        else    //�������ҹյ�
        {
            row=MT9V03X_H-2;
        }
        StarPoint.Y=row;    //��㣺�ҹյ�or���½�
        StarPoint.X=column;
        uint8 start_row=row;
        //Ѱ�Ҳ����յ�
        row=MT9V03X_H-2;column=1;flag_1=0;
        if(BinaryImage[row][column]==IMAGE_BLACK)       //���½�Ϊ�ڣ�������յ㣩
        {
            for(;column+1<MT9V03X_W-1;column++)   //��ָ���ƶ����ײ����Ҷ�
            {
                if(BinaryImage[row][column+1]==IMAGE_WHITE)
                {
                    break;
                }
            }
            while(column+2<MT9V03X_W-1&&row-2>0)  //�����Ϸ�Ѱ��
            {
                if(BinaryImage[row][column+1]==IMAGE_BLACK||BinaryImage[row][column+2]==IMAGE_BLACK) //���
                {
                    column++;   //����
                    flag_1=1;
                }
                if(BinaryImage[row-1][column]==IMAGE_BLACK||BinaryImage[row-2][column]==IMAGE_BLACK) //�Ϻ�
                {
                    row--;      //����
                    flag_1=1;
                }
                if(flag_1==1)
                {
                    flag_1=0;
                    continue;
                }
                break;  //̽��û���ƶ�
            }
            //ͨ�����Է�����������յ�
            Point point_1,point_2;
            point_1.Y=MT9V03X_H-2;
            point_1.X=LeftLine[MT9V03X_H-2];
            point_2.Y=row;
            point_2.X=column;
            for(;row-1>0;row--) //����ɨ����յ�
            {
                if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)   //��-��
                {
                    break;  //�����յ�Y����
                }
            }
            column=SlopeUntie_X(point_1, point_2, row); //�����յ�X����
            if(column>StarPoint.X)
            {
                column=(uint8)StarPoint.X;
            }
        }
        else    //��������յ�
        {
            row=2*(MT9V03X_H/3);//��ֹ�·�С�ڶ��ĸ���
            for(;row-1>0;row--) //����ɨ
            {
                if(BinaryImage[row][column]==IMAGE_WHITE&&BinaryImage[row-1][column]==IMAGE_BLACK)   //��-��
                {
                    break;
                }
            }
        }
        EndPoint.Y=row;     //�յ㣺��յ��Ϸ��߽紦or��߽��Ϸ�
        EndPoint.X=column;
        //��������ת����
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
        flag=1; //�������߱�־
        return 1;
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
uint8 CrossLoopIdentify_R(Point InflectionR)
{
    static uint8 flag,flag_in,flag_end;
    switch(flag)
    {
        case 0: //С��ʶ��ʮ�ֻػ�����ڣ����в���ֱ��
        {
            if(CrossLoopBegin_R(InflectionR,0)==1&&flag_in==0)  //��һ��ʶ�𵽻ػ����
            {
                StartIntegralAngle_Z(20);   //���������Ǹ����뻷
                flag_in=1;                  //�����ظ���������
            }
            if(flag_in==1&&icm_angle_z_flag==1)  //������ʶ���Ѿ��뻷
            {
                flag_in=0;flag=1;   //��ת��״̬1
                break;
            }
            break;
        }
        case 1: //С���ڻ��У�����Ѱ��
        {
            if(flag_in==1&&icm_angle_z_flag==1) //������ʶ���Ѿ��ӽ�����
            {
                flag_in=0;flag=2;   //��ת��״̬2
                break;
            }
            if(flag_in==0)
            {
                StartIntegralAngle_Z(200);  //���������Ǹ�������
                flag_in=1;                  //�����ظ�����������
            }
            break;
        }
        case 2: //С��ʶ��ʮ�ֻػ��ĳ��ڣ���ת����
        {
            if(CrossLoopEnd_R(0)==1&&flag_end==0)  //��һ�μ�⵽�ػ�����
            {
                StartIntegralAngle_Z(30);   //��������
                flag_end=1;                 //�����ظ���������
            }
            if(flag_end==1&&icm_angle_z_flag==1)  //������ʶ���Ѿ�����
            {
                flag_end=0;flag=0;  //��ת��״̬0���ȴ���������
                //��������
                CrossLoopBegin_R(InflectionR,1);
                CrossLoopEnd_R(1);
                return 1;
            }
            break;
        }
    }
    return 0;
}
