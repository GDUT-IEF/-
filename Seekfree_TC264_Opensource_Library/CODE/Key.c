/*
 * Key.c
 *
 *  Created on: 2022��4��12��
 *      Author: yue
 */

#include "Key.h"
#include "zf_stm_systick.h"
#include "zf_gpio.h"
#include "ImageProcess.h"

/*
 ** ��������: ����ɨ�账��
 ** ��    ��: ��
 ** �� �� ֵ: ��Ӧ���°�����ֵ��0=�ް������£�������Ӧ�鿴�궨��
 ** ��    ��: WBN
 */
uint8 KeyScan(void)
{
    static uint8 key_up=1;     //�����ɿ���־
    if(key_up&&(KEY_S1==1||KEY_S2==1||KEY_S3==1||KEY_S4==1||KEY_S5==1))
    {
        systick_delay_ms(STM0,10);
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

/*
 ** ��������: ��������
 ** ��    ��: ��
 ** �� �� ֵ: �Ƿ��˳����Σ�1:�˳�����  0:��������
 ** ��    ��: WBN
 */
uint8 KeyParameter(void)
{
    switch(KeyScan())
    {
        case KEY_S1_PRES:   //S1
        {
            gpio_toggle(LED_WHITE);
            break;
        }
        case KEY_S2_PRES:   //S2
        {
            gpio_toggle(LED_GREEN);
            break;
        }
        case KEY_S3_PRES:   //S3
        {
            gpio_toggle(LED_BLUE);
            break;
        }
        case KEY_S4_PRES:   //S4
        {
            gpio_toggle(LED_RED);
            break;
        }
        case KEY_S5_PRES:   //S5���˳�����
        {
            return 1;
        }
    }
    systick_delay_ms(STM0,100);
    return 0;
}



