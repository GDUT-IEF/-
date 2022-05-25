/*
 * Crossroads.c
 * Created on: 2022��5��25��
 * Author: 30516
 * Effect: ���ʮ��·����ص�Դ����
 */

#include "ImageSpecial.h"
#include "ICM20602.h"           //�����ǻ�����ɱ�־�����Լ��������ֺ���

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
    for(row=DownInflectionL.Y-5;row>1;row--)
    {
        if(BinaryImage[row][DownInflectionL.X]==IMAGE_WHITE && BinaryImage[row-1][DownInflectionL.X]==IMAGE_BLACK)  //�׵�->�ڵ�
        {
            //�Ӱ׺�������ϼ������Һ����ף��õ��ܺõĹյ�ȥ����
            for(cloum=DownInflectionL.X;cloum<MT9V03X_W/2;cloum++)
            {
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

    for(row=DownInflectionR.Y-5;row>1;row--)
    {
        if(BinaryImage[row][DownInflectionR.X]==IMAGE_WHITE && BinaryImage[row-1][DownInflectionR.X]==IMAGE_BLACK)  //�ɰ׵�������
        {
            //�Ӱ׺�������ϼ������Һ����ף��õ��ܺõĹյ�ȥ����
            for(cloum=DownInflectionR.X;cloum>MT9V03X_W/2;cloum--)
            {
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
        else
            return 0;
    }
    //��߶��߳���һ�룬�ұ�Ҳ���ڶ��ߣ��ҹյ���ڣ������ҹյ�����һ�ζ�Ӧ����߶��ߣ������ҹյ㲻��������߸���
    else if(LostNum_LeftLine>70 && LostNum_RightLine>15 && DownInflectionR.X!=0 && LeftLine[DownInflectionR.Y-5]==0)
    {
        //ֱ�����¹յ����ϳ��ҵ���ɫ��Ե
        for(row=DownInflectionR.Y;row>1;row--)
        {
            if(BinaryImage[row][DownInflectionR.X]==IMAGE_WHITE && BinaryImage[row-1][DownInflectionR.X]==IMAGE_BLACK)  //�ɰ׵�������
            {
                //ע��˴�����˵������һ���ͣ�����ˣ���Ϊб���ʱ���Ϲյ㱾���ͱȽ��м�
                for(cloum=DownInflectionR.X;cloum>30;cloum--)
                {
                    if(BinaryImage[row-3][cloum]==IMAGE_BLACK && BinaryImage[row-3][cloum-1]==IMAGE_WHITE)  //�ڵ�->�׵�
                    {
                        //��¼�Ϲյ�
                        UpInflectionR.Y=row-1;UpInflectionR.X=cloum;
                        FillingLine('R', DownInflectionR, UpInflectionR);
                        return 2;//����б��ʮ��
                    }
                    else return 0;
                }
            }
            else return 0;
        }
    }
    //�ұ߶��߳���һ�룬���Ҳ���ڶ��ߣ���յ���ڣ������ҹյ�����һ�ζ�Ӧ����߶���
    else if(LostNum_RightLine>70 && LostNum_LeftLine>15 && DownInflectionL.X!=0 && RightLine[DownInflectionL.Y-5]==MT9V03X_W-1)
    {
        for(row=DownInflectionL.Y;row>1;row--)
        {
            if(BinaryImage[row][UpInflectionL.X]==IMAGE_WHITE && BinaryImage[row-1][UpInflectionL.X]==IMAGE_BLACK)  //�ɰ׵�������
            {
                //�Ӱ׺�������ϼ������Һ����ף��õ��ܺõĹյ�ȥ����
                //ע��˴�����˵������һ���ͣ�����ˣ���Ϊб���ʱ���Ϲյ㱾���ͱȽ��м�
                for(cloum=DownInflectionL.X;cloum<MT9V03X_W-30;cloum++)
                {
                    if(BinaryImage[row-3][cloum]==IMAGE_BLACK && BinaryImage[row-3][cloum+1]==IMAGE_WHITE)  //�ڵ�->�׵�
                    {
                        //��¼�Ϲյ�
                        UpInflectionL.Y=row-1;UpInflectionL.X=cloum;
                        FillingLine('L', DownInflectionL, UpInflectionL);
                        return 3;//����б��ʮ��
                    }
                    else return 0;
                }
            }
            else return 0;
        }
    }
    else return 0;
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

    switch(StatusChange)
    {
        //���״̬
        case 0:
        {
            NowFlag=CrossRoadsIdentify(DownInflectionL, DownInflectionR);
            if(NowFlag!=0)
            {
                StatusChange=1;//��ת����һ��״̬ʮ����
            }
            break;
        }
        //��;״̬�����������ǻ��֣���������
        case 1:
        {
            NowFlag=CrossRoadsIdentify(DownInflectionL, DownInflectionR);
            if(NowFlag==0)
            {
                StartIntegralAngle_Z(250);//����������,���ܹ���ȫת��270������һ��ƫ��ͽ���ʶ�����״̬
                StatusChange=2;
            }
            break;
        }
        //����״̬
        case 2:
        {
            if(icm_angle_z_flag==1)//�����ǻ��ִﵽ��ʮ�ֻ�״̬�����¸�״̬
            {
                StatusChange=3;
            }
            break;
        }
        //ȷ���Ѿ���ȫ����ʮ��
        case 3:
        {
            NowFlag=CrossRoadsIdentify(DownInflectionL, DownInflectionR);
            if(NowFlag==0)
            {
                return 1;
            }
            break;
        }
        default:break;
    }
    return 0;
}
