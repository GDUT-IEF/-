/*
 * Garage.c
 * Created on: 2022��5��25��
 * Author: 30516
 * Effect: ��ų�����ص�Դ����
 * *�ұ߲���⣺180 ��⣺170
 * ��߲���⣺200 ��⣺180���Ǹо�ͣ������
 */

#include "ImageSpecial.h"
#include "PID.h"
#include "LED.h" //debug
#include "headfile.h"
#include "ICM20602.h"           //�����ǻ�����ɱ�־�����Լ��������ֺ���

#define GARAGE_IDENTIFY_MODE 0    //����ģʽ���Ϲյ�
#define IN_L_GARAGE_ANGLE   50  //����⿪�������ǻ��ֵ�Ŀ��Ƕ�
#define IN_R_GARAGE_ANGLE   60  //���ҿ⿪�������ǻ��ֵ�Ŀ��Ƕ�

/********************************************************************************************
 ** ��������: Sobel���Ӽ��������
 ** ��    ��: ��
 ** �� �� ֵ: Sobel��ֵ
 ** ��    ��: ʦ��
 *********************************************************************************************/
int64 SobelTest(void)
{
    int64 Sobel = 0;
    int64 temp = 0;

    for (uint8 i = MT9V03X_H-1-20; i > 50 ; i--)
    {
        for (uint8 j = 40; j < MT9V03X_W-1-40; j++)
        {
            int64 Gx = 0, Gy = 0;
            Gx = (-1*BinaryImage(i-1, j-1) + BinaryImage(i-1, j+1) - 2*BinaryImage(i, j-1)
                  + 2*BinaryImage(i, j+1) - BinaryImage(i+1, j-1) + BinaryImage(i+1, j+1));
            Gy = (-1 * BinaryImage(i-1, j-1) - 2 * BinaryImage(i-1, j) - BinaryImage(i-1, j+1)
                  + BinaryImage(i+1, j+1) + 2 * BinaryImage(i+1, j) + BinaryImage(i+1, j+1));
            temp += FastABS(Gx) + FastABS(Gy);
            Sobel += temp / 255;
            temp = 0;
        }
    }
    return Sobel;
}

/********************************************************************************************
 ** ��������: �󳵿�ʶ���ߺ���
 ** ��    ��: char Choose : ѡ���Ƿ�Ҫ��⣬'Y'or'N'
 **           Point InflectionL:��յ�
 **           Point InflectionR:�ҹյ�
 ** �� �� ֵ: 0���Ѿ����˳�����
 **           1����δ���공��
 ** ��    ��: LJF
 ** ע    ��: �����������������ڲ��ߣ��ҵ��Ϲյ㣬����������ʶ��ʶ����Sobel��������Sobel����״̬���ĵ�һ���ж�
 *********************************************************************************************/
uint8 GarageLIdentify(char Choose,Point InflectionL,Point InflectionR)
{
    Point UpInflection;//�Ϲյ�ı���
    UpInflection.X=0,UpInflection.Y=0;//��ʼ��Ϊ0
    uint8 NoInflectionLFlag=0;//�����յ㲻���ڵı�־����������ѡȡ���ֲ��߷�ʽ

    /**********************debug************************/
//    lcd_showint32(TFT_X_MAX-50, 0, InflectionL.X, 3);
//    lcd_showint32(TFT_X_MAX-50, 1, InflectionR.X, 3);
    /***************************************************/

#if GARAGE_IDENTIFY_MODE
    //�˴�ʹ�ñ����������Ϲյ�ķ���
    if(InflectionL.X==0 || (InflectionL.Y-InflectionR.Y)<10 || InflectionL.X>MT9V03X_H/2 || BinaryImage[InflectionL.Y+5][5]!=IMAGE_BLACK)
    {
        if(BinaryImage[MT9V03X_H/2+10][3]==IMAGE_WHITE)
        {
            //�������±��������ҵ��Ϲյ�
            GetUpInflection('L', 20, MT9V03X_H/2+15, &UpInflection);
            NoInflectionLFlag=1;
        }
        else return 0;//����˵����߶��Ǻڵ���ֱ�ӷ����Ѿ��������
    }
    else//�¹յ����
        GetUpInflection('L', 20, InflectionL.Y+15, &UpInflection);
#else
    //�˴�ʹ��ֱ�Ǻڰ��������Ϲյ㷨
    //������ڰ�����·���˲�����յ㲻����,�������ҹյ�֮��ĺ������̫�࣬��Ϊɨ�ߵĻ��ҹյ���ܳ����ڰ������м�
    //������յ����е�ͼ�������Ҫ�Ǻڵ�
    if(InflectionL.X==0 || (InflectionL.Y-InflectionR.Y)<10 || InflectionL.X>MT9V03X_W/2 || BinaryImage[InflectionL.Y+5][5]!=IMAGE_BLACK)
    {
        /**********************debug************************/
//        lcd_showint32(0, 0, BinaryImage[MT9V03X_H/2+10][3], 3);
        /***************************************************/

        //�м���ߵĵĵ�ȥ���Ϲյ�
        if(BinaryImage[MT9V03X_H/2+15][3]==IMAGE_WHITE)
        {
            InflectionL.X=3;InflectionL.Y=MT9V03X_H/2+15;
            NoInflectionLFlag=1;
        }
        else return 0;//����˵����߶��Ǻڵ���ֱ�ӷ����Ѿ��������
    }
    if(NoInflectionLFlag==1 && Choose=='Y')//�����Ϲյ㲻һ����ֱ�ǣ�����ȥ��ʱ����������Ǹ�ֱ����
    {
        for(int row=InflectionL.Y-10;row>10;row--)
        {
            /******************debug:�Ѵ������ϵ��ҵ�켣������******************/
        //        lcd_drawpoint(InflectionL.X, row, YELLOW);
            /******************************************************************/
            //�������ϰ׵���
            if(BinaryImage[row][InflectionL.X]==IMAGE_WHITE && BinaryImage[row-1][InflectionL.X]==IMAGE_BLACK)
            {
                UpInflection.X=InflectionL.X;
                UpInflection.Y=row-3;//�������if���ҵ�������Ҫ���и�ֵ���ܲ�����
                break;
            }
        }
    }
    else
    {
        GetRightangleUPInflection('L',InflectionL,&UpInflection,10,MT9V03X_W-10);
    }
#endif
    //�ж��Ƿ��ҵ��Ϲյ㣬����Ų���
    if(UpInflection.X!=0 && UpInflection.Y!=0)
    {
        switch(Choose)
        {
            //�����
            case 'Y':
            {
                Point RightDownPoint;
                RightDownPoint.X=MT9V03X_W-5;RightDownPoint.Y=MT9V03X_H-5;
                FillingLine('R', RightDownPoint, UpInflection);
                //��ƫ������
                Bias=Regression_Slope(RightDownPoint.Y, UpInflection.Y, RightLine);
                //��б���������ƫ����и�����
                if(Bias<=1)
                    Bias=Bias*1.75;//������Ϊ����⣬�о������Ϲյ㲹�ߵ�ʱ��̫����ʹ�ô򵽿�ߣ������ֶ���С��ǣ��Ͼ����������ä����
                                   //�޸�֮��о����ã������ͷ�����ſ���ICM�Ļ��ǿ��Եģ�������������ڳ�ͷ����������ICMȥ�ж�
                else
                    Bias=Bias*1.5;
                return 1;
            }
            //�������
            case 'N':
            {
                if(NoInflectionLFlag==0)//���û��ʧ�¹յ������¹յ�����Ѳ��
                {
                    Bias=DifferentBias(InflectionL.Y+5, InflectionL.Y, CentreLine);
                }
                else
                {
                    Bias=DifferentBias(UpInflection.Y, UpInflection.Y-5, CentreLine);//ֱ�����Ϲյ��������������ȥѭ��
                }
                return 1;
            }
            default :break;
        }
    }
    return 0;
}
/********************************************************************************************
 ** ��������: �󳵿��״̬��ת��
 ** ��    ��: char Choose: ѡ�������״̬�����ǲ����״̬��
 **           LeftLine����������
 **           RightLine����������
 ** �� �� ֵ: 0��Ԫ��״̬û����
 **           1��Ԫ��״̬����
 ** ��    ��: LJF
 *********************************************************************************************/
uint8 GarageLStatusIdentify(char Choose,Point InflectionL,Point InflectionR,uint8* GarageLFlag)
{
    static uint8 StatusChange;//��һ��ʶ��Ľ����״̬����
    uint8 NowFlag=0;//��ε�ʶ����
    int64 SobelResult=0;//����������Ľ��
    switch(StatusChange)
    {
        //��һ��״̬Sobel��⣬ͨ�����ٿ���ʶ����
        case 0:
        {
            gpio_set(LED_BLUE, 0);
//            lcd_showuint8(0, 1, LostNum_LeftLine);
            if(LostNum_LeftLine>30)
            {
                SobelResult=SobelTest();
//            lcd_showint32(0, 0, SobelResult, 5);
            }
            if(SobelResult>ZebraTresholeL)
            {
                gpio_set(LED_BLUE, 1);
                StatusChange=1;
                break;
            }
            break;
        }
        case 1:
        {
            gpio_set(LED_GREEN, 0);
            switch(Choose)
            {
                case 'Y':
                {
                    gpio_set(LED_GREEN, 1);
                    //���������ǻ����ж��Ƿ����ɹ�
                    StartIntegralAngle_Z(IN_L_GARAGE_ANGLE);
                    StatusChange=2;
                    break;
                }
                case 'N':
                {
                    NowFlag=GarageLIdentify('N', InflectionL, InflectionR);
                    *GarageLFlag=NowFlag;//��ʶ��������ȥ���������滹�費��Ҫ����Ѳ�����ƫ��
                    if(NowFlag==0)
                    {
                        gpio_set(LED_GREEN, 1);
                        StatusChange=3;//��ת������״̬
                        break;
                    }
                    break;
                }
            }
            break;
        }
        case 2:
        {
            gpio_set(LED_WHITE, 0);
#if 0   //0:ֱ�Ӵ������ 1:���ߴ������
            NowFlag=GarageLIdentify('Y', InflectionL, InflectionR);
            *GarageLFlag=NowFlag;//��ʶ��������ȥ���������滹�費��Ҫ����Ѳ�����ƫ��
#else
            SteerK.P=20;SteerK.D=0;//��΢���������ȥ������ֹ����ӷ����򳵿����ȥ
            Bias=5;//�������
            *GarageLFlag=1;//����ȥ���������ѭ������ѭ��
#endif
            //����Ƿ����ɹ������ɹ�ͣ��
            if(icm_angle_z_flag==1)
            {
                return 1;
            }
            break;
        }
        case 3:
        {
            gpio_set(LED_WHITE, 0);
            SobelResult=SobelTest();
            if(SobelResult<ZebraTresholeL)
            {
                gpio_set(LED_WHITE, 1);
                return 1;//�ٴ�sobelһ��ȷ��״̬��û����
            }
            else
            {
                StatusChange=1;//����ص�״̬1�����ж�
                break;
            }
        }
    }
    return 0;
}
/********************************************************************************************
 ** ��������: �ҳ���ʶ���ߺ���
 ** ��    ��: char Choose : ѡ���Ƿ�Ҫ��⣬'Y'or'N'
 **           Point InflectionL:��յ�
 **           Point InflectionR:�ҹյ�
 ** �� �� ֵ: 0���Ѿ����˳�����
 **           1����δ���공��
 ** ��    ��: LJF
 ** ע    ��: �����������������ڲ��ߣ��ҵ��Ϲյ㣬����������ʶ��ʶ����Sobel��������Sobel����״̬���ĵ�һ���ж�
 *********************************************************************************************/
uint8 GarageRIdentify(char Choose,Point InflectionL,Point InflectionR)
{
    Point UpInflection;//�Ϲյ�ı���
    UpInflection.X=0,UpInflection.Y=0;//��ʼ��Ϊ0
    uint8 NoInflectionLFlag=0;//�����յ㲻���ڵı�־����������ѡȡ���ֲ��߷�ʽ

    /**********************debug************************/
//    lcd_showint32(TFT_X_MAX-50, 0, InflectionL.X, 3);
//    lcd_showint32(TFT_X_MAX-50, 1, InflectionR.X, 3);
    /***************************************************/

#if GARAGEIDENTIFYMODE
    //�˴�ʹ�ñ����������Ϲյ�ķ���
    if(InflectionL.X==0 || (InflectionL.Y-InflectionR.Y)<10 || InflectionL.X>MT9V03X_H/2 || BinaryImage[InflectionL.Y+5][5]!=IMAGE_BLACK)
    {
        if(BinaryImage[MT9V03X_H/2+10][3]==IMAGE_WHITE)
        {
            //�������±��������ҵ��Ϲյ�
            GetUpInflection('R', 20, MT9V03X_H/2+15, &UpInflection);
            NoInflectionLFlag=1;
        }
        else return 0;//����˵����߶��Ǻڵ���ֱ�ӷ����Ѿ��������
    }
    else//�¹յ����
        GetUpInflection('R', 20, InflectionL.Y+15, &UpInflection);
#else
    //�˴�ʹ��ֱ�Ǻڰ��������Ϲյ㷨
    //������ڰ�����·���˲����ҹյ㲻����,�������ҹյ�֮��ĺ������̫�࣬��Ϊɨ�ߵĻ��ҹյ���ܳ����ڰ������м�
    //�����ҹյ����е�ͼ�����ұ�-5Ҫ�Ǻڵ�
    /**********************debug************************/
//    lcd_showuint8(30, 1, BinaryImage[InflectionR.Y+5][MT9V03X_W-1]);
    /***************************************************/
    if(InflectionR.X==0 || InflectionR.X<MT9V03X_W/2 || BinaryImage[InflectionR.Y+5][MT9V03X_W-5]==IMAGE_WHITE)
    {
        /**********************debug************************/
//        lcd_showint32(0, 0, BinaryImage[MT9V03X_H/2+10][3], 3);
//        gpio_toggle(LED_BLUE);
        /***************************************************/
        //�м��ұߵĵĵ�ȥ���Ϲյ�,�˴���������֣���������һ��Ҫ�ҵ��׵���Ϊ���Ϲյ������
        for(int row=MT9V03X_H/2+15;row<MT9V03X_H;row++)
        {
            if(BinaryImage[row][MT9V03X_W-3]==IMAGE_WHITE)
            {
                InflectionR.X=MT9V03X_W-3;InflectionR.Y=row+10;//+10����Ϊ�����ѭ����-10�ˣ���ֹ��Ϊ-10ֱ�ӵ���ɫ���������Ҳ����յ�
                NoInflectionLFlag=1;
                break;
            }
        }
        if(NoInflectionLFlag!=1) return 0;//����˵����߶��Ǻڵ���ֱ�ӷ����Ѿ��������
    }
    if(NoInflectionLFlag==1 && Choose=='Y')
    {
        for(int row=InflectionR.Y-10;row>10;row--)
        {
            /******************debug:�Ѵ������ϵ��ҵ�켣������******************/
//          lcd_drawpoint(InflectionL.X, row, YELLOW);
            /******************************************************************/
            //�������ϰ׵���
            if(BinaryImage[row][InflectionR.X]==IMAGE_WHITE && BinaryImage[row-1][InflectionR.X]==IMAGE_BLACK)
            {
                UpInflection.X=InflectionR.X;
                UpInflection.Y=row-4;//�������if���ҵ�������Ҫ���и�ֵ���ܲ�����
                break;
            }
        }
    }
    else
    {
        GetRightangleUPInflection('R',InflectionR,&UpInflection,10,10);
    }
#endif
    //�ж��Ƿ��ҵ��Ϲյ㣬����Ų���
    if(UpInflection.X!=0 && UpInflection.Y!=0)
    {
        switch(Choose)
        {
            //���ҿ�
            case 'Y':
            {
                Point LeftDownPoint;
                LeftDownPoint.X=5;LeftDownPoint.Y=MT9V03X_H-5;
                FillingLine('L', LeftDownPoint, UpInflection);//���ҿⲹ����
                //��ƫ������
                Bias=Regression_Slope(LeftDownPoint.Y, UpInflection.Y, LeftLine);
                //��б���������ƫ����и�����
                if(Bias<=1)
                    Bias=Bias*1.75;//������Ϊ����⣬�о������Ϲյ㲹�ߵ�ʱ��̫����ʹ�ô򵽿�ߣ������ֶ���С��ǣ��Ͼ����������ä����
                                   //�޸�֮��о����ã������ͷ�����ſ���ICM�Ļ��ǿ��Եģ�������������ڳ�ͷ����������ICMȥ�ж�
                else
                    Bias=Bias*1.5;
                return 1;
            }
            //�������
            case 'N':
            {
               if(NoInflectionLFlag==0)//���û��ʧ�¹յ������¹յ�����Ѳ��
               {
                    /******************debug*******************************************/
//                    for(int i=0;i<MT9V03X_W-1;i++)
//                    {
//                        lcd_drawpoint(i, InflectionR.Y+5, PURPLE);
//                        lcd_drawpoint(i, InflectionR.Y, PURPLE);
//                    }
                    /******************************************************************/
                    Bias=DifferentBias(InflectionR.Y+5, InflectionR.Y, CentreLine);
               }
               else
               {
                  /******************debug*******************************************/
//                  for(int i=0;i<MT9V03X_W-1;i++)
//                  {
//                      lcd_drawpoint(i, UpInflection.Y, PURPLE);
//                      lcd_drawpoint(i, UpInflection.Y-5, PURPLE);
//                  }
                  /******************************************************************/
                   //Ī�������BUG���ǲ��߿������ǰ������еĵ��������ĵ�����˲��ߣ���δ����ԭ��
                   Bias=DifferentBias(UpInflection.Y, UpInflection.Y-5, CentreLine);//ֱ�����Ϲյ��������������ȥѭ��
               }
               return 1;
            }
            default :break;
        }
    }
    return 0;
}
/********************************************************************************************
 ** ��������: �ұ߳��ⲻ����״̬��ת��
 ** ��    ��: Point InflectionL�����¹յ�
 **           Point InflectionR: ���¹յ�
 **           uint8* GarageLFlag����״̬���ڵ�ʶ����������س�ȥ
 ** �� �� ֵ: 0��Ԫ��״̬û����
 **           1��Ԫ��״̬����
 ** ��    ��: LJF
 *********************************************************************************************/
uint8 GarageROStatusIdentify(Point InflectionL,Point InflectionR,uint8* GarageLFlag)
{
    static uint8 StatusChange;//��һ��ʶ��Ľ����״̬����
    uint8 NowFlag=0;//��ε�ʶ����
    int64 SobelResult=0;//����������Ľ��
    switch(StatusChange)
    {
        //��һ��״̬Sobel��⣬ͨ�����ٿ���ʶ����
        case 0:
        {
            gpio_set(LED_BLUE, 0);
            if(LostNum_RightLine>25)
            {
                SobelResult=SobelTest();
            }
            if(SobelResult>ZebraTresholeL)
            {
                gpio_set(LED_BLUE, 1);
                StatusChange=1;
                break;
            }
            break;
        }
        case 1:
        {
            NowFlag=GarageRIdentify('N', InflectionL, InflectionR);
            *GarageLFlag=NowFlag;//��ʶ��������ȥ���������滹�費��Ҫ����Ѳ�����ƫ��
            if(NowFlag==0)
            {
                gpio_set(LED_GREEN, 1);
                StatusChange=2;//��ת������״̬
                break;
            }
            break;
        }
        case 2:
        {
            gpio_set(LED_WHITE, 0);
            SobelResult=SobelTest();
            if(SobelResult<ZebraTresholeL)
            {
                gpio_set(LED_WHITE, 1);
                return 1;//�ٴ�sobelһ��ȷ��״̬��û����
            }
            else
            {
                StatusChange=1;//����ص�״̬1�����ж�
                break;
            }
        }
    }
    return 0;
}
/********************************************************************************************
 ** ��������: �ұ߳�������״̬��ת��
 ** ��    ��: Point InflectionL�����¹յ�
 **           Point InflectionR: ���¹յ�
 **           uint8* GarageLFlag����״̬���ڵ�ʶ����������س�ȥ
 ** �� �� ֵ: 0��Ԫ��״̬û����
 **           1��Ԫ��״̬����
 ** ��    ��: LJF
 *********************************************************************************************/
uint8 GarageRIStatusIdentify(Point InflectionL,Point InflectionR,uint8* GarageLFlag)
{
    static uint8 StatusChange;//��һ��ʶ��Ľ����״̬����
    uint8 NowFlag=0;//��ε�ʶ����
    int64 SobelResult=0;//����������Ľ��
    switch(StatusChange)
    {
        //��һ��״̬Sobel��⣬ͨ�����ٿ���ʶ����
        case 0:
        {
//            gpio_set(LED_BLUE, 0);
            if(LostNum_RightLine>25)
            {
                SobelResult=SobelTest();
            }
            if(SobelResult>ZebraTresholeR)
            {
                StatusChange=1;
                break;
            }
            break;
        }
        case 1:
        {
//            gpio_set(LED_GREEN, 0);
            //���������ǻ����ж��Ƿ����ɹ�
            StartIntegralAngle_Z(IN_R_GARAGE_ANGLE);
            StatusChange=2;
            break;
        }
        case 2:
        {
//            gpio_set(LED_WHITE, 0);
#if 0   //0:ֱ�Ӵ������ 1:���ߴ������
            NowFlag=GarageRIdentify('Y', InflectionL, InflectionR);
            *GarageLFlag=NowFlag;//��ʶ��������ȥ���������滹�費��Ҫ����Ѳ�����ƫ��
#else
            Bias=-5;//����
            *GarageLFlag=1;
#endif
            //����Ƿ����ɹ������ɹ�ͣ��
            if(icm_angle_z_flag==1)
            {
                return 1;
            }
            break;
        }
    }
    return 0;
}
/********************************************************************************************
 ** ��������: ��ɳ��⣨������
 ** ��    ��: ��
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 *********************************************************************************************/
void OutGarage(void)
{
//    systick_delay_ms(STM0,50);
    //������Ҵ���������һ������ʱʵ�ֳ���
    Bias=-10;
    systick_delay_ms(STM0,300);
}
