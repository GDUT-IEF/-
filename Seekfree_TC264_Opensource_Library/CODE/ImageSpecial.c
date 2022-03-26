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

uint8 circle_island_flag=0;   //�����ж�������־����    //���ڻ������жϲ���һ֡ͼƬ��������Ҫȫ�ֱ����Ա�����һ֡ͼƬ����Ϣ
uint8 circle_island_num=0;    //�����ж������ļ���ֵ    //��ֹ�����Զ�ļ�֡ͼƬ�����γ�����
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
    //������������
    if(LostNum_LeftLine>C_LOSTLINE)   //��߶��ߣ�������������
    {
        for(int row=MT9V03X_H;row-1>0;row--)  //�������ϼ����߽���
        {
            if(LeftLine[row]==0&&LeftLine[row-1]!=0&&row>C_INROW)    //���ж��߶���һ�в������Ҹ��в���̫Զ   //C_INROW���ڷ�ֹ���м���������
            {
                float bias_rightline=Regression_Slope(row,20,RightLine);   //����ұ߽��ߵ�б��
                if(fabsf(bias_rightline)<C_LINEBIAS)    //�ұ߽�Ϊֱ��
                {
                    Point StarPoint,EndPoint;   //���岹�ߵ������յ�
                    EndPoint.Y=row;             //�յ㸳ֵ
                    EndPoint.X=LeftLine[row];
                    StarPoint.Y=120;            //��㸳ֵ
                    StarPoint.X=MT9V03X_W-1;
                    //�����ⲿ�ִ����ֹ���л��������Ϊ����
                    int c_left_flag=0;
                    for(;row-1>0;row--)
                    {
                        if(LeftLine[row]==0&&LeftLine[row-1]==0)    //�ֳ��ֶ������
                        {
                            c_left_flag=1;
                        }
                    }
                    if(c_left_flag==0)
                    {
                        FillingLine(LeftLine, CentreLine, RightLine,StarPoint,EndPoint);    //����
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
uint8 CircleIslandEnd(Point InflectionL,Point InflectionR)
{
    if(InflectionL.X!=0&&InflectionL.Y!=0&&InflectionR.X!=0&&InflectionR.Y!=0)  //���ҹյ������
    {
        /*�����ｫ�������������Ҫ��Ҫ����ʱ*/

        return 1;
    }
    return 0;
}

/*���������������жϻ����������̵�����״̬ת�ƺ���*/
//�ж�����һ�Ƿ��������������1������������0
uint8 CircleIsFlag_1(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    if(InflectionL.X!=0&&InflectionL.Y!=0)  //�ж�����һ���Ƿ������յ����Ҳ�ֱ��
    {
        float bias_right=Regression_Slope(119,0,RightLine);   //����ұ߽���б��
        if(fabsf(bias_right)<G_LINEBIAS)    //�ұ߽�Ϊֱ��
        {
            return 1;   //��յ���ֱ�����棬����һ����
        }
    }
    return 0;
}
//�ж��������Ƿ��������������1������������0
uint8 CircleIsFlag_2(int *LeftLine,int *RightLine)
{
    /*Ѱ��Բ���Ĵ��ڣ��������������²������м䶪�ߵ����*/
    for(int row=MT9V03X_H/2;row-1>0;row--)               //����Ļ�м�����ɨ
    {
        if(LeftLine[row]==0&&LeftLine[row-1]!=0)         //��߽�������һ�ж�����һ�в�����
        {
            for(row=MT9V03X_H/2;row+1<MT9V03X_H-1;row++) //����Ļ�м�����ɨ
            {
                if(LeftLine[row]==0&&LeftLine[row+1]!=0) //��߽�������һ�ж�����һ�в�����
                {
                    return 1;   //�ҵ���Բ���������е㣬����������
                }
            }
        }
    }
    return 0;
}
//�ж��������Ƿ��������������1������������0
uint8 CircleIsFlag_3(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    /*�����ҵ���յ㣬Ȼ���ж���յ������Ƿ��кڵ�*/
    if(InflectionL.X!=0&&InflectionL.Y!=0)  //������յ�
    {
        for(int row=InflectionL.Y,column=LeftLine[InflectionL.X];row-1>0;row--)  //�ӹյ�����ɨ��
        {
            if(BinaryImage[row][column]==IMAGE_BLACK&&BinaryImage[row-1][column])
            {
                return 1;
            }
        }
    }
    return 0;
}

/*
 *******************************************************************************************
 ** ��������: ʶ�𻷵�
 ** ��    ��: LeftLine����������
 **           RightLine����������
 **           InflectionL�����¹յ�
 **           InflectionR�����¹յ�
 ** �� �� ֵ: 0��û��ʶ�𵽻���
 **           1��ʶ�𵽻������ڳ������
 **           2��ʶ�𵽻������ڳ����Ҳ�
 ** ��    ��: WBN
 ** ע    �⣺����ֻ�л�����С����ߵ����
 **           �û������жϷ���ʹ��һ��ѭ����������������ֻ��������Ῠ���ڸú�����ֱ��С����������
 **           ����һ��ʱ���У�������ֻ�ж�ʱ���ж��еĺ�����ִ�У�������������һ�����⣺���뻷����
 **           ��ѭ���е�ɨ�߸���Bias���־Ͳ���ִ�У�Ҳ����Bias������£�������Ҫ��������������м�
 **           ��ɨ�߸���Bias�Ĳ���ֱ��������return���������
 **           ����һ�����⣺����flag֮����������ж�û�����������ܳ�������flag�ڲ�ͬ�ط���֤�����
 ********************************************************************************************
 */
uint8 CircleIslandIdentify(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    //ʹ��switchʵ�ּ򵥵�״̬������
    switch(circle_island_flag)
    {
        case 0:
        {
            if(CircleIsFlag_1(LeftLine, RightLine, InflectionL, InflectionR)==1)
            {
                circle_island_flag=1;   //��ת���¸�״̬
            }
            break;
        }
        case 1:
        {
            if(CircleIsFlag_2(LeftLine, RightLine)==1)
            {
                circle_island_flag=2;   //��ת���¸�״̬
                circle_island_num=0;    //���ü���ֵ
            }
            else
            {
                circle_island_num++;
                if(circle_island_num>C_NUM) //����ֵ������ֵ
                {
                    circle_island_flag=0;   //����״̬
                    circle_island_num=0;    //���ü���ֵ
                }
            }
            break;
        }
        case 2:
        {
            if(CircleIsFlag_3(LeftLine, RightLine, InflectionL, InflectionR)==1)
            {
                circle_island_flag=3;   //��ת���¸�ת̬
                circle_island_num=0;    //���ü���ֵ
            }
            else
            {
                circle_island_num++;
                if(circle_island_num>C_NUM)
                {
                    circle_island_flag=0;   //����״̬
                    circle_island_num=0;    //���ü���ֵ

                }
            }
            break;
        }
        case 3:
        {
            while(CircleIslandEnd(InflectionL,InflectionR)==0)  //ʶ�𵽻�����������ѭ��
            {
                ImageBinary();                                  //ͼ���ֵ��
                GetImagBasic(LeftLine,CentreLine,RightLine);    //����ɨ��
                Bias=DifferentBias(100,60,CentreLine);          //����ƫ��
            }
            circle_island_flag=0;   //����״̬
            circle_island_num=0;    //���ü���ֵ
            break;
        }
    }
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
uint8 ForkIdentify(int startline,int endline,int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR)
{
    Point UpInflectionC;
    if(DownInflectionL.X!=0 && DownInflectionR.X!=0 && LeftLine[DownInflectionL.Y-5]!=0 && RightLine[DownInflectionR.Y-5]!=MT9V03X_W-1)//�����ҹյ����,�����ҹյ㲻��̫����ֶ������
    {
        GetForkUpInflection(DownInflectionL, DownInflectionR, &UpInflectionC);//ȥ�����Ϲյ�
        if(UpInflectionC.X!=0 && UpInflectionC.Y!=0)
        {
            FillingLine(LeftLine, CentreLine, RightLine, DownInflectionL,UpInflectionC);//��������˾��ڷ���֮ǰ����
            return 1;//�����յ�����������
        }
    }
    else if(DownInflectionL.X==0 && DownInflectionR.X==0 && LeftLine[MT9V03X_H-20]==0 && RightLine[MT9V03X_H-20]==MT9V03X_W-1)//��������¹յ㲻���ڲ�������һ�γ��־Ͷ��ߵĻ��Ļ�,���Ǿ�ȥ���治�������ϵĹյ�
    {
        Point ImageDownPointL,ImageDownPointR;//�Ի�������½Ǻ����½���Ϊ���Ҳ��ߵĵ�
        ImageDownPointL.X=0,ImageDownPointL.Y=MT9V03X_H-20,ImageDownPointR.X=MT9V03X_W-1,ImageDownPointR.Y=MT9V03X_H-20;
        GetForkUpInflection(ImageDownPointL, ImageDownPointR, &UpInflectionC);
        if(UpInflectionC.X!=0 && UpInflectionC.Y!=0)
        {
            FillingLine(LeftLine, CentreLine, RightLine, ImageDownPointL,UpInflectionC);//��������˾��ڷ���֮ǰ����
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
    if(LostNum_LeftLine>40 && LostNum_RightLine>40 && DownInflectionR.X!=0 && DownInflectionL.X!=0 && LeftLine[DownInflectionL.X-5]==0 && RightLine[DownInflectionR.X-5]==MT9V03X_W-1)//�������ߴ������ߣ����������¹յ㶼����
    {
        GetCrossRoadsUpInflection(LeftLine, RightLine, DownInflectionL, DownInflectionR, &UpInflectionL, &UpInflectionR);
        FillingLine(LeftLine, CentreLine, RightLine, DownInflectionL, UpInflectionL);
        FillingLine(LeftLine, CentreLine, RightLine, DownInflectionR, UpInflectionR);
        return 1;//����ʮ��
    }
    else if(LostNum_LeftLine>60 && DownInflectionR.X!=0 && LeftLine[DownInflectionR.Y-5]==0)//��߶��߳���һ�룬�����ҹյ�����һ�ζ�Ӧ����߶���
    {
        for(row=DownInflectionR.Y;row>1;row--)//ֱ�����¹յ����ϳ��ҵ��Ϲյ�
        {
            if(BinaryImage[row][UpInflectionR.X]==IMAGE_WHITE && BinaryImage[row-1][UpInflectionR.X]==IMAGE_BLACK)  //�ɰ׵�������
            {
                UpInflectionR.Y=row-1;//��¼�Ϲյ�
                FillingLine(LeftLine, CentreLine, RightLine, DownInflectionR, UpInflectionR);
                break;//��¼��֮����˳�ѭ��
            }
        }
        return 2;//����б��ʮ��
    }
    else if(LostNum_RightLine>60 && DownInflectionL.X!=0 && RightLine[DownInflectionL.Y-5]==MT9V03X_W-1)//�ұ߶��߳���һ�룬������յ�����һ�ζ�Ӧ����߶���
    {
        for(row=DownInflectionL.Y;row>1;row--)
        {
            if(BinaryImage[row][UpInflectionL.X]==IMAGE_WHITE && BinaryImage[row-1][UpInflectionL.X]==IMAGE_BLACK)  //�ɰ׵�������
            {
                UpInflectionL.Y=row-1;//��¼�Ϲյ�
                FillingLine(LeftLine, CentreLine, RightLine, DownInflectionL, UpInflectionL);
                break;//��¼��֮����˳�ѭ��
            }
        }
        return 3;//����б��ʮ��
    }
    else return 0;
}
