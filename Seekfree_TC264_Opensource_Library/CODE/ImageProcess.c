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
#include "zf_ccu6_pit.h"

uint8 bias_startline=95,bias_endline=50;        //��̬ǰհ
uint8 Fork_flag=0;              //����ʶ��ı�־����
uint8 Garage_flag=0;            //����ʶ���־����
uint8 speed_case_1=200,speed_case_2=170,speed_case_3=155,speed_case_4=165,speed_case_5=160,speed_case_6=160,speed_case_7=170;
uint32 SobelResult=0;
int LeftLine[MT9V03X_H]={0}, CentreLine[MT9V03X_H]={0}, RightLine[MT9V03X_H]={0};   //ɨ�ߴ�������������
uint8 stop_flag=0;

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
    static uint8 flag=4;  //״̬����ת��־λ
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
//    CircleIslandIdentify_R(RightLine, InflectionR);
    /****************************״̬��***************************/
#if 0
    switch(flag)
    {
        case 0: //ʶ����ʮ�ֻػ�
        {

            break;
        }
        case 1: //ʶ���ҳ��⣬ֱ��
        {

            break;
        }
        case 2: //ʶ���һ������
        {

            break;
        }
        case 3: //ʶ���һ���
        {
            if(CircleIslandIdentify_R(RightLine, InflectionR)==1)
            {
                flag=4;
            }
            break;
        }
        case 4: //ʶ����ʮ�ֻػ�
        {
            if(CrossLoopIdentify_R(LeftLine, RightLine, InflectionL, InflectionR)==1)
            {
                flag=5;
                gpio_set(LED_GREEN, 0);
            }
            break;
        }
        case 5: //ʶ���󻷵�
        {
            if(CircleIslandIdentify_L(LeftLine, InflectionL)==1)
            {
                flag=6;
            }
            break;
        }
        case 6: //ʶ��ڶ�������
        {
            Stop();
            break;
        }
        case 7: //ʶ���󳵿⣬���
        {

            break;
        }
    }
#endif
    /***************************ƫ�����**************************/
    if(Fork_flag!=0||Garage_flag!=0)    //��ʶ���������Ѿ�������Bias
    {
        Garage_flag=0;Fork_flag=0;      //����flag
    }
    else
    {
        Bias=DifferentBias(bias_startline,bias_endline,CentreLine); //��̬ǰհ����ƫ��
        bias_startline=95;bias_endline=50;                          //�ָ�Ĭ��ǰհ
    }
    //LCD����ͼ��
#if 0
    for(uint8 i=0;i<MT9V03X_W-1;i++)
    {
        lcd_drawpoint(i, bias_startline, YELLOW);
        lcd_drawpoint(i, bias_endline, YELLOW);
    }
    lcd_showfloat(0, 0, Bias, 1, 2);
#endif
}

/*
 *******************************************************************************************
 ** ��������: ͣ����ʹ��PID������ٶȽ�Ϊ0���رն���ж�
 ** ��    ��: ��InflectionL�����¹յ�
 **           InflectionR�����¹յ�
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 ********************************************************************************************
 */
void Stop(void)
{
    base_speed=-5;   //����Ŀ���ٶ�Ϊ0
    stop_flag=1;     //��������ͣ����ֹ����³�����
    pit_disable_interrupt(CCU6_0, PIT_CH1); //�رն���ж�
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
    for(uint8 column=0;column<MT9V03X_W-1;column++)
    {
        lcd_drawpoint(column,Inflection.Y,color);
    }
    for(uint8 row=0;row<MT9V03X_H-1;row++)
    {
        lcd_drawpoint(Inflection.X,row,color);
    }
}

/*
 *******************************************************************************************
 ** ��������: ��xy�ύ��ķ�ʽ����LCD�϶�λһ������
 ** ��    ��:
 **           color����ɫ
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 ********************************************************************************************
 */
void LcdDrawPoint_V2(uint8 row,uint8 column,uint16 color)
{
    for(uint8 cloum=0;cloum<MT9V03X_W-1;cloum++)
    {
        lcd_drawpoint(cloum,row,color);
    }
    for(uint8 row=0;row<MT9V03X_H-1;row++)
    {
        lcd_drawpoint(column,row,color);
    }
}

/*
 *******************************************************************************************
 ** ��������: ��һ����ֱ��Y��ֱ�ߵķ�ʽ���Ƴ�Y����
 ** ��    ��:
 **           color����ɫ
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 ********************************************************************************************
 */
void LcdDrawRow(uint8 row,uint16 color)
{
    for(uint8 column=0;column<MT9V03X_W-1;column++)
    {
        lcd_drawpoint(column,row,color);
    }
}

/*
 *******************************************************************************************
 ** ��������: ��һ����ֱ��X��ֱ�ߵķ�ʽ���Ƴ�X����
 ** ��    ��:
 **           color����ɫ
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 ********************************************************************************************
 */
void LcdDrawColumn(uint8 column,uint16 color)
{
    for(uint8 row=0;row<MT9V03X_H-1;row++)
    {
        lcd_drawpoint(column,row,color);
    }
}
