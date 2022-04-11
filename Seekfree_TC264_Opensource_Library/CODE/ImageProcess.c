/*
 * ImageProcess.c
 *
 * Created on: 2022��3��29��
 * Author: 30516
 * Effect: Image element processing logic
 */
#include "ImageProcess.h"
#include "zf_gpio.h"

#define FastABS(x) (x > 0 ? x : x * -1.0f)
#define BinaryImage(i, j)    BinaryImage[i][j]

int64 SobelTest()
{
    int64 Sobel = 0;
    int64 temp = 0;
    for (uint8 i = MT9V03X_H-1-20; i > 20 ; i--)
    {
        for (uint8 j = 20; j < MT9V03X_W-1-20; j++)
        {
            int64 Gx = 0, Gy = 0;
            Gx = (-1*BinaryImage(i-1, j-1) + BinaryImage(i-1, j+1) - 2*BinaryImage(i, j-1)
                  + 2*BinaryImage(i, j+1) - BinaryImage(i+1, j-1) + BinaryImage(i+1, j+1));
            Gy = (-1 * BinaryImage(i-1, j-1) - 2 * BinaryImage(i-1, j) - BinaryImage(i-1, j+1)
                  + BinaryImage(i+1, j+1) + 2 * BinaryImage(i+1, j) + BinaryImage(i+1, j+1));
            temp += FastABS(Gx) + FastABS(Gy);
            Sobel += temp / 255;
            temp = 0;
        }
    }
    return Sobel;
}

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

//    GarageIdentify(LeftLine, RightLine, LeftDownPoint, RightDownPoint);

//    int64 Sobel=SobelTest();
//    lcd_showint32(0, 0, Sobel, 5);
//    printf("%d\r\n",Sobel);

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

