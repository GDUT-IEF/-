#ifndef _MYIIC_H
#define _MYIIC_H
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������
//IIC��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2019/9/18
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#include "zf_gpio.h"

//����IO
#define OLED_SCL    P22_0
#define OLED_SDA    P33_8

//IO��������
#define SDA_IN()    gpio_dir(OLED_SDA, GPI, NO_PULL)    //���뷽��
#define SDA_OUT()   gpio_dir(OLED_SDA, GPO, PUSHPULL)   //�������

//IO����
#define READ_SDA         gpio_get (OLED_SDA)         //IO�ڻ�ȡ�����ƽ
#define SDA_0()          gpio_set (OLED_SDA, 0)      //IO������͵�ƽ
#define SDA_1()          gpio_set (OLED_SDA, 1)      //IO������ߵ�ƽ
#define SCL_0()          gpio_set (OLED_SCL, 0)      //IO������͵�ƽ
#define SCL_1()          gpio_set (OLED_SCL, 1)      //IO������ߵ�ƽ

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(unsigned char txd);			//IIC����һ���ֽ�
unsigned char IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
unsigned char IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(unsigned char daddr,unsigned char addr,unsigned char data);
unsigned char IIC_Read_One_Byte(unsigned char daddr,unsigned char addr);

#endif

