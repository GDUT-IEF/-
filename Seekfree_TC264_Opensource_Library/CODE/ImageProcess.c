/*
 * ImageProcess.c
 *
 * Created on: 2022��3��29��
 * Author: 30516
 * Effect: Image element processing logic
 */
#include "ImageProcess.h"
#include "zf_gpio.h"

uint8 CrossRoads_flag=0;        //ʮ�ֱ�־����
uint8 Fork_flag=0;              //����ʶ��ı�־����
uint8 CircleIsland_flag=0;      //������־����
uint8 Garage_flag=0;            //����ʶ���־����
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
//    CrossLoopEnd_S();
    CrossLoopBegin_F(LeftLine, RightLine, LeftDownPoint, RightDownPoint);
//    if(SobelLCount<SobelLTestStop)
//    {
//        Garage_flag=GarageIdentify('L', LeftDownPoint, RightDownPoint);//ʶ�𳵿�
//        if(Garage_flag==0)//���ûʶ�𵽳��⣬�ټ���ʶ������������,����
//        {
//            Fork_flag=ForkIdentify(LeftLine, RightLine, LeftDownPoint, RightDownPoint);
//        }
//    }
//    else
//    {
//        gpio_toggle(P21_4);
//        Fork_flag=ForkIdentify(LeftLine, RightLine, LeftDownPoint, RightDownPoint);
//    }
//    lcd_showuint8(0, 0, Fork_flag);
//    if(Fork_flag!=0 || Garage_flag!=0)  //��ʶ���������Ѿ�������Bias
//    {
//        Garage_flag=0;Fork_flag=0;
//        gpio_toggle(P21_5);
//        return;
//    }
//    else
//    {
//        Bias=DifferentBias(100,60,CentreLine);//�����⴦��ʱ��ƫ�����
//    }
    /****************************״̬��***************************/
//    switch(flag)
//    {
//        case 0: //ʶ���󻷵�
//        {
//            gpio_set(LED_WHITE, 0);
//            if(CircleIslandIdentify_L(LeftLine, RightLine, LeftDownPoint, RightDownPoint)==9)
//            {
//                gpio_set(LED_WHITE, 1);
//                flag=1; //��ת��״̬1
//            }
//            break;
//        }
//        case 1: //ʶ���һ��ʮ�ֻػ�����
//        {
//            gpio_set(LED_GREEN, 0);
//            if(CrossLoopEnd_F()==1)
//            {
//                gpio_set(LED_GREEN, 1);
//                flag=2; //��ת��״̬2
//            }
//            break;
//        }
//        case 2: //ʶ���һ���
//        {
//            gpio_set(LED_BLUE, 0);
//            if(CircleIslandIdentify_R(LeftLine, RightLine, LeftDownPoint, RightDownPoint)==9)
//            {
//                gpio_set(LED_BLUE, 1);
//                flag=3; //��ת��״̬3
//            }
//            break;
//        }
//        case 3: //ʶ���󳵿⣬�����
//        {
//            flag=4; //����Ļ�ûд�ã�������
//            break;
//        }
//        case 4: //ʶ�������ڶ���ʮ�ֻػ�����
//        {
//            flag=5; //����ӳ�����߽��벻̫�ȶ�
//            break;
//        }
//        case 5://ʶ��������ҳ���
//        {
//            if(LostNum_RightLine>40 && LostNum_RightLine<90 && LostNum_LeftLine<10 && LostNum_LeftLine>0)
//            {
//                Garage_flag=GarageIdentify('R', LeftDownPoint, RightDownPoint);//ʶ�𳵿�
//                if(Garage_flag==0)//���ûʶ�𵽳��⣬�ټ���ʶ������������,����
//                {
//                    Fork_flag=ForkIdentify(LeftLine, RightLine, LeftDownPoint, RightDownPoint);
//                }
//            }
//            else
//            {
//                Fork_flag=ForkIdentify(LeftLine, RightLine, LeftDownPoint, RightDownPoint);
//            }
//        }
//    }
    /***************************ƫ�����**************************/
    Bias=DifferentBias(100,60,CentreLine);//�����⴦��ʱ��ƫ�����
//    lcd_showfloat(0, 0, Bias, 2, 2);
}

