/*
 * Forkroads.c
 * Created on: 2022��5��25��
 * Author: 30516
 * Effect: �������·����ص�Դ����
 * �����������ٶ�Ϊ220���ӻ����Ǳ�����ıȽϺã��ӳ�����߲��г����ˣ�û��״̬�����ǵ���Ԫ��ʶ��
 */

#include "ImageSpecial.h"
#include "PID.h"
#include <stdlib.h> //abs������fabs��math.h
#include "LED.h"
#include "zf_gpio.h"

#define L_FINDWHIDE_THRE  10 //Y�յ��м�����߰�ɫ����ֹͣ����ֵ
#define R_FINDWHIDE_THRE  150//Y�յ��м����ұ߰�ɫ����ֹͣ����ֵ
#define ROW_FINDWHIDE_THRE 110//Y�յ��е����޵���ֵ
#define CLOUMN_FINDWHIDE_THRE   10//������Ѱ�Ұ�ɫ����Ŀ�ȣ�����Ҳ����������������
#define SEED_TRANSVERSE_GROW_THRE    8//������������������������ֵ��Χ��������Ĭ�Ϻ��������ĵ�һ��Ϊ�յ�
#define FORK_INFLECTION_WIDTH  120//������debug,���յ���60�и������ҹյ�Ĳ�ֵ����ȫ��ʱ������û�ж�ʧ�Ĺյ��������һ���򵥵ı�����ϵ������ѭ��˼·��
#define FORK_DEBUG  0

extern uint8 bias_startline,bias_endline;        //��̬ǰհ

/*********************************************************************************
 ** ��������: Ѱ����������϶��㣬��ֹ��Ϊɨ��ɨ����������һ��ʹ��Bias̫С
 ** ��    ��: uint8 row,uint8 cloumn:ȥ��Ѱ���ⲹ�������ϵ�Ļ�׼������
 **           Point *LeftUpPoint:��ߵ������
 ** �� �� ֵ: ��
 ** ˵    ��: �����������֮��Ӧ���ٲ�һ����ߵĴ�ֱ�ߺ��ұߴ��Ϲյ㲹�����Ͻǹյ�
 ** ��    ��: LJF
 **********************************************************************************/
void ForkFindSpecialPoint(int row,int cloumn,Point *LeftUpPoint)
{
    if(BinaryImage[row][cloumn]==IMAGE_WHITE)
    {
        for(;row>5;row--)
        {
#if FORK_DEBUG
            lcd_drawpoint(cloumn, row, PURPLE);
#endif
            if(BinaryImage[row][cloumn]==IMAGE_WHITE && BinaryImage[row-1][cloumn]==IMAGE_BLACK)
            {
                LeftUpPoint->X=cloumn,LeftUpPoint->Y=row;
                break;
            }
        }
    }
}
/*********************************************************************************
 ** ��������: �������������������ȵ�Ѱ��Y�Ϲյ�
 ** ��    ��:char Choose��ѡ�����ڹȵ���߻����ұ�
 **          Point Seed
 **          int endline
 **          Point *UpInflectionC
 ** �� �� ֵ: ��
 ** ˵    ��:
 ** ��    ��: LJF
 **********************************************************************************/
void SeedGrowFindUpInflection(char Choose,Point Seed,int endline,Point *UpInflectionC)
{
    char transversenum=0;//��¼�����Ƿ�һֱ�����ƶ�,���Ӻ��������Ĵ���
    Point tempSeed;//��ʱ������
    for(;Seed.Y<endline && Seed.X<MT9V03X_W-1 && Seed.X>0;)
    {
#if FORK_DEBUG
        lcd_drawpoint(Seed.X, Seed.Y, GREEN);
#endif
        switch(Choose)
        {
            case 'L':
                if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_BLACK && BinaryImage[Seed.Y][Seed.X+1]==IMAGE_BLACK)
                {
                    Seed.Y++,Seed.X++;
                    transversenum=0;
                }
                else if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_BLACK && BinaryImage[Seed.Y][Seed.X+1]==IMAGE_WHITE)
                {
                    Seed.Y++;
                    transversenum=0;
                }
                else if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_WHITE && BinaryImage[Seed.Y][Seed.X+1]==IMAGE_BLACK)
                {
                    Seed.X++;
                    if(transversenum==0)//�ж��Ƿ��ǵ�һ��������
                    {
                        tempSeed=Seed;
                    }
                    transversenum++;//˵�������ұ���
                }
                else if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_WHITE && BinaryImage[Seed.Y][Seed.X+1]==IMAGE_WHITE)
                {
                    if(transversenum!=0)//˵��֮ǰһֱ�����������ҵ��˹ȵ�
                    {
                        UpInflectionC->Y=tempSeed.Y,UpInflectionC->X=tempSeed.X;
                    }
                    else
                    {
                        UpInflectionC->Y=Seed.Y,UpInflectionC->X=Seed.X;
                    }
                    return;
                }
                break;
            case 'R':
                if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_BLACK && BinaryImage[Seed.Y][Seed.X-1]==IMAGE_BLACK)
                {
                    Seed.Y++,Seed.X--;
                    transversenum=0;
                }
                else if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_BLACK && BinaryImage[Seed.Y][Seed.X-1]==IMAGE_WHITE)
                {
                    Seed.Y++;
                    transversenum=0;
                }
                else if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_WHITE && BinaryImage[Seed.Y][Seed.X-1]==IMAGE_BLACK)
                {
                    Seed.X--;
                    if(transversenum!=0)//�ж��Ƿ��ǵ�һ��������
                    {
                        tempSeed=Seed;
                    }
                    transversenum++;;//˵�����������
                }
                else if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_WHITE && BinaryImage[Seed.Y][Seed.X-1]==IMAGE_WHITE)
                {
                    if(transversenum!=0)//˵��֮ǰһֱ�����������ҵ��˹ȵ�
                    {
                        UpInflectionC->Y=tempSeed.Y,UpInflectionC->X=tempSeed.X;
                    }
                    else
                    {
                        UpInflectionC->Y=Seed.Y,UpInflectionC->X=Seed.X;
                    }
                    return;
                }
                break;
            default:break;
        }
        //�����Ӻ��������Ĵ�����������ֵ
        if(transversenum>SEED_TRANSVERSE_GROW_THRE)
        {
            UpInflectionC->Y=tempSeed.Y,UpInflectionC->X=tempSeed.X;
        }
    }
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
    int row=0, cloumnL=0, cloumnR=0, cloumnnum=0;//cloumnnum��¼�ӻ�׼��ƫ���˶����к�ɫ����
    Point Seed;
    char Choose=0,flagL=0,flagR=0;//�ж����ڹȵ���߻����ұߵĺ���,�Լ��ж�����������û�а�ɫ�����FLAG
    UpInflectionC->X = 0; UpInflectionC->Y = 0;//�Ϲյ�����
    UpInflectionC->X = (DownInflectionL.X + DownInflectionR.X) / 2;//V���Ϲյ��������Ϊ���ҹյ��ֵ
    //��ֹ��Ϊ����Ѱ�յ�ʹ�����
    if(UpInflectionC->X<0) UpInflectionC->X=0;
    if(UpInflectionC->X>MT9V03X_W-1) UpInflectionC->X=MT9V03X_W-1;
    row = (DownInflectionL.Y + DownInflectionR.Y) / 2;//��ʼ��Ϊ���ҹյ��еľ�ֵ
    for (; row > 20; row--)
    {
#if FORK_DEBUG
        lcd_drawpoint(UpInflectionC->X, row, PURPLE);
#endif
        if (BinaryImage[row][UpInflectionC->X] == IMAGE_WHITE && BinaryImage[row - 1][UpInflectionC->X] == IMAGE_BLACK)
        {
            //���������Ұ�ɫ����
            for (cloumnL = UpInflectionC->X; cloumnL > L_FINDWHIDE_THRE; cloumnL--)
            {
#if FORK_DEBUG
                lcd_drawpoint(cloumnL, row-1, YELLOW);
#endif
                if(BinaryImage[row - 1][cloumnL] == IMAGE_BLACK)
                {
                    cloumnnum++;
                }
                if (BinaryImage[row - 1][cloumnL] == IMAGE_BLACK && BinaryImage[row - 1][cloumnL - 1] == IMAGE_WHITE && BinaryImage[row - 1][cloumnL - 3] == IMAGE_WHITE)
                {
                    cloumnnum=0;
                    flagL = 1;
                    break;
                }
                //����1��ѭ�������˻�û�ҵ���ɫ���򣬻�����2����ɫ����̫���˴�������ֵ���������ҵ���ɫ���򲹵���Ҳ����
                if (cloumnL == L_FINDWHIDE_THRE + 1 || cloumnnum>CLOUMN_FINDWHIDE_THRE)
                {
                    cloumnnum=0;
                    Choose = 'R';//����Ҳ���˵���ڹȵ��ұ�
                    break;
                }
            }
            for (cloumnR = UpInflectionC->X; cloumnR < R_FINDWHIDE_THRE && Choose!='R'; cloumnR++)
            {
#if FORK_DEBUG
                lcd_drawpoint(cloumnR, row-1, YELLOW);
#endif
                if(BinaryImage[row - 1][cloumnR] == IMAGE_BLACK)
                {
                    cloumnnum++;
                }
                if (BinaryImage[row - 1][cloumnR] == IMAGE_BLACK && BinaryImage[row - 1][cloumnR + 1] == IMAGE_WHITE && BinaryImage[row - 1][cloumnL + 3] == IMAGE_WHITE)
                {
                    cloumnnum=0;
                    flagR = 1;
                    break;
                }
                if (cloumnR == R_FINDWHIDE_THRE - 1 || cloumnnum>CLOUMN_FINDWHIDE_THRE)
                {
                    cloumnnum=0;
                    Choose = 'L';//�ұ��Ҳ���˵���ڹȵ����
                    break;
                }
            }
            break;
        }
    }
    if ((flagL == 0 || flagR == 0) && Choose!=0)//˵����һ����û�а�ɫ�����
    {
        Seed.X = UpInflectionC->X, Seed.Y = row - 1;
#if FORK_DEBUG
        for(int j=0;j<MT9V03X_W-1;j++)//����100��������
        {
            lcd_drawpoint(j, ROW_FINDWHIDE_THRE, PURPLE);
        }
#endif
        SeedGrowFindUpInflection(Choose, Seed, ROW_FINDWHIDE_THRE, UpInflectionC);
    }
    else
    {
        UpInflectionC->Y = row - 1;
    }
}

/********************************************************************************************
 ** ��������: ʶ������
 ** ��    ��:
 **           int *LeftLine������
 **           int *RightLine:����
 **           Point *InflectionL:��߹յ�
 **           Point *InflectionR:�ұ߹յ�
 ** �� �� ֵ:  0��û��ʶ�𵽻���
 **           1����������
 ** ��    ��: LJF
 ** ע    �⣺1 . Ŀǰ���������������ʱ��ĺ�������Ϊ����ǰ�涼���и�������Ի���ֳ���б���������ʱ�����ҹյ㲢��һ��������
 **           2.����ǽ�����ĺ�����������ʱ��Ӧ����дһ�������ڽ���������ٿ�����������ж�
 *********************************************************************************************/
uint8 ForkIdentify(int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR)
{
#if FORK_DEBUG
    lcd_showint32(0, 0, LostNum_RightLine, 3);
    lcd_showint32(0, 1, LostNum_LeftLine, 3);
    lcd_showint32(TFT_X_MAX-50, 0, DownInflectionL.X, 3);
    lcd_showint32(TFT_X_MAX-50, 1, DownInflectionL.Y, 3);
    lcd_showint32(TFT_X_MAX-50, 2, DownInflectionR.X, 3);
    lcd_showint32(TFT_X_MAX-50, 3, DownInflectionR.Y, 3);
#endif
    Point UpInflectionC,LeftUpPoint,LeftDownPoint;//�Ϲյ㣬��߹յ㣬����϶��㣬������
    //�����ҹյ����,���������յ�Ҫ��ͼ���°벿��
    if(DownInflectionL.X!=0 && DownInflectionR.X!=0 && DownInflectionL.X<120 && DownInflectionR.X>30)
    {
        //ȡ��������ҹյ��������жϣ�������������
        if(abs((DownInflectionL.Y-DownInflectionR.Y))<40)//���������յ������С��30���Ž����ж�
        {
            GetForkUpInflection(DownInflectionL, DownInflectionR, &UpInflectionC);//ȥ�����Ϲյ�
            if(UpInflectionC.Y!=0)//ֱ�ӷ���Y���ɣ��ӿ��ٶȣ���ΪXĬ�Ͼͻḳֵ��
            {
                FillingLine('R',DownInflectionR,UpInflectionC);//��������˾��ڷ���֮ǰ����
                //��һ������
                ForkFindSpecialPoint(UpInflectionC.Y,DownInflectionL.X,&LeftUpPoint);
                if(LeftUpPoint.Y!=0)
                {
                    LeftDownPoint.Y=UpInflectionC.Y,LeftDownPoint.X=DownInflectionL.X;
                    FillingLine('L',LeftDownPoint,LeftUpPoint);
                    FillingLine('R',UpInflectionC,LeftUpPoint);
                    UpInflectionC.Y=LeftUpPoint.Y;//Ϊ��ʹ�ò����������ߵ�ƫ�ʹ�õ�
                }
//                Bias=DifferentBias(bias_startline,bias_endline,CentreLine);//�����⴦��ʱ��ƫ�����
                //�ж�Bias�����
                if(UpInflectionC.Y<bias_endline)//starline<endline<Up.y��������ѭ��
                {
                    Bias=DifferentBias(bias_startline,bias_endline,CentreLine);
                }
                else if(UpInflectionC.Y<bias_startline && bias_endline<UpInflectionC.Y)//starline<UP.y<endline,������ʼ�е��Ϲյ�
                {
                    Bias=DifferentBias(bias_startline,UpInflectionC.Y,CentreLine);
                }
                else if(bias_startline<UpInflectionC.Y)//UP.y<starline<endline
                {
                    Bias=DifferentBias(DownInflectionR.Y,UpInflectionC.Y,CentreLine);
                }
                gpio_toggle(LED_BLUE);
                return 1;//�����յ���������������������
            }
        }
        else
            return 0;
    }
    else if((DownInflectionL.X==0 && DownInflectionR.X==0) || (BinaryImage[MT9V03X_H-5][5]==IMAGE_WHITE && BinaryImage[MT9V03X_H-5][MT9V03X_W-5]==IMAGE_WHITE))//��������¹յ㲻���ڲ�������һ�γ��־Ͷ��ߵĻ��Ļ�,���Ǿ�ȥ���治�������ϵĹյ�
    {

        Point ImageDownPointL,ImageDownPointR;//�Ի�������½Ǻ����½���Ϊ���Ҳ��ߵĵ�
        //���Ϲյ����һ��Ԥ��ĵ㣬������д��Ϊ��Ļ�м�
        if(LeftLine[MT9V03X_H-1]!=0)//����������һ��û�ж���
            ImageDownPointL.X=LeftLine[MT9V03X_H-1]+10,ImageDownPointL.Y=MT9V03X_H-1;
        else ImageDownPointL.X=0,ImageDownPointL.Y=MT9V03X_H-1;
        if(RightLine[MT9V03X_H-1]!=MT9V03X_W-1)
            ImageDownPointR.X=RightLine[MT9V03X_H-1]-10,ImageDownPointR.Y=MT9V03X_H-1;
        else ImageDownPointR.X=MT9V03X_W-1,ImageDownPointR.Y=MT9V03X_H-1;
        //��Ѱ�Ϲյ�
        GetForkUpInflection(ImageDownPointL, ImageDownPointR, &UpInflectionC);
        if(UpInflectionC.Y!=0 && UpInflectionC.Y>40)//ֱ�ӷ���Y���ɣ��ӿ��ٶȣ���ΪXĬ�Ͼͻḳֵ��
        {
            FillingLine('R',ImageDownPointR,UpInflectionC);//��������˾��ڷ���֮ǰ����
            ForkFindSpecialPoint(UpInflectionC.Y,2,&LeftUpPoint);
            if(LeftUpPoint.Y!=0)
            {
                LeftDownPoint.Y=UpInflectionC.Y,LeftDownPoint.X=2;
                FillingLine('L',LeftDownPoint,LeftUpPoint);
                FillingLine('R',UpInflectionC,LeftUpPoint);
                UpInflectionC.Y=LeftUpPoint.Y;//Ϊ��ʹ�ò����������ߵ�ƫ�ʹ�õ�
            }
//            Bias=DifferentBias(bias_startline,bias_endline,CentreLine);//�����⴦��ʱ��ƫ�����
            //�ж�Bias�����
            if(UpInflectionC.Y<bias_endline)//starline<endline<Up.y��������ѭ��
            {
                Bias=DifferentBias(bias_startline,bias_endline,CentreLine);
            }
            else if(UpInflectionC.Y<bias_startline && bias_endline<UpInflectionC.Y)//starline<UP.y<endline,������ʼ�е��Ϲյ�
            {
                Bias=DifferentBias(bias_startline,UpInflectionC.Y,CentreLine);
            }
            else if(bias_startline<UpInflectionC.Y)//UP.y<starline<endline
            {
                Bias=DifferentBias(ImageDownPointR.Y,UpInflectionC.Y,CentreLine);
            }
            gpio_toggle(LED_GREEN);
            return 1;//�������붪ʧ���ҹյ���һ֡
        }
    }
    //�ұ߶��߳���60����յ���ڣ�������յ㲻�����ϰ�ƽ����ֹ����
    else if(LostNum_RightLine>55 && DownInflectionL.X>0 && DownInflectionL.X<90 && DownInflectionL.Y>40)
    {
        Point ImageDownPointR;//����յ�ԳƵĵ�ȥ���ߺ��ҹյ�
        //���Լ��趨���ҹյ�ȥ���Ϲյ�
//        ImageDownPointR.X=MT9V03X_W-1,ImageDownPointR.YDownInflectionL.Y=DownInflectionL.Y;
        //���õ���ѭ������˼����յ㣬������ȣ���б���Ϲյ��ұߵĶ�����һ��
        ImageDownPointR.X=DownInflectionL.X+(145-(119-DownInflectionL.Y)*1.1);ImageDownPointR.Y=DownInflectionL.Y;
        GetForkUpInflection(DownInflectionL, ImageDownPointR, &UpInflectionC);
        if(UpInflectionC.Y!=0)//ֱ�ӷ���Y���ɣ��ӿ��ٶȣ���ΪXĬ�Ͼͻḳֵ��
        {
            //��б��ʱ����ʵ�ҹյ����������Ļ�����½ǣ��������ڲ��ߵ�ʱ�������½�Ϊ���ߵ�
            ImageDownPointR.X=MT9V03X_W-1;ImageDownPointR.Y=MT9V03X_H-1;
            FillingLine('R',ImageDownPointR,UpInflectionC);//��������˾��ڷ���֮ǰ����
//            Bias=DifferentBias(bias_startline,bias_endline,CentreLine);//�����⴦��ʱ��ƫ�����
            //�ж�Bias�����
            if(UpInflectionC.Y<bias_endline)//starline<endline<Up.y��������ѭ��
            {
                Bias=DifferentBias(bias_startline,bias_endline,CentreLine);
            }
            else if(UpInflectionC.Y<bias_startline && bias_endline<UpInflectionC.Y)//starline<UP.y<endline,������ʼ�е��Ϲյ�
            {
                Bias=DifferentBias(bias_startline,UpInflectionC.Y,CentreLine);
            }
            else if(bias_startline<UpInflectionC.Y)//UP.y<starline<endline
            {
                Bias=DifferentBias(ImageDownPointR.Y,UpInflectionC.Y,CentreLine);
            }
            gpio_toggle(LED_RED);
            return 1;//������б������
        }
    }
    //��߶��߳���60,�ҹյ����,�����ҹյ㲻�����ϰ�ƽ����ֹ����
    else if(LostNum_LeftLine>55 && DownInflectionR.X>60 && DownInflectionR.Y>40)
    {
        Point ImageDownPointL;//����յ�ԳƵĵ�ȥ���ߺ��ҹյ�
        //��յ�������һ��������ϵ��Խ�����ײ��˹յ���Խ��.��б������һ��
//        ImageDownPointL.X=DownInflectionR.X-(145-(119-DownInflectionL.Y)*1.1);ImageDownPointL.Y=DownInflectionR.Y;
        ImageDownPointL.X=DownInflectionR.X-FORK_INFLECTION_WIDTH;ImageDownPointL.Y=DownInflectionR.Y;
        GetForkUpInflection(ImageDownPointL, DownInflectionR, &UpInflectionC);
        if(UpInflectionC.Y!=0)//ֱ�ӷ���Y���ɣ��ӿ��ٶȣ���ΪXĬ�Ͼͻḳֵ��
        {
            FillingLine('R',DownInflectionR,UpInflectionC);//��������˾��ڷ���֮ǰ����
            //��һ������
            ForkFindSpecialPoint(UpInflectionC.Y,5,&LeftUpPoint);
            if(LeftUpPoint.Y!=0)
            {
                LeftDownPoint.Y=UpInflectionC.Y,LeftDownPoint.X=5;
                FillingLine('L',LeftDownPoint,LeftUpPoint);
                FillingLine('R',UpInflectionC,LeftUpPoint);
                UpInflectionC.Y=LeftUpPoint.Y;//Ϊ��ʹ�ò����������ߵ�ƫ�ʹ�õ�
            }
//            Bias=DifferentBias(bias_startline,bias_endline,CentreLine);//�����⴦��ʱ��ƫ�����
            //�ж�Bias�����
            if(UpInflectionC.Y<bias_endline)//starline<endline<Up.y��������ѭ��
            {
                Bias=DifferentBias(bias_startline,bias_endline,CentreLine);
            }
            else if(UpInflectionC.Y<bias_startline && bias_endline<UpInflectionC.Y)//starline<UP.y<endline,������ʼ�е��Ϲյ�
            {
                Bias=DifferentBias(bias_startline,UpInflectionC.Y,CentreLine);
            }
            else if(bias_startline<UpInflectionC.Y)//UP.y<starline<endline
            {
                Bias=DifferentBias(DownInflectionR.Y,UpInflectionC.Y,CentreLine);
            }
            gpio_toggle(LED_WHITE);
            return 1;//������б������
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
uint8 ForkFStatusIdentify(Point DownInflectionL,Point DownInflectionR,uint8 *ForkFlag)
{
    static uint8 StatusChange,num1,num3,numspecial;//����ʶ��������ʱ״̬������������״̬�Ƿ���ת
    uint8 NowFlag=0;//��ε�ʶ����

    NowFlag=ForkIdentify(LeftLine, RightLine, DownInflectionL, DownInflectionR);
    *ForkFlag=NowFlag;//��ʶ�����ͳ�ȥ
    //���Ᵽ��
    if(numspecial<200)//��ֹ�ܾö�û�г��ֽ�����ڵ�״̬����ʱȥ�жϳ���
    {
        numspecial++;
    }
    else if(StatusChange<1)//�ж�״̬��û�жȹ����״̬����û����ǿ������
    {
        StatusChange=2;
    }
    //״̬����ʼ����
    switch(StatusChange)
    {
        //���״̬
        case 0:
        {
            if(NowFlag==1)
            {
                StatusChange=1;//ֻҪ��ʼʶ���������˵���Ѿ�����ڽ׶���
            }
            break;
        }
        //��;״̬
        case 1:
        {
            if(num1<100)  //���㹻����ʱ���ó��ߵ�����������
            {
                num1++;
                break;
            }
            else if(NowFlag==0)
            {
                StatusChange=2;//�����м����̬֮����ת��������
            }
            break;
        }
        //����״̬
        case 2:
        {
            if(NowFlag==1)
            {
                StatusChange=3;
            }
            break;
        }
        //ȷ���Ѿ��������ˣ���������ھͳ������ˣ�ʹ�ó�������ʵ��ɨ�߳���
        case 3:
        {
            if(num3<35)  //���㹻����ʱ���ó��߳�������
            {
                num3++;
                break;
            }
            else
            {
                return 1;
            }
        }
        default:break;
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
uint8 ForkSStatusIdentify(Point DownInflectionL,Point DownInflectionR,uint8 *NowFlag)
{
    static uint8 StatusChange,num1,num3,numspecial;//����ʶ��������ʳ״̬������������״̬�Ƿ���ת

    if(numspecial<255)//��ֹ�ܾö�û�г��ֽ�����ڵ�״̬����ʱȥ�жϳ���
    {
        numspecial++;
    }
    else if(StatusChange<1)//�ж�״̬��û�жȹ����״̬����û����ǿ������
    {
        StatusChange=2;
    }

    switch(StatusChange)
    {
        //���״̬
        case 0:
        {
            if(*NowFlag==1)
            {
                SteerK.P=18.25;//��СKP��������
                StatusChange=1; //ֻҪ��ʼʶ���������˵���Ѿ�����ڽ׶���
            }
            break;
        }
        //��;״̬
        case 1:
        {
            if(num1<100)  //���㹻����ʱ���ó��ߵ�����������
            {
                if(num1==50)
                {
                    SteerK.D=3;//����ֱ��֮��D��С��ֹ��
                    base_speed+=15; //�����������٣�ȷ�����������������Żᴥ��
                }
                num1++;
                break;
            }
            if(*NowFlag==0)
            {
                StatusChange=2; //�����м����̬֮����ת��������
                base_speed-=5; //�����ڼ���
            }
            break;
        }
        //����״̬
        case 2:
        {
            if(*NowFlag==1)
            {
                StatusChange=3;
            }
            break;
        }
        //ȷ���Ѿ��������ˣ���������ھͳ������ˣ�ʹ�ó�������ʵ��ɨ�߳���
        case 3:
        {
            if(num3<50)
            {
                num3++;
                break;
            }
            else
            {
                SteerK.P=19.25;//��ԭKP
                return 1;
            }
        }
        default:break;
    }
    return 0;
}

