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

#include "myiic.h"
#include "zf_stm_systick.h"

//IIC��ʼ��
void IIC_Init(void)
{
    gpio_init (OLED_SCL, GPO, 1, PUSHPULL);
    gpio_init (OLED_SDA, GPO, 1, PUSHPULL);
    
    SDA_1();
    SCL_1();
}

//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	SDA_1();
	SCL_1();
	systick_delay_us(STM0,4);
	SDA_0();//START:when CLK is high,DATA change form high to low
 	systick_delay_us(STM0,4);
 	SCL_0();//ǯסI2C���ߣ�׼�����ͻ��������
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	SCL_0();
	SDA_0();//STOP:when CLK is high DATA change form low to high
	systick_delay_us(STM0,4);
	SCL_1();
	SDA_1();//����I2C���߽����ź�
	systick_delay_us(STM0,4);
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
unsigned char IIC_Wait_Ack(void)
{
    unsigned char ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	SDA_1();systick_delay_us(STM0,1);
	SCL_1();systick_delay_us(STM0,1);
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	SCL_0();//ʱ�����0
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
    SCL_0();
	SDA_OUT();
	SDA_0();
	systick_delay_us(STM0,2);
	SCL_1();
	systick_delay_us(STM0,2);
	SCL_0();
}
//������ACKӦ��		    
void IIC_NAck(void)
{
    SCL_0();
	SDA_OUT();
	SDA_1();
	systick_delay_us(STM0,2);
	SCL_1();
	systick_delay_us(STM0,2);
	SCL_0();
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(unsigned char txd)
{                        
    unsigned char t;
	SDA_OUT(); 	    
	SCL_0();//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {
        if((txd&0x80)>>7==1)    //SDA����1
        {
            SDA_1();
        }
        else                    //SDA����0
        {
            SDA_0();
        }
        txd<<=1; 	  
        systick_delay_us(STM0,2);   //��TEA5767��������ʱ���Ǳ����
        SCL_1();
		systick_delay_us(STM0,2);
		SCL_0();
		systick_delay_us(STM0,2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
unsigned char IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        SCL_0();
        systick_delay_us(STM0,2);
        SCL_1();
        receive<<=1;
        if(READ_SDA)receive++;   
        systick_delay_us(STM0,1);
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}



