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
//    CrossLoopEnd(LeftLine, RightLine);
    /****************************״̬��***************************/
    switch(flag)
    {
        case 0: //ʶ���󻷵�
        {
            gpio_set(P21_4, 0);
            gpio_set(P21_5, 1);
            gpio_set(P20_9, 1);
            if(CircleIslandIdentify_L(LeftLine, RightLine, LeftDownPoint, RightDownPoint)==9)
            {
                flag=1; //��ת��״̬1
            }
            break;
        }
        case 1: //ʶ��ʮ�ֻػ�����
        {
            gpio_set(P21_4, 1);
            gpio_set(P21_5, 0);
            gpio_set(P20_9, 1);
            if(CrossLoopEnd(LeftLine, RightLine)==1)
            {
                flag=2; //��ת��״̬2
            }
            break;
        }
        case 2: //ʶ���һ���
        {
            gpio_set(P21_4, 1);
            gpio_set(P21_5, 1);
            gpio_set(P20_9, 0);
            if(CircleIslandIdentify_R(LeftLine, RightLine, LeftDownPoint, RightDownPoint)==9)
            {
                flag=3; //��ת��״̬3
            }
            break;
        }
        case 3: //ʶ���󳵿⣬�����
        {

            break;
        }
    }
    /***************************ƫ�����**************************/
    Bias=DifferentBias(100,60,CentreLine);//�����⴦��ʱ��ƫ�����
}

