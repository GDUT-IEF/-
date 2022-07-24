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
#include "ICM20602.h"
#include "Key.h"

#define STATE_LED_DEBUG 0

uint8 bias_startline=95,bias_endline=50;        //��̬ǰհ
uint8 Fork_flag=0;              //����ʶ��ı�־����
uint8 Garage_flag=0;            //����ʶ���־����
uint8 Circle_flag=0;            //����Ѱ����־����
uint8 speed_case_1=200,speed_case_2=220,speed_case_3=200,speed_case_4=200,speed_case_5=200,speed_case_6=200,speed_case_7=200;
uint32 SobelResult=0;
int LeftLine[MT9V03X_H]={0}, CentreLine[MT9V03X_H]={0}, RightLine[MT9V03X_H]={0};   //ɨ�ߴ�������������
uint8 process_flag=0;   //״̬����ת��־

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
    static uint8 encoder_flag,icm_flag;  //�Ƿ��ѯ��������flag,�Ƿ��ѯ�����ǵ�flag
    Point InflectionL,InflectionR;     //�����¹յ�
    InflectionL.X=0;InflectionL.Y=0;InflectionR.X=0;InflectionR.Y=0;
    /*****************************ɨ��*****************************/
    if(process_flag==1)
    {
        GetImagBasic_Garage(LeftLine, CentreLine, RightLine, 'L');
    }
    else
    {
        GetImagBasic(LeftLine, CentreLine, RightLine, 'L');
    }
    /*************************��Ѱ�����¹յ�***********************/
    GetDownInflection(110,45,LeftLine,RightLine,&InflectionL,&InflectionR);
    /*************************����Ԫ���ж�*************************/
#if IMAGE_KEY_DEBUG
    ImageParameterHandle(key_num_1, key_num_2, InflectionL, InflectionR);   //������ʾͼ��Debug
#endif
    /****************************״̬��***************************/
#if !IMAGE_KEY_DEBUG
    switch(process_flag)
    {
        case 0: //ʶ����ʮ�ֻػ�
        {
#if STATE_LED_DEBUG
            gpio_set(LED_RED, 0);
#endif
            if(CrossLoopIdentify_L(LeftLine, RightLine, InflectionL, InflectionR)==1)
            {
#if STATE_LED_DEBUG
                gpio_set(LED_RED, 1);
#endif
                base_speed=speed_case_1;
                process_flag=1;
            }
            break;
        }
        case 1: //ʶ���ҳ��⣬ֱ��
        {
#if STATE_LED_DEBUG
            gpio_set(LED_YELLOW, 0);
#endif
            if(RNINGarageStatusIdentify(InflectionL, InflectionR, &Garage_flag)==1)
            {
#if STATE_LED_DEBUG
                gpio_set(LED_YELLOW, 1);
#endif
                EncoderDistance(1, 1.6, 0, 0);//�˴�Ϊ����ͨ��������ֹ��������
                encoder_flag=1;
                base_speed=speed_case_2;
                process_flag=2;
            }
            break;
        }
        case 2: //ʶ���һ������
        {
            if(encoder_flag==1)//��ѯ���������ȱ�����״̬���˲���ת
            {
                if(encoder_dis_flag==1)//�˴��궨���������
                {
                    encoder_flag=0;
                }
                break;
            }
#if STATE_LED_DEBUG
                    gpio_set(LED_WHITE, 0);
#endif
            if(ForkFStatusIdentify(InflectionL, InflectionR, &Fork_flag)==1)
            {
#if STATE_LED_DEBUG
                gpio_set(LED_WHITE, 1);
#endif
                base_speed=speed_case_3;
                process_flag=3;
            }
            break;
        }
        case 3: //ʶ���һ���
        {
#if STATE_LED_DEBUG
            gpio_set(LED_BLUE, 0);
#endif
            if(CircleIslandIdentify_R(RightLine, InflectionR)==1)
            {
#if STATE_LED_DEBUG
                gpio_set(LED_BLUE, 1);
#endif
                base_speed=speed_case_4;
                process_flag=4;
            }
            break;
        }
        case 4: //ʶ����ʮ�ֻػ�
        {
#if STATE_LED_DEBUG
                gpio_set(LED_GREEN, 0);
#endif
            if(CrossLoopIdentify_R(LeftLine, RightLine, InflectionL, InflectionR)==1)
            {
#if STATE_LED_DEBUG
                gpio_set(LED_GREEN, 1);
#endif
                base_speed=speed_case_5;
                process_flag=5;
            }
            break;
        }
        case 5: //ʶ���󻷵�
        {
#if STATE_LED_DEBUG
            gpio_set(LED_RED, 0);
#endif
            if(CircleIslandIdentify_L(LeftLine, InflectionL)==1)
            {
#if STATE_LED_DEBUG
                gpio_set(LED_RED, 1);
#endif
                StartIntegralAngle_Z(30);//��״̬֮��ת��30�����Ҳſ�������ʶ�𣬱���״̬������������
                icm_flag=1;
                base_speed=speed_case_6;
                process_flag=6;
            }
            break;
        }
        case 6: //ʶ��ڶ�������
        {
            if(icm_flag==1)
            {
                if(icm_angle_z_flag==1)
                {
                    icm_flag=0;
                }
                break;
            }
#if STATE_LED_DEBUG
            gpio_set(LED_YELLOW, 0);
#endif
            if(ForkSStatusIdentify(InflectionL, InflectionR, &Fork_flag)==1)
            {
#if STATE_LED_DEBUG
            gpio_set(LED_YELLOW, 1);
#endif
                base_speed=speed_case_7;
                process_flag=7;
            }
            break;
        }
        case 7: //ʶ���󳵿⣬���
        {
#if STATE_LED_DEBUG
            gpio_set(LED_WHITE, 0);
#endif
            if(GarageInIdentify()==1)
            {
#if STATE_LED_DEBUG
            gpio_set(LED_WHITE, 1);
#endif
                Stop();
            }
            break;
        }
    }
#endif
    /***************************ƫ�����**************************/
    if(Fork_flag!=0||Garage_flag!=0||Circle_flag!=0)    //��ʶ���������Ѿ�������Bias
    {
        Garage_flag=0;Fork_flag=0;Circle_flag=0;      //����flag
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
    lcd_showfloat(0, 1, Bias, 1, 2);
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
    base_speed=0;                   //����Ŀ���ٶ�Ϊ0
    MotorK_L.I=1.5;MotorK_R.I=1.5;  //�޸�PID��������ͣ��
    pit_disable_interrupt(CCU6_0, PIT_CH1); //�رն���ж�
    gpio_set(P21_4, 0);
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
