/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,��ɿƼ�
 * All rights reserved.
 * ��������QQȺ����Ⱥ��824575535
 *
 * �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
 * ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
 *
 * @file       		main
 * @company	   		�ɶ���ɿƼ����޹�˾
 * @author     		��ɿƼ�(QQ3184284598)
 * @version    		�鿴doc��version�ļ� �汾˵��
 * @Software 		ADS v1.2.2
 * @Target core		TC264D
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-3-23
 ********************************************************************************************************************/

//���̵��뵽���֮��Ӧ��ѡ�й���Ȼ����refreshˢ��һ��֮���ٱ���
//����Ĭ������Ϊ�ر��Ż��������Լ��һ�����ѡ��properties->C/C++ Build->Setting
//Ȼ�����Ҳ�Ĵ������ҵ�C/C++ Compiler->Optimization->Optimization level�������Ż��ȼ�
//һ��Ĭ���½����Ĺ��̶���Ĭ�Ͽ�2���Ż�����˴��Ҳ��������Ϊ2���Ż�

//����TCϵ��Ĭ���ǲ�֧���ж�Ƕ�׵ģ�ϣ��֧���ж�Ƕ����Ҫ���ж���ʹ��enableInterrupts();�������ж�Ƕ��
//�򵥵�˵ʵ���Ͻ����жϺ�TCϵ�е�Ӳ���Զ�������disableInterrupts();���ܾ���Ӧ�κε��жϣ������Ҫ�����Լ��ֶ�����enableInterrupts();�������жϵ���Ӧ��

//ͷ�ļ�����
#include "headfile.h"       //��ɵķ�װ��
#include "Binarization.h"   //��ֵ������
#include "Steer.h"          //�������
#include "Motor.h"          //�������
#include "ImageBasic.h"     //ͼ��Ļ�������
#include "ImageSpecial.h"   //ͼ������Ԫ�ش���
#include "ImageTack.h"      //ѭ��������
#include "PID.h"            //PID
#include "BluetoothSend.h"  //����������Ϣ���ֻ�APP��λ��
#include "Filter.h"         //�˲�ͷ�ļ�

#pragma section all "cpu0_dsram"    //���������#pragma section all restore���֮���ȫ�ֱ���������CPU0��RAM��

//�������
int LeftLine[MT9V03X_H]={0}, CentreLine[MT9V03X_H]={0}, RightLine[MT9V03X_H]={0};   //ɨ�ߴ�������������
SteerPID SteerK;    //���PID����
MotorPID MotorK;    //���PID����
char power_switch=1;//��Դ�ܿ���

int core0_main(void)
{
	get_clk();//��ȡʱ��Ƶ��  ��ر���
	//�û��ڴ˴����ø��ֳ�ʼ��������
	//***************************��������**************************
	Point LeftDownPoint,RightDownPoint;     //�����¹յ�
	LeftDownPoint.X=0;LeftDownPoint.Y=0;RightDownPoint.X=0;RightDownPoint.Y=0;
	Point ForkUpPoint;
	ForkUpPoint.X=0;ForkUpPoint.Y=0;
	Point CrossRoadUpLPoint,CrossRoadUpRPoint;
	CrossRoadUpLPoint.X=0;CrossRoadUpLPoint.Y=0;CrossRoadUpRPoint.X=0;CrossRoadUpRPoint.Y=0;
	float Bias=0;
	uint32 StreePWM=STEER_MID;
	int flag;//����ʶ��ı�־����
	//*****************************************************************

	//***************************�����ĳ�ʼ��**************************
	uart_init(UART_0, 115200, UART0_TX_P14_0, UART0_RX_P14_1);      //��ʼ������0�������λ��ͨѶ
	uart_init(BLUETOOTH_CH9141_UART, BLUETOOTH_CH9141_UART_BAUD, BLUETOOTH_CH9141_UART_TX, BLUETOOTH_CH9141_UART_RX);//��ʼ������ģ�����õĴ���
	lcd_init();                                                     //��ʼ��TFT��Ļ
	gpio_init(P20_8, GPO, 1, PUSHPULL);                             //��ʼ��LED������P20_8Ϊ���
	gpio_init(P20_9, GPO, 1, PUSHPULL);
    gpio_init(P21_4, GPO, 1, PUSHPULL);
    gpio_init(P21_5, GPO, 1, PUSHPULL);
    //*****************************************************************

    //**************************������ģ���ʼ��**************************
	mt9v03x_init(); //��ʼ������ͷ
	//********************************************************************

	//**************************����ģ���ʼ��**************************
	gtm_pwm_init(STEER_PIN, 50, STEER_MID);                         //��ʼ�����
	gtm_pwm_init(LEFT_MOTOR_PIN1,17*1000,0);                        //��ʼ������
	gpio_init(P02_6, GPO, 1, PUSHPULL);
//	gtm_pwm_init(LEFT_MOTOR_PIN2,17*1000,0);
	gtm_pwm_init(RIGHT_MOTOR_PIN1,17*1000,0);                       //��ʼ���ҵ��
	gpio_init(P02_7, GPO, 1, PUSHPULL);
//	gtm_pwm_init(RIGHT_MOTOR_PIN2,17*1000,0);

	gpt12_init(LEFT_ENCODER, GPT12_T2INB_P33_7, GPT12_T2EUDB_P33_6);    //��ʼ���������
	gpt12_init(RIGHT_ENCODER, GPT12_T6INA_P20_3, GPT12_T6EUDA_P20_0);   //��ʼ���ұ�����
	//********************************************************************

	/**********************PID��ʼ��***********************************************/
	PID_init(&SteerK,&MotorK);
	/**********************��ʱ���жϳ�ʼ��**************************/
//	pit_interrupt_ms(CCU6_0,PIT_CH0,5);
	/**************************************************************/

    //�ȴ����к��ĳ�ʼ�����
	IfxCpu_emitEvent(&g_cpuSyncEvent);
	IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
	enableInterrupts();

	/*�����������*/
	MotorSetPWM(1500,1500);

	while (TRUE)
	{
	    /*��ʼ������*/
	    //ͼ����ģ��
	    if(mt9v03x_finish_flag)
	    {
	        ImageBinary();//ͼ���ֵ��
	        //SPI����ͼ��1.8TFT
	        lcd_displayimage032(BinaryImage[0],MT9V03X_W,MT9V03X_H);    //��ֵ�����ͼ��

	        /*ɨ�ߺ�������*/
	        GetImagBasic(LeftLine,CentreLine,RightLine);

	        /*·�����*/
	        GetDownInflection(100,40,LeftLine,RightLine,&LeftDownPoint,&RightDownPoint);
	        if(!CrossRoadsIdentify(LeftLine,RightLine,LeftDownPoint,RightDownPoint))    //ʮ��
	        {
	            flag=ForkIdentify(100,40,LeftLine,RightLine,LeftDownPoint,RightDownPoint);  //����
	        }

	        //�����߻�����
            for(int i=MT9V03X_H;i>0;i--)
            {
//                lcd_showint32(0, 0, LeftLine[i], 3);
//                systick_delay_ms(STM0,100);
                lcd_drawpoint(LeftLine[i],i,GREEN);
                lcd_drawpoint(CentreLine[i],i,RED);
//                BluetooothSendBias(LeftLine[i]);
                lcd_drawpoint(RightLine[i],i,BLUE);
            }

	        /*б�ʺ�������*/
//	        Bias=Regression_Slope(100,40,CentreLine);
//	        Bias=DifferentBias(100,40,CentreLine);
            if(flag==1)
            {
                Bias=DifferentBias(110,90,CentreLine);
                gpio_toggle(P21_4);
                flag=0;
            }
            else
            {
                Bias=DifferentBias(100,60,CentreLine);
            }
            lcd_showfloat(0, 0, Bias, 3, 3);

//	        BluetooothSendBias(Bias);//��������


	        gpio_toggle(P20_8);//��תIO��LED
            mt9v03x_finish_flag = 0;//��ͼ��ʹ����Ϻ���������־λ�����򲻻Ὺʼ�ɼ���һ��ͼ��
	    }

	    /*����ת����Ԫ�ز���*/
	    StreePWM=Steer_Position_PID(Bias,SteerK);
	    printf("Bias=%f     StreePWM=%d\r\n",Bias,StreePWM);
	    SteerCtrl(StreePWM);

	    /*����ٶȻ�����*/
//	    MotorEncoder(&encoder_l,&encoder_r);              //��ȡ���ҵ��������
//	    BluetoothSendToApp(encoder_l,encoder_r);
//	    printf("encoder_l=%d      encoder_r=%d\r\n",encoder_l,encoder_r);
//	    systick_delay_ms(STM0,100);
//	    pwm_l=Speed_PI_Left(encoder_l,50,MotorK);    //���ҵ��PID
//	    pwm_r=Speed_PI_Right(encoder_r,50,MotorK);
//	    MotorSetPWM(pwm_l,pwm_r);                             //���PWM��ֵ

	}
}

#pragma section all restore


