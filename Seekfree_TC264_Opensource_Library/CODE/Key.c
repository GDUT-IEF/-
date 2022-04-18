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

/*
 ** ��������: ��������
 ** ��    ��: ��
 ** �� �� ֵ: �Ƿ��˳����Σ�1:�˳�����  0:��������
 ** ��    ��: WBN
 */
uint8 KeyParameter(void)
{
    static uint8 key_num;   //�����Ĳ���
    static float key_steer_p=SteerK.P,key_steer_d=SteerK.D,key_motor_p=MotorK.P,key_motor_i=MotorK.I,key_diff_speed_kp=diff_speed_kp;    //��ʼ�����ڵ�PID����
    static int16 key_base_speed=base_speed; //��ʼ�����ڵĻ����ٶ�
    switch(KeyScan())
    {
        case KEY_S1_PRES:   //S1��ѡ����һ������
        {
            key_num++;  //ѡ����һ������
            break;
        }
        case KEY_S2_PRES:   //S2��ѡ����һ������
        {
            key_num--;  //ѡ����һ������
            break;
        }
        case KEY_S3_PRES:   //S3��Ŀǰ��������
        {
            switch(key_num)
            {
                case 0: //�����ٶ�
                {
                    key_base_speed+=1;
                    lcd_showstr(0, 0, "Base Speed:")
                    lcd_showint16(0, 1, key_base_speed);
                }
                case 1: //����
                {
                    key_diff_speed_kp+=0.1;
                    lcd_showstr(0, 0, "Different Speed:")
                    lcd_showfloat(0, 1, key_diff_speed_kp, 2, 3);
                }
                case 2: //���P
                {
                    key_steer_p+=0.1;
                    lcd_showstr(0, 0, "Steer.P:")
                    lcd_showfloat(0, 1, key_steer_p, 2, 3);
                }
                case 3: //���D
                {
                    key_steer_d+=0.1;
                    lcd_showstr(0, 0, "Steer.D:")
                    lcd_showfloat(0, 1, key_steer_d, 2, 3);
                }
                case 4: //���P
                {
                    key_motor_p+=0.1;
                    lcd_showstr(0, 0, "Motor.P:")
                    lcd_showfloat(0, 1, key_motor_p, 2, 3);
                }
                case 5: //���I
                {
                    key_motor_i+=0.1;
                    lcd_showstr(0, 0, "Motor.I:")
                    lcd_showfloat(0, 1, key_motor_i, 2, 3);
                }

            }
            break;
        }
        case KEY_S4_PRES:   //S4��Ŀǰ������С
        {
            switch(key_num)
            {
                case 0: //�����ٶ�
                {
                    key_base_speed-=1;
                    lcd_showstr(0, 0, "Base Speed:")
                    lcd_showint16(0, 1, key_base_speed);
                }
                case 1: //����
                {
                    key_diff_speed_kp-=0.1;
                    lcd_showstr(0, 0, "Different Speed:")
                    lcd_showfloat(0, 1, key_diff_speed_kp, 2, 3);
                }
                case 2: //���P
                {
                    key_steer_p-=0.1;
                    lcd_showstr(0, 0, "Steer.P:")
                    lcd_showfloat(0, 1, key_steer_p, 2, 3);
                }
                case 3: //���D
                {
                    key_steer_d-=0.1;
                    lcd_showstr(0, 0, "Steer.D:")
                    lcd_showfloat(0, 1, key_steer_d, 2, 3);
                }
                case 4: //���P
                {
                    key_motor_p-=0.1;
                    lcd_showstr(0, 0, "Motor.P:")
                    lcd_showfloat(0, 1, key_motor_p, 2, 3);
                }
                case 5: //���I
                {
                    key_motor_i-=0.1;
                    lcd_showstr(0, 0, "Motor.I:")
                    lcd_showfloat(0, 1, key_motor_i, 2, 3);
                }

            }
            break;
        }
        case KEY_S5_PRES:   //S5��������ֵ���˳�����
        {
            base_speed=key_base_speed;
            diff_speed_kp=key_diff_speed_kp;
            SteerK.P=key_steer_p;
            SteerK.D=key_steer_d;
            MotorK.P=key_motor_p;
            MotorK.I=key_motor_i;
            return 1;
        }
    }
    systick_delay_ms(STM0,100);
    return 0;
}



