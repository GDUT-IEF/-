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

uint8 CrossRoads_flag=0;        //ʮ�ֱ�־����
uint8 Fork_flag=0;              //����ʶ��ı�־����
uint8 CircleIsland_flag=0;      //������־����
uint8 Garage_flag=0;            //����ʶ���־����
uint8 speed_case_1=200,speed_case_2=150,speed_case_3=130,speed_case_4=155,speed_case_5=145,speed_case_6=160,speed_case_7=135;

uint32 SobelResult=0;

void Stop(void)
{
    while(1)
    {
        base_speed=0;
        diff_speed_kp=0;
    }
}

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
    static uint8 flag;
    static uint8 case_5,case_0,case_2,case_1,case_4,case_6,case_3;
    Point LeftDownPoint,RightDownPoint;     //�����¹յ�
    LeftDownPoint.X=0;LeftDownPoint.Y=0;RightDownPoint.X=0;RightDownPoint.Y=0;
    Point ForkUpPoint;
    ForkUpPoint.X=0;ForkUpPoint.Y=0;
    Point CrossRoadUpLPoint,CrossRoadUpRPoint;
    CrossRoadUpLPoint.X=0;CrossRoadUpLPoint.Y=0;CrossRoadUpRPoint.X=0;CrossRoadUpRPoint.Y=0;
    /*****************************ɨ��*****************************/
    GetImagBasic(LeftLine,CentreLine,RightLine);
    /*************************��Ѱ�����¹յ�***********************/
    GetDownInflection(110,45,LeftLine,RightLine,&LeftDownPoint,&RightDownPoint);
    /*************************����Ԫ���ж�*************************/
//    CircleIsFlag_1_L(LeftLine, RightLine, LeftDownPoint, RightDownPoint);
//    CircleIsFlag_1_1_L(LeftLine, RightLine);
    /****************************״̬��***************************/
#if 1
    switch(flag)
    {
        case 0: //ʶ���󻷵�
        {
//            flag=2; //�����ã���ת��ָ��״̬
            if(case_0<200)  //�������ʱһ���ٿ�����һ��Ԫ�ص�ʶ�𣬷�ֹ����
            {
                case_0++;
                break;
            }
            gpio_set(LED_WHITE, 0);
            if(CircleIslandIdentify_L(LeftLine, RightLine, LeftDownPoint, RightDownPoint)==1)
            {
                gpio_set(LED_WHITE, 1);
                base_speed=speed_case_1; //�������½����һ��ʮ�ֻػ�
                flag=1;         //��ת��״̬1
            }
            break;
        }
        case 1: //ʶ���һ��ʮ�ֻػ�
        {
            if(case_1<100)   //��ʱһ���ٽ���ʮ���ж�
            {
                case_1++;
                break;
            }
            gpio_set(LED_GREEN, 0);
            if(CrossLoopEnd_F()==1)
            {
                gpio_set(LED_GREEN, 1);
                base_speed=speed_case_2; //�������½����һ���
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
                if(CircleIsFlag_3_L()==1)
                {
                    base_speed=120;//�����뻷��Ϊ������׼��
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
                base_speed=speed_case_2;    //����case2����
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
                diff_speed_kp=0.1;  //������ٹ�U����
                base_speed=speed_case_5; //���ٽ���ڶ���ʮ�ֻػ�
                flag=5;         //��ת��״̬5
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
                diff_speed_kp=0.05; //���ٸĻ�ȥ
                case_5++;
            }
            gpio_set(P21_4, 0);
            if(CrossLoopEnd_S()==1)
            {
                gpio_set(P21_4, 1);
                base_speed=speed_case_6; //���ٽ�������
                flag=6;         //��ת��״̬6
            }
            else
            {
               CrossLoopBegin_S(LeftLine, RightLine, LeftDownPoint, RightDownPoint);
               if(CircleIsFlag_3_L()==1)
               {
                   base_speed=130;//�����뻷��Ϊ������׼��
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
                MotorK.P=15;    //�����Ӧ�ٶ�
                MotorK.I=1.2;
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
    if(Fork_flag!=0 || Garage_flag!=0)  //��ʶ���������Ѿ�������Bias
    {
        Garage_flag=0;Fork_flag=0;
        return;
    }
    else
    {
        Bias=DifferentBias(95,50,CentreLine);//�����⴦��ʱ��ƫ�����
//        lcd_showfloat(0, 7, Bias, 2, 3);
    }
}
