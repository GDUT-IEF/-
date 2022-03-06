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

void BluetoothReceiveFromApp(int P,int I)
{
    uint8 uart_rx_buf[11];                       //���ݰ�
    uint8 err;                                   //У���
    if(uart_query(UART_2,uart_rx_buf)==1)        //���յ�����
    {
        if(uart_rx_buf[0]==0xA5&&uart_rx_buf[10]==0x5A)
        {
            //���У���
            err = ((uint8)(uart_rx_buf[1]+uart_rx_buf[2]+uart_rx_buf[3]+uart_rx_buf[4]+uart_rx_buf[5]+uart_rx_buf[6]+uart_rx_buf[7]+uart_rx_buf[8])&0xFF);
            if(err!=uart_rx_buf[9])
            {
                return; //У��ʹ���ֱ��return
            }
            //У�����ȷ����ֵ����
            P=(int)uart_rx_buf[1];
            I=(int)uart_rx_buf[5];

        }
    }
}

