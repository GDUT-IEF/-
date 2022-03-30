/*
 * ImageProcess.c
 *
 * Created on: 2022��3��29��
 * Author: 30516
 * Effect: Image element processing logic
 */
#include "ImageProcess.h"
#include "zf_gpio.h"

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
    uint8 CrossRoads_flag=0;        //ʮ�ֱ�־����
    uint8 Fork_flag=0;              //����ʶ��ı�־����
    uint8 CircleIsland_flag=0;      //������־����
    /******************************ɨ��***************************/
    GetImagBasic(LeftLine,CentreLine,RightLine);
    /*************************��Ѱ�����¹յ�***********************/
    GetDownInflection(110,10,LeftLine,RightLine,&LeftDownPoint,&RightDownPoint);
    /*****************************����Ԫ���ж�********************************/
    if(!CrossRoadsIdentify(LeftLine,RightLine,LeftDownPoint,RightDownPoint))        //ʮ��
    {
        Fork_flag=ForkIdentify(100,40,LeftLine,RightLine,LeftDownPoint,RightDownPoint);  //����
    }
    CircleIslandIdentify(LeftLine, RightLine, LeftDownPoint, RightDownPoint); //����

//    CircleIsFlag_2(LeftLine, RightLine, LeftDownPoint, RightDownPoint);
//    CircleIslandBegin(LeftLine, RightLine);

//    CircleIsFlag_3(LeftLine, RightLine);
//    CircleIsFlag_2(LeftLine, RightLine, LeftDownPoint, RightDownPoint);

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

