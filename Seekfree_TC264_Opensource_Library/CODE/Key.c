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
#include "LED.h"
#include "oled.h"           //OLED��ʾ

extern float encoder_distance;

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
 ** ��������: ����PID���ε�OLED������ʾ
 ** ��    ��: ��
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 */
void PIDParameterDisplay(uint8 key_num)
{
    OLED_clear();
    switch(key_num)
    {
        case 0: //���P
        {
            OLED_ShowStr(0, 1, "Steer.P:", 2);
            OLED_ShowFloat(0, 4, SteerK.P, 2);
            break;
        }
        case 1: //���D
        {
            OLED_ShowStr(0, 1, "Steer.D:", 2);
            OLED_ShowFloat(0, 4, SteerK.D, 2);
            break;
        }
        case 2: //����P
        {
            OLED_ShowStr(0, 1, "Motor_L.P:", 2);
            OLED_ShowFloat(0, 4, MotorK_L.P, 2);
            break;
        }
        case 3: //����I
        {
            OLED_ShowStr(0, 1, "Motor_L.I:", 2);
            OLED_ShowFloat(0, 4, MotorK_L.I, 2);
            break;
        }
        case 4: //�ҵ��P
        {
            OLED_ShowStr(0, 1, "Motor_R.P:", 2);
            OLED_ShowFloat(0, 4, MotorK_R.P, 2);
            break;
        }
        case 5: //�ҵ��I
        {
            OLED_ShowStr(0, 1, "Motor_R.I:", 2);
            OLED_ShowFloat(0, 4, MotorK_R.I, 2);
            break;
        }
    }
}

/*
 ** ��������: ����PID����
 ** ��    ��: ��
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 */
void KeyPID(void)
{
    uint8 key_num=0;
    while(1)
    {
        switch(KeyScan())
        {
            case KEY_UP:    //�������ֵ
            {
                switch(key_num)
                {
                    case 0:SteerK.P+=1;     break;
                    case 1:SteerK.D+=1;     break;
                    case 2:MotorK_L.P+=10;   break;
                    case 3:MotorK_L.I+=0.1;   break;
                    case 4:MotorK_R.P+=1;   break;
                    case 5:MotorK_R.I+=0.1;   break;
                }
                PIDParameterDisplay(key_num);
                break;
            }
            case KEY_DOWN:  //��С����ֵ
            {
                switch(key_num)
                {
                    case 0:SteerK.P-=1;     break;
                    case 1:SteerK.D-=1;     break;
                    case 2:MotorK_L.P-=10;   break;
                    case 3:MotorK_L.I-=0.1;   break;
                    case 4:MotorK_R.P-=10;   break;
                    case 5:MotorK_R.I-=0.1;   break;
                }
                PIDParameterDisplay(key_num);
                break;
            }
            case KEY_LEFT:  //����л�����
            {
                if(key_num>0)
                {
                    key_num--;
                }
                PIDParameterDisplay(key_num);
                break;
            }
            case KEY_RIGHT: //��ǰ�л�����
            {
                if(key_num<5)
                {
                    key_num++;
                }
                PIDParameterDisplay(key_num);
                break;
            }
            case KEY_ENTER: //�˳�����
            {
                OLED_clear();
                OLED_ShowFloat(0, 1, SteerK.P, 2);
                OLED_ShowFloat(0, 2, SteerK.D, 2);
                OLED_ShowFloat(0, 3, MotorK_L.P, 2);
                OLED_ShowFloat(0, 4, MotorK_L.I, 2);
                OLED_ShowFloat(0, 5, MotorK_R.P, 2);
                OLED_ShowFloat(0, 6, MotorK_R.I, 2);
                return;
            }
        }
        systick_delay_ms(STM0,100);
    }
}

/*
 ** ��������: ����Process���ε�OLED������ʾ
 ** ��    ��: ��
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 */
void ProcessParameterDisplay(uint8 key_num)
{
    OLED_clear();
    switch(key_num)
    {
        case 0: //״̬��״̬
        {
            OLED_ShowStr(0, 1, "process_flag:", 2);
            OLED_ShowNum(0, 4, process_flag, 1, 1);
            break;
        }
        case 1: //�ٶ�
        {
            OLED_ShowStr(0, 1, "base_speed:", 2);
            OLED_ShowNum(0, 4, base_speed, 3, 1);
            break;
        }
        case 2: //���������
        {
            OLED_ShowStr(0, 1, "encoder_distance", 2);
            OLED_ShowFloat(0, 4, encoder_distance, 1);
            break;
        }
    }
}

/*
 ** ��������: ����Process����
 ** ��    ��: ��
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 */
void KeyProcess(void)
{
    OLED_clear();
    OLED_ShowNum(0, 1, process_flag, 1, 1);
    OLED_ShowNum(0, 2, base_speed, 1, 1);
    OLED_ShowFloat(0, 3, encoder_distance, 1);

    uint8 key_num=0;
    while(1)
    {
        switch(KeyScan())
        {
            case KEY_UP:    //�������ֵ
            {
                switch(key_num)
                {
                    case 0: process_flag++;         break;
                    case 1: base_speed+=5;          break;
                    case 2: encoder_distance+=1;    break;
                }
                ProcessParameterDisplay(key_num);
                break;
            }
            case KEY_DOWN:  //��С����ֵ
            {
                switch(key_num)
                {
                    case 0: process_flag--;         break;
                    case 1: base_speed-=5;          break;
                    case 2: encoder_distance-=1;    break;
                }
                ProcessParameterDisplay(key_num);
                break;
            }
            case KEY_LEFT:  //����л�����
            {
                if(key_num>0)
                {
                    key_num--;
                }
                ProcessParameterDisplay(key_num);
                break;
            }
            case KEY_RIGHT: //��ǰ�л�����
            {
                if(key_num<2)
                {
                    key_num++;
                }
                ProcessParameterDisplay(key_num);
                break;
            }
            case KEY_ENTER: //�˳�����
            {
                OLED_clear();
                OLED_ShowNum(0, 1, process_flag, 1, 1);
                OLED_ShowNum(0, 2, base_speed, 3, 1);
                OLED_ShowFloat(0, 3, encoder_distance, 1);
                return;
            }
        }
        systick_delay_ms(STM0,100);
    }
}

