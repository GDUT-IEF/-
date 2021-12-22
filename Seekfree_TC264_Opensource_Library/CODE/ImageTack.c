/*
 * ImageTack.c
 *  Created on: 2021��12��22��
 *  Author: 30516
 *  Effect: ���ڴ�Ŵ�ͼ������ȡѭ��Ԫ�صĺ���
 */

/*
 *******************************************************************************************
 ** ��������: ������������������ɢ�ĵ����ָ����һ�εĻع鷽��
 ** ��    ��: int starline:    ��ɢ�����ʼ��
 **           int endline:     ��ɢ��Ľ�����
 **           int *CentreLine����������
 ** �� �� ֵ: ��ϳ����Ļع鷽�̵�б��
 ** ��    ��: LJF
 ** ע    �⣺����X��ƽ��ֵ��Yƽ��ֵ�Լ�б�ʵ�ʱ������漰���������͵����⣬��δ�õ��������֤C����������������û��
 ********************************************************************************************
 */
int Regression_Slope(int startline,int endline,int *CentreLine)
{
    //Y=BX+A
    int i=0,SumX=0,SumY=0,SumLines=0,B,A;//��ԭ���Ƿݴ�����B��A��float���ͣ������������ص�����int����������ĳ�int
    float SumUp=0,SumDown=0,avrX=0,avrY=0;
    SumLines=endline-startline;   // startline Ϊ��ʼ�У� //endline ������ //SumLines

    for(i=startline;i<endline;i++)
    {
        SumX+=i;
        SumY+=CentreLine[i];    //����Middle_blackΪ������ߵ�����
    }
    avrX=(float)(SumX/SumLines);     //X��ƽ��ֵ
    avrY=(float)(SumY/SumLines);     //Y��ƽ��ֵ
    SumUp=0;
    SumDown=0;
    for(i=startline;i<endline;i++)
    {
        SumUp+=(CentreLine[i]-avrY)*(i-avrX);
        SumDown+=(i-avrX)*(i-avrX);
    }
    if(SumDown==0)
        B=0;
    else
        B=(int)(SumUp/SumDown);
    A=(SumY-B*SumX)/SumLines;  //�ؾ�
    return B;  //����б��
}


