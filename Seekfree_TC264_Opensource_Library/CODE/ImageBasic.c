#include "ImageBasic.h"

//��������
int mid=MT9V03X_W/2;    //��ʼ��ɨ�ߵ��е�Ϊͼ���е�

/*
 ** ��������: ɨ����ȡ����������
 ** ��    ��: uint8 *LeftLine��    ��������
 **           uint8* CentreLine����������
 **           uint8 *RightLine��  ��������
 ** �� �� ֵ: ��
 ** ��    ��: LJF
 */

void GetImagBasic(int *LeftLine, int *CentreLine, int *RightLine)
{
    int row,cloum;   //��,��
    //��ʼɨ�ߣ��������ϣ����м������ߣ�
    for(row=0;row<MT9V03X_H;row++) //ͼ���ԭ�������½�
    {
        //���ɨ��
        for(cloum=mid;(cloum-BORDER_BIAS)>0;cloum--)
        {
            if(BinaryImage[row][cloum]==0&&BinaryImage[row][cloum-BORDER_BIAS]==0)  //�ж���߽�㣨BORDER_BIAS��żȻ���أ�
            {
                LeftLine[row]=cloum;    //��¼��߽��
                break;
            }
        }
        //�ұ�ɨ��
        for(cloum=mid;(cloum+BORDER_BIAS)<MT9V03X_W;cloum++)
        {
            if(BinaryImage[row][cloum]==0&&BinaryImage[row][cloum+BORDER_BIAS]==0)  //�ж��ұ߽�㣨BORDER_BIAS��żȻ���أ�
            {
                RightLine[row]=cloum;   //��¼�ұ߽��
                break;
            }
        }
        //���ݴ���
        CentreLine[row]=(LeftLine[row]+RightLine[row])/2;   //��¼����
        mid=CentreLine[row];      //����һ�ε�����ֵΪ��һ��ɨ�ߵ��м��
    }
}

