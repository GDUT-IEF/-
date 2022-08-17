/*
 * Crossroads.c
 * Created on: 2022��5��25��
 * Author: 30516
 * Effect: ���ʮ��·����ص�Դ����
 */

#include "ImageSpecial.h"
#include "ICM20602.h"           //�����ǻ�����ɱ�־�����Լ��������ֺ���
#include "LED.h"                //debug
#include "headfile.h"

#define CROSSROADSIDENTIFYMODE 0    //����ģʽ���Ϲյ�

/********************************************************************************************
 ** ��������: ʶ��ʮ��·��
 ** ��    ��: �������飺int *LeftLine �������飺int *RightLine//ȫ�ֱ���
 **           ���¹յ㣺Point DownInflectionL
 **           ���¹յ㣺Point DownInflectionR
 ** �� �� ֵ: 0������ʮ��·��
 **           1������ʮ��
 **           2����б��ʮ��
 **           3����б��ʮ��
 ** ��    ��: LJF
 ** ע    �⣺��
 *********************************************************************************************/
uint8 CrossRoadsIdentify(Point DownInflectionL,Point DownInflectionR)
{
    int row=0,cloum=0;//��ʼ��
    Point UpInflectionL,UpInflectionR;//�����Ϲյ�
    UpInflectionL.X=0;UpInflectionL.Y=0;//���Ϲյ�����
    UpInflectionR.X=0;UpInflectionR.Y=0;//���Ϲյ�����

    /**********************debug************************/
//    lcd_showint32(0, 0, LostNum_LeftLine, 3);
//    lcd_showint32(0, 1, LostNum_RightLine, 3);
//    lcd_showint32(TFT_X_MAX-50, 0, DownInflectionL.X, 3);
//    lcd_showint32(TFT_X_MAX-50, 1, DownInflectionR.X, 3);
    /***************************************************/

    //�������ߴ������ߣ����������¹յ㶼����,���������ǰ׵�
    if(LostNum_LeftLine>30 && LostNum_RightLine>30 && DownInflectionR.X!=0 && DownInflectionL.X!=0 && BinaryImage[50][MT9V03X_W/2]==IMAGE_WHITE)
    {
        DownInflectionL.Y-=10;DownInflectionR.Y-=10;//����յ����
        //��Ѱʮ���Ϲյ�
        GetRightangleUPInflection('L', DownInflectionL, &UpInflectionL, 10, MT9V03X_W-20);
        GetRightangleUPInflection('R', DownInflectionR, &UpInflectionR, 10, 20);
        if(UpInflectionL.Y!=0 && UpInflectionR.Y!=0)
        {
            DownInflectionL.Y+=10;DownInflectionR.Y+=10;//���߻ָ���
            FillingLine('L', DownInflectionL, UpInflectionL);
            FillingLine('R', DownInflectionR, UpInflectionR);
            return 1;//����ʮ��
        }
    }
    //������һ�ж��ߣ��������ҹյ��Ҳ�������ʮ���е�״̬
    else if(LeftLine[MT9V03X_H-10]==0 && RightLine[MT9V03X_H-10]==MT9V03X_W-1 && DownInflectionL.X==0 && DownInflectionR.X==0)
    {
        Point PointL,PointR;//��ʱ�������¹յ�
        PointL.X=10;PointL.Y=MT9V03X_H;//����һ�����½ǵĵ�
        PointR.X=MT9V03X_W-10;PointR.Y=MT9V03X_H;//����һ�����½ǵĵ�
        //��ʧ�����¹յ��ʱ����ݱ���ȥ���Ϲյ�
        GetRightangleUPInflection('L', PointL, &UpInflectionL, 10, MT9V03X_W-20);
        GetRightangleUPInflection('R', PointR, &UpInflectionR, 10, 20);
        if(UpInflectionL.Y!=0 && UpInflectionR.Y!=0)
        {
            PointL.X=LeftLine[UpInflectionL.Y-7];PointL.Y=UpInflectionL.Y-7;//Ѱ����ȷ�����ϸ����Ϲյ�һ��ĵ�������
            PointR.X=RightLine[UpInflectionR.Y-7];PointR.Y=UpInflectionR.Y-7;//Ѱ����ȷ�����ϸ����Ϲյ�һ��ĵ�������
            FillinLine_V2('L', MT9V03X_H-1, UpInflectionL.Y, UpInflectionL, PointL);
            FillinLine_V2('R', MT9V03X_H-1, UpInflectionR.Y, UpInflectionR, PointR);
            return 1;//����ʮ��
        }
    }
    //��߶��߳���һ��[60,����]���ұ�Ҳ���ڶ���[10,60]���ҹյ���ڣ������ҹյ�����һ�ζ�Ӧ����߶��ߣ������ҹյ㲻��������߸���
    else if(LostNum_LeftLine>60 && LostNum_RightLine>10 && LostNum_RightLine<60 && DownInflectionR.X!=0 && LeftLine[DownInflectionR.Y-5]==0)
    {
        DownInflectionR.Y-=10;//����յ����
        //��Ѱʮ���Ϲյ�
        GetRightangleUPInflection('R', DownInflectionR, &UpInflectionR, 10, 20);
        if(UpInflectionR.Y!=0)
        {
            DownInflectionR.Y+=10;//�ָ��յ��������ڲ���
            FillingLine('R', DownInflectionR, UpInflectionR);
            //���Ϲյ����������һ����
            for(uint8 column=UpInflectionR.X-3;column>3;column--)
            {
                if(BinaryImage[UpInflectionR.Y][column]==IMAGE_WHITE && BinaryImage[UpInflectionR.Y][column-1]==IMAGE_BLACK)
                {
                    UpInflectionL.Y=UpInflectionR.Y;UpInflectionL.X=column-1;
                    break;
                }
            }
            DownInflectionL.Y=MT9V03X_H-5;DownInflectionL.X=5;
//            FillingLine('L', DownInflectionL, UpInflectionL);
            return 2;//����б��ʮ��
        }
    }
    //�ұ߶��߳���һ�룬���Ҳ���ڶ��ߣ���յ���ڣ������ҹյ�����һ�ζ�Ӧ����߶���
    else if(LostNum_RightLine>60 && LostNum_LeftLine>10 && LostNum_LeftLine<60 && DownInflectionL.X!=0 && RightLine[DownInflectionL.Y-5]==MT9V03X_W-1)
    {
        DownInflectionL.Y-=10;//����յ����
        //��Ѱʮ���Ϲյ�
        GetRightangleUPInflection('L', DownInflectionL, &UpInflectionL, 10, MT9V03X_W-20);
        if(UpInflectionL.Y!=0)
        {
            DownInflectionL.Y+=10;//�ָ��յ��������ڲ���
            FillingLine('L', DownInflectionL, UpInflectionL);
            //���Ϲյ����������һ����
            for(uint8 column=UpInflectionL.X+3;column<MT9V03X_W-3;column++)
            {
                if(BinaryImage[UpInflectionL.Y][column]==IMAGE_WHITE && BinaryImage[UpInflectionL.Y][column+1]==IMAGE_BLACK)
                {
                    UpInflectionR.Y=UpInflectionL.Y;UpInflectionR.X=column-1;
                    break;
                }
            }
            DownInflectionR.Y=MT9V03X_H-5;DownInflectionR.X=MT9V03X_W-5;
//            FillingLine('R', DownInflectionR, UpInflectionR);
            return 3;//����б��ʮ��
        }
    }
    return 0;
}
/********************************************************************************************
 ** ��������: ʮ�ֵ�״̬��ת��
 ** ��    ��: Point InflectionL�����¹յ�
 **           Point InflectionR�����¹յ�
 ** �� �� ֵ: 0��ʮ�ֻ�δ����
 **           1��ʮ���ѽ���
 ** ��    ��: LJF
 *********************************************************************************************/
uint8 CrossRoadsStatusIdentify(Point DownInflectionL,Point DownInflectionR)
{
    //ʮ��״̬������������״̬�Ƿ���ת
    static uint8 StatusChange;
    uint8 NowFlag=0;//ʮ��ʶ����ݴ��־����

    NowFlag=CrossRoadsIdentify(DownInflectionL, DownInflectionR);
    switch(StatusChange)
    {
        //����ICMΪ���ж��Ƿ����ʮ��Ԫ��
        case 0:
        {
//            gpio_set(LED_BLUE, 0);
            //���������״̬
            if(NowFlag==1)
            {
//                gpio_set(LED_BLUE, 1);
                StartIntegralAngle_Z(270);//������������Ϊ��״̬��־
                StatusChange=1;//���������ǻ��ֳ���״̬
            }
            break;
        }
        //����״̬
        case 1:
        {
//            gpio_set(LED_GREEN, 0);
            //�����ǻ��ִﵽ��ʮ�ֻ�״̬,֤���Ѿ�ֱ�Ž���ʮ���м䣬���Բ����ٲ���Ҳ�ܳ�ȥ��
            if(icm_angle_z_flag==1)
            {
//                gpio_set(LED_GREEN, 1);
                return 1;
            }
            break;
        }
    }
    return 0;
}
