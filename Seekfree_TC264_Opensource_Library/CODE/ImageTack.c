/*
 * ImageTack.c
 *  Created on: 2021��12��22��
 *  Author: 30516
 *  Effect: ���ڴ�Ŵ�ͼ������ȡѭ��Ԫ�صĺ���
 */

#include "ImageTack.h"
#include "SEEKFREE_18TFT.h"

//��������
float Bias=0;       //ƫ��

/*
 *******************************************************************************************
 ** ��������: ������������������ɢ�ĵ����ָ����һ�εĻع鷽�̣����������Իع鷽����ƫ��
 ** ��    ��: int starline:    ��ɢ�����ʼ��
 **           int endline:     ��ɢ��Ľ�����
 **           int *CentreLine����������
 ** �� �� ֵ: ��ϳ����Ļع鷽�̵�б�ʵĵ���
 ** ��    ��: LJF
 ** ע    �⣺ - ��Ϊƫ����int���͵�����return��ʱ�������ƫ��С��45��Ҳ����б��С��1��ʱ����ܻ���Ϊ����ȷʵ�����0
 **           - ͼ��ԭ�������Ϸ����������ǽ��й�ʽ�����ʱ���ͼ��ģ�⵽�������ޣ����Խ���Y�������ʱ��Ӹ�����
 **           - startline>endline
 **           - ��Ҫ���ֹյ�ʱ�������ģ�����Ǹ���
 ********************************************************************************************
 */
float Regression_Slope(int startline,int endline,int *CentreLine)
{
    //Y=BX+A
    int i=0,SumX=0,SumY=0,SumLines=0;
    float SumUp=0,SumDown=0,avrX=0,avrY=0,Bias=0;
    SumLines=startline-endline;   // startline Ϊ��ʼ�У� //endline ������ //SumLines

    for(i=startline;i>endline;i--)
    {
        SumX += i;
        SumY += CentreLine[i];
//        SumY-=i;                      //Y�����������
//        SumX+=CentreLine[i];         //X�����������
    }
    avrX=(float)(SumX/SumLines);     //X��ƽ��ֵ
    avrY=(float)(SumY/SumLines);     //Y��ƽ��ֵ

    for(i=startline;i>endline;i--)
    {
        SumUp+=(CentreLine[i]-avrY)*(i-avrY);//����
        SumDown+=(i-avrX)*(i-avrX);//��ĸ
//        SumUp+=(CentreLine[i]-avrX)*(-i-avrY);//����
//        SumDown+=(CentreLine[i]-avrX)*(CentreLine[i]-avrX);//��ĸ
    }
    if(SumDown==0)
        Bias=0;
    else
        Bias=SumUp/SumDown;
//    if(SumUp==0)//����Ϊ0ʱ��ֱ����x��ƽ�У����Դ�ʱBias�ķ�ĸΪ0��Ҫ������
//        Bias=57.3;//tan89��Ϊ57.2899
//    else
//        //B=(int)(SumUp/SumDown);б��
//        Bias=SumDown/SumUp;//����Ҫ������Y��ļн�������б�ʵĵ�������������
//    //A=(SumY-B*SumX)/SumLines;  //�ؾ�
    return Bias;
}


/********************************************************************************************
 ** ��������: ����������в���(ֱ��)
 ** ��    ��: int *LeftLine: ���ߣ�int *CentreLine: ���ߣ�int *RightLine: ���� ����Ϊȫ�ֱ����ˣ�
 **           char Choose: ѡ�����߻�������
 **           Point StarPoint:    ���
 **           Point EndPoint:     �յ�
 ** �� �� ֵ: ��
 ** ��    ��: LJF
 ** ע    �⣺- StarPoint.Y>EndPoint.Y
 **           - ��ͼ��ӳ�䵽�������޽���y=kx+b�Ĳ�����y��ȫȡ������֮����ڵ�ʱ���ٸ�����
 **           - 2022/2/27 17:40 DeBuglog: KӦ��Ϊ�����ͣ����򾫶���ʧΪ0
 *********************************************************************************************/
void FillingLine(char Choose, Point StarPoint,Point EndPoint)
{
    float K;//б��Ϊ�����ͣ�����K<1ʱ��K=0
    int B,Y,X;

    K=(float)(-EndPoint.Y+StarPoint.Y)/(EndPoint.X-StarPoint.X);//k=(y2-y1)/(x2-x1)��ǿ������ת���������ʧ������ȻΪ0
    B=-StarPoint.Y-K*StarPoint.X;//b=y-kx

    for(Y=StarPoint.Y;Y>EndPoint.Y;Y--)
    {
        X=(int)((-Y-B)/K);          //ǿ������ת����ָ��������ʱ��ֻ��������

        //�ж�X�᲻��Խ��
        if(X<0)                X=0;
        else if(X>MT9V03X_W-1) X=MT9V03X_W-1;

        switch(Choose)
        {
            case 'L':
                LeftLine[Y]=X;
                CentreLine[Y]=(X+RightLine[Y])/2;
                break;
            case 'R':
                RightLine[Y]=X;
                CentreLine[Y]=(LeftLine[Y]+X)/2;//������������ߵ��޸ģ���Ϊ������ֲ����ߵ����������ʮ�־�ֱ�ӳ壬б��Ͳ�һ�߶���
                break;
            default:break;
        }
        //BinaryImage[Y][X]=IMAGE_BLACK;//BinaryImage[H][W],x=(y-b)/k//�Զ�ֵ��ͼ����в���
    }
}

/*
 *******************************************************************************************
 ** ��������: ������������������ɢ�ĵ����������ߵ�ƫ��Bias
 ** ��    ��: int starline:    ��ɢ�����ʼ��
 **           int endline:     ��ɢ��Ľ�����
 **           int *CentreLine�� ��������
 ** �� �� ֵ: ƫ��Bias
 ** ��    ��: WBN
 ********************************************************************************************
 */
float DifferentBias(int startline,int endline,int *CentreLine)
{
    static float last_bias;
    float bias=0;
    int rownum=0;//���ڼ������˶����е�ƫ��

    for(int i=startline;i>endline;i--)
    {
        if(BinaryImage[i][CentreLine[i]]==IMAGE_WHITE)
        {
            bias+=(float)(MT9V03X_W/2-CentreLine[i]);  //�ۻ�ƫ��
            rownum++;
        }
    }

    bias=bias/rownum/10;   //��ƫ���ֵ
    if(bias<0.5&&bias>-0.5) //�ֶμ�Ȩ
    {
        bias=bias*0.1;
    }

    if(bias==bias)  //bias����ֵ
    {
        last_bias=bias;
        return bias;
    }
    else
    {
        return last_bias;   //������󣬺��Դ˴μ��㣬������һ�ε�ֵ
    }
}
