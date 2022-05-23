/*
 * LED.c
 *
 *  Created on: 2022��5��23��
 *      Author: yue
 */

#include "LED.h"
#include "zf_gpio.h"

/*
 ** ��������: ��ʼ��LED��ӦIO�ڣ������˺��İ�����ذ��ϵ�LED
 ** ��    ��: ��
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 */
void LEDInit(void)
{
    gpio_init(P20_8, GPO, 1, PUSHPULL);     //��ʼ�����İ��LED
    gpio_init(P20_9, GPO, 1, PUSHPULL);
    gpio_init(P21_4, GPO, 1, PUSHPULL);
    gpio_init(P21_5, GPO, 1, PUSHPULL);
    gpio_init(P23_1, GPO, 1, PUSHPULL);     //��ʼ�����ذ��LED
    gpio_init(P22_1, GPO, 1, PUSHPULL);
    gpio_init(P22_2, GPO, 1, PUSHPULL);
    gpio_init(P22_3, GPO, 1, PUSHPULL);
    gpio_init(P21_2, GPO, 1, PUSHPULL);
}


