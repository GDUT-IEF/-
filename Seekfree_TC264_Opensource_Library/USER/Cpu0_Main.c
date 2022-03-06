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
#include "BluetoothSend.h" //����������Ϣ���ֻ�APP��λ��

#pragma section all "cpu0_dsram"    //���������#pragma section all restore���֮���ȫ�ֱ���������CPU0��RAM��

//�������
int *LeftLine,*CentreLine,*RightLine;   //����������
SteerPID SteerK;
MotorPID MotorK;

int core0_main(void)
{
	get_clk();//��ȡʱ��Ƶ��  ��ر���
	//�û��ڴ˴����ø��ֳ�ʼ��������
	//***************************��������**************************
	int LeftLine[MT9V03X_H]={0}, CentreLine[MT9V03X_H]={0}, RightLine[MT9V03X_H]={0};   //ɨ�ߴ�������������
	Point LeftDownPoint,RightDownPoint;
	LeftDownPoint.X=0;LeftDownPoint.Y=0;RightDownPoint.X=0;RightDownPoint.Y=0;
	Point ForkUpPoint;
	ForkUpPoint.X=0;ForkUpPoint.Y=0;
	float Bias=0;

	int row;

	int16 left_encoder=0,right_encoder=0;

	//*****************************************************************

	//***************************�����ĳ�ʼ��**************************
	uart_init(UART_0, 115200, UART0_TX_P14_0, UART0_RX_P14_1);      //��ʼ������0�������λ��ͨѶ
	uart_init (BLUETOOTH_CH9141_UART, BLUETOOTH_CH9141_UART_BAUD, BLUETOOTH_CH9141_UART_TX, BLUETOOTH_CH9141_UART_RX);//��ʼ������ģ�����õĴ���
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
//	gtm_pwm_init(STEER_PIN, 50, STEER_MID);                         //��ʼ�����
	gtm_pwm_init(LEFT_MOTOR_PIN1,17*1000,0);                        //��ʼ������
	gtm_pwm_init(LEFT_MOTOR_PIN2,17*1000,0);
	gtm_pwm_init(RIGHT_MOTOR_PIN1,17*1000,0);                       //��ʼ���ҵ��
	gtm_pwm_init(RIGHT_MOTOR_PIN2,17*1000,0);
	gpt12_init(LEFT_ENCODER, GPT12_T2INB_P33_7, GPT12_T2EUDB_P33_6);    //��ʼ���������
	gpt12_init(RIGHT_ENCODER, GPT12_T6INA_P20_3, GPT12_T6EUDA_P20_0);   //��ʼ���ұ�����
	//********************************************************************

	/**********************PID��ʼ��***********************************************/
	PID_init(&SteerK,&MotorK);
	/**************************************************************/

    //�ȴ����к��ĳ�ʼ�����
	IfxCpu_emitEvent(&g_cpuSyncEvent);
	IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
	enableInterrupts();

	while (TRUE)
	{
	    /*��ʼ������*/


	    //ͼ����ģ��
	    if(mt9v03x_finish_flag)
	    {
	        ImageBinary();//ͼ���ֵ��
	        //SPI����ͼ��1.8TFT
	        lcd_displayimage032(BinaryImage[0],MT9V03X_W,MT9V03X_H);    //��ֵ�����ͼ��
//	        lcd_displayimage032(mt9v03x_image[0],MT9V03X_W,MT9V03X_H);  //ԭʼ�Ҷ�ͼ��


	        /*ɨ�ߺ�������*/
//	        GetImagBasic(LeftLine,CentreLine,RightLine);
//	        for(int i=MT9V03X_H;i>0;i--)    //LCD�ϵ��ߴ������ϻ�
//	        {
//	            lcd_drawpoint(CentreLine[i],i,RED); //�к�
//	            lcd_showint32(0,0,i,3);
//                lcd_showint32(0,1,RightLine[i],3);
//
//                lcd_showint32(0,5,i,3);
//                lcd_showint32(0,6,LeftLine[i],3);
//                systick_delay_ms(STM0, 500);
//	            lcd_drawpoint(LeftLine[i],i,GREEN);  //��
//                RightLine[i]=RightLine[i]*160/188;
//                lcd_drawpoint(RightLine[i],i,BLUE);//��
//	        }
//
//	        /*б�ʺ�������*/
//	        Bias=Regression_Slope(80,40,CentreLine);
//	        lcd_showfloat(0,0,Bias,2,3);
//	        systick_delay_ms(STM0, 1000);
//
	        /*�����¹յ㺯������*/
//	        GetDownInflection(110,40,LeftLine,RightLine,&LeftDownPoint,&RightDownPoint);
//	        lcd_showint32(0,3,LeftDownPoint.Y,3);
//            lcd_showint32(0,4,LeftDownPoint.X,3);
//            lcd_showint32(0,6,RightDownPoint.Y,3);
//            lcd_showint32(0,7,RightDownPoint.X,3);
	        //��ӡ���
//	        lcd_drawpoint(LeftDownPoint.X,LeftDownPoint.Y,GREEN);
//	        //��ӡ�ұ�
//	        lcd_drawpoint(RightDownPoint.X,RightDownPoint.Y,GREEN);
//            systick_delay_ms(STM0, 800);
//
	        /*�����Ϲյ㺯������*/
//	        GetForkUpInflection(LeftDownPoint,RightDownPoint,&ForkUpPoint);
//	        lcd_drawpoint(ForkUpPoint.X*160/188,ForkUpPoint.Y,GREEN);

	        /*����ʶ��������*/
//	        ForkIdentify(110,40,LeftLine,RightLine,&LeftDownPoint,&RightDownPoint,&ForkUpPoint);
//	        lcd_drawpoint(RightDownPoint.X,RightDownPoint.Y,GREEN);//���
//	        lcd_drawpoint(LeftDownPoint.X,LeftDownPoint.Y,GREEN);
//	        lcd_drawpoint(ForkUpPoint.X,ForkUpPoint.Y,GREEN);
//	        lcd_showint32(0,3,LeftDownPoint.Y,3);//��ӡ����
//	        lcd_showint32(0,0,LeftDownPoint.X,3);
//	        systick_delay_ms(STM0, 2000);
//	        lcd_showint32(0,3,ForkUpPoint.Y,3);
//            lcd_showint32(0,0,ForkUpPoint.X,3);
//            systick_delay_ms(STM0, 2000);
//	        FillingLine(LeftDownPoint,ForkUpPoint);//���߻���
//	        lcd_displayimage032(BinaryImage[0],MT9V03X_W,MT9V03X_H);
//	        systick_delay_ms(STM0, 800);
//	        FillingLine(RightDownPoint,ForkUpPoint);
//            lcd_displayimage032(BinaryImage[0],MT9V03X_W,MT9V03X_H);
//            systick_delay_ms(STM0, 800);

//
            /*���뻷��ǰ���жϲ���*/
//            CircleIslandBegin(LeftLine,RightLine,LeftDownPoint,RightDownPoint);    //���ü�⻷����ڵĺ���


	        gpio_toggle(P20_8);//��תIO��LED
            mt9v03x_finish_flag = 0;//��ͼ��ʹ����Ϻ���������־λ�����򲻻Ὺʼ�ɼ���һ��ͼ��
	    }

	    /*�����������*/
//	    SteerCtrl(STEER_MID);
//	    MotorCtrl(1000,1000);

	    /*����������*/
	    MotorEncoder(&left_encoder,&right_encoder);
//	    Speed_PI_Left(left_encoder,1000,MotorK);
//	    Speed_PI_Right(right_encoder,1000,MotorK);
//	    printf("left_encoder=%d,right_encoder=%d",left_encoder,right_encoder);
	    BluetoothSendToApp(left_encoder,right_encoder); //�������ݵ���λ��������ʾ
//	    BluetoothReceiveFromApp(MotorK.P,MotorK.I);     //������λ�������ݵ���P��I����
//	    MotorCtrl(Speed_PI_Left(left_encoder,1000,MotorK),Speed_PI_Right(right_encoder,1000,MotorK));
	    lcd_showint32(0,0,MotorK.P,3);
	    lcd_showint32(0,6,MotorK.I,3);
	    systick_delay_ms(STM0, 100);
	}
}

#pragma section all restore


