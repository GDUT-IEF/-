/*
 * ImageSpecial.c
  *  �����ϸ�������Ԫ�ص�ʶ��
  *  ���ļ��ĺ���ֻʵ��ʶ���Ƿ��и�Ԫ�صĳ����Լ����ֵ�λ�ã����ڸ�Ԫ�س����˶��ٴ��Լ���Ӧ��ʩ�����ڸ��ļ���Χ��
 *  Created on: 2022��1��17��
 *      Author: yue
 */

#include "ImageSpecial.h"
#include <math.h>
#include "zf_gpio.h"

/*
 *******************************************************************************************
 ** ��������: ʶ��������
 ** ��    ��: *LeftLine��  ��������
 **           *RightLine����������
 ** �� �� ֵ: 0��û��ʶ��������
 **           1��ʶ���������ҳ����ڳ����
 **           2��ʶ���������ҳ����ڳ��Ҳ�
 ** ��    ��: WBN
 ** ע    �⣺1 . Ĭ���ڽ���������ʶ���·�ζ���ֱ�ߣ�����ͷ�Ѿ�����
 **           2.����û��ʵ��ͼ���ο���ֻ���ȼ���һ������״̬������������ǡ�ò�������ͼ��
 ********************************************************************************************
 */
uint8 StartLineFlag(int *LeftLine,int *RightLine)
{
    /*
     ** �������ߵĵط����г��⣬��������ԭ�򣬳����п��ܳ����ڳ�����߻��ұߣ�����Ҳ�ȶ���������������жϲ��ֱ���
     ** �Գ��������Ϊ�����ӹյ㿪ʼ��Ϊɨ�ߵ�ԭ�㣬�������ϣ���������ɨ��
     ** �̶�һ�У���������ɨ���ҵ�һ���ڵ㣬�̶����У���������ɨ����¼�������ֵĺڵ���������ߵĿ�ȣ������ÿ�ȴ�����ֵBLACK_WIDTH����
     ** ��ȷ������һ�����ߡ�����ɨ��һ�У����������㹻��������ĺ��ߣ������趨����ֵBLACK_NUM��������Ϊ��һ�з��ϰ����ߡ����ո����ۻ�
     ** ���������е�ɨ�裬���������������㹻�ࣨ�����趨����ֵBLACK_TIMES��������Ϊ��һ��ͼƬ�д��ڰ����ߣ�����·����������
     **/

    int row,cloum;          //��,��
    int Black_width=0;      //�̶��У�����ɨ���Ǽ�¼ÿ�κڵ�ĸ�������һ�����ߵĿ�ȣ�
    int Black_num=0;        //��¼�к��ߵ���������Ϊ�жϸ����Ƿ�Ϊ�����ߵ�����
    int Black_times=0;      //��¼��������ߵ�����������Ϊ�жϸ�·���Ƿ�Ϊ�����ߵ�����

    Point InflectionL, InflectionR; //�¹յ�
    InflectionL.X=0;
    InflectionL.Y=0;
    InflectionR.X=0;
    InflectionR.Y=0;
    GetDownInflection(0,MT9V03X_H,LeftLine,RightLine,&InflectionL,&InflectionR);    //��ȡ�¹յ�


    if(InflectionL.X!=0&&InflectionL.Y!=0)    //�յ㣨���⣩�����
    {
        for(row=InflectionL.X,cloum=InflectionL.Y;row<MT9V03X_H;row++)        //����յ㿪ʼ�̶��У���������ɨ
        {
            if(BinaryImage[row][cloum]==IMAGE_BLACK)    //�ҵ���һ���ڵ�
            {
                for(;cloum<MT9V03X_W;cloum++)                                 //�̶��У���������ɨ
                {
                    if(BinaryImage[row][cloum]==IMAGE_BLACK)    //ɨ���ڵ�
                    {
                        Black_width++;   //���߿��+1
                    }
                    else                                        //ɨ���׵�
                    {
                        if(Black_width>=S_BLACK_WIDTH)    //�жϺ��߿���Ƿ�������ֵ
                        {
                            Black_num++; //�к�������+1
                        }
                        Black_width=0;   //��һ�ΰ׵��жϺ����ú��߿��
                    }
                    if(Black_num>=S_BLACK_NUM)    //����������ߵ���ֵ����һ�У�
                    {
                        Black_times++;  //��������ߵ�����+1
                        break;
                    }
                }
                Black_num=0;    //��һ�е�ɨ�߽����������к�����
            }
            if(Black_times>=S_BLACK_TIMES)    //�����������·�ε���ֵ
            {
                return 1;
            }
        }
        return 0;
    }

    if(InflectionR.X!=0&&InflectionR.Y!=0)    //�յ㣨���⣩���ұ�
    {
        for(row=InflectionL.X,cloum=InflectionL.Y;row<MT9V03X_H;row++)        //���ҹյ㿪ʼ�̶��У���������ɨ
        {
            if(BinaryImage[row][cloum]==IMAGE_BLACK)    //�ҵ���һ���ڵ�
            {
                for(;cloum>0;cloum--)                                         //�̶��У���������ɨ
                {
                    if(BinaryImage[row][cloum]==IMAGE_BLACK)    //ɨ���ڵ�
                    {
                        Black_width++;   //���߿��+1
                    }
                    else                                        //ɨ���׵�
                    {
                        if(Black_width>=S_BLACK_WIDTH)    //�жϺ��߿���Ƿ�������ֵ
                        {
                            Black_num++; //�к�������+1
                        }
                        Black_width=0;   //��һ�ΰ׵��жϺ����ú��߿��
                    }
                    if(Black_num>=S_BLACK_NUM)    //����������ߵ���ֵ����һ�У�
                    {
                        Black_times++;  //��������ߵ�����+1
                        break;
                    }
                }
                Black_num=0;    //��һ�е�ɨ�߽����������к�����
            }
            if(Black_times>=S_BLACK_TIMES)    //�����������·�ε���ֵ
            {
                return 2;
            }
        }
        return 0;
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
                if(fabs(bias_rightline)<C_LINEBIAS)
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
//                        /*Debug*/
//                        //�����߻�����
//                        for(int i=MT9V03X_H;i>0;i--)
//                        {
//                            lcd_drawpoint(LeftLine[i],i,GREEN);
//                            lcd_drawpoint(CentreLine[i],i,RED);
//                            lcd_drawpoint(RightLine[i],i,BLUE);
//                        }
                        return 1;
                    }
                }
            }
        }
    }
    /*��ʱ�����ǻ������ұߵ����*/
//    //�������ұ�
//    if(LostNum_RightLine>C_LOSTLINE)   //�ұ߶��ߣ�������������
//    {
//        for(int row=MT9V03X_H;row-1>0;row--)  //�������ϼ����߽���
//        {
//            if(RightLine[row]==MT9V03X_W-1&&RightLine[row-1]!=MT9V03X_W-1)    //���ж��߶���һ�в������Ҹ��в���̫Զ   //C_INROW���ڷ�ֹ���м���������
//            {
//                //���������������ԣ���������������ʱ��ǰ��������һ��ֱ�ߣ�����������жϲ�ͬ�ڻ����������ߵ����
////                float bias_leftline=Regression_Slope(row,20,LeftLine);   //�����߽��ߵ�б��
////                lcdz_showfloat(0, 0, bias_leftline, 2, 2);
////                if(fabs(bias_leftline)<C_LINEBIAS)
////                {
//                    Point StarPoint,EndPoint;   //���岹�ߵ������յ�
//                    EndPoint.Y=row;             //�յ㸳ֵ
//                    EndPoint.X=RightLine[row];
//                    StarPoint.Y=120;            //��㸳ֵ
//                    StarPoint.X=0;
//                    //�����ⲿ�ִ����ֹ���л��������Ϊ���ڣ�ȡ���ⲿ���жϵ�����ͬ��
////                    int c_right_flag=0;
////                    for(;row-1>0;row--)
////                    {
////                        if(RightLine[row]==MT9V03X_W-1&&RightLine[row-1]==MT9V03X_W-1)    //�ֳ��ֶ������
////                        {
////                            c_right_flag=1;
////                        }
////                    }
////                    if(c_right_flag==0)
////                    {
//                        gpio_toggle(P21_5);
//                        FillingLine(LeftLine, CentreLine, RightLine,StarPoint,EndPoint);    //����
//                        /*Debug*/
//                        //�����߻�����
//                        for(int i=MT9V03X_H;i>0;i--)
//                        {
//                            lcd_drawpoint(LeftLine[i],i,GREEN);
//                            lcd_drawpoint(CentreLine[i],i,RED);
//                            lcd_drawpoint(RightLine[i],i,BLUE);
//                        }
//                        return 2;
////                    }
////                }
//            }
//        }
//    }

    return 0;
}

uint8 CircleIslandEnd(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{

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
            for(cloumnL=UpInflectionC->X;cloumnL>30;cloumnL--)
            {
                if(BinaryImage[i-1][cloumnL]==IMAGE_WHITE)
                    break;
                if(cloumnL==30)
                    return;//�������˶�û���ҵ��׵ļ����������˳��ж�
            }
            for(cloumnR=UpInflectionC->X;cloumnR<MT9V03X_H-30;cloumnR++)
            {
                if(BinaryImage[i-1][cloumnL]==IMAGE_WHITE)
                    break;
                if(cloumnL==MT9V03X_H-31)
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
        if(UpInflectionC.X!=0)
        {
            FillingLine(LeftLine, CentreLine, RightLine, DownInflectionL,UpInflectionC);//��������˾��ڷ���֮ǰ����
            return 1;//�����յ�����������
        }
    }
    else if(DownInflectionL.X==0 && DownInflectionR.X==0 && LeftLine[MT9V03X_H]==0)//��������¹յ㲻���ڲ�������һ�оͶ��ߵĻ��Ļ�,���Ǿ�ȥ���治�������ϵĹյ�
    {
        Point ImageDownPointL,ImageDownPointR;//�Ի�������½Ǻ����½���Ϊ���Ҳ��ߵĵ�
        ImageDownPointL.X=0,ImageDownPointL.Y=0,ImageDownPointR.X=MT9V03X_W-1,ImageDownPointR.Y=0;
        GetForkUpInflection(ImageDownPointL, ImageDownPointR, &UpInflectionC);
        if(UpInflectionC.X!=0)
        {
            FillingLine(LeftLine, CentreLine, RightLine, DownInflectionL,UpInflectionC);//��������˾��ڷ���֮ǰ����
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
