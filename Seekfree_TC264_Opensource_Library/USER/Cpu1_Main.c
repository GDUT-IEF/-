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

#include "headfile.h"
#pragma section all "cpu1_dsram"

#include "Binarization.h"   //��ֵ������
#include "Steer.h"          //�������
#include "ImageProcess.h"   //ͼ������
#include "PID.h"            //PID
#include <string.h>
#include "Key.h"            //��������

//���������#pragma section all restore���֮���ȫ�ֱ���������CPU1��RAM��

uint8 outgarage_flag=0;     //�����flag
uint8 key_flag=0;           //�������ε�flag

void core1_main(void)
{
	disableInterrupts();
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    //�û��ڴ˴����ø��ֳ�ʼ��������

    /*����ģ��ĳ�ʼ����CPU0�����*/

	//�ȴ����к��ĳ�ʼ�����
	IfxCpu_emitEvent(&g_cpuSyncEvent);
	IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
    enableInterrupts();

    //**********��������*************
    //*******************************
    pit_interrupt_ms(CCU6_0,PIT_CH0,6); //��ʼ�������ʱ���ж�
    pit_interrupt_ms(CCU6_0,PIT_CH1,20);//��ʼ�������ʱ���ж�
//    //��ɳ���
//    if(outgarage_flag==0)
//    {
//        OutGarage();
//        outgarage_flag=1;
//    }

    while (TRUE)
    {
        //ͼ����ģ��
        if(mt9v03x_finish_flag)
        {
            ImageBinary();      //ͼ���ֵ��
//            lcd_displayimage032(BinaryImage[0],MT9V03X_W,MT9V03X_H);    //���Ͷ�ֵ�����ͼ��LCD
            //��110������
//            for(int cloum=0;cloum<MT9V03X_W-1;cloum++)
//            {
//                lcd_drawpoint(cloum,110,PURPLE);
//            }
            ImageProcess();     //ͼ����Ԫ��ʶ��
            //�����߻�����
//            for(int i=MT9V03X_H-1;i>0;i--)
//            {
//                lcd_drawpoint(LeftLine[i],i,GREEN);
//                lcd_drawpoint(CentreLine[i],i,RED);
//                lcd_drawpoint(RightLine[i],i,BLUE);
//            }
            mt9v03x_finish_flag = 0;//��ͼ��ʹ����Ϻ���������־λ�����򲻻Ὺʼ�ɼ���һ��ͼ��
        }
        //����
        gpio_set(P20_8,1);
    }
}



#pragma section all restore
