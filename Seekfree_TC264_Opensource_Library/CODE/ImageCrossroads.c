/*
 * Crossroads.c
 * Created on: 2022��5��25��
 * Author: 30516
 * Effect: ���ʮ��·����ص�Դ����
 */

#include "ImageSpecial.h"
#include "ICM20602.h"           //�����ǻ�����ɱ�־�����Լ��������ֺ���
#include "LED.h"                //debug
#include "headfile.h"

/*********************************************************************************
 ** ��������: ���������¹յ���Ѱ��ʮ��·�ڵ������Ϲյ�
 ** ��    ��: �����������Ѿ���ȫ�ֱ���
 **           Point InflectionL: ��߹յ�
 **           Point InflectionR: �ұ߹յ�
 **           Point *UpInflectionC: ����Ϲյ�
 **           Point *UpInflectionC: �ұ��Ϲյ�
 ** �� �� ֵ: ��
 ** ˵    ��: �˺�������������ʮ��ʱ��һ����������������ʶ����
 ** ��    ��: LJF
 **********************************************************************************/
void GetCrossRoadsUpInflection(Point DownInflectionL,Point DownInflectionR,Point *UpInflectionL,Point *UpInflectionR)
{
    int row=0,cloum=0;//��ʼ��,��
    UpInflectionL->X=0;UpInflectionL->Y=0;//���Ϲյ�����
    UpInflectionR->X=0;UpInflectionR->Y=0;//���Ϲյ�����

    //�����¹յ������ң��ҵ�������
    for(row=DownInflectionL.Y-15;row>1;row--)
    {
        /******************debug:�Ѵ������ϵ��ҵ�켣������******************/
//        lcd_drawpoint(DownInflectionL.X-3, row, YELLOW);
        /******************************************************************/
        //�������������Ұ׵�ʱ���������������Һ�ɫ����ƫһ��,��ֹ��Ϊ�ҹյ㺯���ҵĹյ�ܹ�ʹ�ò��߲���
        if(BinaryImage[row][DownInflectionL.X-3]==IMAGE_WHITE && BinaryImage[row-1][DownInflectionL.X-3]==IMAGE_BLACK)  //�׵�->�ڵ�
        {
            //�Ӱ׺�������ϼ������Һ����ף��õ��ܺõĹյ�ȥ����
            //�����ұ�ɨ��ʮ�У�������΢��һ��б�������ҹյ��ִ���
            for(cloum=DownInflectionL.X;cloum<MT9V03X_W/2+30;cloum++)
            {
                /******************debug:�Ѵ������ҵ��ҵ�켣������******************/
//                lcd_drawpoint(cloum, row, YELLOW);
                /******************************************************************/
                if(BinaryImage[row-3][cloum]==IMAGE_BLACK && BinaryImage[row-3][cloum+1]==IMAGE_WHITE)  //�ڵ�->�׵�
                {
                    //��¼�Ϲյ�
                    UpInflectionL->Y=row-1;UpInflectionL->X=cloum;
                    break;
                }
            }
            break;//��¼��֮����˳�ѭ��
        }
    }

    for(row=DownInflectionR.Y-15;row>1;row--)
    {
        /******************debug:�Ѵ������ϵ��ҵ�켣������******************/
//        lcd_drawpoint(DownInflectionR.X+3, row, YELLOW);
        /******************************************************************/
        //�������������Ұ׵�ʱ���������������Һ�ɫ����ƫһ��,��ֹ��Ϊ�ҹյ㺯���ҵĹյ�ܹ�ʹ�ò��߲���
        if(BinaryImage[row][DownInflectionR.X+3]==IMAGE_WHITE && BinaryImage[row-1][DownInflectionR.X+3]==IMAGE_BLACK)  //�ɰ׵�������
        {
            //�Ӱ׺�������ϼ������Һ����ף��õ��ܺõĹյ�ȥ����
            //�������ɨ��ʮ�У�������΢��һ��б�������ҹյ��ִ���
            for(cloum=DownInflectionR.X;cloum>MT9V03X_W/2-30;cloum--)
            {
                /******************debug:�Ѵ������ҵ��ҵ�켣������******************/
//                lcd_drawpoint(cloum, row, YELLOW);
                /******************************************************************/
                if(BinaryImage[row-3][cloum]==IMAGE_BLACK && BinaryImage[row-3][cloum-1]==IMAGE_WHITE)  //�ڵ�->�׵�
                {
                    //��¼�Ϲյ�
                    UpInflectionR->Y=row-1;UpInflectionR->X=cloum;
                    break;
                }
            }
            break;//��¼��֮����˳�ѭ��
        }
    }
}
/********************************************************************************************
 ** ��������: ʶ��ʮ��·��
 ** ��    ��: �������飺int *LeftLine �������飺int *RightLine//ȫ�ֱ���
 **           ���¹յ㣺Point DownInflectionL
 **           ���¹յ㣺Point DownInflectionR
 ** �� �� ֵ: 0������ʮ��·��
 **           1������ʮ��
 **           2����б��ʮ��
 **           3����б��ʮ��
 ** ��    ��: LJF
 ** ע    �⣺��
 *********************************************************************************************/
uint8 CrossRoadsIdentify(Point DownInflectionL,Point DownInflectionR)
{
    int row=0,cloum=0;//��ʼ��
    Point UpInflectionL,UpInflectionR;//�����Ϲյ�
    UpInflectionL.X=0;UpInflectionL.Y=0;//���Ϲյ�����
    UpInflectionR.X=0;UpInflectionR.Y=0;//���Ϲյ�����

    /**********************debug************************/
//    lcd_showint32(0, 0, LostNum_LeftLine, 3);
//    lcd_showint32(0, 1, LostNum_RightLine, 3);
//    lcd_showint32(TFT_X_MAX-50, 0, DownInflectionL.X, 3);
//    lcd_showint32(TFT_X_MAX-50, 1, DownInflectionR.X, 3);
    /***************************************************/

    //�������ߴ������ߣ����������¹յ㶼����,���������ǰ׵�
    if(LostNum_LeftLine>30 && LostNum_RightLine>30 && DownInflectionR.X!=0 && DownInflectionL.X!=0 && BinaryImage[50][MT9V03X_W/2]==IMAGE_WHITE)
    {
        GetCrossRoadsUpInflection(DownInflectionL, DownInflectionR, &UpInflectionL, &UpInflectionR);
        if(UpInflectionL.Y!=0 && UpInflectionR.Y!=0)
        {
            FillingLine('L', DownInflectionL, UpInflectionL);
            FillingLine('R', DownInflectionR, UpInflectionR);
            return 1;//����ʮ��
        }
    }
    //������һ�ж��ߣ��������ҹյ��Ҳ�������ʮ���е�״̬
    else if(LeftLine[MT9V03X_H-10]==0 && RightLine[MT9V03X_H-10]==MT9V03X_W-1 && DownInflectionL.X==0 && DownInflectionR.X==0)
    {
        Point PointL,PointR;//��ʱ�������¹յ�
        PointL.X=10;PointL.Y=MT9V03X_H;//����һ�����½ǵĵ�
        PointR.X=MT9V03X_W-10;PointR.Y=MT9V03X_H;//����һ�����½ǵĵ�
        //��ʧ�����¹յ��ʱ����ݱ���ȥ���Ϲյ�
        GetCrossRoadsUpInflection(PointL, PointR, &UpInflectionL, &UpInflectionR);
        if(UpInflectionL.Y!=0 && UpInflectionR.Y!=0)
        {
            PointL.X=LeftLine[UpInflectionL.Y-7];PointL.Y=UpInflectionL.Y-7;//Ѱ����ȷ�����ϸ����Ϲյ�һ��ĵ�������
            PointR.X=RightLine[UpInflectionR.Y-7];PointR.Y=UpInflectionR.Y-7;//Ѱ����ȷ�����ϸ����Ϲյ�һ��ĵ�������
            FillinLine_V2('L', MT9V03X_H, UpInflectionL.Y, UpInflectionL, PointL);
            FillinLine_V2('R', MT9V03X_H, UpInflectionR.Y, UpInflectionR, PointR);
            return 1;//����ʮ��
        }
    }
    //��߶��߳���һ��[60,����]���ұ�Ҳ���ڶ���[10,60]���ҹյ���ڣ������ҹյ�����һ�ζ�Ӧ����߶��ߣ������ҹյ㲻��������߸���
    else if(LostNum_LeftLine>60 && LostNum_RightLine>10 && LostNum_RightLine<60 && DownInflectionR.X!=0 && LeftLine[DownInflectionR.Y-5]==0)
    {
        //ֱ�����¹յ����ϳ��ҵ���ɫ��Ե
        for(row=DownInflectionR.Y-5;row>1;row--)
        {
            /******************debug:�Ѵ������ϵ��ҵ�켣������******************/
//            lcd_drawpoint(DownInflectionR.X, row, YELLOW);
            /******************************************************************/
            if(BinaryImage[row][DownInflectionR.X]==IMAGE_WHITE && BinaryImage[row-1][DownInflectionR.X]==IMAGE_BLACK)  //�ɰ׵�������
            {
                //ע��˴�����˵������һ���ͣ�����ˣ���Ϊб���ʱ���Ϲյ㱾���ͱȽ��м�
                for(cloum=DownInflectionR.X;cloum>30;cloum--)
                {
                    /******************debug:�Ѵ���������ҵ�켣������******************/
//                    lcd_drawpoint(cloum, row, YELLOW);
                    /******************************************************************/
                    if(BinaryImage[row-3][cloum]==IMAGE_BLACK && BinaryImage[row-3][cloum-1]==IMAGE_WHITE)  //�ڵ�->�׵�
                    {
                        //��¼�Ϲյ�
                        UpInflectionR.Y=row-1;UpInflectionR.X=cloum;
                        FillingLine('R', DownInflectionR, UpInflectionR);
                        return 2;//����б��ʮ��
                    }
                }
            }
        }
    }
    //�ұ߶��߳���һ�룬���Ҳ���ڶ��ߣ���յ���ڣ������ҹյ�����һ�ζ�Ӧ����߶���
    else if(LostNum_RightLine>60 && LostNum_LeftLine>10 && LostNum_LeftLine<60 && DownInflectionL.X!=0 && RightLine[DownInflectionL.Y-5]==MT9V03X_W-1)
    {
        for(row=DownInflectionL.Y-5;row>1;row--)//��ʼ�о���֮ǰ�Ļ����ϼ���5���Ͼ��յ��ҵ�̫�ϸ��������Ҳ��������ϸ����������յ����һ�о��ǰ�����
        {
            /******************debug:�Ѵ���������ҵ�켣������******************/
//            lcd_drawpoint(DownInflectionL.X, row, YELLOW);
            /******************************************************************/
            if(BinaryImage[row][DownInflectionL.X]==IMAGE_WHITE && BinaryImage[row-1][DownInflectionL.X]==IMAGE_BLACK)  //�ɰ׵�������
            {
                //�Ӱ׺�������ϼ������Һ����ף��õ��ܺõĹյ�ȥ����
                //ע��˴�����˵������һ���ͣ�����ˣ���Ϊб���ʱ���Ϲյ㱾���ͱȽ��м�
                for(cloum=DownInflectionL.X;cloum<MT9V03X_W-30;cloum++)
                {
                    /******************debug:�Ѵ���������ҵ�켣������******************/
//                    lcd_drawpoint(cloum, row, YELLOW);
                    /******************************************************************/
                    if(BinaryImage[row-3][cloum]==IMAGE_BLACK && BinaryImage[row-3][cloum+1]==IMAGE_WHITE)  //�ڵ�->�׵�
                    {
                        //��¼�Ϲյ�
                        UpInflectionL.Y=row-1;UpInflectionL.X=cloum;
                        FillingLine('L', DownInflectionL, UpInflectionL);
                        return 3;//����б��ʮ��
                    }
                }
            }
        }
    }
    return 0;
}
/********************************************************************************************
 ** ��������: ʮ�ֵ�״̬��ת��
 ** ��    ��: Point InflectionL�����¹յ�
 **           Point InflectionR�����¹յ�
 ** �� �� ֵ: 0��ʮ�ֻ�δ����
 **           1��ʮ���ѽ���
 ** ��    ��: LJF
 *********************************************************************************************/
uint8 CrossRoadsStatusIdentify(Point DownInflectionL,Point DownInflectionR)
{
    //ʮ��״̬������������״̬�Ƿ���ת
    static uint8 StatusChange;
    uint8 NowFlag=0;//ʮ��ʶ����ݴ��־����

    NowFlag=CrossRoadsIdentify(DownInflectionL, DownInflectionR);
    switch(StatusChange)
    {
        //����ICMΪ���ж��Ƿ����ʮ��Ԫ��
        case 0:
        {
//            gpio_set(LED_BLUE, 0);
            //���������״̬
            if(NowFlag==1)
            {
//                gpio_set(LED_BLUE, 1);
                StartIntegralAngle_Z(270);//������������Ϊ��״̬��־
                StatusChange=1;//���������ǻ��ֳ���״̬
            }
            break;
        }
        //����״̬
        case 1:
        {
//            gpio_set(LED_GREEN, 0);
            //�����ǻ��ִﵽ��ʮ�ֻ�״̬,֤���Ѿ�ֱ�Ž���ʮ���м䣬���Բ����ٲ���Ҳ�ܳ�ȥ��
            if(icm_angle_z_flag==1)
            {
//                gpio_set(LED_GREEN, 1);
                return 1;
            }
            break;
        }
    }
    return 0;
}
