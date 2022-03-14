/*
 * BluetoothSend.c
 *
 *  Created on: 2022��3��6��
 *      Author: 30516
 */

#include "BluetoothSend.h"

//int16תΪuint8
void ShortToByte(int16 x,uint8 *y)
{
    y[1]=(x&0xFF00)>>8;
    y[0]=(x&0xFF);
}

void Float_to_Byte(float f,unsigned char *byte)
{

    unsigned long longdata = 0;
    longdata = *(unsigned long*)&f;
    byte[3] = (longdata & 0xFF000000) >> 24;
    byte[2] = (longdata & 0x00FF0000) >> 16;
    byte[1] = (longdata & 0x0000FF00) >> 8;
    byte[0] = (longdata & 0x000000FF);

}

void BluetooothSendBias(float Bias)
{
    uint8 uart_tx_buf[7];                       //���ݰ�
    uart_tx_buf[0]=0xA5;                        //��ͷ
    Float_to_Byte(Bias,&uart_tx_buf[1]);        //ԭ����
    //����У���
    for(int cnt_tx=1;cnt_tx<=7-3;cnt_tx++)
        uart_tx_buf[7-2] += uart_tx_buf[cnt_tx];
    uart_tx_buf[7-2] = uart_tx_buf[7-2]&0xff;
//    uart_tx_buf[5]=(uint8)Bias;                 //����У���
    uart_tx_buf[6]=0x5A;                        //��β

    uart_putbuff(UART_2,uart_tx_buf,7);         //ͨ������2�������ݵ�����
}

void BluetoothSendToApp(int16 EncoderL,int16 EncoderR)
{
    uint8 uart_tx_buf[7];                       //���ݰ�
    uart_tx_buf[0]=0xA5;                        //��ͷ
    ShortToByte(EncoderL,&uart_tx_buf[1]);      //ԭ����
    ShortToByte(EncoderR,&uart_tx_buf[3]);      //ԭ����
    uart_tx_buf[5]=(uint8)(EncoderL+EncoderR);  //У���
    uart_tx_buf[6]=0x5A;                        //��β

    uart_putbuff(UART_2,uart_tx_buf,7);         //ͨ������2�������ݵ�����
}


