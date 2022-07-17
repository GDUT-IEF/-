/*
 * Garage.c
 * Created on: 2022��5��25��
 * Author: 30516
 * Effect: ��ų�����ص�Դ����
 * *�ұ߲���⣺200 ��⣺170
 * ��߲���⣺230���������ֱ���䷢����250���ӹ���ֱ���䷢�� ��⣺180���Ǹо�ͣ������
 */

#include "ImageSpecial.h"
#include "PID.h"
#include "ICM20602.h"           //�����ǻ�����ɱ�־�����Լ��������ֺ���

#include "LED.h" //debug
#include "SEEKFREE_18TFT.h"
#include "zf_gpio.h"

//Sobel���Ӽ��
#define FastABS(x) (x > 0 ? x : x * -1.0f)
#define BinaryImage(i, j)    BinaryImage[i][j]
#define ZebraTresholeL 1200  //���������Ե���ֵ
#define ZebraTresholeR 1300  //���������Գ������ұߵ���ֵ

#define GARAGE_IDENTIFY_MODE 0    //����ģʽ���Ϲյ�
#define IN_L_GARAGE_ANGLE   50  //����⿪�������ǻ��ֵ�Ŀ��Ƕ�
#define IN_R_GARAGE_ANGLE   60  //���ҿ⿪�������ǻ��ֵ�Ŀ��Ƕ�
#define L_GARAGE_LOSTLLINE_MIN_THR 25   //��߳��⿪������������߶�����С��ֵ
#define L_GARAGE_LOSTLLINE_MAX_THR 35   //��߳��⿪������������߶��������ֵ
#define L_GARAGE_LOSTRLINE_MAX_THR 50   //��߳��⿪�����������ұ߶��������ֵ
#define R_GARAGE_LOSTRLINE_THR 35   //�ұ߳��⿪�����������ұ߶�����ֵ

#define GARAGE_DEBUG    0       //�Ƿ���Ҫ���������DEBUG
#define GARAGE_LED_DEBUG 1

/********************************************************************************************
 ** ��������: Sobel���Ӽ��������
 ** ��    ��: uint8 starline,uint8 endline,uint8 starcloumn,uint8 endcloumn
 ** �� �� ֵ: Sobel��ֵ
 ** ��    ��: LJF
 ** ע    �⣺֮ǰ������MT9V03X_H-1-20~50
 **                ����40~MT9V03X_W-1-40
 *********************************************************************************************/
int64 SobelTest(uint8 starline,uint8 endline,uint8 starcloumn,uint8 endcloumn)
{
    int64 Sobel = 0;
    int64 temp = 0;

    for (uint8 i = starline; i > endline ; i--)
    {
        for (uint8 j = starcloumn; j < endcloumn; j++)
        {
            int64 Gx = 0, Gy = 0;
            Gx = -BinaryImage(i-1, j-1)+BinaryImage(i-1, j+1)
                 -2*BinaryImage(i, j-1)+2*BinaryImage(i, j+1)
                 -BinaryImage(i+1, j-1)+BinaryImage(i+1, j+1);
            Gy = BinaryImage(i-1, j-1)+2*BinaryImage(i-1, j)+BinaryImage(i-1, j+1)
                 -BinaryImage(i+1, j+1)-2*BinaryImage(i+1, j)-BinaryImage(i+1, j+1);
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
    float LastBias=Bias;//��¼��һ�ε�Bias

#if GARAGE_DEBUG
    lcd_showint32(TFT_X_MAX-50, 0, InflectionL.X, 3);
    lcd_showint32(TFT_X_MAX-50, 1, InflectionR.X, 3);
#endif

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
#if GARAGE_DEBUG
            lcd_drawpoint(InflectionL.X, row, YELLOW);
#endif
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
#if GARAGE_DEBUG
                for(int i=0;i<MT9V03X_W-1;i++)
                {
                    lcd_drawpoint(i, UpInflection.Y-8, PURPLE);
                    lcd_drawpoint(i, UpInflection.Y-5, PURPLE);
                    lcd_drawpoint(i, InflectionL.Y+5, YELLOW);
                    lcd_drawpoint(i, InflectionL.Y+3, YELLOW);
                }
#endif
//                Bias=DifferentBias(UpInflection.Y-5, UpInflection.Y-8, CentreLine);
                if(NoInflectionLFlag==0)//���û��ʧ�¹յ������¹յ�����Ѳ��
                {
                    Bias=DifferentBias(InflectionL.Y+5, InflectionL.Y+3, CentreLine);
                }
                else
                {
                    Bias=DifferentBias(UpInflection.Y-5, UpInflection.Y-8, CentreLine);//ֱ�����Ϲյ��������������ȥѭ��
                }
                if(Bias>1.5) Bias=LastBias;//���ƫ������̫���������һ�εĿ���
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
#if GARAGE_DEBUG
            lcd_showuint8(0, 0, LostNum_LeftLine);
            lcd_showuint8(0, 1, LostNum_RightLine);
#endif
            if(LostNum_LeftLine>L_GARAGE_LOSTLLINE_MIN_THR && LostNum_LeftLine<L_GARAGE_LOSTLLINE_MAX_THR && LostNum_RightLine<L_GARAGE_LOSTRLINE_MAX_THR)
            {
                SobelResult=SobelTest(80,50,40,MT9V03X_W-1-40);
#if GARAGE_DEBUG
                lcd_showint32(0, 2, SobelResult, 5);
#endif
            }
            if(SobelResult>ZebraTresholeL)
            {
                StatusChange=1;
                *GarageLFlag=1;//����������ѭ��
                Bias=DifferentBias(95, 80, CentreLine);//�ֶ�����ǰհ
                break;
            }
            break;
        }
        case 1:
        {
            switch(Choose)
            {
                case 'Y':
                {
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
            SobelResult=SobelTest(80,50,40,MT9V03X_W-1-40);
            if(SobelResult<ZebraTresholeL)
            {
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
    uint8 NoInflectionRFlag=0;//�����յ㲻���ڵı�־����������ѡȡ���ֲ��߷�ʽ

#if GARAGE_DEBUG
    lcd_showint32(TFT_X_MAX-50, 0, InflectionL.X, 3);
    lcd_showint32(TFT_X_MAX-50, 1, InflectionR.X, 3);
#endif

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
    if(InflectionR.X==0 || InflectionR.X<MT9V03X_W/2 || BinaryImage[InflectionR.Y+5][MT9V03X_W-5]==IMAGE_WHITE)
    {
        //�м��ұߵĵĵ�ȥ���Ϲյ�,�˴���������֣���������һ��Ҫ�ҵ��׵���Ϊ���Ϲյ������
        for(int row=MT9V03X_H/2+15;row<MT9V03X_H;row++)
        {
            if(BinaryImage[row][MT9V03X_W-3]==IMAGE_WHITE)
            {
                InflectionR.X=MT9V03X_W-3;InflectionR.Y=row+10;//+10����Ϊ�����ѭ����-10�ˣ���ֹ��Ϊ-10ֱ�ӵ���ɫ���������Ҳ����յ�
                NoInflectionRFlag=1;
                break;
            }
        }
        if(NoInflectionRFlag!=1) return 0;//����˵����߶��Ǻڵ���ֱ�ӷ����Ѿ������ҿ�
    }
    if(NoInflectionRFlag==1 && Choose=='Y')
    {
        for(int row=InflectionR.Y-10;row>10;row--)
        {
#if GARAGE_DEBUG
           lcd_drawpoint(InflectionL.X, row, YELLOW);
#endif
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
                if(NoInflectionRFlag==1 && LeftLine[MT9V03X_H-5]<MT9V03X_W/2)//�ҹյ㲻���ڲ����������в��ڰ�������
                {
                    LeftDownPoint.X=LeftLine[MT9V03X_H-5];LeftDownPoint.Y=MT9V03X_H-5;
                    InflectionR.X=MT9V03X_W-5,InflectionR.Y=MT9V03X_H-5;//���ұ߲���ֱ�ߵ����
                    FillingLine('L', LeftDownPoint, UpInflection);//���ҿⲹ����
                    FillingLine('R', InflectionR, UpInflection);//���ҿⲹ�ұߴ�ֱ���ߣ�����������ɨ�ߴ���Ӱ��
                }
                else if(NoInflectionRFlag==0 && LeftLine[MT9V03X_H-5]<MT9V03X_W/2)//�ҹյ���ڣ������������в��ڰ�������
                {
                    LeftDownPoint.X=LeftLine[InflectionR.Y],LeftDownPoint.Y=InflectionR.Y;
                    FillingLine('L', LeftDownPoint, UpInflection);//���ҿⲹ����
                    Unilaterally_Plan_CenterLine('M', 'L', LeftDownPoint.Y, UpInflection.Y);//���������Ӱ��
                }
                else//��������Ļ���½���Ϊ�������
                {
                    LeftDownPoint.X=2,LeftDownPoint.Y=MT9V03X_H-5;
                    FillingLine('L', LeftDownPoint, UpInflection);//���ҿⲹ����
                    FillingLine('R', InflectionR, UpInflection);//���ҿⲹ�ұߴ�ֱ���ߣ�����������ɨ�ߴ���Ӱ��
                }

                //��ƫ������
                Bias=DifferentBias_Garage(LeftDownPoint.Y, UpInflection.Y, CentreLine);
                return 1;
            }
            //�������
            case 'N':
            {
               if(NoInflectionRFlag==0)//���û��ʧ�¹յ������¹յ�����Ѳ��
               {
#if GARAGE_DEBUG
                    for(int i=0;i<MT9V03X_W-1;i++)
                    {
                        lcd_drawpoint(i, InflectionR.Y+5, PURPLE);
                        lcd_drawpoint(i, InflectionR.Y+2, PURPLE);
                    }
#endif
                   Bias=DifferentBias(InflectionR.Y+5, InflectionR.Y+2, CentreLine);
               }
               else
               {
#if GARAGE_DEBUG
                  for(int i=0;i<MT9V03X_W-1;i++)
                  {
                      lcd_drawpoint(i, UpInflection.Y-2, PURPLE);
                      lcd_drawpoint(i, UpInflection.Y-5, PURPLE);
                  }
#endif
                   Bias=DifferentBias(UpInflection.Y-2, UpInflection.Y-5, CentreLine);//ֱ�����Ϲյ��������������ȥѭ��
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
            if(LostNum_RightLine>R_GARAGE_LOSTRLINE_THR)
            {
                SobelResult=SobelTest(MT9V03X_H-1-20,50,40,MT9V03X_W-1-40);
            }
            if(SobelResult>ZebraTresholeR)
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
            SobelResult=SobelTest(MT9V03X_H-1-20,50,40,MT9V03X_W-1-40);
            if(SobelResult<ZebraTresholeR)
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
            if(LostNum_RightLine>R_GARAGE_LOSTRLINE_THR)
            {
                SobelResult=SobelTest(65,50,50,MT9V03X_W-1-50);
            }
            if(SobelResult>ZebraTresholeR)
            {
//                base_speed=100;//��ǰ���ٷ�ֹMCU��λ
                StatusChange=1;
                break;
            }
            break;
        }
        case 1:
        {
            //���������ǻ����ж��Ƿ����ɹ�
            StartIntegralAngle_Z(IN_R_GARAGE_ANGLE);
            StatusChange=2;
            break;
        }
        case 2:
        {
//            gpio_set(LED_WHITE, 0);
#if 1   //0:ֱ�Ӵ������ 1:���ߴ������
            NowFlag=GarageRIdentify('Y', InflectionL, InflectionR);
            *GarageLFlag=NowFlag;//��ʶ��������ȥ���������滹�費��Ҫ����Ѳ�����ƫ��
#else
            Bias=-5;//����
            *GarageLFlag=1;
#endif
            //����Ƿ����ɹ������ɹ�ͣ��
            if(icm_angle_z_flag==1)
            {
                Stop();
                return 1;
            }
//            //����ҹյ㲻�����ˣ�����ר��Ϊ���д�ĺ���
//            if(InflectionR.X==0 || InflectionR.X<MT9V03X_W/2 || BinaryImage[InflectionR.Y+5][MT9V03X_W-5]==IMAGE_WHITE)
//            {
//                StatusChange=4
//            }
//            break;
//        }
        }
//        case 3:
//        {
//            ;
//        }
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
