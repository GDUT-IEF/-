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

void KeyDisplay(uint8 flag)
{
    lcd_clear(WHITE);
    switch(flag)
    {
        case 0:
        {
            lcd_showstr(0, 0, "base_speed:");
            lcd_showint16(0, 1, base_speed);
            break;
        }
        case 1:
        {
            lcd_showstr(0, 0, "diff_speed_kp:");
            lcd_showfloat(0, 1, diff_speed_kp, 2, 3);
            break;
        }
        case 2:
        {
            lcd_showstr(0, 0, "Steer.P:");
            lcd_showfloat(0, 1, SteerK.P, 2, 3);
            break;
        }
        case 3:
        {
            lcd_showstr(0, 0, "Steer.D:");
            lcd_showfloat(0, 1, SteerK.D, 2, 3);
            break;
        }
        case 4:
        {
            lcd_showstr(0, 0, "Motor.P:");
            lcd_showfloat(0, 1, MotorK.P, 2, 3);
            break;
        }
        case 5:
        {
            lcd_showstr(0, 0, "Motor.I:");
            lcd_showfloat(0, 1, MotorK.I, 2, 3);
            break;
        }
    }
}

void ParameterDisplay(void)
{
    lcd_showint16(0, 0, base_speed);
    lcd_showfloat(0, 1, diff_speed_kp, 2, 3);
    lcd_showfloat(0, 2, SteerK.P, 2, 3);
    lcd_showfloat(0, 3, SteerK.D, 2, 3);
    lcd_showfloat(0, 4, MotorK.P, 2, 3);
    lcd_showfloat(0, 5, MotorK.I, 2, 3);
}

/*
 ** ��������: ��������
 ** ��    ��: ��
 ** �� �� ֵ: �Ƿ��˳����Σ�1:�˳�����  0:��������
 ** ��    ��: WBN
 */
uint8 KeyParameter(void)
{
    static uint8 key_num;   //�����Ĳ���
    switch(KeyScan())
    {
        case KEY_S1_PRES:   //S1��ѡ����һ������
        {
            if(key_num>0)
            {
                key_num--;  //ѡ����һ������
            }
            KeyDisplay(key_num);
            break;
        }
        case KEY_S2_PRES:   //S2��ѡ����һ������
        {
            if(key_num<5)
            {
                key_num++;  //ѡ����һ������
            }
            KeyDisplay(key_num);
            break;
        }
        case KEY_S3_PRES:   //S3��Ŀǰ��������
        {
            KeyDisplay(key_num);
            switch(key_num)
            {
                case 0: //�����ٶ�
                {
                    base_speed+=1;
                    lcd_showint16(0, 1, base_speed);
                    break;
                }
                case 1: //����
                {
                    diff_speed_kp+=0.1;
                    lcd_showfloat(0, 1, diff_speed_kp, 2, 3);
                    break;
                }
                case 2: //���P
                {
                    SteerK.P+=0.1;
                    lcd_showfloat(0, 1, SteerK.P, 2, 3);
                    break;
                }
                case 3: //���D
                {
                    SteerK.D+=0.1;
                    lcd_showfloat(0, 1, SteerK.D, 2, 3);
                    break;
                }
                case 4: //���P
                {
                    MotorK.P+=0.1;
                    lcd_showfloat(0, 1, MotorK.P, 2, 3);
                    break;
                }
                case 5: //���I
                {
                    MotorK.I+=0.1;
                    lcd_showfloat(0, 1, MotorK.I, 2, 3);
                    break;
                }
                default:
                {
                    lcd_showstr(0, 4, "ERROR!!!");
                    break;
                }
            }
            break;
        }
        case KEY_S4_PRES:   //S4��Ŀǰ������С
        {
            KeyDisplay(key_num);
            switch(key_num)
            {
                case 0: //�����ٶ�
                {
                    base_speed-=1;
                    lcd_showint16(0, 1, base_speed);
                    break;
                }
                case 1: //����
                {
                    diff_speed_kp-=0.1;
                    lcd_showfloat(0, 1, diff_speed_kp, 2, 3);
                    break;
                }
                case 2: //���P
                {
                    SteerK.P-=0.1;
                    lcd_showfloat(0, 1, SteerK.P, 2, 3);
                    break;
                }
                case 3: //���D
                {
                    SteerK.D-=0.1;
                    lcd_showfloat(0, 1, SteerK.D, 2, 3);
                    break;
                }
                case 4: //���P
                {
                    MotorK.P-=0.1;
                    lcd_showfloat(0, 1, MotorK.P, 2, 3);
                    break;
                }
                case 5: //���I
                {
                    MotorK.I-=0.1;
                    lcd_showfloat(0, 1, MotorK.I, 2, 3);
                    break;
                }
                default:
                {
                    lcd_showstr(0, 4, "ERROR!!!");
                    break;
                }
            }
            break;
        }
        case KEY_S5_PRES:   //S5��������ֵ���˳�����
        {
            lcd_clear(WHITE);
            lcd_showint16(0, 0, base_speed);
            lcd_showfloat(0, 1, diff_speed_kp, 2, 3);
            lcd_showfloat(0, 2, SteerK.P, 2, 3);
            lcd_showfloat(0, 3, SteerK.D, 2, 3);
            lcd_showfloat(0, 4, MotorK.P, 2, 3);
            lcd_showfloat(0, 5, MotorK.I, 2, 3);
            systick_delay_ms(STM0,4000);
            return 1;
        }
        default:break;
    }
    systick_delay_ms(STM0,100);
    return 0;
}



