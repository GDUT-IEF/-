
 
/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,��ɿƼ�
 * All rights reserved.
 * ��������QQȺ����Ⱥ��824575535
 *
 * �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
 * ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
 *
 * @file       		isr
 * @company	   		�ɶ���ɿƼ����޹�˾
 * @author     		��ɿƼ�(QQ3184284598)
 * @version    		�鿴doc��version�ļ� �汾˵��
 * @Software 		ADS v1.2.2
 * @Target core		TC264D
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-3-23
 ********************************************************************************************************************/


#include "isr_config.h"
#include "isr.h"
#include "motor.h"
#include "steer.h"
#include "PID.h"
#include "ImageTack.h"
#include "zf_gpio.h"
#include <math.h>
#include "Filter.h"
#include "ICM20602.h"
#include "LED.h"
#include  "zf_stm_systick.h"
#include "ImageProcess.h"

uint32 SteerPWM=STEER_MID;    //���PWM
float icm_target_angle_z=0;   //������Z�����Ŀ��Ƕ�
uint8 icm_angle_z_flag=0;     //������Z����ִﵽĿ��Ƕ�
//PIT�жϺ���  ʾ��

//����ٶȻ������ж�
IFX_INTERRUPT(cc60_pit_ch0_isr, 0, CCU6_0_CH0_ISR_PRIORITY)
{
	enableInterrupts();//�����ж�Ƕ��

	//�������ṹ���٣����ٰ�
	int diff_steerpwm=SteerPWM-STEER_MID;
	double radian;
	if(diff_steerpwm>0) //��ת
	{
	    radian=0.0082794*diff_steerpwm;    //��������ֱ��ʵ�ʼн� /   //�������ֵ��ʵ������Ǹı䣬����Ҫ�޸Ĵ˴�
	    speed_l=(41/(41+15*tan(radian)))*base_speed;    //��ת���ּ���    //�˴���ǰ���־��ǰ������������һ�㲻��Ҫ�Ķ�
	    speed_r=base_speed;
//	    speed_r=((41+30*tan(radian))/(41+15*tan(radian)))*base_speed;  //���ּ���
	}
	else                //��ת
	{
	    diff_steerpwm=-diff_steerpwm;
	    radian=0.0082794*diff_steerpwm;     //��������ֱ��ʵ�ʼн�    //�������ֵ��ʵ������Ǹı䣬����Ҫ�޸Ĵ˴�
	    speed_r=(41/(41+15*tan(radian)))*base_speed;    //��ת���ּ���    //�˴���ǰ���־��ǰ������������һ�㲻��Ҫ�Ķ�
	    speed_l=base_speed;
//	    speed_l=((41+30*tan(radian))/(41+15*tan(radian)))*base_speed;  //���ּ���
	}
	MotorCtrl(speed_l,speed_r);         //PID���Ƶ���ٶ�
	//����
	gpio_set(P20_8,0);
//	MotorCtrl(base_speed, base_speed);
//	printf("%d,%d\n",speed_l,speed_r);

	PIT_CLEAR_FLAG(CCU6_0, PIT_CH0);
}

//���ת�򻷿����ж�
IFX_INTERRUPT(cc60_pit_ch1_isr, 0, CCU6_0_CH1_ISR_PRIORITY)
{
	enableInterrupts();//�����ж�Ƕ��

	//���PID����
    SteerPWM=Steer_Position_PID(Bias,Slope,SteerK);
    SteerCtrl(SteerPWM);

//    printf("%f,%d\n",Bias,SteerPWM);  //����

//	//�����ʾ
//	static int16 pwm=STEER_LEFT,flag=0;
//	if(pwm>STEER_RIGHT&&flag==0)    //�Ұ�
//	{
//	    pwm--;
//	}
//	else    //�л�����
//	{
//	    flag=1;
//	}
//	if(pwm<STEER_LEFT&&flag==1)     //���
//	{
//	    pwm++;
//	}
//	else    //�л�����
//	{
//	    flag=0;
//	}
//	SteerCtrl(pwm);
//	if(pwm==STEER_MID||pwm==STEER_LEFT||pwm==STEER_RIGHT)  //��ͣ�鿴��ֵ
//	{
//	    systick_delay_ms(STM0,1000);
//	}

	PIT_CLEAR_FLAG(CCU6_0, PIT_CH1);
}

//�����ǽǶȻ����ж�
IFX_INTERRUPT(cc61_pit_ch0_isr, 0, CCU6_1_CH0_ISR_PRIORITY)
{
	enableInterrupts();//�����ж�Ƕ��

	float angle_z=GetICM20602Angle_Z(0);    //�ǶȻ���
	if(angle_z>icm_target_angle_z||angle_z<-icm_target_angle_z)  //�жϻ��ֽǶ��Ƿ����Ŀ��Ƕ�
	{
	    icm_angle_z_flag=1;                     //���ֵ���Ŀ��flag=1
	    pit_disable_interrupt(CCU6_1, PIT_CH0); //�ر��ж�
	}

//	printf("%f\n",angle_z);
//	lcd_showfloat(0, 0, angle_z, 3, 3);

	PIT_CLEAR_FLAG(CCU6_1, PIT_CH0);
}

IFX_INTERRUPT(cc61_pit_ch1_isr, 0, CCU6_1_CH1_ISR_PRIORITY)
{
	enableInterrupts();//�����ж�Ƕ��
	PIT_CLEAR_FLAG(CCU6_1, PIT_CH1);

}




IFX_INTERRUPT(eru_ch0_ch4_isr, 0, ERU_CH0_CH4_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
	if(GET_GPIO_FLAG(ERU_CH0_REQ4_P10_7))//ͨ��0�ж�
	{
		CLEAR_GPIO_FLAG(ERU_CH0_REQ4_P10_7);
	}

	if(GET_GPIO_FLAG(ERU_CH4_REQ13_P15_5))//ͨ��4�ж�
	{
		CLEAR_GPIO_FLAG(ERU_CH4_REQ13_P15_5);
	}
}

IFX_INTERRUPT(eru_ch1_ch5_isr, 0, ERU_CH1_CH5_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
	if(GET_GPIO_FLAG(ERU_CH1_REQ5_P10_8))//ͨ��1�ж�
	{
		CLEAR_GPIO_FLAG(ERU_CH1_REQ5_P10_8);
	}

	if(GET_GPIO_FLAG(ERU_CH5_REQ1_P15_8))//ͨ��5�ж�
	{
		CLEAR_GPIO_FLAG(ERU_CH5_REQ1_P15_8);
	}
}

//��������ͷpclk����Ĭ��ռ���� 2ͨ�������ڴ���DMA��������ﲻ�ٶ����жϺ���
//IFX_INTERRUPT(eru_ch2_ch6_isr, 0, ERU_CH2_CH6_INT_PRIO)
//{
//	enableInterrupts();//�����ж�Ƕ��
//	if(GET_GPIO_FLAG(ERU_CH2_REQ7_P00_4))//ͨ��2�ж�
//	{
//		CLEAR_GPIO_FLAG(ERU_CH2_REQ7_P00_4);
//
//	}
//	if(GET_GPIO_FLAG(ERU_CH6_REQ9_P20_0))//ͨ��6�ж�
//	{
//		CLEAR_GPIO_FLAG(ERU_CH6_REQ9_P20_0);
//
//	}
//}



IFX_INTERRUPT(eru_ch3_ch7_isr, 0, ERU_CH3_CH7_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
	if(GET_GPIO_FLAG(ERU_CH3_REQ6_P02_0))//ͨ��3�ж�
	{
		CLEAR_GPIO_FLAG(ERU_CH3_REQ6_P02_0);
		if		(CAMERA_GRAYSCALE == camera_type)	mt9v03x_vsync();
		else if (CAMERA_BIN_UART  == camera_type)	ov7725_uart_vsync();
		else if	(CAMERA_BIN       == camera_type)	ov7725_vsync();

	}
	if(GET_GPIO_FLAG(ERU_CH7_REQ16_P15_1))//ͨ��7�ж�
	{
		CLEAR_GPIO_FLAG(ERU_CH7_REQ16_P15_1);

	}
}



IFX_INTERRUPT(dma_ch5_isr, 0, ERU_DMA_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��

	if		(CAMERA_GRAYSCALE == camera_type)	mt9v03x_dma();
	else if (CAMERA_BIN_UART  == camera_type)	ov7725_uart_dma();
	else if	(CAMERA_BIN       == camera_type)	ov7725_dma();
}


//�����жϺ���  ʾ��
IFX_INTERRUPT(uart0_tx_isr, 0, UART0_TX_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrTransmit(&uart0_handle);
}
IFX_INTERRUPT(uart0_rx_isr, 0, UART0_RX_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrReceive(&uart0_handle);
}
IFX_INTERRUPT(uart0_er_isr, 0, UART0_ER_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrError(&uart0_handle);
}

//����1Ĭ�����ӵ�����ͷ���ô���
IFX_INTERRUPT(uart1_tx_isr, 0, UART1_TX_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrTransmit(&uart1_handle);
}
IFX_INTERRUPT(uart1_rx_isr, 0, UART1_RX_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrReceive(&uart1_handle);
    if		(CAMERA_GRAYSCALE == camera_type)	mt9v03x_uart_callback();
    else if (CAMERA_BIN_UART  == camera_type)	ov7725_uart_callback();
}
IFX_INTERRUPT(uart1_er_isr, 0, UART1_ER_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrError(&uart1_handle);
}

/*
 ** ���ڴ����жϵ�һ��ע��
 ** IFX_INTERRUPT�����������жϷ������ĺ�
 ** ����������1.�жϷ�����������    2.0ָʾCPU0�ṩ�жϷ���     3.���ȼ�
 **
 ** ���������꺯���������ô���2���жϣ���һ����TX���ڶ�����RX����������ER����������ֻ��ע�ڶ���RX���յ��ж�
 ** */

//����2Ĭ�����ӵ�����ת����ģ��
IFX_INTERRUPT(uart2_tx_isr, 0, UART2_TX_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrTransmit(&uart2_handle);
}
IFX_INTERRUPT(uart2_rx_isr, 0, UART2_RX_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrReceive(&uart2_handle);

    //�������������ѡ�񴮿�2���Ӳ�ͬģ���
//    switch(wireless_type)
//    {
//    	case WIRELESS_SI24R1:
//    	{
//    		wireless_uart_callback();
//    	}break;
//
//    	case WIRELESS_CH9141:
//		{
//		    bluetooth_ch9141_uart_callback();
//		}break;
//    	default:break;
//    }

    bluetooth_ch9141_uart_callback();   //ֱ�ӵ�������ģ��Ļص�����

}
IFX_INTERRUPT(uart2_er_isr, 0, UART2_ER_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrError(&uart2_handle);
}



IFX_INTERRUPT(uart3_tx_isr, 0, UART3_TX_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrTransmit(&uart3_handle);
}
IFX_INTERRUPT(uart3_rx_isr, 0, UART3_RX_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrReceive(&uart3_handle);
}
IFX_INTERRUPT(uart3_er_isr, 0, UART3_ER_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrError(&uart3_handle);
}
