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
#include "PID.h"            //�޸�PID����
#include "SEEKFREE_18TFT.h" //LCD��ʾ
#include "LED.h"

/*
 ** ��������: ��ʼ��������ӦIO��
 ** ��    ��: ��
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 */
void KeyInit(void)
{
    gpio_init(P33_10, GPI, 0, PULLDOWN);    //��ʼ������
    gpio_init(P33_11, GPI, 0, PULLDOWN);
    gpio_init(P33_12, GPI, 0, PULLDOWN);
    gpio_init(P33_13, GPI, 0, PULLDOWN);
    gpio_init(P32_4, GPI, 0, PULLDOWN);
}

/*
 ** ��������: ����ɨ�账��
 ** ��    ��: ��
 ** �� �� ֵ: ��Ӧ���°�����ֵ��0=�ް������£�������Ӧ�鿴�궨��
 ** ��    ��: WBN
 */
uint8 KeyScan(void)
{
    static uint8 key_up=1;     //�����ɿ���־
    if(key_up&&(KEY_S1==0||KEY_S2==0||KEY_S3==0||KEY_S4==0||KEY_S5==0))
    {
        systick_delay_ms(STM0,10);
        key_up=0;
        if(KEY_S1==0)       return KEY_UP;
        else if(KEY_S2==0)  return KEY_DOWN;
        else if(KEY_S3==0)  return KEY_LEFT;
        else if(KEY_S4==0)  return KEY_RIGHT;
        else if(KEY_S5==0)  return KEY_ENTER;
    }
    else if(KEY_S1==1||KEY_S2==1||KEY_S3==1||KEY_S4==1||KEY_S5==1)
        key_up=1;
    return 0;   //�ް�������
}

/*
 ** ��������: ����PID����
 ** ��    ��: ��
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 */
void KeyPID(void)
{
    while(1)
    {
        switch(KeyScan())
        {
            case KEY_UP:
            {
                gpio_toggle(LED_BLUE);
                break;
            }
            case KEY_DOWN:
            {
                gpio_toggle(LED_GREEN);
                break;
            }
            case KEY_LEFT:
            {
                gpio_toggle(LED_RED);
                break;
            }
            case KEY_RIGHT:
            {
                gpio_toggle(LED_WHITE);
                break;
            }
            case KEY_ENTER:
            {
                gpio_toggle(LED_YELLOW);
                break;
            }
        }
        systick_delay_ms(STM0,100);
    }
}

