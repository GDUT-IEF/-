/*
 * Key.c
 *
 *  Created on: 2022��4��12��
 *      Author: yue
 */

#include "Key.h"
#include "zf_stm_systick.h"

/*
 ** ��������: ��������
 ** ��    ��: ��
 ** �� �� ֵ: ��Ӧ���°�����ֵ��0=�ް������£�������Ӧ�鿴�궨��
 ** ��    ��: WBN
 */
uint8 KeyScan()
{
    static uint8 key_up=1;     //�����ɿ���־
    if(key_up&&(KEY_S1==1||KEY_S2==1||KEY_S3==1||KEY_S4==1||KEY_S5==1))
    {
        systick_delay_ms(STM0,50);
        key_up=0;
        if(KEY_S1==1)       return KEY_S1_PRES;
        else if(KEY_S2==1)  return KEY_S2_PRES;
        else if(KEY_S3==1)  return KEY_S3_PRES;
        else if(KEY_S4==1)  return KEY_S4_PRES;
        else if(KEY_S5==1)  return KEY_S5_PRES;
    }
    else if(KEY_S1==0||KEY_S2==0||KEY_S3==0||KEY_S4==0||KEY_S5==0)
        key_up=1;
    return 0;   //�ް�������
}


