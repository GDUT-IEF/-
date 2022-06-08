/*
 * Garage.c
 * Created on: 2022��5��25��
 * Author: 30516
 * Effect: ��ų�����ص�Դ����
 */

#include "ImageSpecial.h"
#include "PID.h"
#include "LED.h" //debug
#include "headfile.h"
#include "ICM20602.h"           //�����ǻ�����ɱ�־�����Լ��������ֺ���

//���������㳬����ֵ�Ĵ���������ĳ��ֵ�Ͳ��ٽ���sobel����
uint8 SobelLCount=0;             //���������
uint8 SobelRCount=0;             //�ұ�������
#define GARAGEIDENTIFYMODE 0    //����ģʽ���Ϲյ�

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
 ** ��������: �󳵿��������������б���
 ** ��    ��: char Direction: ѡ������߳��⻹�Ǽ���ұ߳���L����R
 **           Point InflectionL�����¹յ�
 **           Point InflectionR�����¹յ�
 ** �� �� ֵ:  0��û��ʶ��������
 **           1: �жϳ���
 ** ��    ��: LJF
 *********************************************************************************************/
uint8 DealGarageLSpecial(void)
{
    int row=0,cloumn=0,flag=0,BiasRow=0;
    Point LeftDownPoint,LeftUpPoint;
    LeftDownPoint.X=5,LeftDownPoint.Y=MT9V03X_H-5,LeftUpPoint.X=0,LeftUpPoint.Y=0;
    //�����൱��������������ж����������������Ϊ�׵�
    if(BinaryImage[MT9V03X_H/2+10][5]==IMAGE_WHITE)
    {
        row=MT9V03X_H/2+10;cloumn=5;
        for(;row>0;row--)
        {
            //�����½ǰ׵������ҵ�������
            if(flag==0 && BinaryImage[row][cloumn]==IMAGE_WHITE && BinaryImage[row-1][cloumn]==IMAGE_BLACK)
            {
                BiasRow=row;//����ƫ�����
                flag++;
            }
            //�ټ����ҵ�������
            if(flag==1 && BinaryImage[row][cloumn]==IMAGE_BLACK && BinaryImage[row-1][cloumn]==IMAGE_WHITE)
            {
                LeftUpPoint.Y=row+5;
                break;
            }
        }
        if(row==0)//��������û�ҵ�˵������
            return 0;
        //���������ҵ�������
        for(;cloumn<MT9V03X_W-30;cloumn++)
        {
            if(BinaryImage[LeftUpPoint.Y][cloumn]==IMAGE_BLACK && BinaryImage[LeftUpPoint.Y][cloumn+1]==IMAGE_WHITE)
            {
                LeftUpPoint.X=cloumn;
                break;
            }
        }
        FillingLine('L', LeftDownPoint, LeftUpPoint);
        //�ò�����ȥ��б��ƫ��
        Bias=Regression_Slope(LeftDownPoint.Y, LeftUpPoint.Y, LeftLine);
        //��б���������ƫ����и�����
        if(Bias<=1)
            Bias=Bias*2.5;
        else
            Bias=Bias*2;
        return 1;
    }
    else
        return 0;
}
/********************************************************************************************
 ** ��������: ʶ��������
 ** ��    ��: char Direction: ѡ������߳��⻹�Ǽ���ұ߳���L����R
 **           Point InflectionL�����¹յ�
 **           Point InflectionR�����¹յ�
 ** �� �� ֵ:  0��û��ʶ��������
 **           1��ʶ���������ҳ����ڳ��Ҳ�
 **           2��ʶ���������ҳ����ڳ����
 ** ��    ��: LJF
 *********************************************************************************************/
uint8 GarageIdentify(char Direction,Point InflectionL,Point InflectionR)
{
    int64 SobelResult=0;//����������Ľ��
    Point UpInflection;//�Ϲյ�ı���
    UpInflection.X=0,UpInflection.Y=0;//��ʼ��Ϊ0
    uint8 NoInflectionLFlag=0;//�����յ㲻���ڵı�־���������ڸ����ߵ����¹յ㸳ֵ
    switch(Direction)
    {
        case 'L':
            //��߶���(40,95)���ұ߶���(0,20)�������Ǹ�ת��ĵط���ֻ��һС�Σ���������������
            SobelResult=SobelTest();//��������������
            if(SobelResult>ZebraTresholeL)
            {
                SobelLCount++;
                //����������������е�֡�ж�
                if(DealGarageLSpecial()==1)
                {
                    return 2;
                }

                //�����յ㲻������õ����Ǹ���˵���Ǹ����Ѿ�����ͼ����
                if(InflectionL.X==0)
                {
                    InflectionL.X=3;
                    InflectionL.Y=MT9V03X_H/2+10;
                    NoInflectionLFlag=1;

                }
                //���������ҵ��������ڣ�ȷ���Ϲյ������
                for(int row=InflectionL.Y;row-1>0;row--)
                {
                    if(BinaryImage[row][InflectionL.X]==IMAGE_WHITE && BinaryImage[row-1][InflectionL.X]==IMAGE_BLACK)
                    {
                        UpInflection.Y=row-5;//�õ��������֮����и��Ϲյ�
                        break;//����ѭ��
                    }
                }
                //�������ұ��ҵ��������ף�ȷ���Ϲյ������
                for(int column=InflectionL.X;column<MT9V03X_W;column++)
                {
                    if(BinaryImage[UpInflection.Y][column]==IMAGE_BLACK && BinaryImage[UpInflection.Y][column+1]==IMAGE_WHITE)
                    {
                        UpInflection.X=column+1;//�õ��������֮����и��Ϲյ�
                        break;//����ѭ��
                    }
                }
                if(NoInflectionLFlag==1)
                {
                    InflectionL.Y=MT9V03X_H;InflectionL.X=UpInflection.X-10;
                }
                FillingLine('L', InflectionL, UpInflection);
                Bias=Regression_Slope(InflectionL.Y, UpInflection.Y, LeftLine);
                //��б���������ƫ����и�����
                if(Bias<=1)
                    Bias=Bias*2.25;
                else
                    Bias=Bias*1.75;
                return 2;
            }
            break;
        case 'R':
            //�������Ϊ�˷�ֹ���������п��Լ����ұ߶��ߴ���40С��90��߶���С��10����
            //�ұ߶��ߡ���߲����ߡ������ҹյ��������������
            SobelResult=SobelTest();//��������������
            if(SobelResult>ZebraTresholeR)
            {
                MotorK_L.P=15;
                MotorK_L.I=1.5;
                MotorK_R.P=15;
                MotorK_R.I=1.5;
                /*����һ���ұߴ�����⣬д��whileѭ�����ٶ�ͣ���������*/
                base_speed=130;
                systick_delay_ms(STM0,15);//��10ms��ֹ�����⣬����ٶ�����ȥ����ȡ��
                Bias=-15;//�ұߴ���
                systick_delay_ms(STM0,150);
                while(1)
                {
//                    Bias=0;
                    base_speed=0;
                }
                return 1;
            }
            break;
        default:break;
    }
    return 0;
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

#if GARAGEIDENTIFYMODE
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
    if(InflectionL.X==0 || (InflectionL.Y-InflectionR.Y)<10 || InflectionL.X>MT9V03X_H/2 || BinaryImage[InflectionL.Y+5][5]!=IMAGE_BLACK)
    {
        /**********************debug************************/
//        lcd_showint32(0, 0, BinaryImage[MT9V03X_H/2+10][3], 3);
        /***************************************************/

        //�м���ߵĵĵ�ȥ���Ϲյ�
        if(BinaryImage[MT9V03X_H/2+10][3]==IMAGE_WHITE)
        {
            InflectionL.X=3;InflectionL.Y=MT9V03X_H/2+15;
            NoInflectionLFlag=1;
        }
        else return 0;//����˵����߶��Ǻڵ���ֱ�ӷ����Ѿ��������
    }
    //��ʼ����ȥѰ���Ϲյ�
    for(int row=InflectionL.Y-10;row>10;row--)
    {
        /******************debug:�Ѵ������ϵ��ҵ�켣������******************/
//        lcd_drawpoint(InflectionL.X, row, YELLOW);
        /******************************************************************/
        //�������ϰ׵���
        if(BinaryImage[row][InflectionL.X]==IMAGE_WHITE && BinaryImage[row-1][InflectionL.X]==IMAGE_BLACK)
        {
            //��������Ƿ�����������������ȥ�ж��Ƿ�ʧ�յ㣬��ʧ�˵Ļ�����Ҫ�����½Ǻ����Ͻǣ�Ϊ�˴�Ǵ��
            if(NoInflectionLFlag==1 && Choose=='Y')
            {
                UpInflection.X=InflectionL.X;
                UpInflection.Y=row-3;//�������if���ҵ�������Ҫ���и�ֵ���ܲ�����
            }
            else
            {
                for(int column=InflectionL.X;column<MT9V03X_W-10;column++)
                {
                    /******************debug:�Ѵ������ҵ��ҵ�켣������******************/
//                    lcd_drawpoint(column, row, YELLOW);
                    /******************************************************************/
                    //�������Һ�����
                    if(BinaryImage[row-3][column]==IMAGE_BLACK && BinaryImage[row-3][column+1]==IMAGE_WHITE)
                    {
                        UpInflection.X=column;UpInflection.Y=row-3;
                        break;
                    }
                }
            }
            break;//��ȥ֮����˳�ѭ��������û��Ҫ��ͼ�����������ʧ��Ҳ�˳�˵��ȷʵ��ʧ����
        }
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
                //�ж��Ƿ�ʧ�¹յ�
               if(NoInflectionLFlag==1)
               {
                   Point LeftPoint;
                   LeftPoint.X=LeftLine[UpInflection.Y-5];LeftPoint.Y=UpInflection.Y-5;
                   FillinLine_V2('L', MT9V03X_H, UpInflection.Y, UpInflection, LeftPoint);
                   //��ƫ������
                   Bias=Regression_Slope(MT9V03X_H, UpInflection.Y, LeftLine);
                   //��б���������ƫ����и�����
                   if(Bias<=1) Bias=Bias*2.25;
                   else Bias=Bias*1.75;
                   /******************debug*******************************************/
//                   gpio_toggle(LED_BLUE);
                   /******************************************************************/
                   return 1;
               }
               else
               {
                   FillingLine('L', InflectionL, UpInflection);
                   //��ƫ������
                   Bias=Regression_Slope(InflectionL.Y, UpInflection.Y, LeftLine);
                   //��б���������ƫ����и�����
                   if(Bias<=1) Bias=Bias*2.25;
                   else Bias=Bias*1.75;
                   /******************debug*******************************************/
//                  gpio_toggle(LED_GREEN);
                  /******************************************************************/
                   return 1;
               }
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
            SobelResult=SobelTest();
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
                    StartIntegralAngle_Z(80);
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
            NowFlag=GarageLIdentify('Y', InflectionL, InflectionR);
            *GarageLFlag=NowFlag;//��ʶ��������ȥ���������滹�費��Ҫ����Ѳ�����ƫ��
            //����Ƿ����ɹ������ɹ�ͣ��
            if(icm_angle_z_flag==1)
            {
                while(1);
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
    //�����ҹյ����е�ͼ�����ұ�Ҫ�Ǻڵ�
    if(InflectionR.X==0 || (InflectionL.Y-InflectionR.Y)<10 || InflectionR.X>MT9V03X_H/2 || BinaryImage[InflectionR.Y+5][MT9V03X_W]!=IMAGE_BLACK)
    {
        /**********************debug************************/
//        lcd_showint32(0, 0, BinaryImage[MT9V03X_H/2+10][3], 3);
        /***************************************************/

        //�м��ұߵĵĵ�ȥ���Ϲյ�
        if(BinaryImage[MT9V03X_H/2+10][MT9V03X_W-3]==IMAGE_WHITE)
        {
            InflectionR.X=MT9V03X_W-3;InflectionR.Y=MT9V03X_H/2+15;
            NoInflectionLFlag=1;
        }
        else return 0;//����˵����߶��Ǻڵ���ֱ�ӷ����Ѿ��������
    }
    //��ʼ����ȥѰ���Ϲյ�
    for(int row=InflectionR.Y-10;row>10;row--)
    {
        /******************debug:�Ѵ������ϵ��ҵ�켣������******************/
//        lcd_drawpoint(InflectionL.X, row, YELLOW);
        /******************************************************************/
        //�������ϰ׵���
        if(BinaryImage[row][InflectionR.X]==IMAGE_WHITE && BinaryImage[row-1][InflectionR.X]==IMAGE_BLACK)
        {
            //��������Ƿ�����������������ȥ�ж��Ƿ�ʧ�յ㣬��ʧ�˵Ļ�����Ҫ�����½Ǻ����Ͻǣ�Ϊ�˴�Ǵ��
            if(NoInflectionLFlag==1 && Choose=='Y')
            {
                UpInflection.X=InflectionR.X;
                UpInflection.Y=row-3;//�������if���ҵ�������Ҫ���и�ֵ���ܲ�����
            }
            else
            {
                for(int column=InflectionR.X;column>10;column--)
                {
                    /******************debug:�Ѵ������ҵ��ҵ�켣������******************/
//                    lcd_drawpoint(column, row, YELLOW);
                    /******************************************************************/
                    //�������Һ�����
                    if(BinaryImage[row-3][column]==IMAGE_BLACK && BinaryImage[row-3][column-1]==IMAGE_WHITE)
                    {
                        UpInflection.X=column;UpInflection.Y=row-3;
                        break;
                    }
                }
            }
            break;//��ȥ֮����˳�ѭ��������û��Ҫ��ͼ�����������ʧ��Ҳ�˳�˵��ȷʵ��ʧ����
        }
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
                LeftDownPoint.X=5;LeftDownPoint.Y=5;
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
                //�ж��Ƿ�ʧ�¹յ�
               if(NoInflectionLFlag==1)
               {
                   Point RightPoint;
                   RightPoint.X=RightLine[UpInflection.Y-5];RightPoint.Y=UpInflection.Y-5;
                   FillinLine_V2('R', MT9V03X_H, UpInflection.Y, UpInflection, RightPoint);
                   //��ƫ������
                   Bias=Regression_Slope(MT9V03X_H, UpInflection.Y, RightLine);
                   //��б���������ƫ����и�����
                   if(Bias<=1) Bias=Bias*2.25;
                   else Bias=Bias*1.75;
                   /******************debug*******************************************/
//                   gpio_toggle(LED_BLUE);
                   /******************************************************************/
                   return 1;
               }
               else
               {
                   FillingLine('R', InflectionL, UpInflection);
                   //��ƫ������
                   Bias=Regression_Slope(InflectionL.Y, UpInflection.Y, RightLine);
                   //��б���������ƫ����и�����
                   if(Bias<=1) Bias=Bias*2.25;
                   else Bias=Bias*1.75;
                   /******************debug*******************************************/
//                  gpio_toggle(LED_GREEN);
                  /******************************************************************/
                   return 1;
               }
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
            SobelResult=SobelTest();
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
            gpio_set(LED_BLUE, 0);
            SobelResult=SobelTest();
            if(SobelResult>ZebraTresholeL)
            {
                StatusChange=1;
                break;
            }
            break;
        }
        case 1:
        {
            gpio_set(LED_GREEN, 0);
            //���������ǻ����ж��Ƿ����ɹ�
            StartIntegralAngle_Z(80);
            StatusChange=2;
            break;
        }
        case 2:
        {
            gpio_set(LED_WHITE, 0);
            NowFlag=GarageRIdentify('Y', InflectionL, InflectionR);
            *GarageLFlag=NowFlag;//��ʶ��������ȥ���������滹�費��Ҫ����Ѳ�����ƫ��
            //����Ƿ����ɹ������ɹ�ͣ��
            if(icm_angle_z_flag==1)
            {
                while(1);
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
