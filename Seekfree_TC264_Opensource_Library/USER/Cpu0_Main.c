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
#include "Steer.h"          //�������
#include "Motor.h"          //�������
#include "PID.h"            //PID
#include "protocol.h"       //Ұ����λ��Э��

#pragma section all "cpu0_dsram"    //���������#pragma section all restore���֮���ȫ�ֱ���������CPU0��RAM��

int core0_main(void)
{
	get_clk();//��ȡʱ��Ƶ��  ��ر���

	/***************************�����ĳ�ʼ��**************************/
//	uart_init(UART_0, 115200, UART0_TX_P14_0, UART0_RX_P14_1);      //��ʼ������0�������λ��ͨѶ
	uart_init(UART_2, 115200, UART2_TX_P10_5, UART2_RX_P10_6);      //��ʼ������ģ�����õĴ���2
	lcd_init();                          //��ʼ��TFT��Ļ
	gpio_init(P20_8, GPO, 1, PUSHPULL);  //��ʼ��LED
	gpio_init(P20_9, GPO, 1, PUSHPULL);
    gpio_init(P21_4, GPO, 1, PUSHPULL);
    gpio_init(P21_5, GPO, 1, PUSHPULL);
    /**************************������ģ���ʼ��**********************/
	mt9v03x_init();     //��ʼ������ͷ
	/***************************����ģ���ʼ��***********************/
	gtm_pwm_init(STEER_PIN, 50, STEER_MID);       //��ʼ�����
	gtm_pwm_init(LEFT_MOTOR_PIN1,17*1000,0);      //��ʼ������
	gpio_init(P02_6, GPO, 1, PUSHPULL);
//	gtm_pwm_init(LEFT_MOTOR_PIN2,17*1000,0);
	gtm_pwm_init(RIGHT_MOTOR_PIN1,17*1000,0);     //��ʼ���ҵ��
	gpio_init(P02_7, GPO, 1, PUSHPULL);
//	gtm_pwm_init(RIGHT_MOTOR_PIN2,17*1000,0);
	gpt12_init(LEFT_ENCODER, GPT12_T2INB_P33_7, GPT12_T2EUDB_P33_6);    //��ʼ���������
	gpt12_init(RIGHT_ENCODER, GPT12_T6INA_P20_3, GPT12_T6EUDA_P20_0);   //��ʼ���ұ�����
	/**********************PID��ʼ��***********************************************/
	PID_init(&SteerK,&MotorK);          //��ʼ��PID����
	/********************��ʱ���жϳ�ʼ��****************************/
	pit_interrupt_ms(CCU6_0,PIT_CH0,6); //��ʼ����ʱ���ж�
	/****************************************************************/
    //�ȴ����к��ĳ�ʼ�����
	IfxCpu_emitEvent(&g_cpuSyncEvent);
	IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
	enableInterrupts();

	while (TRUE)
	{
//	    receiving_process();    //����Ұ����λ���·�������
	    /*ͼ������CPU1������ѯ�ķ�ʽִ��*/
	    /*���ƴ�����CPU0���Զ�ʱ���жϵķ�ʽִ��*/
	}
}

#pragma section all restore


