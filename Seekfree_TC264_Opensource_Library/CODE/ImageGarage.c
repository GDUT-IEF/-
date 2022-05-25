/*
 * Garage.c
 * Created on: 2022��5��25��
 * Author: 30516
 * Effect: ��ų�����ص�Դ����
 */

#include "ImageSpecial.h"
#include "PID.h"

//���������㳬����ֵ�Ĵ���������ĳ��ֵ�Ͳ��ٽ���sobel����
uint8 SobelLCount=0;             //���������
uint8 SobelRCount=0;             //�ұ�������

/********************************************************************************************
 ** ��������: Sobel���Ӽ��������
 ** ��    ��: ��
 ** �� �� ֵ: Sobel��ֵ
 ** ��    ��: ʦ��
 *********************************************************************************************/
int64 SobelTest(void)
{
    int64 Sobel = 0;
    int64 temp = 0;

    for (uint8 i = MT9V03X_H-1-20; i > 50 ; i--)
    {
        for (uint8 j = 40; j < MT9V03X_W-1-40; j++)
        {
            int64 Gx = 0, Gy = 0;
            Gx = (-1*BinaryImage(i-1, j-1) + BinaryImage(i-1, j+1) - 2*BinaryImage(i, j-1)
                  + 2*BinaryImage(i, j+1) - BinaryImage(i+1, j-1) + BinaryImage(i+1, j+1));
            Gy = (-1 * BinaryImage(i-1, j-1) - 2 * BinaryImage(i-1, j) - BinaryImage(i-1, j+1)
                  + BinaryImage(i+1, j+1) + 2 * BinaryImage(i+1, j) + BinaryImage(i+1, j+1));
            temp += FastABS(Gx) + FastABS(Gy);
            Sobel += temp / 255;
            temp = 0;
        }
    }
    return Sobel;
}
/********************************************************************************************
 ** ��������: �󳵿��������������б���
 ** ��    ��: char Direction: ѡ������߳��⻹�Ǽ���ұ߳���L����R
 **           Point InflectionL�����¹յ�
 **           Point InflectionR�����¹յ�
 ** �� �� ֵ:  0��û��ʶ��������
 **           1: �жϳ���
 ** ��    ��: LJF
 *********************************************************************************************/
uint8 DealGarageLSpecial(void)
{
    int row=0,cloumn=0,flag=0,BiasRow=0;
    Point LeftDownPoint,LeftUpPoint;
    LeftDownPoint.X=5,LeftDownPoint.Y=MT9V03X_H-5,LeftUpPoint.X=0,LeftUpPoint.Y=0;
    //�����൱��������������ж����������������Ϊ�׵�
    if(BinaryImage[MT9V03X_H/2+10][5]==IMAGE_WHITE)
    {
        row=MT9V03X_H/2+10;cloumn=5;
        for(;row>0;row--)
        {
            //�����½ǰ׵������ҵ�������
            if(flag==0 && BinaryImage[row][cloumn]==IMAGE_WHITE && BinaryImage[row-1][cloumn]==IMAGE_BLACK)
            {
                BiasRow=row;//����ƫ�����
                flag++;
            }
            //�ټ����ҵ�������
            if(flag==1 && BinaryImage[row][cloumn]==IMAGE_BLACK && BinaryImage[row-1][cloumn]==IMAGE_WHITE)
            {
                LeftUpPoint.Y=row+5;
                break;
            }
        }
        if(row==0)//��������û�ҵ�˵������
            return 0;
        //���������ҵ�������
        for(;cloumn<MT9V03X_W-30;cloumn++)
        {
            if(BinaryImage[LeftUpPoint.Y][cloumn]==IMAGE_BLACK && BinaryImage[LeftUpPoint.Y][cloumn+1]==IMAGE_WHITE)
            {
                LeftUpPoint.X=cloumn;
                break;
            }
        }
        FillingLine('L', LeftDownPoint, LeftUpPoint);
        //�ò�����ȥ��б��ƫ��
        Bias=Regression_Slope(LeftDownPoint.Y, LeftUpPoint.Y, LeftLine);
        //��б���������ƫ����и�����
        if(Bias<=1)
            Bias=Bias*2.5;
        else
            Bias=Bias*2;
        return 1;
    }
    else
        return 0;
}
/********************************************************************************************
 ** ��������: ʶ��������
 ** ��    ��: char Direction: ѡ������߳��⻹�Ǽ���ұ߳���L����R
 **           Point InflectionL�����¹յ�
 **           Point InflectionR�����¹յ�
 ** �� �� ֵ:  0��û��ʶ��������
 **           1��ʶ���������ҳ����ڳ��Ҳ�
 **           2��ʶ���������ҳ����ڳ����
 ** ��    ��: LJF
 *********************************************************************************************/
uint8 GarageIdentify(char Direction,Point InflectionL,Point InflectionR)
{
    int64 SobelResult=0;//����������Ľ��
    Point UpInflection;//�Ϲյ�ı���
    UpInflection.X=0,UpInflection.Y=0;//��ʼ��Ϊ0
    uint8 NoInflectionLFlag=0;//�����յ㲻���ڵı�־���������ڸ����ߵ����¹յ㸳ֵ
    switch(Direction)
    {
        case 'L':
            //��߶���(40,95)���ұ߶���(0,20)�������Ǹ�ת��ĵط���ֻ��һС�Σ���������������
            SobelResult=SobelTest();//��������������
            if(SobelResult>ZebraTresholeL)
            {
                SobelLCount++;
                //����������������е�֡�ж�
                if(DealGarageLSpecial()==1)
                {
                    return 2;
                }

                //�����յ㲻������õ����Ǹ���˵���Ǹ����Ѿ�����ͼ����
                if(InflectionL.X==0)
                {
                    InflectionL.X=3;
                    InflectionL.Y=MT9V03X_H/2+10;
                    NoInflectionLFlag=1;

                }
                //���������ҵ��������ڣ�ȷ���Ϲյ������
                for(int row=InflectionL.Y;row-1>0;row--)
                {
                    if(BinaryImage[row][InflectionL.X]==IMAGE_WHITE && BinaryImage[row-1][InflectionL.X]==IMAGE_BLACK)
                    {
                        UpInflection.Y=row-5;//�õ��������֮����и��Ϲյ�
                        break;//����ѭ��
                    }
                }
                //�������ұ��ҵ��������ף�ȷ���Ϲյ������
                for(int column=InflectionL.X;column<MT9V03X_W;column++)
                {
                    if(BinaryImage[UpInflection.Y][column]==IMAGE_BLACK && BinaryImage[UpInflection.Y][column+1]==IMAGE_WHITE)
                    {
                        UpInflection.X=column+1;//�õ��������֮����и��Ϲյ�
                        break;//����ѭ��
                    }
                }
                if(NoInflectionLFlag==1)
                {
                    InflectionL.Y=MT9V03X_H;InflectionL.X=UpInflection.X-10;
                }
                FillingLine('L', InflectionL, UpInflection);
                Bias=Regression_Slope(InflectionL.Y, UpInflection.Y, LeftLine);
                //��б���������ƫ����и�����
                if(Bias<=1)
                    Bias=Bias*2.25;
                else
                    Bias=Bias*1.75;
                return 2;
            }
            break;
        case 'R':
            //�������Ϊ�˷�ֹ���������п��Լ����ұ߶��ߴ���40С��90��߶���С��10����
            //�ұ߶��ߡ���߲����ߡ������ҹյ��������������
            SobelResult=SobelTest();//��������������
            if(SobelResult>ZebraTresholeR)
            {
                MotorK.P=15;
                MotorK.I=1.5;
                /*����һ���ұߴ�����⣬д��whileѭ�����ٶ�ͣ���������*/
                base_speed=130;
                systick_delay_ms(STM0,15);//��10ms��ֹ�����⣬����ٶ�����ȥ����ȡ��
                Bias=-15;//�ұߴ���
                systick_delay_ms(STM0,150);
                while(1)
                {
//                    Bias=0;
                    base_speed=0;
                }
                return 1;
            }
            break;
        default:break;
    }
    return 0;
}
/********************************************************************************************
 ** ��������: �󳵿��״̬��ת��
 ** ��    ��: LeftLine����������
 **           RightLine����������
 ** �� �� ֵ: 0��û��ʶ�𵽻���
 **           1��ʶ�𵽻������ڳ������
 ** ��    ��: LJF
 *********************************************************************************************/
uint8 GarageLStatusIdentify(Point InflectionL,Point InflectionR,uint8 GarageLFlag)
{
    static uint8 LastFlag,StatusChange,Statusnums;//��һ��ʶ��Ľ����״̬����
    uint8 NowFlag=0;//��ε�ʶ����
    NowFlag=GarageLFlag;
    switch(StatusChange)
    {
        case 0:
        {
            //��û��ʶ���󳵿⵽ʶ���󳵿�
            if(LastFlag==0 && NowFlag==2)
            {
                StatusChange=1;
            }
            break;
        }
        case 1:
        {
            //��ʶ���󳵿⵽ʶ�𲻵��󳵿�
            if(LastFlag==2 && NowFlag==0)
            {
                StatusChange=2;
            }
            break;
        }
        case 2:
        {
            //��ʶ�𲻵��󳵿⵽ʶ�𲻵��󳵿�
            if(LastFlag==0 && NowFlag==0 && Statusnums>1)
            {
                StatusChange=3;
            }
            Statusnums++;//�ӳ�һ֡����ֹ����
            break;
        }
    }
    LastFlag=NowFlag;
    if(StatusChange==3)
    {
        StatusChange=0;
        return 1;
    }
    else
        return 0;
}
/*
 *******************************************************************************************
 ** ��������: ��ɳ��⣨������
 ** ��    ��: ��
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 ********************************************************************************************
 */
void OutGarage(void)
{
//    systick_delay_ms(STM0,50);
    //������Ҵ���������һ������ʱʵ�ֳ���
    Bias=-10;
    systick_delay_ms(STM0,300);
}
