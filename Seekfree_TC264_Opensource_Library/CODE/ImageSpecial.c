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

/*
 *******************************************************************************************
 ** ��������: ʶ��������
 ** ��    ��: *LeftLine��  ��������
 **           *RightLine����������
 **           InflectionL�����¹յ�
 **           InflectionR�����¹յ�
 ** �� �� ֵ: 0��û��ʶ��������
 **           1��ʶ���������ҳ����ڳ����
 **           2��ʶ���������ҳ����ڳ��Ҳ�
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 GarageIdentify(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    //������С�����
    if(InflectionL.X!=0&&InflectionL.Y!=0)  //��յ����
    {
        float bias_right=Regression_Slope(119,0,RightLine);   //����ұ߽���б��
        if(fabsf(bias_right)<G_LINEBIAS)    //�ұ߽�Ϊֱ��
        {
            int row=0;          //�̶���ɨ�������
            int zebra_num=0;    //�����߱�־������
            if(InflectionL.X-G_HIGH<0)  //��ֹ����Խ��
            {
                return 0;   //�������Խ����ֻ����ĳ�ֲ�֪������
            }
            row=InflectionL.X-G_HIGH;
            for(int column=RightLine[row];column-1>0;column--)    //�̶��У����ҵ�����ɨ��
            {
                if(BinaryImage[row][column]!=BinaryImage[row][column-1])    //�õ�����һ���㲻ͬ��ɫ //���ںڰ������
                {
                    zebra_num++;    //�����߱�־+1
                }
                if(zebra_num>G_ZEBRA_NUM)   //�����߱�־������������ֵ
                {
                    return 1;       //���س��������
                }
            }
        }
    }
    //������С���Ҳ�
    if(InflectionR.X!=0&&InflectionR.Y!=0)  //�ҹյ���ڣ��������ұߣ�
    {
        float bias_left=Regression_Slope(119,0,LeftLine);   //����ұ߽���б��
        if(fabsf(bias_left)<G_LINEBIAS)    //�ұ߽�Ϊֱ��
        {
            int row=0;          //�̶���ɨ�������
            int zebra_num=0;    //�����߱�־������
            if(InflectionR.X-G_HIGH<0)  //��ֹ����Խ��
            {
                return 0;   //�������Խ����ֻ����ĳ�ֲ�֪������
            }
            row=InflectionR.X-G_HIGH;
            for(int column=LeftLine[row];column-1>0;column--)    //�̶��У����ҵ�����ɨ��
            {
                if(BinaryImage[row][column]!=BinaryImage[row][column-1])    //�õ�����һ���㲻ͬ��ɫ //���ںڰ������
                {
                    zebra_num++;    //�����߱�־+1
                }
                if(zebra_num>G_ZEBRA_NUM)   //�����߱�־������������ֵ
                {
                    return 2;       //���س������ұ�
                }
            }
        }
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: ʶ�𻷵����
 ** ��    ��: LeftLine����������
 **           RightLine����������
 ** �� �� ֵ: 0��û��ʶ�𵽻���
 **           1��ʶ�𵽻������ڳ������
 **           2��ʶ�𵽻������ڳ����Ҳ�
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 CircleIslandBegin(int *LeftLine,int *RightLine)
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
                for(int column=0;column+1<MT9V03X_W-1;column++)
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
 ** ��������: ʶ�𻷵�����
 ** ��    ��: InflectionL�����¹յ�
 **           InflectionR�����¹յ�
 ** �� �� ֵ: 0��û��ʶ�𵽻���
 **           1��ʶ�𵽻����������ڳ������
 **           2��ʶ�𵽻����������ڳ����Ҳ�
 ** ��    ��: WBN
 ** ע    �⣺�ú�������ʱӦȷ��С�����ڻ�����
 ********************************************************************************************
 */
uint8 CircleIslandEnd()
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
            Bias=100;
            systick_delay_ms(STM0,300);
            return 1;
        }
    }
    return 0;
}

/*���������������жϻ����������̵�����״̬ת�ƺ���*/
//�ж�����һ�Ƿ��������������1������������0
uint8 CircleIsFlag_1(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    if(InflectionR.X!=0&&InflectionR.Y!=0)     //��ֹ��������
    {
        return 0;
    }
    if(InflectionL.X!=0&&InflectionL.Y!=0)  //�ж�����һ���Ƿ������յ����Ҳ�ֱ��
    {
        float bias_right=Regression_Slope(119,0,RightLine);   //����ұ߽���б��
        if(fabsf(bias_right)<G_LINEBIAS)    //�ұ߽�Ϊֱ��
        {
            for(int row=InflectionL.Y;row-1>0;row--)    //����յ㿪ʼ������ɨ
            {
                if(BinaryImage[row][InflectionL.X]==IMAGE_WHITE&&BinaryImage[row-1][InflectionL.X]==IMAGE_BLACK)
                {
                    for(int column=InflectionL.X;column<MT9V03X_W-1;column++)   //������ɨ
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
//�ж��������Ƿ��������������1������������0
uint8 CircleIsFlag_2(int *LeftLine,int *RightLine)
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
                    for(;row-1>0;row--)    //����ɨ
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
//�ж��������Ƿ��������������1������������0
uint8 CircleIsFlag_3(int *LeftLine,int *RightLine)
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
 ** ��    ��: WBN
 ** ע    �⣺����ֻ�л�����С����ߵ����
 ********************************************************************************************
 */
uint8 CircleIslandIdentify(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    //ʹ��switchʵ�ּ򵥵�״̬������
    switch(circle_island_flag)
    {
        case 0: //��ʱС��δ���ﻷ������ʼ�жϻ������ڲ���·�Σ�������Ҫ����
        {
            gpio_set(P21_4, 1);
            //������num_1��������ȷ������ת��״̬һ��ʱ��ʶ�𵽻����в����ڴ�֮ǰ��10֡ͼƬ����һ֡ʶ���˻�������
            if(CircleIsFlag_1(LeftLine, RightLine, InflectionL, InflectionR)==1)    //ʶ�𻷵����ڣ����в���
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
            if(CircleIsFlag_2(LeftLine, RightLine)==1)    //ʶ�𵽻����в�������״̬ת��
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
            gpio_set(P21_4, 1);
            circle_island_num_1++;
            if(circle_island_num_1>8) //ͨ��֡��ǿ�й���״̬һ
            {
                circle_island_flag=0;
                circle_island_num_1=0;
                circle_island_num_2=0;
            }
            if(CircleIslandBegin(LeftLine, RightLine)==1)   //ʶ�𵽻�����ڣ����в���
            {
                circle_island_num_2++;
                circle_island_num_1=0;  //ʶ�𵽻�������num1
            }
            if(CircleIsFlag_3(LeftLine, RightLine)==1)      //ʶ���Ѿ����뻷��
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
            gpio_set(P21_4, 0);
            mt9v03x_finish_flag = 0;//��ͼ��ʹ����Ϻ���������־λ�����򲻻Ὺʼ�ɼ���һ��ͼ��
            while(CircleIslandEnd()==0)  //ʶ�𵽻�����������ѭ��
            {
                if(mt9v03x_finish_flag)
                {
                    ImageBinary();                                  //ͼ���ֵ��
//                    lcd_displayimage032(BinaryImage[0],MT9V03X_W,MT9V03X_H);    //���Ͷ�ֵ�����ͼ��LCD
                    GetImagBasic(LeftLine,CentreLine,RightLine);    //����ɨ��
                    Bias=DifferentBias(110,60,CentreLine);          //����ƫ���ʱ�ڻ�����ȡ����ǰհ
                    mt9v03x_finish_flag = 0;//��ͼ��ʹ����Ϻ���������־λ�����򲻻Ὺʼ�ɼ���һ��ͼ��
                }
            }
            mt9v03x_finish_flag = 0;//��ͼ��ʹ����Ϻ���������־λ�����򲻻Ὺʼ�ɼ���һ��ͼ��
            circle_island_flag=0;   //����״̬
            circle_island_num_1=0;
            circle_island_num_2=0;
            gpio_set(P21_4, 1);
            return 0;
        }
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: ʶ��ʮ�ֻػ�����
 ** ��    ��: LeftLine����������
 **           RightLine����������
 **           InflectionL����յ�
 **           InflectionR���ҹյ�
 ** �� �� ֵ: 0��û��ʶ��ʮ�ֻػ�����
 **           1��ʶ��ʮ�ֻػ�����
 ** ��    ��: WBN
 ** ע    �⣺����Ĺյ���ȷ��������ͼ�����ڹյ���յ�����ݾ�Ϊ0
 ********************************************************************************************
 */
uint8 CrossLoopEnd(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    int row;  //��

    if(InflectionL.X!=0&&InflectionL.Y!=0)  //�յ������
    {
        for(row=InflectionL.Y;row>0;row--)
        {
            if(LeftLine[row]==0&&LeftLine[row-1]!=0)  //���ж��߶���һ�в�����
            {
                return 1;
            }
        }
    }
    if(InflectionR.X!=0&&InflectionR.Y!=0)  //�յ����ұ�
    {
        for(row=InflectionR.Y;row>0;row--)
        {
            if(RightLine[row]==MT9V03X_W-1&&RightLine[row-1]!=MT9V03X_W-1)  //���ж��߶���һ�в�����
            {
                return 1;
            }
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
 ** �� �� ֵ: 0��û��ʶ�𵽻���
 **           1��ʶ������
 ** ��    ��: LJF
 ** ע    �⣺1 . Ŀǰ���������������ʱ��ĺ�������Ϊ����ǰ�涼���и�������Ի���ֳ���б���������ʱ�����ҹյ㲢��һ��������
 **           2.����ǽ�����ĺ�����������ʱ��Ӧ����дһ�������ڽ���������ٿ�����������ж�
 *********************************************************************************************/
uint8 ForkIdentify(int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR)
{
    Point UpInflectionC;
    if(DownInflectionL.X!=0 && DownInflectionR.X!=0 && LeftLine[DownInflectionL.Y-5]!=0 && RightLine[DownInflectionR.Y-5]!=MT9V03X_W-1)//�����ҹյ����,�����ҹյ㲻��̫����ֶ������
    {
        if(abs((DownInflectionL.Y-DownInflectionR.Y))<30)//���������յ������С��30���Ž����ж�
        {
            GetForkUpInflection(DownInflectionL, DownInflectionR, &UpInflectionC);//ȥ�����Ϲյ�
            if(UpInflectionC.Y!=0)//ֱ�ӷ���Y���ɣ��ӿ��ٶȣ���ΪXĬ�Ͼͻḳֵ��
            {
                FillingLine('R',DownInflectionR,UpInflectionC);//��������˾��ڷ���֮ǰ����
                Bias=DifferentBias(DownInflectionR.Y,UpInflectionC.Y,CentreLine);//��Ϊ����������������һ�ξ��룬���´��̫�࣬���Ի��ǰ���ԭ���ķ���
                return 1;//�����յ�����������
            }
        }
        else
            return 0;
    }
    else if(DownInflectionL.X==0 && DownInflectionR.X==0 && LeftLine[MT9V03X_H-10]==0 && RightLine[MT9V03X_H-10]==MT9V03X_W-1)//��������¹յ㲻���ڲ�������һ�γ��־Ͷ��ߵĻ��Ļ�,���Ǿ�ȥ���治�������ϵĹյ�
    {
        Point ImageDownPointL,ImageDownPointR;//�Ի�������½Ǻ����½���Ϊ���Ҳ��ߵĵ�
        ImageDownPointL.X=0,ImageDownPointL.Y=MT9V03X_H-10,ImageDownPointR.X=MT9V03X_W-1,ImageDownPointR.Y=MT9V03X_H-10;
        GetForkUpInflection(ImageDownPointL, ImageDownPointR, &UpInflectionC);
        if(UpInflectionC.Y!=0 && UpInflectionC.Y>40)//ֱ�ӷ���Y���ɣ��ӿ��ٶȣ���ΪXĬ�Ͼͻḳֵ��
        {
            FillingLine('R',ImageDownPointR,UpInflectionC);//��������˾��ڷ���֮ǰ����
            Bias=DifferentBias(ImageDownPointR.Y,UpInflectionC.Y,CentreLine);//�ڴ˴��Ͷ�ƫ����м��㣬�Ϳ��Ա������һ�������߱����ߵ������⣬ͬʱ�ⲿʹ��һ����־����ʶ����֮����һ���򲻽��������Զ����ǰհƫ�����
            return 1;//�����յ�����������
        }
    }
    return 0;
}

/*********************************************************************************
 ** ��������: ���������¹յ���Ѱ��ʮ��·�ڵ������Ϲյ�
 ** ��    ��: Point InflectionL: ��߹յ�
 **           Point InflectionR: �ұ߹յ�
 **           Point *UpInflectionC: ����Ϲյ�
 **           Point *UpInflectionC: �ұ��Ϲյ�
 ** �� �� ֵ: ��
 ** ˵    ��: �˺�������������ʮ��ʱ��һ����������������ʶ����
 ** ��    ��: LJF
 **********************************************************************************/
void GetCrossRoadsUpInflection(int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR,Point *UpInflectionL,Point *UpInflectionR)
{
    int row=0;//��ʼ��
    UpInflectionL->X=DownInflectionL.X+10;UpInflectionL->Y=0;//���Ϲյ�����
    UpInflectionR->X=DownInflectionR.X-10;UpInflectionR->Y=0;//���Ϲյ�����

    for(row=DownInflectionL.Y;row>0;row--)
    {
        //��ͼ��������м��
        if(BinaryImage[row][UpInflectionL->X]==IMAGE_WHITE && BinaryImage[row-1][UpInflectionL->X]==IMAGE_BLACK)  //�ɰ׵�������
        {
            //��¼�Ϲյ�
            UpInflectionL->Y=row-1;
            break;//��¼��֮����˳�ѭ��
        }
    }

    for(row=DownInflectionR.Y;row>0;row--)
    {
        if(BinaryImage[row][UpInflectionR->X]==IMAGE_WHITE && BinaryImage[row-1][UpInflectionR->X]==IMAGE_BLACK)  //�ɰ׵�������
        {
            //��¼�Ϲյ�
            UpInflectionR->Y=row-1;
            break;//��¼��֮����˳�ѭ��
        }
    }
}

/********************************************************************************************
 ** ��������: ʶ��ʮ��·��
 ** ��    ��: �������飺int *LeftLine
 **           �������飺int *RightLine
 **           ���¹յ㣺Point DownInflectionL
 **           ���¹յ㣺Point DownInflectionR
 ** �� �� ֵ: 0������ʮ��·��
 **           1������ʮ��
 **           2����б��ʮ��
 **           3����б��ʮ��
 ** ��    ��: LJF
 ** ע    �⣺��
 *********************************************************************************************/
uint8 CrossRoadsIdentify(int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR)
{
    int row=0;//��ʼ��
    Point UpInflectionL,UpInflectionR;//�����Ϲյ�
    UpInflectionL.X=DownInflectionL.X+10;UpInflectionL.Y=0;//���Ϲյ�����
    UpInflectionR.X=DownInflectionR.X-10;UpInflectionR.Y=0;//���Ϲյ�����
    if(LostNum_LeftLine>40 && LostNum_RightLine>40 && DownInflectionR.X!=0 && DownInflectionL.X!=0 && LeftLine[DownInflectionL.Y-5]==0 && RightLine[DownInflectionR.Y-5]==MT9V03X_W-1 && BinaryImage[50][MT9V03X_W/2]==IMAGE_WHITE)//�������ߴ������ߣ����������¹յ㶼����
    {
        GetCrossRoadsUpInflection(LeftLine, RightLine, DownInflectionL, DownInflectionR, &UpInflectionL, &UpInflectionR);
        FillingLine('L', DownInflectionL, UpInflectionL);
        FillingLine('R', DownInflectionR, UpInflectionR);
        return 1;//����ʮ��
    }
    else if(LostNum_LeftLine>70 && DownInflectionR.X!=0 && LeftLine[DownInflectionR.Y-5]==0)//��߶��߳���һ�룬�����ҹյ�����һ�ζ�Ӧ����߶���
    {
        for(row=DownInflectionR.Y;row>1;row--)//ֱ�����¹յ����ϳ��ҵ��Ϲյ�
        {
            if(BinaryImage[row][UpInflectionR.X]==IMAGE_WHITE && BinaryImage[row-1][UpInflectionR.X]==IMAGE_BLACK)  //�ɰ׵�������
            {
                UpInflectionR.Y=row-1;//��¼�Ϲյ�
                FillingLine('R', DownInflectionR, UpInflectionR);
                break;//��¼��֮����˳�ѭ��
            }
        }
        return 2;//����б��ʮ��
    }
    else if(LostNum_RightLine>70 && DownInflectionL.X!=0 && RightLine[DownInflectionL.Y-5]==MT9V03X_W-1)//�ұ߶��߳���һ�룬������յ�����һ�ζ�Ӧ����߶���
    {
        for(row=DownInflectionL.Y;row>1;row--)
        {
            if(BinaryImage[row][UpInflectionL.X]==IMAGE_WHITE && BinaryImage[row-1][UpInflectionL.X]==IMAGE_BLACK)  //�ɰ׵�������
            {
                UpInflectionL.Y=row-1;//��¼�Ϲյ�
                FillingLine('L', DownInflectionL, UpInflectionL);
                break;//��¼��֮����˳�ѭ��
            }
        }
        return 3;//����б��ʮ��
    }
    else return 0;
}
