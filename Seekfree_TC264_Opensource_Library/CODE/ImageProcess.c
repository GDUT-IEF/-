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

#define STATE_LED_DEBUG 0   //ʹ�����ذ�LED����Debug

uint8 bias_startline=70,bias_endline=50;        //��̬ǰհ
uint8 Fork_flag=0;              //����ʶ��ı�־����
uint8 Garage_flag=0;            //����ʶ���־����
uint8 Circle_flag=0;            //����Ѱ����־����
int16 speed_case_1=220,speed_case_2=240,speed_case_3=220,speed_case_4=220,speed_case_5=220,speed_case_6=220,speed_case_7=200;
int LeftLine[MT9V03X_H]={0}, CentreLine[MT9V03X_H]={0}, RightLine[MT9V03X_H]={0};   //ɨ�ߴ�������������
uint8 process_flag=3;   //״̬����ת��־

/*1:��ʮ�ֻػ� 2���ұ�ʮ�ֻػ� 3���󻷵� 4���һ��� 5����������ֱ�� 6�������������µ� 7���ұ߳��ⲻ��� 8����� 9��ʮ��·�� 'E':������ 'M':������ 'S':ͣ��*/
uint8 process_status[20]={1,  7,  5,  4,  2,  3,  5,  8};//��״̬��Ԫ��ִ��˳������
uint16 process_speed[20]={230,240,250,230,230,230,230,220};//���������Ӧ��Ԫ��·�ε��ٶ�
uint8 process_encoder[5]={3,3,3,3};//�������ƾ�������� **ע���ҳ��ⲻ���ı��������벻�ڴ˴�**
uint8 process_icm[5];//�����ǻ����������
uint8 process_status_cnt=0;//Ԫ��״̬����ļ�����
uint8 process_encoder_cnt=0;//���������ľ������������
uint8 process_icm_cnt=0;//�����ǻ��ֽǶȵĽǶ����������

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
    if(process_status[process_status_cnt]==7 || process_status[process_status_cnt]==1)    //���ó���ר��ɨ�߷��������Ӱ�����Ӱ��
    {
        GetImagBasic_Garage(LeftLine, CentreLine, RightLine, 'L');
    }
    else    //����ɨ��
    {
        GetImagBasic(LeftLine, CentreLine, RightLine, 'L');
    }
    /*************************��Ѱ�����¹յ�***********************/
    GetDownInflection(110,45,LeftLine,RightLine,&InflectionL,&InflectionR);
    /*************************����Ԫ���ж�*************************/
    /****************************״̬��***************************/
#if 1
    switch(process_status[process_status_cnt])
    {
        case 1: //ʶ����ʮ�ֻػ�
        {
            if(CrossLoopIdentify_L(InflectionL)==1)
            {
                process_status_cnt++;
                base_speed=process_speed[process_status_cnt];
            }
            break;
        }
        case 2: //ʶ����ʮ�ֻػ�
        {
            if(CrossLoopIdentify_R(InflectionR)==1)
            {
                process_status_cnt++;
                base_speed=process_speed[process_status_cnt];
            }
            break;
        }
        case 3: //ʶ���󻷵�
        {
            if(CircleIslandIdentify_L(LeftLine, InflectionL)==1)
            {
                process_status_cnt++;
                base_speed=process_speed[process_status_cnt];
            }
            break;
        }
        case 4: //ʶ���һ���
        {
            if(CircleIslandIdentify_R(RightLine, InflectionR)==1)
            {
                process_status_cnt++;
                base_speed=process_speed[process_status_cnt];
            }
            break;
        }
        case 5: //ʶ��û�µ�������
        {
            if(ForkFStatusIdentify(InflectionL, InflectionR, &Fork_flag)==1)
            {
                process_status_cnt++;
                base_speed=process_speed[process_status_cnt];
            }
            break;
        }
        case 6: //ʶ�����µ�������
        {
            if(ForkSStatusIdentify(InflectionL, InflectionR, &Fork_flag)==1)
            {
                process_status_cnt++;
                base_speed=process_speed[process_status_cnt];
            }
            break;
        }
        case 7: //ʶ���ҳ��⣬ֱ��
        {
            //������
            if(encoder_flag==0)
            {
                EncoderDistance(1, 0.7, 0, 0);
                encoder_flag=1;
            }
            if(encoder_dis_flag==1)
            {
                process_status_cnt++;
                base_speed=process_speed[process_status_cnt];
            }
            break;
        }
        case 8: //ʶ���󳵿⣬���
        {
            if(GarageInIdentify()==1)
            {
                Stop();
            }
            break;
        }
        case 9: //ʶ��ʮ��·��
        {
            if(CrossRoadsStatusIdentify(InflectionL, InflectionR)==1)
            {
                process_status_cnt++;
                base_speed=process_speed[process_status_cnt];
            }
            break;
        }
        case 'E'://������������Ԫ��ѭ��
        {
            if(encoder_flag==0)//�ж��Ƿ�δ����������
            {
                if(process_encoder[process_encoder_cnt]>0)
                {
                    EncoderDistance(1, process_encoder[process_encoder_cnt], 0, 0);//����������
                    process_encoder_cnt++;
                    encoder_flag=1;
                }
                else//�����⵽����������Ҫ������=0����ô���ϻ���һ��״̬
                {
                    process_status_cnt++;
                    base_speed=process_speed[process_status_cnt];
                    break;
                }
            }
            if(encoder_dis_flag==1 && encoder_flag==1)//�жϱ������Ƿ������
            {
                encoder_flag=0;
                process_status_cnt++;
                base_speed=process_speed[process_status_cnt];
            }
            break;
        }
        case 'M'://�����ǻ��Ƕ���Ԫ��ѭ��
        {
            if(icm_flag==0)//�ж��Ƿ�δ����������
            {
                if(process_icm[process_icm_cnt]>0)
                {
                    StartIntegralAngle_Z(process_icm[process_icm_cnt]);//����������
                    process_icm_cnt++;
                    icm_flag=1;
                }
                else//�����⵽����������Ҫ������=0����ô���ϻ���һ��״̬
                {
                    process_status_cnt++;
                    base_speed=process_speed[process_status_cnt];
                    break;
                }
            }
            if(icm_angle_z_flag==1 && icm_flag==1)
            {
                icm_flag=0;
                process_status_cnt++;
                base_speed=process_speed[process_status_cnt];
            }
            break;
        }
    }
#endif
    /***************************ƫ�����**************************/
    if(Garage_flag!=0)  //��ʶ���������Ѿ�������Bias
    {
        Garage_flag=0;  //����flag
    }
    else if (process_status[process_status_cnt]==7)//����ֱ��ƫ��
    {
        Bias=DifferentBias_Garage(bias_startline,bias_endline,CentreLine);
    }
    else
    {
        Bias=DifferentBias_Circle(bias_startline,bias_endline,CentreLine); //��̬ǰհ����ƫ��
        Slope=Regression_Slope(bias_startline,bias_endline,CentreLine);    //��̬ǰհ����б��
        bias_startline=70;bias_endline=50;                                 //�ָ�Ĭ��ǰհ
    }
    //LCD����ͼ��
#if 0
    for(uint8 i=0;i<MT9V03X_W-1;i++)
    {
        lcd_drawpoint(i, bias_startline, YELLOW);
        lcd_drawpoint(i, bias_endline, YELLOW);
    }
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
