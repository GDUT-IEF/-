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
#include "ImageSpecial.h"

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
    uint8 key_num=0;
    ProcessParameterDisplay(key_num);
    while(1)
    {
        switch(KeyScan())
        {
            case KEY_UP:    //�������ֵ
            {
                switch(key_num)
                {
                    case 0: if(process_flag<7)process_flag++;break;
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
                    case 0: if(process_flag>0)process_flag--;break;
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

/*
 ** ��������: ����Image���ε�OLED������ʾ
 ** ��    ��: ��
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 */
void ImageParameterDisplay(uint8 key_num_1,uint8 key_num_2)
{
    OLED_clear();
    //��ȷ��key_num_1����ȷ��key_num_2
    switch(key_num_1)
    {
        case 0: //��ʮ��
        {
            OLED_ShowStr(0, 1, "CrossLoop_L", 2);
            switch(key_num_2)
            {
                case 0:OLED_ShowStr(0, 4, "CrossLoopBegin_L", 1);       break;
                case 1:OLED_ShowStr(0, 4, "CrossLoopOverBegin_L", 1);   break;
                case 2:OLED_ShowStr(0, 4, "CrossLoopEnd_L", 1);         break;
            }
            break;
        }
        case 1: //�ҳ���ֱ��
        {
            OLED_ShowStr(0, 1, "Garage_R", 2);
            switch(key_num_2)
            {
                case 0:OLED_ShowStr(0, 4, "RNINGarageIdentify", 1);break;
                case 1:OLED_ShowStr(0, 4, "ZebraIndentify", 1);break;
            }
            break;
        }
        case 2: //��һ������
        {
            OLED_ShowStr(0, 1, "ForkRoad_First", 2);
            switch(key_num_2)
            {
                case 0:OLED_ShowStr(0, 4, "ForkTurnRIdentify", 1);break;
            }
            break;
        }
        case 3: //�һ���
        {
            OLED_ShowStr(0, 1, "CircleIsland_R", 2);
            switch(key_num_2)
            {
                case 0:OLED_ShowStr(0, 4, "CircleIslandExit_R", 1);     break;
                case 1:OLED_ShowStr(0, 4, "CircleIslandMid_R", 1);      break;
                case 2:OLED_ShowStr(0, 4, "CircleIslandBegin_R", 1);    break;
                case 3:OLED_ShowStr(0, 4, "CircleIslandEnd_R", 1);      break;
                case 4:OLED_ShowStr(0, 4, "CircleIslandOverBegin_R", 1);break;
            }
            break;
        }
        case 4: //��ʮ��
        {
            OLED_ShowStr(0, 1, "CrossLoop_R", 2);
            switch(key_num_2)
            {
                case 0:OLED_ShowStr(0, 4, "CrossLoopBegin_R", 1);       break;
                case 1:OLED_ShowStr(0, 4, "CrossLoopOverBegin_R", 1);   break;
                case 2:OLED_ShowStr(0, 4, "CrossLoopEnd_R", 1);         break;
            }
            break;
        }
        case 5: //�󻷵�
        {
            OLED_ShowStr(0, 1, "CircleIsland_L", 2);
            switch(key_num_2)
            {
                case 0:OLED_ShowStr(0, 4, "CircleIslandExit_L", 1);     break;
                case 1:OLED_ShowStr(0, 4, "CircleIslandMid_L", 1);      break;
                case 2:OLED_ShowStr(0, 4, "CircleIslandBegin_L", 1);    break;
                case 3:OLED_ShowStr(0, 4, "CircleIslandEnd_L", 1);      break;
                case 4:OLED_ShowStr(0, 4, "CircleIslandOverBegin_L", 1);break;
            }
            break;
        }
        case 6: //�ڶ�������
        {
            OLED_ShowStr(0, 1, "ForkRoad_Second", 2);
            switch(key_num_2)
            {
                case 0:OLED_ShowStr(0, 4, "ForkTurnRIdentify", 1);break;
            }
            break;
        }
        case 7: //�󳵿����
        {
            OLED_ShowStr(0, 1, "Garage_In", 2);
            switch(key_num_2)
            {
                case 0:OLED_ShowStr(0, 4, "ZebraCrossingSearch", 1);break;
                case 1:OLED_ShowStr(0, 4, "GarageInBegin", 1);      break;
                case 2:OLED_ShowStr(0, 4, "GarageInEnd", 1);        break;
            }
            break;
        }
    }
}

/*
 ** ��������: ����Image���ε�ʵ�ʲ����������������Process�б�����
 ** ��    ��: ��
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 */
void ImageParameterHandle(uint8 key_num_1,uint8 key_num_2,Point InflectionL,Point InflectionR)
{
    uint8 a=0;
    //��ȷ��key_num_1����ȷ��key_num_2
    switch(key_num_1)
    {
        case 0: //��ʮ��
        {
            switch(key_num_2)
            {
                case 0:CrossLoopBegin_L(LeftLine, RightLine, InflectionL, InflectionR);     break;
                case 1:CrossLoopOverBegin_L(LeftLine, RightLine, InflectionL, InflectionR); break;
                case 2:CrossLoopEnd_L();                                                    break;
            }
            break;
        }
        case 1: //�ҳ���ֱ��
        {
            switch(key_num_2)
            {
                case 0:RNINGarageIdentify(InflectionL, InflectionR);break;
                case 1:ZebraIndentify(80, 50, &a);break;
            }
            break;
        }
        case 2: //��һ������
        {
            switch(key_num_2)
            {
                case 0:if(ForkTurnRIdentify(LeftLine, RightLine, InflectionL, InflectionR)==1)gpio_toggle(LED_WHITE);break;
            }
            break;
        }
        case 3: //�һ���
        {
            switch(key_num_2)
            {
                case 0:CircleIslandExit_R(InflectionR);     break;
                case 1:if(CircleIslandMid_R()==1)gpio_toggle(LED_WHITE);break;
                case 2:CircleIslandBegin_R();               break;
                case 3:CircleIslandEnd_R();                 break;
                case 4:CircleIslandOverBegin_R(RightLine);  break;
            }
            break;
        }
        case 4: //��ʮ��
        {
            switch(key_num_2)
            {
                case 0:CrossLoopBegin_R(LeftLine, RightLine, InflectionL, InflectionR);       break;
                case 1:CrossLoopOverBegin_R(LeftLine, RightLine, InflectionL, InflectionR);   break;
                case 2:CrossLoopEnd_R();                                                      break;
            }
            break;
        }
        case 5: //�󻷵�
        {
            switch(key_num_2)
            {
                case 0:CircleIslandExit_L(InflectionL);     break;
                case 1:if(CircleIslandMid_L()==1)gpio_toggle(LED_WHITE);break;
                case 2:CircleIslandBegin_L();               break;
                case 3:CircleIslandEnd_L();                 break;
                case 4:CircleIslandOverBegin_L(LeftLine);   break;
            }
            break;
        }
        case 6: //�ڶ�������
        {
            switch(key_num_2)
            {
                case 0:if(ForkTurnRIdentify(LeftLine, RightLine, InflectionL, InflectionR)==1)gpio_toggle(LED_WHITE);break;
            }
            break;
        }
        case 7: //�󳵿����
        {
            switch(key_num_2)
            {
                case 0:if(ZebraCrossingSearch(MT9V03X_H/2+15, MT9V03X_H/2-15)==1)gpio_toggle(LED_WHITE); break;
                case 1:GarageInBegin();                                     break;
                case 2:if(GarageInEnd()==1)gpio_toggle(LED_WHITE);          break;
            }
            break;
        }
    }
}

uint8 key_num_1=0,key_num_2=2;  //key_num_1����һ��ѡ��ѡ��״̬����key_num_2���ڶ���ѡ��ѡ������

/*
 ** ��������: ����Image����
 ** ��    ��: ��
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 */
void KeyImage(void)
{
    uint8 sum_num_2=0;  //����key_num_1�������������Ӷ�����key_num_2
    uint8 flag=0;       //�Ƿ���Ч���°���
    ImageParameterDisplay(key_num_1, key_num_2);
    while(1)
    {
        //����ɨ��
        switch(KeyScan())
        {
            case KEY_UP:    //��ǰ�л�����
            {
                if(key_num_2<sum_num_2) //sum_num_2+1������
                {
                    key_num_2++;
                    flag=1;
                }
                break;
            }
            case KEY_DOWN:  //����л�����
            {
                if(key_num_2>0)
                {
                    key_num_2--;
                    flag=1;
                }
                break;
            }
            case KEY_LEFT:  //����л�״̬
            {
                if(key_num_1>0)
                {
                    key_num_1--;
                    key_num_2=0;
                    flag=1;
                }
                break;
            }
            case KEY_RIGHT: //��ǰ�л�״̬
            {
                if(key_num_1<7) //8��״̬
                {
                    key_num_1++;
                    key_num_2=0;
                    flag=1;
                }
                break;
            }
            case KEY_ENTER: //
            {

                break;
            }
        }
        //�������ݴ���
        if(flag==1) //��Ч��������
        {
            switch(key_num_1)
            {
                case 0:sum_num_2=2; break;  //��ʮ��
                case 1:sum_num_2=1; break;  //�󳵿�
                case 2:sum_num_2=0; break;  //����
                case 3:sum_num_2=4; break;  //�һ���
                case 4:sum_num_2=2; break;  //��ʮ��
                case 5:sum_num_2=4; break;  //�󻷵�
                case 6:sum_num_2=0; break;  //����
                case 7:sum_num_2=2; break;  //���
            }
            ImageParameterDisplay(key_num_1, key_num_2);    //����������ʾ
            flag=0;
        }

        systick_delay_ms(STM0,100);
    }
}

