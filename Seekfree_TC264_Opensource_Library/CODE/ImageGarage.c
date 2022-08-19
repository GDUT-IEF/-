/*
 * Garage.c
 * Created on: 2022��5��25��
 * Author: 30516
 * Effect: ��ų�����ص�Դ����
 * *�ұ߲���⣺240 ��⣺200(�ǶȲ��ܿ���������)
 * ��߲���⣺230���������ֱ���䷢����250���ӹ���ֱ���䷢�� ��⣺180���Ǹо�ͣ������
 */

#include "ImageSpecial.h"
#include "PID.h"
#include "ICM20602.h"           //�����ǻ�����ɱ�־�����Լ��������ֺ���
#include "ImageProcess.h"

#include "LED.h" //debug
#include "SEEKFREE_18TFT.h"
#include "zf_gpio.h"
#include "Motor.h"

extern uint8 bias_startline,bias_endline;        //��̬ǰհ
uint8   Garage_LastRightangleRow=20;            //�����ȫ�ֱ�����һ�δ������±�����������Ѱ��ֱ�ǹյ������
uint8   zebre_row=0;                             //ʶ�𵽰����ߵ���
uint8   zebre_column=0;

#define GARAGE_IDENTIFY_MODE 1    //����ģʽ���Ϲյ�
//Sobel���Ӽ��
#define FastABS(x) (x > 0 ? x : x * -1.0f)
#define BinaryImage(i, j)    BinaryImage[i][j]
//�ҹյ㺯����debug
#define SEEDGROWFINDVALLEY_DEBUG 0         //���������ҹȵ��Ƿ���DEBUG
#define SEEDGROWFINDPEAK_DEBUG  0
//�󳵿�
#define ZebraTresholeL 1200  //���������Ե���ֵ
#define IN_GARAGE_ZEBRA_THR_L   250//���������������ֵ,���и��г�ȡ����sobel�����ٶ�
#define IN_L_GARAGE_ANGLE   40  //����⿪�������ǻ��ֵ�Ŀ��Ƕ�
#define L_GARAGE_LOSTLLINE_MIN_THR 25   //��߳��⿪������������߶�����С��ֵ
#define L_GARAGE_LOSTLLINE_MAX_THR 35   //��߳��⿪������������߶��������ֵ
#define L_GARAGE_LOSTRLINE_MAX_THR 50   //��߳��⿪�����������ұ߶��������ֵ
#define LIN_GARAGE_LOSTLLINE_MIN_THR 30 //��߳����������������С��ֵ
#define LIN_GARAGE_LOSTRLINE_MAX_THR 30 //��߳�������������������ֵ
#define LINGARAGEENTRANCE_SEEDGROW_THR 100        //��������������������������ֵ
//�ҳ���
#define ZebraTresholeR 300  //���������Գ������ұߵ���ֵ
#define IN_R_GARAGE_ANGLE   60  //���ҿ⿪�������ǻ��ֵ�Ŀ��Ƕ�
#define R_GARAGE_LOSTRLINE_THR 35   //�ұ߳��⿪�����������ұ߶�����ֵ
#define RINGARAGEENTRANCE_SEEDGROW_THR 5        //�ұ������������������������ֵ
//DEBUG�����ĺ궨��
#define LINGARAGEENTRANCE_DEBUG    0    //��߳�������Ƿ���debug
#define RINGARAGEENTRANCE_DEBUG 0                //�ұ������ڴ��Ƿ���DEBUG
#define RNOINGARAGE_DEBUG   0   //�ҳ��ⲻ����DEBUG
#define GARAGE_DEBUG    0       //�Ƿ���Ҫ���������DEBUG
#define GARAGE_LED_DEBUG 0
#define L_IN_GARAGE_LED_DEBUG 1

/********************************************************************************************
 ** ��������: Sobel���Ӽ��������
 ** ��    ��: uint8 starline,uint8 endline,uint8 starcloumn,uint8 endcloumn
 ** �� �� ֵ: Sobel��ֵ
 ** ��    ��: LJF
 ** ע    �⣺֮ǰ������MT9V03X_H-1-20~50
 **                ����40~MT9V03X_W-1-40
 *********************************************************************************************/
int64 SobelTest(uint8 starline,uint8 endline,uint8 starcloumn,uint8 endcloumn)
{
    int64 Sobel = 0;
    int64 temp = 0;

    for (uint8 i = starline; i > endline ; i-=2)
    {
        for (uint8 j = starcloumn; j < endcloumn; j+=2)
        {
            int64 Gx = 0, Gy = 0;
            Gx = -BinaryImage(i-1, j-1)+BinaryImage(i-1, j+1)
                 -2*BinaryImage(i, j-1)+2*BinaryImage(i, j+1)
                 -BinaryImage(i+1, j-1)+BinaryImage(i+1, j+1);
            Gy = BinaryImage(i-1, j-1)+2*BinaryImage(i-1, j)+BinaryImage(i-1, j+1)
                 -BinaryImage(i+1, j+1)-2*BinaryImage(i+1, j)-BinaryImage(i+1, j+1);
            temp += FastABS(Gx) + FastABS(Gy);
            Sobel += temp / 255;
            temp = 0;
        }
    }
    return Sobel;
}
/********************************************************************************************
 ** ��������: ��������
 ** ��    ��: start_line�������ʼ��
 **           end_line����������
 ** �� �� ֵ: 1����⵽������
 **           0��û�м�⵽������
 ** ��    ��: WBN
 *********************************************************************************************/
uint8 ZebraIndentify(uint8 start_line,uint8 end_line,uint8* black_width)
{
    uint8 num=0,black_finish_flag=0;//�ڰ�����Ĵ�������ɫ������ʧ�ı�־����
    for(uint8 row=start_line;row-2>end_line;row-=2)    //����ɨ
    {
        num=0;*black_width=0,black_finish_flag=0;
        for(uint8 column=MT9V03X_W-1;column>5;column--)   //����ɨ
        {
//            lcd_drawpoint(column, row, PURPLE);
            if(BinaryImage[row][column]!=BinaryImage[row][column-1])    //�����
            {
                num++;
            }
            if(num>7)
            {
                zebre_row=row;
                zebre_column=column;
                return 1;
            }
        }
    }
    return 0;
}
/*********************************************************************************
 ** ��������: �������������������ȵ�Ѱ��Y�Ϲյ�
 ** ��    ��:char Choose��ѡ�����ڹȵ���߻����ұ�
 **          Point Seed
 **          int endline
 **          Point *UpInflectionC
 **          char TRANSVERSE_THR:�����������ƶ�����ֵ
 ** �� �� ֵ: ��
 ** ��    ��: LJF
 **********************************************************************************/
void SeedGrowFindValley_Garage(char Choose,Point Seed,int endline,Point *UpInflectionC,char TRANSVERSE_THR)
{
    char transversenum=0;//��¼�����Ƿ�һֱ�����ƶ�,���Ӻ��������Ĵ���
    Point tempSeed;//��ʱ������
    for(;Seed.Y<endline && Seed.X<MT9V03X_W-1 && Seed.X>0;)
    {
#if SEEDGROWFINDVALLEY_DEBUG
        lcd_drawpoint(Seed.X, Seed.Y, GREEN);
#endif
        switch(Choose)
        {
            case 'L':
                if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_BLACK && BinaryImage[Seed.Y][Seed.X+1]==IMAGE_BLACK)
                {
                    Seed.Y++,Seed.X++;
                }
                else if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_BLACK && BinaryImage[Seed.Y][Seed.X+1]==IMAGE_WHITE)
                {
                    Seed.Y++;
                }
                else if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_WHITE && BinaryImage[Seed.Y][Seed.X+1]==IMAGE_BLACK)
                {
                    Seed.X++;
                }
                else if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_WHITE && BinaryImage[Seed.Y][Seed.X+1]==IMAGE_WHITE)
                {
                        UpInflectionC->Y=Seed.Y,UpInflectionC->X=Seed.X;
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
        if(transversenum>TRANSVERSE_THR)
        {
            UpInflectionC->Y=tempSeed.Y,UpInflectionC->X=tempSeed.X;
            break;
        }
        if(Seed.X>MT9V03X_W-1 || Seed.X<0 || Seed.Y<0 || Seed.Y>MT9V03X_H-1)
        {
            break;//��ֹ�Ҳ�����ѭ��
        }
    }
}
//�������������ҵ�ɽ���������溯���෴
void SeedGrowFindPeak_Garage(char Choose,Point Seed,int endline,Point *PeakInflection,char TRANSVERSE_THR)
{
    for(;Seed.Y>endline && Seed.X<MT9V03X_W-1 && Seed.X>0;)
    {
#if SEEDGROWFINDPEAK_DEBUG
        lcd_drawpoint(Seed.X, Seed.Y, GREEN);
#endif
        switch(Choose)
        {
            //��������ɽ
            case 'R':
            {
                if(BinaryImage[Seed.Y-1][Seed.X]==IMAGE_WHITE && BinaryImage[Seed.Y][Seed.X+1]==IMAGE_WHITE)
                {
                    Seed.X++;Seed.Y--;
                }
                else if(BinaryImage[Seed.Y-1][Seed.X]==IMAGE_BLACK && BinaryImage[Seed.Y][Seed.X+1]==IMAGE_WHITE)
                {
                    Seed.X++;
                }
                else if(BinaryImage[Seed.Y-1][Seed.X]==IMAGE_WHITE && BinaryImage[Seed.Y][Seed.X+1]==IMAGE_BLACK)
                {
                    Seed.Y--;
                }
                else if(BinaryImage[Seed.Y-1][Seed.X]==IMAGE_BLACK && BinaryImage[Seed.Y][Seed.X+1]==IMAGE_BLACK)
                {
                    PeakInflection->Y=Seed.Y,PeakInflection->X=Seed.X;
                    return;
                }
                break;
            }
            //�������ɽ
            case 'L':
            {
                if(BinaryImage[Seed.Y-1][Seed.X]==IMAGE_WHITE && BinaryImage[Seed.Y][Seed.X-1]==IMAGE_WHITE)
                {
                    Seed.X--;Seed.Y--;
                }
                else if(BinaryImage[Seed.Y-1][Seed.X]==IMAGE_BLACK && BinaryImage[Seed.Y][Seed.X-1]==IMAGE_WHITE)
                {
                    Seed.X--;
                }
                else if(BinaryImage[Seed.Y-1][Seed.X]==IMAGE_WHITE && BinaryImage[Seed.Y][Seed.X-1]==IMAGE_BLACK)
                {
                    Seed.Y--;
                }
                else if(BinaryImage[Seed.Y-1][Seed.X]==IMAGE_BLACK && BinaryImage[Seed.Y][Seed.X-1]==IMAGE_BLACK)
                {
                    PeakInflection->Y=Seed.Y,PeakInflection->X=Seed.X;
                    return;
                }
                break;
            }
            default:break;
        }
        if(Seed.X>MT9V03X_W-1 || Seed.X<0 || Seed.Y<0 || Seed.Y>MT9V03X_H-1)
        {
            break;//��ֹ�Ҳ�����ѭ��
        }
    }
}
//���������״̬ʶ����
uint8 LINGarageEntrance(Point InflectionL,Point InflectionR)
{
#if LINGARAGEENTRANCE_DEBUG
    lcd_showint32(TFT_X_MAX-50, 0, InflectionL.X, 3);
    lcd_showint32(TFT_X_MAX-50, 1, InflectionR.X, 3);
#endif
    //�Ϲյ�,���е��ȵ׵����ӵ�,�����ߵ����,�����ߵ����,�����ߵ��յ�,��ʼ��Ѱֱ�ǹյ�Ļ�׼��
    Point UpInflection,PeakSeed,ValleySeed,RDownPoint,LDownPoint,LUpPoint,StarPoint;
    UpInflection.X=0,UpInflection.Y=0;//��ʼ��Ϊ0
    char NoInflectionLFlag=0;//1���������ҹյ� 0�������ҹյ�
    static float LastBias;
    //�ж����¹յ��Ƿ�Ϊ������
    if(InflectionL.X==0 || (InflectionL.Y-InflectionR.Y)<10 || InflectionL.X>MT9V03X_W/2 || BinaryImage[InflectionL.Y+5][5]!=IMAGE_BLACK)
    {
        StarPoint.Y=MT9V03X_H/2+15;StarPoint.X=5;//��Ļ���б߱ߵ�
        //�ж����µ��Ƿ�������õ���ȥѰ�����ӵĻ�׼��
        if(BinaryImage[StarPoint.Y][StarPoint.X]==IMAGE_WHITE)
        {
            InflectionL=StarPoint;
            NoInflectionLFlag=1;//�ѹյ㲻���ڵ�flag��1
        }
    }
    else //��յ����
    {
        StarPoint.Y=InflectionL.Y;StarPoint.X=5;//��Ļ���б߱ߵ�
        NoInflectionLFlag=0;
    }
    //�ҵ�
    //��ɽ����͹ȵ׵����ӵ�
    for(int row=StarPoint.Y-5;row>10;row--)//�����󳵿�Ƚ��Զ��׼����Ǹ��յ�����б���������ģ��Ƚ��ʺ�����������
    {
#if LINGARAGEENTRANCE_DEBUG
        lcd_drawpoint(StarPoint.X, row, PURPLE);
#endif
        if(BinaryImage[row][StarPoint.X]==IMAGE_WHITE&&BinaryImage[row-1][StarPoint.X]==IMAGE_BLACK)
        {
            if(BinaryImage[row-1][StarPoint.X+5]==IMAGE_WHITE)//����ҵ��ĵ���ұ��ǰ�ɫ˵������ɽ��Ҫ����ɽ��
            {
                PeakSeed.X=StarPoint.X;PeakSeed.Y=row;//��ʼ��Ҳ�ǰ�ɫ�ķ�����������
                SeedGrowFindPeak_Garage('R', PeakSeed, 10, &ValleySeed, 100);
            }
            else
            {
                ValleySeed.X=StarPoint.X;ValleySeed.Y=row-1;
            }
            break;
        }
    }
    if(ValleySeed.X!=0 && ValleySeed.Y!=0)
    {
        ValleySeed.X+=5;//��ֹ��Ϊɽ�������˼��ʹ���Ϲյ��Ҵ�
        SeedGrowFindValley_Garage('L', ValleySeed, MT9V03X_H-1, &UpInflection,LINGARAGEENTRANCE_SEEDGROW_THR);//�����������������ȵ�
        ValleySeed.X-=5;//���꼴��ԭ
#if LINGARAGEENTRANCE_DEBUG
        LcdDrawPoint_V2(PeakSeed.Y, PeakSeed.X, PINK);
        LcdDrawPoint_V2(ValleySeed.Y, ValleySeed.X, YELLOW);
        LcdDrawPoint_V2(UpInflection.Y, UpInflection.X, GREEN);
#endif
        //�ж��Ƿ��ҵ����Ϲյ�
        if(UpInflection.Y!=0 && UpInflection.Y>30)
        {
            //������Ƿ�ʧ�յ���ȷ�����ߵ����
            if(NoInflectionLFlag==1)
            {
                //�ж����������һ���Ƿ������������ߣ�������������Ϊ�����ϵĵ㣬�����Զ���
                if(RightLine[MT9V03X_H-2]>130)
                {
                    RDownPoint.Y=MT9V03X_H-2;RDownPoint.X=RightLine[MT9V03X_H-2];
                }
                else
                {
                    RDownPoint.Y=MT9V03X_H-2;RDownPoint.X=MT9V03X_W-2;
                }
                LDownPoint.Y=MT9V03X_H-2;LDownPoint.X=1;
                LUpPoint.X=1;LUpPoint.Y=ValleySeed.Y;
            }
            else
            {
                RDownPoint.Y=InflectionL.Y;RDownPoint.X=RightLine[InflectionL.Y-5];
                LDownPoint.Y=InflectionL.Y;LDownPoint.X=InflectionL.X;
                LUpPoint.X=1;LUpPoint.Y=ValleySeed.Y;
            }
            //����
            FillingLine('R', RDownPoint, UpInflection);
            FillingLine('R', UpInflection, ValleySeed);
            FillingLine('L', LDownPoint, LUpPoint);
            //��ƫ��Ĵ���
            if(LUpPoint.Y<bias_endline)
            {
                Bias=DifferentBias_Garage(bias_startline,bias_endline, CentreLine);
            }
            else if(LUpPoint.Y<bias_startline && bias_endline<LUpPoint.Y)
            {
                Bias=DifferentBias_Garage(bias_startline,LUpPoint.Y, CentreLine);
            }
            else if(bias_startline<LUpPoint.Y)
            {
                Bias=DifferentBias_Garage(RDownPoint.Y,LUpPoint.Y, CentreLine);
            }
//            if(Bias<1) Bias=LastBias;//���ƫ�������ұߵ�
//            else LastBias=Bias;//����һ��ƫ�ֵ
#if LINGARAGEENTRANCE_DEBUG
            lcd_showint32(TFT_X_MAX-50, 2, UpInflection.X, 3);
            lcd_showint32(TFT_X_MAX-50, 3, UpInflection.Y, 3);
#endif
            lcd_showint32(TFT_X_MAX-50, 2, UpInflection.X, 3);
            lcd_showint32(TFT_X_MAX-50, 3, UpInflection.Y, 3);
            if(UpInflection.Y>100)
                return 2;//����Ϲյ��Ѿ�����100�������ˣ���������Ѳ��ȥ��⣬���Һ���ͣ������
            return 1;
        }
    }
    return 0;
}
//��೵�����״̬��
uint8 LINGarageStatusIdentify(Point InflectionL,Point InflectionR,uint8* GarageLFlag)
{
    static uint8 StatusChange;//״̬����
    uint8 NowFlag=0;//��ε�ʶ����
    uint8 black_width=0;//ʶ�𵽰��������еĴ�������ĺ�ɫ���
    switch(StatusChange)
    {
        //ʶ�������
        case 0:
        {
            if(ZebraIndentify(80, 40, &black_width)==1)
            {
                NowFlag=LINGarageEntrance(InflectionL, InflectionR);
                *GarageLFlag=NowFlag;
                StatusChange=1;
            }
            break;
        }
        case 1:
        {
            NowFlag=LINGarageEntrance(InflectionL, InflectionR);
            *GarageLFlag=NowFlag;
            if(NowFlag==2)//flag=2��ʱ���ǳ����ſڵ��Ϲյ��Ѿ��������½�
            {
                return 1;
            }
            if(NowFlag==0)
            {
                Bias=4;
            }
            break;
        }
    }
    return 0;
}
/********************************************************************************************
 ** ��������: �ҳ��ⲻ���ʶ���ߺ���
 ** ��    ��: Point InflectionL:��յ�
 **           Point InflectionR:�ҹյ�
 ** �� �� ֵ: 0���Ѿ����˳�����
 **           1����δ���공��
 ** ��    ��: LJF
 ** ע    ��: �����������������ڲ��ߣ��ҵ��Ϲյ㣬����������ʶ��ʶ����Sobel��������Sobel����״̬���ĵ�һ���ж�
 *********************************************************************************************/
uint8 RNINGarageIdentify(Point InflectionL,Point InflectionR)
{
    //�ж����ߵĿ���Ƿ񳬹���ֵ�������Ͳ�����������
//    lcd_showuint8(0, 0, LeftLine[100]);
//    if(LeftLine[100]>45)
//    {
//        return 0;//����Ҫ����
//    }
    //ȷ������ɽ�����ӵı�����
    if(InflectionR.X==0)//�յ㲻����
    {
        if(BinaryImage[MT9V03X_H-5][MT9V03X_W-5]==IMAGE_WHITE)
        {
            InflectionR.X=MT9V03X_W-5;InflectionR.Y=bias_startline;//�������
        }
        else
        {
            InflectionR.X=RightLine[MT9V03X_H-5];InflectionR.Y=MT9V03X_H-5;
        }
    }
    //��߱���һ���ٴ���֤
    uint8 black_width=0;
    for(uint8 column=InflectionR.X;column>InflectionR.X-30;column--)
    {
        //30�������а�����˵�������ǹյ�
        if(BinaryImage[InflectionR.Y][column]==IMAGE_WHITE && BinaryImage[InflectionR.Y][column-1]==IMAGE_BLACK)
        {
            if(BinaryImage[MT9V03X_H-5][MT9V03X_W-5]==IMAGE_WHITE)
            {
                InflectionR.X=MT9V03X_W-5;InflectionR.Y=bias_startline;//�������
            }
            else
            {
                InflectionR.X=RightLine[MT9V03X_H-5];InflectionR.Y=MT9V03X_H-5;
            }
            break;
        }
    }
//    LcdDrawPoint_V2(InflectionR.Y, InflectionR.X, GREEN);
    //�ӱ����������Ұ�����,��ɽ�ߵ�or�ȱߵ�
    Point ValleySeed;
    ValleySeed.X=0;ValleySeed.Y=0;
    if(InflectionR.X!=0 && InflectionR.Y!=0)
    {
        for(uint8 row=InflectionR.Y-15;row>5;row--)
        {
//            gpio_toggle(LED_BLUE);
//            lcd_drawpoint(InflectionR.X, row, PURPLE);
            if(BinaryImage[row][InflectionR.X]==IMAGE_WHITE && BinaryImage[row-1][InflectionR.X]==IMAGE_BLACK)
            {
                ValleySeed.X=InflectionR.X;ValleySeed.Y=row-1;
                break;
            }
        }
    }
//    LcdDrawPoint_V2(ValleySeed.Y, ValleySeed.X, YELLOW);
    Point UpInflection;
    UpInflection.X=0;UpInflection.Y=0;
    //�����ҵ��˹ȱߵ㣬��ʼ����
    if(ValleySeed.X!=0 && ValleySeed.Y!=0)
    {
        SeedGrowFindValley_Garage('R', ValleySeed, 100, &UpInflection, 100);
    }
//    LcdDrawPoint_V2(UpInflection.Y, UpInflection.X, PINK);
    //�ҵ��յ�Ͳ���
    if(UpInflection.Y!=0)
    {
       FillingLine('R', InflectionR, UpInflection);
       //ƫ���
       if(UpInflection.Y<bias_endline)//starline<endline<Up.y��������ѭ��
       {
           Bias=DifferentBias_Garage(bias_startline,bias_endline,CentreLine);
       }
       else if(UpInflection.Y<bias_startline && bias_endline<UpInflection.Y)//starline<UP.y<endline,������ʼ�е��Ϲյ�
       {
           Bias=DifferentBias_Garage(bias_startline,UpInflection.Y,CentreLine);
       }
       else if(bias_startline<UpInflection.Y)//UP.y<starline<endline
       {
           Bias=DifferentBias_Garage(InflectionR.Y,UpInflection.Y,CentreLine);
       }
       return 1;
    }
    return 0;
}
/********************************************************************************************
 ** ��������: �ұ߳��ⲻ����״̬��ת��
 ** ��    ��: Point InflectionL�����¹յ�
 **           Point InflectionR: ���¹յ�
 **           uint8* GarageLFlag����״̬���ڵ�ʶ����������س�ȥ
 ** �� �� ֵ: 0��Ԫ��״̬û����
 **           1��Ԫ��״̬����
 ** ��    ��: LJF
 *********************************************************************************************/
uint8 RNINGarageStatusIdentify(Point InflectionL,Point InflectionR,uint8* GarageLFlag)
{
    static uint8 StatusChange;//��һ��ʶ��Ľ����״̬����
    uint8 no_effect;
    //��Ϊ�ҳ��ⲻ��ⲻ��Ҫ������Ҳ�����Դ˴���״̬�������ܿ���
    switch(StatusChange)
    {
        case 0:
        {
            //��⵽������֮��������������0.7M
            if(ZebraIndentify(65, 55,&no_effect)==1)
            {
                EncoderDistance(1, 0.7, 0, 0);
                StatusChange=1;
            }
            break;
        }
        case 1:
        {
            if(encoder_dis_flag==1)
            {
                StatusChange=0;//����״̬��
                return 1;
            }
            break;
        }
        default:break;
    }
    return 0;
}


/********************************************************************************************
 ** ��������: ��ɳ��⣨������
 ** ��    ��: ��
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 *********************************************************************************************/
void OutGarage(void)
{
    Bias=10;    //�������
    StartIntegralAngle_Z(45);
    while(!icm_angle_z_flag);   //��ת45���������Ѱ��
}
