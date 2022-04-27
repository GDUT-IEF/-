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
 ** ��������: ������ʾ����ʾ��Ӧ�Ĳ���
 ** ��    ��: ��Ӧ������flag
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 */
void KeyParameterDisplay(uint8 flag)
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
        case 6:
        {
            lcd_showstr(0, 0, "Motor.D:");
            lcd_showfloat(0, 1, MotorK.D, 2, 3);
            break;
        }
    }
}

/*
 ** ��������: ������ʾ����ʾ���в���
 ** ��    ��: ��
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 */
void ParameterDisplay(void)
{
    lcd_clear(WHITE);
    lcd_showint16(0, 0, base_speed);
    lcd_showfloat(0, 1, diff_speed_kp, 2, 3);
    lcd_showfloat(0, 2, SteerK.P, 2, 3);
    lcd_showfloat(0, 3, SteerK.D, 2, 3);
    lcd_showfloat(0, 4, MotorK.P, 2, 3);
    lcd_showfloat(0, 5, MotorK.I, 2, 3);
    lcd_showfloat(0, 6, MotorK.D, 2, 3);
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
            KeyParameterDisplay(key_num);
            break;
        }
        case KEY_S2_PRES:   //S2��ѡ����һ������
        {
            if(key_num<6)
            {
                key_num++;  //ѡ����һ������
            }
            KeyParameterDisplay(key_num);
            break;
        }
        case KEY_S3_PRES:   //S3��Ŀǰ��������
        {
            KeyParameterDisplay(key_num);
            switch(key_num)
            {
                case 0: //�����ٶ�
                {
                    base_speed+=5;
                    lcd_showint16(0, 1, base_speed);
                    break;
                }
                case 1: //����
                {
                    diff_speed_kp+=0.05;
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
                    SteerK.D+=1;
                    lcd_showfloat(0, 1, SteerK.D, 2, 3);
                    break;
                }
                case 4: //���P
                {
                    MotorK.P+=1;
                    lcd_showfloat(0, 1, MotorK.P, 2, 3);
                    break;
                }
                case 5: //���I
                {
                    MotorK.I+=0.1;
                    lcd_showfloat(0, 1, MotorK.I, 2, 3);
                    break;
                }
                case 6:
                {
                    MotorK.D+=0.1;
                    lcd_showfloat(0, 1, MotorK.D, 2, 3);
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
            KeyParameterDisplay(key_num);
            switch(key_num)
            {
                case 0: //�����ٶ�
                {
                    base_speed-=5;
                    lcd_showint16(0, 1, base_speed);
                    break;
                }
                case 1: //����
                {
                    diff_speed_kp-=0.05;
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
                    SteerK.D-=1;
                    lcd_showfloat(0, 1, SteerK.D, 2, 3);
                    break;
                }
                case 4: //���P
                {
                    MotorK.P-=1;
                    lcd_showfloat(0, 1, MotorK.P, 2, 3);
                    break;
                }
                case 5: //���I
                {
                    MotorK.I-=0.1;
                    lcd_showfloat(0, 1, MotorK.I, 2, 3);
                    break;
                }
                case 6:
                {
                    MotorK.D-=0.1;
                    lcd_showfloat(0, 1, MotorK.D, 2, 3);
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
            ParameterDisplay();
            systick_delay_ms(STM0,3000);
            return 1;
        }
        default:break;
    }
    systick_delay_ms(STM0,100);
    return 0;
}

/*
 ** ��������: �ٶ���ʾ����ʾ��Ӧ״̬���ٶ�
 ** ��    ��: ��Ӧ�ٶȵ�flag
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 */
void KeyBaseSpeedDisplay(uint8 flag)
{
    lcd_clear(WHITE);
    switch(flag)
    {
        case 0:
        {
            lcd_showstr(0, 0, "Left_CircleIsland:");
            lcd_showint16(0, 1, base_speed);
            break;
        }
        case 1:
        {
            lcd_showstr(0, 0, "First_CrossLoop:");
            lcd_showint16(0, 1, speed_case_1);
            break;
        }
        case 2:
        {
            lcd_showstr(0, 0, "Right_CircleIsland:");
            lcd_showint16(0, 1, speed_case_2);
            break;
        }
        case 3:
        {
            lcd_showstr(0, 0, "Left_Garage:");
            lcd_showint16(0, 1, speed_case_3);
            break;
        }
        case 4:
        {
            lcd_showstr(0, 0, "First_Fork:");
            lcd_showint16(0, 1, speed_case_4);
            break;
        }
        case 5:
        {
            lcd_showstr(0, 0, "Second_CrossLoop:");
            lcd_showint16(0, 1, speed_case_5);
            break;
        }
        case 6:
        {
            lcd_showstr(0, 0, "Second_Fork:");
            lcd_showint16(0, 1, speed_case_6);
            break;
        }
        case 7:
        {
            lcd_showstr(0, 0, "Right_Garage:");
            lcd_showint16(0, 1, speed_case_7);
            break;
        }
    }
}

/*
 ** ��������: �ٶ���ʾ����ʾ����״̬���ٶ�
 ** ��    ��: ��
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 */
void BaseSpeedDisplay(void)
{
    lcd_clear(WHITE);
    lcd_showint16(0, 0, base_speed);
    lcd_showint16(0, 1, speed_case_1);
    lcd_showint16(0, 2, speed_case_2);
    lcd_showint16(0, 3, speed_case_3);
    lcd_showint16(0, 4, speed_case_4);
    lcd_showint16(0, 5, speed_case_5);
    lcd_showint16(0, 6, speed_case_6);
    lcd_showint16(0, 7, speed_case_7);
}

/*
 ** ��������: ������������״̬���ٶ�
 ** ��    ��: ��
 ** �� �� ֵ: �Ƿ��˳����Σ�1:�˳�����  0:��������
 ** ��    ��: WBN
 */
uint8 KeyBaseSpeed(void)
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
            KeyBaseSpeedDisplay(key_num);
            break;
        }
        case KEY_S2_PRES:   //S2��ѡ����һ������
        {
            if(key_num<7)
            {
                key_num++;  //ѡ����һ������
            }
            KeyBaseSpeedDisplay(key_num);
            break;
        }
        case KEY_S3_PRES:   //S3��Ŀǰ��������
        {
            KeyBaseSpeedDisplay(key_num);
            switch(key_num)
            {
                case 0: //�����ٶȣ�״̬0���󻷵�
                {
                    base_speed+=5;
                    lcd_showint16(0, 1, base_speed);
                    break;
                }
                case 1: //״̬1��
                {
                    speed_case_1+=5;
                    lcd_showint16(0, 1, speed_case_1);
                    break;
                }
                case 2: //״̬2��
                {
                    speed_case_2+=5;
                    lcd_showint16(0, 1, speed_case_2);
                    break;
                }
                case 3: //״̬3��
                {
                    speed_case_3+=5;
                    lcd_showint16(0, 1, speed_case_3);
                    break;
                }
                case 4: //״̬4��
                {
                    speed_case_4+=5;
                    lcd_showint16(0, 1, speed_case_4);
                    break;
                }
                case 5: //״̬5��
                {
                    speed_case_5+=5;
                    lcd_showint16(0, 1, speed_case_5);
                    break;
                }
                case 6: //״̬6��
                {
                    speed_case_6+=5;
                    lcd_showint16(0, 1, speed_case_6);
                    break;
                }
                case 7: //״̬7��
                {
                    speed_case_7+=5;
                    lcd_showint16(0, 1, speed_case_7);
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
            KeyBaseSpeedDisplay(key_num);
            switch(key_num)
            {
                case 0: //�����ٶȣ�״̬0���󻷵�
                {
                    base_speed-=5;
                    lcd_showint16(0, 1, base_speed);
                    break;
                }
                case 1: //״̬1��
                {
                    speed_case_1-=5;
                    lcd_showint16(0, 1, speed_case_1);
                    break;
                }
                case 2: //״̬2��
                {
                    speed_case_2-=5;
                    lcd_showint16(0, 1, speed_case_2);
                    break;
                }
                case 3: //״̬3��
                {
                    speed_case_3-=5;
                    lcd_showint16(0, 1, speed_case_3);
                    break;
                }
                case 4: //״̬4��
                {
                    speed_case_4-=5;
                    lcd_showint16(0, 1, speed_case_4);
                    break;
                }
                case 5: //״̬5��
                {
                    speed_case_5-=5;
                    lcd_showint16(0, 1, speed_case_5);
                    break;
                }
                case 6: //״̬6��
                {
                    speed_case_6-=5;
                    lcd_showint16(0, 1, speed_case_6);
                    break;
                }
                case 7: //״̬7��
                {
                    speed_case_7-=5;
                    lcd_showint16(0, 1, speed_case_7);
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
            BaseSpeedDisplay();
            systick_delay_ms(STM0,1000);
            return 1;
        }
        default:break;
    }
    systick_delay_ms(STM0,100);
    return 0;
}

/*
 ** ��������: ����ѡ��ͬ�ķ�������
 ** ��    ��: ��
 ** �� �� ֵ: �Ƿ��˳����Σ�1:�˳�ѡ��ֱ�ӷ���  2���˳�  0:����ѡ��
 ** ��    ��: WBN
 ** ע    �⣺���������ã���Ҫ�Ǽ��׹̶��Ĳ���
 */
uint8 KeyPlan(void)
{
    static uint8 key_num;   //�����Ĳ���
    switch(KeyScan())
    {
        case KEY_S1_PRES:   //S1������һ�����Ԫ�ؼ��ٷ���
        {

            return 1;
        }
        case KEY_S2_PRES:   //S2����������ȫ��Ԫ�ؼ��ٷ���
        {

            return 1;
        }
        case KEY_S3_PRES:   //S3�������������Ԫ�����ٷ���
        {
            
            return 1;
        }
        case KEY_S4_PRES:   //S4�������ģ��˳����зֶ�΢��
        {
            return 2;
        }
        case KEY_S5_PRES:   //S5��Ĭ�Ϸ�����ֱ�ӷ���
        {

            return 1;
        }
        default:break;
    }
    systick_delay_ms(STM0,100);
    return 0;
}

