/*
 * ImageSpecial.c
  *  �����ϸ�������Ԫ�ص�ʶ��
  *  ���ļ��ĺ���ֻʵ��ʶ���Ƿ��и�Ԫ�صĳ����Լ����ֵ�λ�ã����ڸ�Ԫ�س����˶��ٴ��Լ���Ӧ��ʩ�����ڸ��ļ���Χ��
 *  Created on: 2022��1��17��
 *      Author: yue
 */

#include "ImageSpecial.h"
#include <math.h>
#include "zf_gpio.h"            //�����õ�LED
#include "Binarization.h"       //��ֵ��֮���ͼ������
#include "SEEKFREE_18TFT.h"
#include "Steer.h"
#include <stdlib.h>             //abs������fabs��math.h
#include "SEEKFREE_MT9V03X.h"
#include "Motor.h"              //ֹͣ���
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
uint8 DealGarageLSpecial()
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
            if(LostNum_RightLine>40 && LostNum_LeftLine<10)
            {
                SobelResult=SobelTest();//��������������
                if(SobelResult>ZebraTresholeR)
                {
                    /*����һ���ұߴ�����⣬д��whileѭ�����ٶ�ͣ���������*/
                    Bias=-15;//�ұߴ���
                    systick_delay_ms(STM0,450);
                    while(1)
                    {
                        diff_speed_kp=0;
                        base_speed=0;
                        MotorSetTarget(0, 0);
                    }
                    return 1;
                }
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
 ** ��������: ʶ�𻷵���ڣ����
 ** ��    ��: LeftLine����������
 **           RightLine����������
 ** �� �� ֵ: 0��û��ʶ�𵽻���
 **           1��ʶ�𵽻������ڳ������
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CircleIslandBegin_L(int *LeftLine,int *RightLine)
{
    if(BinaryImage[115][5]==IMAGE_BLACK)    //��ֹ��ǰ���뻷��
    {
        return 0;
    }
    //������������
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
 ** ��������: ʶ�𻷵����ڣ����
 ** ��    ��: InflectionL�����¹յ�
 **           InflectionR�����¹յ�
 ** �� �� ֵ: 0��û��ʶ�𵽻���
 **           1��ʶ�𵽻����������ڳ������
 ** ��    ��: WBN
 ** ע    �⣺�ú�������ʱӦȷ��С�����ڻ�����
 ********************************************************************************************
 */
uint8 CircleIslandEnd_L()
{
    if(LostNum_LeftLine>110)    //��ֹ��δ������������
    {
        return 0;
    }
    if(LostNum_LeftLine>C_LOSTNUM&&LostNum_RightLine>C_LOSTNUM)  //���ұ߽������
    {
        if(fabsf(Bias)<1.5)
        {
            /*�����ｫ�������������Ҫ��Ҫ����ʱ*/
            Bias=10;
            systick_delay_ms(STM0,300);
            return 1;
        }
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: �жϻ���Flag1�Ƿ���������
 ** ��    ��: LeftLine����������
 **           RightLine����������
 **           InflectionL�����¹յ�
 **           InflectionR�����¹յ�
 ** �� �� ֵ: 0��Flag1������
 **           1��Flag1�����������
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CircleIsFlag_1_L(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    if(InflectionL.X!=0&&InflectionL.Y!=0)  //�ж�����һ���Ƿ������յ����Ҳ�ֱ��
    {
        float bias_right=Regression_Slope(119,0,RightLine);   //����ұ߽���б��
        if(fabsf(bias_right)<G_LINEBIAS)    //�ұ߽�Ϊֱ��
        {
            for(int row=InflectionL.Y;row-1>0;row--)    //����յ㿪ʼ������ɨ
            {
                if(BinaryImage[row][InflectionL.X]==IMAGE_WHITE&&BinaryImage[row-1][InflectionL.X]==IMAGE_BLACK)
                {
                    for(int column=InflectionL.X;column<MT9V03X_W-1;column++)   //����ɨ
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
                }
            }
        }
    }
    return 0;
}
/*
 *******************************************************************************************
 ** ��������: �жϻ���Flag2�Ƿ���������
 ** ��    ��: LeftLine����������
 **           RightLine����������
 ** �� �� ֵ: 0��Flag2������
 **           1��Flag2�����������
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CircleIsFlag_2_L(int *LeftLine,int *RightLine)
{
    if(LostNum_RightLine>55)   //��ֹ����ʮ�����
    {
        return 0;
    }
    float bias_right=Regression_Slope(119,0,RightLine);   //����ұ߽���б��
    if(fabsf(bias_right)<G_LINEBIAS&&LostNum_LeftLine<35)    //�ұ߽�Ϊֱ������߶���С��
    {
        if(BinaryImage[100][10]==IMAGE_BLACK)    //����λ��Ϊ��
        {
            //�������for��ֹ�ڻ�������ʱ����Ϊ�����в�
            for(int row=80;row+1<MT9V03X_H-1;row++) //����ɨ
            {
                if(LeftLine[row]==0&&LeftLine[row+1]!=0)    //����-������
                {
                    return 0;
                }
            }
            for(int row=80;row-1>0;row--)  //����ɨ
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
uint8 CircleIsFlag_3_L(int *LeftLine,int *RightLine)
{
    if(LostNum_LeftLine>100)    //��߽ӽ�ȫ����
    {
        //������þ���ֵ���������
        if(BinaryImage[90][40]==IMAGE_WHITE&&BinaryImage[30][120]==IMAGE_BLACK)
        {
            return 1;
        }
    }
    return 0;
}

uint8 circle_island_flag=0;     //�����ж�������־����    //���ڻ������жϲ���һ֡ͼƬ��������Ҫȫ�ֱ����Ա�����һ֡ͼƬ����Ϣ
uint8 circle_island_num_1=0;    //�����ж������ļ���ֵ    //��ֹ�����Զ�ļ�֡ͼƬ�����γ�����
uint8 circle_island_num_2=0;    //�����ж������ļ���ֵ    //��ֹ�������ӵ����������

/*
 *******************************************************************************************
 ** ��������: ʶ�𻷵�
 ** ��    ��: LeftLine����������
 **           RightLine����������
 **           InflectionL�����¹յ�
 **           InflectionR�����¹յ�
 ** �� �� ֵ: 0����һ��״̬��δ��ʼʶ�𻷵�
 **           1����һ��״̬����ʼʶ�𻷵����
 **           2����һ��״̬���ɹ�������
 **           9��״̬������
 ** ��    ��: WBN
 ** ע    �⣺����ֻ�л�����С����ߵ����
 ********************************************************************************************
 */
uint8 CircleIslandIdentify_L(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    //ʹ��switchʵ�ּ򵥵�״̬������
    switch(circle_island_flag)
    {
        case 0: //��ʱС��δ���ﻷ������ʼ�жϻ������ڲ���·�Σ�������Ҫ����
        {
            //������num_1��������ȷ������ת��״̬һ��ʱ��ʶ�𵽻����в����ڴ�֮ǰ��10֡ͼƬ����һ֡ʶ���˻�������
            if(CircleIsFlag_1_L(LeftLine, RightLine, InflectionL, InflectionR)==1)    //ʶ�𻷵����ڣ����в���
            {
                circle_island_num_1++;  //ʶ�𵽻�����ڱ��+1
                circle_island_num_2=0;  //ʶ�𲻵�������ڱ��=0���Ա�֤else�ж�֡�����ж���������
                if(circle_island_num_1>10)  //num_1�޷�
                {
                    circle_island_num_1=10;
                }
            }
            else    //û��ʶ�𵽻�������
            {
                circle_island_num_2++;  //ʶ�𲻵�������ڱ��+1
                if(circle_island_num_2>=8)  //��������8֡û��ʶ�𵽻������ڣ�����֮ǰʶ�𵽵Ļ������
                {
                    circle_island_num_1=0;
                    circle_island_num_2=8;  //num_2�޷�
                }
            }
            if(CircleIsFlag_2_L(LeftLine, RightLine)==1)    //ʶ�𵽻����в�������״̬ת��
            {
                if(circle_island_num_1>0)  //��֮ǰ�жϵ����������
                {
                    circle_island_flag=1;   //��ת���¸�״̬
                    circle_island_num_1=0;
                    circle_island_num_2=0;
                    return 1;
                }
            }
            return 0;
        }
        case 1: //��ʱС�����ﻷ���в�����ʼ�жϻ�����ڲ�����뻷��������Ҫ����
        {
            circle_island_num_1++;
            if(circle_island_num_1>8) //ͨ��֡��ǿ�й���״̬һ
            {
                circle_island_flag=0;
                circle_island_num_1=0;
                circle_island_num_2=0;
            }
            if(CircleIslandBegin_L(LeftLine, RightLine)==1)   //ʶ�𵽻�����ڣ����в���
            {
                circle_island_num_2++;
                circle_island_num_1=0;  //ʶ�𵽻�������num1
            }
            if(CircleIsFlag_3_L(LeftLine, RightLine)==1)      //ʶ���Ѿ����뻷��
            {
                if(circle_island_num_2>0) //ȷ��ʶ�𵽻������
                {
                    circle_island_flag=2;   //��ת���¸�״̬
                    circle_island_num_1=0;
                    circle_island_num_2=0;
                    return 2;
                }
            }
            return 1;
        }
        case 2: //��ʱС���Ѿ��ڻ����У���ʼ�жϻ�������
        {
            mt9v03x_finish_flag = 0;//��ͼ��ʹ����Ϻ���������־λ�����򲻻Ὺʼ�ɼ���һ��ͼ��
            while(CircleIslandEnd_L()==0)  //ʶ�𵽻�����������ѭ��
            {
                if(mt9v03x_finish_flag)
                {
                    ImageBinary();                                  //ͼ���ֵ��
                    GetImagBasic(LeftLine,CentreLine,RightLine);    //����ɨ��
                    Bias=DifferentBias(110,60,CentreLine);          //����ƫ���ʱ�ڻ�����ȡ����ǰհ
                    mt9v03x_finish_flag = 0;//��ͼ��ʹ����Ϻ���������־λ�����򲻻Ὺʼ�ɼ���һ��ͼ��
                }
            }
            mt9v03x_finish_flag = 0;//��ͼ��ʹ����Ϻ���������־λ�����򲻻Ὺʼ�ɼ���һ��ͼ��
            circle_island_flag=0;   //����״̬
            circle_island_num_1=0;
            circle_island_num_2=0;
            return 9;
        }
    }
    return 0;
}

/*********************************************************************************
 ** ��������: ���������¹յ���Ѱ�������Ϲյ�
 ** ��    ��: Point InflectionL: ��߹յ�
 **           Point InflectionR: �ұ߹յ�
 **           Point *UpInflectionC: �м��Ϲյ�
 ** �� �� ֵ: ��
 ** ˵    ��: ��������������޸ģ�д�����ҹյ����棬���߻�Ҫ��һ��Ԫ�����͵Ĳ����������������Ҳ�ͬ�Ĺյ㣬����ֻ�������
 ** ��    ��: LJF
 **********************************************************************************/
void GetForkUpInflection(Point DownInflectionL,Point DownInflectionR,Point *UpInflectionC)
{
    int starline,i,cloumnL,cloumnR;
    UpInflectionC->X=0;UpInflectionC->Y=0;//�Ϲյ�����
    UpInflectionC->X=(DownInflectionL.X+DownInflectionR.X)/2;//V���Ϲյ��������Ϊ���ҹյ��ֵ����Ҫ�޸ģ���һ������������
    starline=(DownInflectionL.Y+DownInflectionR.Y)/2;//��ʼ��Ϊ���ҹյ��еľ�ֵ
    //���������ҵ��Ǹ�����ĵ㼴Ϊ�Ϲյ�
    for(i=starline;i>1;i--)
    {
        //ͼ��������[��][��]
        if(BinaryImage[i][UpInflectionC->X]==IMAGE_WHITE && BinaryImage[i-1][UpInflectionC->X]==IMAGE_BLACK)
        {
            for(cloumnL=UpInflectionC->X;cloumnL>10;cloumnL--)
            {
                if(BinaryImage[i-1][cloumnL]==IMAGE_WHITE)
                    break;
                if(cloumnL==11)
                    return;//�������˶�û���ҵ��׵ļ����������˳��ж�
            }
            for(cloumnR=UpInflectionC->X;cloumnR<MT9V03X_W-10;cloumnR++)
            {
                if(BinaryImage[i-1][cloumnR]==IMAGE_WHITE)
                    break;
                if(cloumnR==MT9V03X_W-11)
                    return;//�������˶�û���ҵ��׵ļ����������˳��ж�
            }
            UpInflectionC->Y=i;//Y����������
            return;
        }
    }
}

/********************************************************************************************
 ** ��������: ʶ������
 ** ��    ��: int startline:�û���������ʼ��
 **           int endline:�û������Ľ����У���ʾ��ǰ���ε�ʶ�𣬸����ٶȲ�ͬ���е�����
 **           int *LeftLine������
 **           int *RightLine:����
 **           Point *InflectionL:��߹յ�
 **           Point *InflectionR:�ұ߹յ�
 **           Point *InflectionC:�м�յ�
 ** �� �� ֵ:  0��û��ʶ�𵽻���
 **           1����������
 ** ��    ��: LJF
 ** ע    �⣺1 . Ŀǰ���������������ʱ��ĺ�������Ϊ����ǰ�涼���и�������Ի���ֳ���б���������ʱ�����ҹյ㲢��һ��������
 **           2.����ǽ�����ĺ�����������ʱ��Ӧ����дһ�������ڽ���������ٿ�����������ж�
 *********************************************************************************************/
uint8 ForkIdentify(int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR)
{
    Point UpInflectionC;
    if(DownInflectionL.X!=0 && DownInflectionR.X!=0)//�����ҹյ����,�����ҹյ㲻��̫����ֶ������
    {
        if(abs((DownInflectionL.Y-DownInflectionR.Y))<30)//���������յ������С��30���Ž����ж�
        {
            GetForkUpInflection(DownInflectionL, DownInflectionR, &UpInflectionC);//ȥ�����Ϲյ�
            if(UpInflectionC.Y!=0)//ֱ�ӷ���Y���ɣ��ӿ��ٶȣ���ΪXĬ�Ͼͻḳֵ��
            {
                FillingLine('R',DownInflectionR,UpInflectionC);//��������˾��ڷ���֮ǰ����
                Bias=DifferentBias(DownInflectionR.Y,UpInflectionC.Y,CentreLine);//��Ϊ����������������һ�ξ��룬���´��̫�࣬���Ի��ǰ���ԭ���ķ���
                return 1;//�����յ���������������������
            }
        }
        else
            return 0;
    }
    else if(DownInflectionL.X==0 && DownInflectionR.X==0)//��������¹յ㲻���ڲ�������һ�γ��־Ͷ��ߵĻ��Ļ�,���Ǿ�ȥ���治�������ϵĹյ�
    {
        Point ImageDownPointL,ImageDownPointR;//�Ի�������½Ǻ����½���Ϊ���Ҳ��ߵĵ�
        ImageDownPointL.X=0,ImageDownPointL.Y=MT9V03X_H-10,ImageDownPointR.X=MT9V03X_W-1,ImageDownPointR.Y=MT9V03X_H-10;
        GetForkUpInflection(ImageDownPointL, ImageDownPointR, &UpInflectionC);
        if(UpInflectionC.Y!=0 && UpInflectionC.Y>40)//ֱ�ӷ���Y���ɣ��ӿ��ٶȣ���ΪXĬ�Ͼͻḳֵ��
        {
            FillingLine('R',ImageDownPointR,UpInflectionC);//��������˾��ڷ���֮ǰ����
            Bias=DifferentBias(ImageDownPointR.Y,UpInflectionC.Y,CentreLine);//�ڴ˴��Ͷ�ƫ����м��㣬�Ϳ��Ա������һ�������߱����ߵ������⣬ͬʱ�ⲿʹ��һ����־����ʶ����֮����һ���򲻽��������Զ����ǰհƫ�����
            return 1;//�������붪ʧ���ҹյ���һ֡
        }
    }
    //��յ�x[0,70)
    else if(LostNum_RightLine>60 && DownInflectionL.X>=0 && DownInflectionL.Y>70 && LeftLine[DownInflectionL.Y-5]!=0)
    {
        Point ImageDownPointR;//����յ�ԳƵĵ�ȥ���ߺ��ҹյ�
        ImageDownPointR.X=MT9V03X_W-1,ImageDownPointR.Y=DownInflectionL.Y;
        GetForkUpInflection(DownInflectionL, ImageDownPointR, &UpInflectionC);
        if(UpInflectionC.Y!=0)//ֱ�ӷ���Y���ɣ��ӿ��ٶȣ���ΪXĬ�Ͼͻḳֵ��
        {
            FillingLine('R',ImageDownPointR,UpInflectionC);//��������˾��ڷ���֮ǰ����
            Bias=DifferentBias(ImageDownPointR.Y,UpInflectionC.Y,CentreLine);//�ڴ˴��Ͷ�ƫ����м��㣬�Ϳ��Ա������һ�������߱����ߵ������⣬ͬʱ�ⲿʹ��һ����־����ʶ����֮����һ���򲻽��������Զ����ǰհƫ�����
            return 2;//������б������
        }
    }
    //�ҹյ�x[0,70)
    else if(LostNum_LeftLine>=60 && DownInflectionR.X>=0 && DownInflectionR.Y>70 && RightLine[DownInflectionR.Y-5]!=MT9V03X_W-1)
    {
        Point ImageDownPointL;//����յ�ԳƵĵ�ȥ���ߺ��ҹյ�
        ImageDownPointL.X=5,ImageDownPointL.Y=DownInflectionR.Y;
        GetForkUpInflection(ImageDownPointL, DownInflectionR, &UpInflectionC);
        if(UpInflectionC.Y!=0)//ֱ�ӷ���Y���ɣ��ӿ��ٶȣ���ΪXĬ�Ͼͻḳֵ��
        {
            FillingLine('R',DownInflectionR,UpInflectionC);//��������˾��ڷ���֮ǰ����
            Bias=DifferentBias(DownInflectionR.Y,UpInflectionC.Y,CentreLine);//�ڴ˴��Ͷ�ƫ����м��㣬�Ϳ��Ա������һ�������߱����ߵ������⣬ͬʱ�ⲿʹ��һ����־����ʶ����֮����һ���򲻽��������Զ����ǰհƫ�����
            return 3;//������б������
        }
    }
    return 0;
}

/********************************************************************************************
 ** ��������: ����״̬��ת�жϺ���
 ** ��    ��: Point InflectionL�����¹յ�
 **           Point InflectionR�����¹յ�
 ** �� �� ֵ:  0������δ����
 **           1�������ѽ���
 ** ��    ��: LJF
 *********************************************************************************************/
uint8 ForkStatusIdentify(Point DownInflectionL,Point DownInflectionR,uint8 ForkFlag)
{
    static uint8 LastFlag,StatusChange;//����ʶ��������ʳ״̬������������״̬�Ƿ���ת
    uint8 NowFlag=0;
    NowFlag=ForkFlag;
    switch(StatusChange)
    {
        case 0:
        {
            //��ʶ������ʶ�𲻵���������״̬
            if(LastFlag==1 && NowFlag==0)
            {
                StatusChange=1;
            }
            break;
        }
        case 1:
        {
            //��֡�жϲ���������֤ȷʵ�ǽ��뵽������
            if(LastFlag==0 && NowFlag==0)
            {
                StatusChange=2;
            }
            break;
        }
        case 2:
        {
            //��0��1˵��ʶ���������
            if(LastFlag==0 && NowFlag==1)
            {
                StatusChange=3;
            }
            break;
        }
        case 3:
        {
            //��������0˵�����������
            if(LastFlag==0 && NowFlag==0)
            {
                StatusChange=4;
            }
            break;
        }

    }
    LastFlag=NowFlag;//������һ�ε�״̬
    if(StatusChange==4)
    {
        StatusChange=0;//Ϊ�˼���ʹ�øú������Ѿ�̬�ֲ�����������Ϊ0
        return 1;//���������
    }
    else
        return 0;//����û����
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
    for(uint8 row=60;row-1>0;row--)          //��ֹ��Flag1������
    {
        if(BinaryImage[row][150]==IMAGE_BLACK&&BinaryImage[row-1][150]==IMAGE_WHITE) //������
        {
            return 0;
        }
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
 ** ��������: ʶ�𻷵����ڣ��Ҳ�
 ** ��    ��: InflectionL�����¹յ�
 **           InflectionR�����¹յ�
 ** �� �� ֵ: 0��û��ʶ�𵽻���
 **           1��ʶ�𵽻����������ڳ����Ҳ�
 ** ��    ��: WBN
 ** ע    �⣺�ú�������ʱӦȷ��С�����ڻ�����
 ********************************************************************************************
 */
uint8 CircleIslandEnd_R()
{
    if(LostNum_RightLine>110)    //��ֹ��δ������������
    {
        return 0;
    }
    if(BinaryImage[115][155]==IMAGE_BLACK)  //��ֹ�ս�����������
    {
        return 0;
    }
    if(LostNum_LeftLine>C_LOSTNUM&&LostNum_RightLine>C_LOSTNUM)  //���ұ߽������
    {
        if(fabsf(Bias)<1.5)
        {
            /*�����ｫ�������������Ҫ��Ҫ����ʱ*/
            Bias=-10;
            systick_delay_ms(STM0,300);
            return 1;
        }
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: �жϻ���Flag1�Ƿ�������Ҳ�
 ** ��    ��: LeftLine����������
 **           RightLine����������
 **           InflectionL�����¹յ�
 **           InflectionR�����¹յ�
 ** �� �� ֵ: 0��Flag1������
 **           1��Flag1���������Ҳ�
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CircleIsFlag_1_R(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    if(InflectionR.Y<60)    //��ֹ��������ʱ������
    {
        return 0;
    }
    if(InflectionR.X!=0&&InflectionR.Y!=0)  //�ж�����һ���Ƿ�����ҹյ������ֱ��
    {
        float bias_left=Regression_Slope(119,0,LeftLine);   //�����߽���б��
        if(fabsf(bias_left)<G_LINEBIAS)    //��߽�Ϊֱ��
        {
            for(int row=InflectionR.Y;row-1>0;row--)    //���ҹյ㿪ʼ������ɨ
            {
                if(BinaryImage[row][InflectionR.X]==IMAGE_WHITE&&BinaryImage[row-1][InflectionR.X]==IMAGE_BLACK)
                {
                    for(int column=InflectionR.X;column>0;column--)   //����ɨ
                    {
                        if(BinaryImage[row-1][column]==IMAGE_WHITE)
                        {
                            /*���߲���*/
                            Point end;
                            end.Y=row-1;
                            end.X=column;
                            FillingLine('R', InflectionR, end);   //����
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
 ** ��������: �жϻ���Flag2�Ƿ�������Ҳ�
 ** ��    ��: LeftLine����������
 **           RightLine����������
 ** �� �� ֵ: 0��Flag2������
 **           1��Flag2���������Ҳ�
 ** ��    ��: WBN
 ** ע    �⣺����·���Ի�����
 ********************************************************************************************
 */
uint8 CircleIsFlag_2_R(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    float bias_left=Regression_Slope(119,0,LeftLine);   //�����߽���б��
    if(fabsf(bias_left)<G_LINEBIAS&&LostNum_RightLine<35)    //��߽�Ϊֱ�����ұ߶���С��
    {
        if(BinaryImage[100][149]==IMAGE_BLACK)    //����λ��Ϊ��
        {
            //�������for��ֹ�ڻ�������ʱ����Ϊ�����в�
            for(int row=80;row+1<MT9V03X_H-1;row++) //����ɨ
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
 ** ��������: �жϻ���Flag3�Ƿ�������Ҳ�
 ** ��    ��: LeftLine����������
 **           RightLine����������
 ** �� �� ֵ: 0��Flag3������
 **           1��Flag3���������Ҳ�
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CircleIsFlag_3_R(int *LeftLine,int *RightLine)
{
    if(LostNum_RightLine>100)    //�ұ߽ӽ�ȫ����
    {
        //������þ���ֵ���������
        if(BinaryImage[90][119]==IMAGE_WHITE&&BinaryImage[30][39]==IMAGE_BLACK)
        {
            return 1;
        }
    }
    return 0;
}

//�������ұߵı�־����
uint8 circle_island_flag_r=0;     //�����ж�������־����    //���ڻ������жϲ���һ֡ͼƬ��������Ҫȫ�ֱ����Ա�����һ֡ͼƬ����Ϣ
uint8 circle_island_num_1_r=0;    //�����ж������ļ���ֵ    //��ֹ�����Զ�ļ�֡ͼƬ�����γ�����
uint8 circle_island_num_2_r=0;    //�����ж������ļ���ֵ    //��ֹ�������ӵ����������
/*
 *******************************************************************************************
 ** ��������: ʶ�𻷵�
 ** ��    ��: LeftLine����������
 **           RightLine����������
 **           InflectionL�����¹յ�
 **           InflectionR�����¹յ�
 ** �� �� ֵ: 0����һ��״̬��δ��ʼʶ�𻷵�
 **           1����һ��״̬����ʼʶ�𻷵����
 **           2����һ��״̬���ɹ�������
 **           9��״̬������
 ** ��    ��: WBN
 ** ע    �⣺����ֻ�л�����С���ұߵ����
 **           ���ٶȵ���100ʱ����·�λ�������дӶ�����״̬һ������������״̬һʱ�����Flag1��Flag2
 **           ���жϣ��������տ��ܵ����뻷ʧ��
 ********************************************************************************************
 */
uint8 CircleIslandIdentify_R(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    //ʹ��switchʵ�ּ򵥵�״̬������
    switch(circle_island_flag_r)
    {
        case 0: //��ʱС��δ���ﻷ������ʼ�жϻ������ڲ���·�Σ�������Ҫ����
        {
            //������num_1��������ȷ������ת��״̬һ��ʱ��ʶ�𵽻����в����ڴ�֮ǰ��10֡ͼƬ����һ֡ʶ���˻�������
            if(CircleIsFlag_1_R(LeftLine, RightLine, InflectionL, InflectionR)==1)    //ʶ�𻷵����ڣ����в���
            {
                circle_island_num_1_r++;  //ʶ�𵽻�����ڱ��+1
                circle_island_num_2_r=0;  //ʶ�𲻵�������ڱ��=0���Ա�֤else�ж�֡�����ж���������
                if(circle_island_num_1_r>8)  //num_1�޷�
                {
                    circle_island_num_1_r=8;
                }
            }
            else    //û��ʶ�𵽻�������
            {
                circle_island_num_2_r++;  //ʶ�𲻵�������ڱ��+1
                if(circle_island_num_2_r>=20)  //90-20
                {
                    circle_island_num_1_r=0;
                    circle_island_num_2_r=8;  //num_2�޷�
                }
            }
            if(CircleIsFlag_2_R(LeftLine, RightLine, InflectionL, InflectionR)==1)    //ʶ�𵽻����в�������״̬ת��
            {
                if(circle_island_num_1_r>0)  //��֮ǰ�жϵ����������
                {
                    circle_island_flag_r=1;   //��ת���¸�״̬
                    circle_island_num_1_r=0;
                    circle_island_num_2_r=0;
                    return 1;
                }
            }
            return 0;
        }
        case 1: //��ʱС�����ﻷ���в�����ʼ�жϻ�����ڲ�����뻷��������Ҫ����
        {
            circle_island_num_1_r++;
            if(circle_island_num_1_r>25) //ͨ��֡��ǿ�й���״̬һ
            {
                circle_island_flag_r=0;
                circle_island_num_1_r=0;
                circle_island_num_2_r=0;
            }
            if(CircleIslandBegin_R(LeftLine, RightLine)==1)   //ʶ�𵽻�����ڣ����в���
            {
                circle_island_num_2_r++;
                circle_island_num_1_r=0;  //ʶ�𵽻�������num1
            }
            if(CircleIsFlag_3_R(LeftLine, RightLine)==1)      //ʶ���Ѿ����뻷��
            {
                if(circle_island_num_2_r>0) //ȷ��ʶ�𵽻������
                {
                    circle_island_flag_r=2;   //��ת���¸�״̬
                    circle_island_num_1_r=0;
                    circle_island_num_2_r=0;
                    return 2;
                }
            }
            return 1;
        }
        case 2: //��ʱС���Ѿ��ڻ����У���ʼ�жϻ�������
        {
            mt9v03x_finish_flag = 0;//��ͼ��ʹ����Ϻ���������־λ�����򲻻Ὺʼ�ɼ���һ��ͼ��
            while(CircleIslandEnd_R()==0)  //ʶ�𵽻�����������ѭ��
            {
                if(mt9v03x_finish_flag)
                {
                    ImageBinary();                                  //ͼ���ֵ��
                    GetImagBasic(LeftLine,CentreLine,RightLine);    //����ɨ��
                    Bias=DifferentBias(110,60,CentreLine);          //����ƫ���ʱ�ڻ�����ȡ����ǰհ
                    mt9v03x_finish_flag = 0;//��ͼ��ʹ����Ϻ���������־λ�����򲻻Ὺʼ�ɼ���һ��ͼ��
                }
            }
            mt9v03x_finish_flag = 0;//��ͼ��ʹ����Ϻ���������־λ�����򲻻Ὺʼ�ɼ���һ��ͼ��
            circle_island_flag_r=0;   //����״̬
            circle_island_num_1_r=0;
            circle_island_num_2_r=0;
            return 9;
        }
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: ʶ���һ��ʮ�ֻػ�����
 ** ��    ��: ��
 ** �� �� ֵ: 0��û��ʶ��ʮ�ֻػ�����
 **           1��ʶ��ʮ�ֻػ�����
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CrossLoopEnd_F(void)
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
            diff_speed_kp=0.1;
            systick_delay_ms(STM0,500);
            diff_speed_kp=0.05;
            return 1;
        }
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: ʶ��ڶ���ʮ�ֻػ�����
 ** ��    ��: ��
 ** �� �� ֵ: 0��û��ʶ��ʮ�ֻػ�����
 **           1��ʶ��ʮ�ֻػ�����
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CrossLoopEnd_S(void)
{
    //��ֹ�������У���������ĳ����ǽ�����ʮ�ֻػ���·�絲����
    uint8 row_1=0,flag=0;
    for(uint8 row=65;row-1>0;row--)    //�м�����ɨ
    {
        if(BinaryImage[row][MT9V03X_W/2]==IMAGE_WHITE&&BinaryImage[row-1][MT9V03X_W/2]==IMAGE_BLACK)
        {
            for(;row-1>0;row--) //��������ɨ
            {
                if(BinaryImage[row][MT9V03X_W/2]==IMAGE_BLACK&&BinaryImage[row-1][MT9V03X_W/2]==IMAGE_WHITE)
                {
                    if(row_1-row<10)    //Լ�������ڰ������֮��ľ���
                    {
                        flag=1;
                    }
                }
            }
            break;  //�����break������ȥԶ���ĸ���
        }
    }
    if(flag==0)
    {
        return 0;
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
            diff_speed_kp=0.1;
            systick_delay_ms(STM0,500);
            diff_speed_kp=0.05;
            return 1;
        }
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: ʶ���һ��ʮ�ֻػ����
 ** ��    ��: LeftLine����������
 **           RightLine����������
 **           InflectionL�����¹յ�
 **           InflectionR�����¹յ�
 ** �� �� ֵ: 0��û��ʶ��ʮ�ֻػ����
 **           1��ʶ��ʮ�ֻػ����
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CrossLoopBegin_F(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
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
 ** ��������: ʶ��ڶ���ʮ�ֻػ����
 ** ��    ��: LeftLine����������
 **           RightLine����������
 **           InflectionL�����¹յ�
 **           InflectionR�����¹յ�
 ** �� �� ֵ: 0��û��ʶ��ʮ�ֻػ����
 **           1��ʶ��ʮ�ֻػ����
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CrossLoopBegin_S(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
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
 ** ��������: ��ɳ��⣨������
 ** ��    ��: ��
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 ********************************************************************************************
 */
void OutGarage(void)
{
    //������Ҵ���������һ������ʱʵ�ֳ���
    Bias=-10;
    diff_speed_kp=0.1;
    systick_delay_ms(STM0,500);
    diff_speed_kp=0.05;
}
