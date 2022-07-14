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

#define L_FINDWHIDE_THRE  60 //Y�յ��м�����߰�ɫ����ֹͣ����ֵ
#define R_FINDWHIDE_THRE  100//Y�յ��м����ұ߰�ɫ����ֹͣ����ֵ
#define ROW_FINDWHIDE_THRE 100//Y�յ��е����޵���ֵ
#define FORK_INFLECTION_WIDTH  120//������debug,���յ���60�и������ҹյ�Ĳ�ֵ����ȫ��ʱ������û�ж�ʧ�Ĺյ��������һ���򵥵ı�����ϵ������ѭ��˼·��
#define FORK_DEBUG  0

extern uint8 bias_startline,bias_endline;        //��̬ǰհ

/*********************************************************************************
 ** ��������: �������������������ȵ�Ѱ��Y�Ϲյ�
 ** ��    ��:
 ** �� �� ֵ: ��
 ** ˵    ��:
 ** ��    ��: LJF
 **********************************************************************************/
void SeedGrowFindUpInflection(char Choose,Point Seed,int endline,Point *UpInflectionC)
{
    for(;Seed.Y<endline;)
    {
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
                }
                else if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_BLACK && BinaryImage[Seed.Y][Seed.X-1]==IMAGE_WHITE)
                {
                    Seed.Y++;
                }
                else if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_WHITE && BinaryImage[Seed.Y][Seed.X-1]==IMAGE_BLACK)
                {
                    Seed.X--;
                }
                else if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_WHITE && BinaryImage[Seed.Y][Seed.X-1]==IMAGE_WHITE)
                {
                    UpInflectionC->Y=Seed.Y,UpInflectionC->X=Seed.X;
                    return;
                }
                break;
            default:break;
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
    int row=0, cloumnL=0, cloumnR=0;
    Point Seed;
    char Choose=0,flagL=0,flagR=0;//�ж����ڹȵ���߻����ұߵĺ���,�Լ��ж�����������û�а�ɫ�����FLAG
    UpInflectionC->X = 0; UpInflectionC->Y = 0;//�Ϲյ�����
    UpInflectionC->X = (DownInflectionL.X + DownInflectionR.X) / 2;//V���Ϲյ��������Ϊ���ҹյ��ֵ
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
                lcd_drawpoint(cloumnL, row-1, PURPLE);
#endif
                if (BinaryImage[row - 1][cloumnL] == IMAGE_BLACK && BinaryImage[row - 1][cloumnL - 1] == IMAGE_WHITE)
                {
                    flagL = 1;
                    break;
                }
                if (cloumnL == L_FINDWHIDE_THRE + 1)//�����ʼ���о�С����11����ô�򲻻�return����ֱ�ӵ�����ĸ�ֵ
                {
                    Choose = 'R';//����Ҳ���˵���ڹȵ��ұ�
                    break;
                }
            }
            for (cloumnR = UpInflectionC->X; cloumnR < R_FINDWHIDE_THRE; cloumnR++)
            {
#if FORK_DEBUG
                lcd_drawpoint(cloumnR, row-1, PURPLE);
#endif
                if (BinaryImage[row - 1][cloumnR] == IMAGE_BLACK && BinaryImage[row - 1][cloumnR + 1] == IMAGE_WHITE)
                {
                    flagR = 1;
                    break;
                }
                if (cloumnR == R_FINDWHIDE_THRE - 1)
                {
                    Choose = 'L';//�ұ��Ҳ���˵���ڹȵ����
                    break;
                }
            }
            break;
        }
    }
    if ((flagL == 0 || flagR == 0)&&Choose!=0)//˵����һ����û�а�ɫ�����
    {
        Seed.X = UpInflectionC->X, Seed.Y = row - 1;
        SeedGrowFindUpInflection(Choose, Seed, ROW_FINDWHIDE_THRE, UpInflectionC);
    }
    else
    {
        UpInflectionC->Y = row - 1;
    }

//    int starline,i,cloumnL,cloumnR;
//    UpInflectionC->X=0;UpInflectionC->Y=0;//�Ϲյ�����
//    UpInflectionC->X=(DownInflectionL.X+DownInflectionR.X)/2;//V���Ϲյ��������Ϊ���ҹյ��ֵ����Ҫ�޸ģ���һ������������
//    starline=(DownInflectionL.Y+DownInflectionR.Y)/2;//��ʼ��Ϊ���ҹյ��еľ�ֵ
//    //���������ҵ��Ǹ�����ĵ㼴Ϊ�Ϲյ�
//    for(i=starline;i>20;i--)
//    {
//#if FORK_DEBUG
//        lcd_drawpoint(UpInflectionC->X, i, PURPLE);
//#endif
//        //ͼ��������[��][��]
//        if(BinaryImage[i][UpInflectionC->X]==IMAGE_WHITE && BinaryImage[i-1][UpInflectionC->X]==IMAGE_BLACK)
//        {
//            for(cloumnL=UpInflectionC->X;cloumnL>L_FINDWHIDE_THRE;cloumnL--)
//            {
//#if FORK_DEBUG
//                lcd_drawpoint(cloumnL, i-1, PURPLE);
//#endif
//                if(BinaryImage[i-1][cloumnL]==IMAGE_BLACK && BinaryImage[i-1][cloumnL-1]==IMAGE_WHITE)
//                    break;
//                if(cloumnL==L_FINDWHIDE_THRE+1)//�����ʼ���о�С����11����ô�򲻻�return����ֱ�ӵ�����ĸ�ֵ
//                    return;//�������˶�û���ҵ��׵ļ����������˳��ж�
//            }
//#if FORK_DEBUG
//            lcd_showint32(0, 3, BinaryImage[i+5][UpInflectionC->X+6], 3);
//#endif
//            //��ֹ��ʵ��Y�յ�ֻ������������в���,���յ��������·�����û�ҵ�
//            if(BinaryImage[i+6][UpInflectionC->X+20]==IMAGE_BLACK)
//            {
//                i=i+6;
//                UpInflectionC->X=UpInflectionC->X+20;
//            }
//            for(cloumnR=UpInflectionC->X;cloumnR<R_FINDWHIDE_THRE;cloumnR++)
//            {
//#if FORK_DEBUG
//                lcd_drawpoint(cloumnR, i-1, PURPLE);
//#endif
//                if(BinaryImage[i-1][cloumnR]==IMAGE_BLACK && BinaryImage[i-1][cloumnR+1]==IMAGE_WHITE)
//                    break;
//                if(cloumnR==R_FINDWHIDE_THRE-1)
//                    return;//�������˶�û���ҵ��׵ļ����������˳��ж�
//            }
//            UpInflectionC->Y=i;//Y����������
//            return;
//        }
//    }
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
    Point UpInflectionC;
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
                //�ж������߶��ϵ����Ƿ�ɨ�����ұ���������û��������Ѳ��
                if(abs(CentreLine[UpInflectionC.Y]-CentreLine[UpInflectionC.Y-1])>50)
                {
                    Bias=DifferentBias(bias_startline,UpInflectionC.Y,CentreLine);
                }
                else
                {
                    Bias=DifferentBias(bias_startline,bias_endline,CentreLine);
                }
                gpio_set(LED_BLUE, 0);
                return 1;//�����յ���������������������
            }
        }
        else
            return 0;
    }
    else if((DownInflectionL.X==0 && DownInflectionR.X==0) || (BinaryImage[MT9V03X_H-5][5]==IMAGE_WHITE && BinaryImage[MT9V03X_H-5][MT9V03X_W-5]==IMAGE_WHITE))//��������¹յ㲻���ڲ�������һ�γ��־Ͷ��ߵĻ��Ļ�,���Ǿ�ȥ���治�������ϵĹյ�
    {
        Point ImageDownPointL,ImageDownPointR;//�Ի�������½Ǻ����½���Ϊ���Ҳ��ߵĵ�
        //���Ϲյ����һ��Ԥ���ֻ��������д��Ϊ��Ļ�м�
        if(LeftLine[MT9V03X_H-1]!=0)//����������һ��û�ж���
            ImageDownPointL.X=LeftLine[MT9V03X_H-1]+10,ImageDownPointL.Y=MT9V03X_H;
        else ImageDownPointL.X=0,ImageDownPointL.Y=MT9V03X_H;
        if(RightLine[MT9V03X_H-1]!=MT9V03X_W-1)
            ImageDownPointR.X=RightLine[MT9V03X_H-1]-10,ImageDownPointR.Y=MT9V03X_H;
        else ImageDownPointR.X=MT9V03X_W-1,ImageDownPointR.Y=MT9V03X_H;
        //��Ѱ�Ϲյ�
        GetForkUpInflection(ImageDownPointL, ImageDownPointR, &UpInflectionC);
        if(UpInflectionC.Y!=0 && UpInflectionC.Y>40)//ֱ�ӷ���Y���ɣ��ӿ��ٶȣ���ΪXĬ�Ͼͻḳֵ��
        {
            FillingLine('R',ImageDownPointR,UpInflectionC);//��������˾��ڷ���֮ǰ����
            //�ж������߶��ϵ����Ƿ�ɨ�����ұ���������û��������Ѳ��
            if(abs(CentreLine[UpInflectionC.Y]-CentreLine[UpInflectionC.Y-1])>50)
            {
                Bias=DifferentBias(bias_startline,UpInflectionC.Y,CentreLine);
            }
            else
            {
                Bias=DifferentBias(bias_startline,bias_endline,CentreLine);
            }
            gpio_set(LED_GREEN, 0);
            return 1;//�������붪ʧ���ҹյ���һ֡
        }
    }
    //�ұ߶��߳���60����յ���ڣ�������յ㲻�����ϰ�ƽ����ֹ����
    else if(LostNum_RightLine>55 && DownInflectionL.X!=0 && DownInflectionL.Y>40)
    {
        Point ImageDownPointR;//����յ�ԳƵĵ�ȥ���ߺ��ҹյ�
        //���Լ��趨���ҹյ�ȥ���Ϲյ�
//        ImageDownPointR.X=MT9V03X_W-1,ImageDownPointR.YDownInflectionL.Y=DownInflectionL.Y;
        //���õ���ѭ������˼����յ㣬������ȣ���б���Ϲյ��ұߵĶ�����һ��
        ImageDownPointR.X=DownInflectionL.X+(145-(119-DownInflectionL.Y)*1.1);ImageDownPointR.Y=DownInflectionL.Y;
        GetForkUpInflection(DownInflectionL, ImageDownPointR, &UpInflectionC);
        if(UpInflectionC.Y!=0)//ֱ�ӷ���Y���ɣ��ӿ��ٶȣ���ΪXĬ�Ͼͻḳֵ��
        {
            FillingLine('R',ImageDownPointR,UpInflectionC);//��������˾��ڷ���֮ǰ����
            //�ж������߶��ϵ����Ƿ�ɨ�����ұ���������û��������Ѳ��
            if(abs(CentreLine[UpInflectionC.Y]-CentreLine[UpInflectionC.Y-1])>50)
            {
                Bias=DifferentBias(bias_startline,UpInflectionC.Y,CentreLine);
            }
            else
            {
                Bias=DifferentBias(bias_startline,bias_endline,CentreLine);
            }
            gpio_set(LED_RED, 0);
            return 1;//������б������
        }
    }
    //��߶��߳���60,�ҹյ����,�����ҹյ㲻�����ϰ�ƽ����ֹ����
    else if(LostNum_LeftLine>55 && DownInflectionR.X!=0 && DownInflectionR.Y>40)
    {
        Point ImageDownPointL;//����յ�ԳƵĵ�ȥ���ߺ��ҹյ�
        //��յ�������һ��������ϵ��Խ�����ײ��˹յ���Խ��.��б������һ��
//        ImageDownPointL.X=DownInflectionR.X-(145-(119-DownInflectionL.Y)*1.1);ImageDownPointL.Y=DownInflectionR.Y;
        ImageDownPointL.X=DownInflectionR.X-FORK_INFLECTION_WIDTH;ImageDownPointL.Y=DownInflectionR.Y;
        GetForkUpInflection(ImageDownPointL, DownInflectionR, &UpInflectionC);
        if(UpInflectionC.Y!=0)//ֱ�ӷ���Y���ɣ��ӿ��ٶȣ���ΪXĬ�Ͼͻḳֵ��
        {
            FillingLine('R',DownInflectionR,UpInflectionC);//��������˾��ڷ���֮ǰ����
            //�ж������߶��ϵ����Ƿ�ɨ�����ұ���������û��������Ѳ��
            if(abs(CentreLine[UpInflectionC.Y]-CentreLine[UpInflectionC.Y-1])>50)
            {
                Bias=DifferentBias(bias_startline,UpInflectionC.Y,CentreLine);
            }
            else
            {
                Bias=DifferentBias(bias_startline,bias_endline,CentreLine);
            }
            gpio_set(LED_WHITE, 0);
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

