/*
 * ImageProcess.c
 *
 * Created on: 2022��3��29��
 * Author: 30516
 * Effect: Image element processing logic
 */
#include "ImageProcess.h"
#include "zf_gpio.h"
#include "PID.h"
#include "Motor.h"
#include "LED.h"
#include <stdio.h>

uint8 bias_startline=95,bias_endline=50;        //��̬ǰհ
uint8 Fork_flag=0;              //����ʶ��ı�־����
uint8 Garage_flag=0;            //����ʶ���־����
uint8 CrossLoop_flag=0;         //ʮ�ֻػ�ʶ���־����
uint8 CircleIsland_flag=0;      //����ʶ���־����
uint8 speed_case_1=200,speed_case_2=170,speed_case_3=155,speed_case_4=165,speed_case_5=160,speed_case_6=160,speed_case_7=170;

uint32 SobelResult=0;
int LeftLine[MT9V03X_H]={0}, CentreLine[MT9V03X_H]={0}, RightLine[MT9V03X_H]={0};   //ɨ�ߴ�������������

/********************************************************************************************
 ** ��������: ��ͼ��ĸ���Ԫ��֮����߼�������������Ŀ����Ϊ�˵ó�Bias���ж�ȥ����
 ** ��    ��: ��
 ** �� �� ֵ: ��
 ** ��    ��: LJF
 ** ע    �⣺��
 *********************************************************************************************/
void ImageProcess()
{
    /***************************��������****************************/
    static uint8 flag;  //״̬����ת��־λ
    static uint8 case_5,case_0,case_2,case_1,case_4,case_6,case_3;  //��֡��
    Point InflectionL,InflectionR;     //�����¹յ�
    InflectionL.X=0;InflectionL.Y=0;InflectionR.X=0;InflectionR.Y=0;
    Point ForkUpPoint;  //����
    ForkUpPoint.X=0;ForkUpPoint.Y=0;
    Point CrossRoadUpLPoint,CrossRoadUpRPoint;  //ʮ��·��
    CrossRoadUpLPoint.X=0;CrossRoadUpLPoint.Y=0;CrossRoadUpRPoint.X=0;CrossRoadUpRPoint.Y=0;
    /*****************************ɨ��*****************************/
    GetImagBasic(LeftLine, CentreLine, RightLine, 'L');
    /*************************��Ѱ�����¹յ�***********************/
    GetDownInflection(110,45,LeftLine,RightLine,&InflectionL,&InflectionR);
    /*************************����Ԫ���ж�*************************/
    CircleIslandExit_L(InflectionL);
//    CircleIslandOverExit_L(LeftLine, RightLine);
    /****************************״̬��***************************/
#if 0
    switch(flag)
    {
        case 0: //ʶ���󻷵�
        {
//            flag=4; //�����ã���ת��ָ��״̬
            if(case_0<165)  //�������ʱһ���ٿ�����һ��Ԫ�ص�ʶ�𣬷�ֹ���У���Ӧ�ٶ�180
            {
                case_0++;
                break;
            }
            gpio_set(LED_WHITE, 0);
            if(CircleIslandIdentify_L(LeftLine, RightLine, LeftDownPoint, RightDownPoint)==1)
            {
                gpio_set(LED_WHITE, 1);
                flag=1;         //��ת��״̬1
            }
            break;
        }
        case 1: //ʶ���һ��ʮ�ֻػ�
        {
            if(case_1<90)   //��ʱһ���ٽ���ʮ���ж�
            {
                CircleIslandOverBegin_L(LeftLine, RightLine);   //��ֹ�󻷵������״̬�ٴι��뻷��
                if(case_1>10)   //��ʱ�������£������ӵ�����̬��ʱ��
                {
                    base_speed=speed_case_1;
                }
                case_1++;
                break;
            }
            gpio_set(LED_GREEN, 0);
            if(CrossLoopEnd_F()==1)
            {
                gpio_set(LED_GREEN, 1);
                base_speed=speed_case_2; //�������½����һ���
                bias_startline=95;       //�����ָ���̬ǰհ
                flag=2;         //��ת��״̬2
            }
            else
            {
                if(CrossLoopBegin_F(LeftLine, RightLine, LeftDownPoint, RightDownPoint)==1)
                {
                    if(case_1==30)  //ֻ����һ��
                    {
                        case_1++;
                        base_speed=150; //�ֶμ���
                    }
                }
            }
            break;
        }
        case 2: //ʶ���һ���
        {
            if(case_2<100)   //��ʱ����ʶ��
            {
                case_2++;
                break;
            }
            gpio_set(LED_BLUE, 0);
            if(CircleIslandIdentify_R(LeftLine, RightLine, LeftDownPoint, RightDownPoint)==1)
            {
                gpio_set(LED_BLUE, 1);
                flag=3;          //��ת��״̬3
            }
            break;
        }
        case 3: //ʶ���󳵿�
        {
            if(case_3<160)//֡�ʴ�50���100������֡��ҲҪ�����������Ǵ�S
            {
                case_3++;
                break;
            }
            base_speed=speed_case_3;  //���ٽ����󳵿�
            gpio_set(LED_RED, 0);
            if(LostNum_LeftLine>40 && LostNum_RightLine<30)
            {
                Garage_flag=GarageIdentify('L', LeftDownPoint, RightDownPoint);//ʶ�𳵿�
            }
            if(GarageLStatusIdentify(LeftDownPoint, RightDownPoint,Garage_flag)==1)
            {
                gpio_set(LED_RED, 1);
                flag=4;          //��ת��״̬4
            }
            break;
        }
        case 4: //ʶ������
        {
            if(case_4<50)    //�ӳٷ�ֹ����
            {
                case_4++;
                break;
            }
            base_speed=speed_case_4;  //���ٽ�������
            gpio_set(LED_YELLOW, 0);
            Fork_flag=ForkIdentify(LeftLine, RightLine, LeftDownPoint, RightDownPoint);   //��ȡ����״̬
            if(ForkFStatusIdentify(LeftDownPoint, RightDownPoint,Fork_flag)==1)
            {
                gpio_set(LED_YELLOW, 1);
                base_speed=speed_case_5; //���ٽ���ڶ���ʮ�ֻػ�
                flag=5;         //��ת��״̬5
                SteerK.D=0;
            }
            break;
        }
        case 5: //ʶ��ڶ���ʮ�ֻػ�
        {
            if(case_5<110)  //�����������ʱһ���ٿ�����һ��Ԫ�ص�ʶ�𣬷�ֹ����
            {
                case_5++;
                break;
            }
            if(case_5==110)
            {
                case_5++;
            }
            gpio_set(P21_4, 0);
            if(CrossLoopEnd_S()==1)
            {
                gpio_set(P21_4, 1);
                base_speed=speed_case_6; //���ٽ�������
                bias_startline=95;       //�����ָ���̬ǰհ
                SteerK.D=5;
                flag=6;         //��ת��״̬6
            }
            else
            {
               CrossLoopBegin_S(LeftLine, RightLine, LeftDownPoint, RightDownPoint);
               if(CrossLoopIn_S()==1)
               {
                   base_speed=160;     //�뻷���٣�Ϊ������׼��
                   bias_startline=100; //�뻷������̬ǰհ
               }
            }
            break;
        }
        case 6: //ʶ��ڶ�������
        {
            if(case_6<90)  //����ʮ�ֻػ�����ʱһ���ٿ�����һ��Ԫ�ص�ʶ�𣬷�ֹS�����г��������
            {
                case_6++;
                break;
            }
            gpio_set(P21_5, 0);
            Fork_flag=ForkIdentify(LeftLine, RightLine, LeftDownPoint, RightDownPoint);   //��ȡ����״̬
            if(ForkSStatusIdentify(LeftDownPoint, RightDownPoint,Fork_flag)==1)
            {
                gpio_set(P21_5, 1);
                base_speed=speed_case_7; //����׼�����
                flag=7;         //��ת��״̬7
            }
            break;
        }
        case 7: //ʶ���ҳ��⣬���
        {
            gpio_set(P20_9, 0);
            Garage_flag=GarageIdentify('R', LeftDownPoint, RightDownPoint);//ʶ�𳵿�
            break;
        }
    }
#endif
    /***************************ƫ�����**************************/
    if(Fork_flag!=0||Garage_flag!=0||CrossLoop_flag!=0||CircleIsland_flag!=0)  //��ʶ���������Ѿ�������Bias
    {
        Garage_flag=0;Fork_flag=0;CrossLoop_flag=0;CircleIsland_flag=0;
        return;
    }
    else
    {
        Bias=DifferentBias(bias_startline,bias_endline,CentreLine);//�����⴦��ʱ��ƫ�����
    }
//    //LCD����ͼ��
//    for(uint8 i=0;i<MT9V03X_W-1;i++)
//    {
//        lcd_drawpoint(i, bias_startline, RED);
//        lcd_drawpoint(i, bias_endline, RED);
//    }
}

/*
 *******************************************************************************************
 ** ��������: ͣ��
 ** ��    ��: ��InflectionL�����¹յ�
 **           InflectionR�����¹յ�
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 ********************************************************************************************
 */
void Stop(void)
{
    while(1)
    {
//        base_speed=0;
        encoder_dis_flag=1; //Ϊ�˱�����絼��MCU������������Ȳ�ʹ��PIDͣ��
    }
}

/*
 *******************************************************************************************
 ** ��������: ��xy�ύ��ķ�ʽ����LCD�϶�λһ������
 ** ��    ��: Inflection������
 **           color����ɫ
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 ********************************************************************************************
 */
void LcdDrawPoint(Point Inflection,uint16 color)
{
    for(int cloum=0;cloum<MT9V03X_W-1;cloum++)
    {
        lcd_drawpoint(cloum,Inflection.Y,color);
    }
    for(int row=0;row<MT9V03X_H-1;row++)
    {
        lcd_drawpoint(Inflection.X,row,color);
    }
}
