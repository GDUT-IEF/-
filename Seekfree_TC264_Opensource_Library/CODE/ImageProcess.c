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
    Fork_flag=ForkIdentify(LeftLine,RightLine,LeftDownPoint,RightDownPoint);  //����
    if(Fork_flag==0)
    {
        CrossRoads_flag=CrossRoadsIdentify(LeftLine,RightLine,LeftDownPoint,RightDownPoint);//ʮ��
        if(CrossRoads_flag==0||CircleIsland_flag!=0)    //ʶ�𲻵�ʮ�ֻ򻷵�������״̬0
        {
            gpio_set(P20_9,0);
            CircleIsland_flag=CircleIslandIdentify(LeftLine, RightLine, LeftDownPoint, RightDownPoint); //����
        }
        else
        {
            gpio_set(P20_9,1);
        }
    }
//    gpio_set(P21_5, 1);
//    int64 Sobel=SobelTest();
//    if(Sobel>3900)  //ʶ�𵽳���
//    {
//        //���ｫ����Ҵ���������
////        Bias=-100;
////        systick_delay_ms(STM0,500);
//        gpio_set(P21_5, 0);
//    }

    /***************************ƫ�����**************************/
    if(Fork_flag!=0)  //��ʶ���������Ѿ�������Bias
    {
        return;
    }
    else
    {
        Bias=DifferentBias(100,60,CentreLine);//�����⴦��ʱ��ƫ�����
    }
}

