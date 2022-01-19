#include "ImageBasic.h"

//��������
int Mid=MT9V03X_W/2;                        //��ʼ��ɨ�ߵ��е�Ϊͼ���е�
int Lost_CentreLine=0;                      //���߶�ʧ��������(ɨ�ߵ�������)
int LostNum_LeftLine=0,LostNum_RightLine=0; //��¼���ұ߽綪����

/*
 ** ��������: ɨ����ȡ���������ߵ�����(�����������е�λ�ô�����������,�����������е�λ�ô�����������)
 ** ��    ��: int *LeftLine��     ��������
 **           int *CentreLine�� ��������
 **           int *RightLine��   ��������
 ** �� �� ֵ: ��
 ** ��    ��: LJF
 */

void GetImagBasic(int *LeftLine, int *CentreLine, int *RightLine)
{
    int row,cloum;   //��,��
    //��ʼɨ��(��������,���м�������),Ϊ��ɨ�ߵ��Ͻ���,������BORDER_BIAS��������ɨ�߷�Χ��С��ͼ���С
    for(row=0;(row-BORDER_BIAS)<MT9V03X_H;row++) //ͼ���ԭ�������½�
    {
        //���ɨ��
        for(cloum=Mid;(cloum-BORDER_BIAS)>0;cloum--)
        {
            if(BinaryImage[row][cloum]==IMAGE_BLACK && BinaryImage[row][cloum-BORDER_BIAS]==IMAGE_BLACK)  //�ж���߽�㣨BORDER_BIAS��żȻ���أ�
            {
                LeftLine[row]=cloum;    //��¼��߽��
                break;
            }
        }
        //�ұ�ɨ��
        for(cloum=Mid;(cloum+BORDER_BIAS)<MT9V03X_W;cloum++)
        {
            if(BinaryImage[row][cloum]==IMAGE_BLACK && BinaryImage[row][cloum+BORDER_BIAS]==IMAGE_BLACK)  //�ж��ұ߽�㣨BORDER_BIAS��żȻ���أ�
            {
                RightLine[row]=cloum;   //��¼�ұ߽��
                break;
            }
        }
        //���ݴ���
        CentreLine[row]=(LeftLine[row]+RightLine[row])/2;   //��¼���ߵ�
        if(LeftLine[row]==0)     //��߽綪��
            LostNum_LeftLine++;  //������+1
        if(RightLine[row]==0)    //�ұ߽綪��
        {
            RightLine[row]=MT9V03X_W;//���ұ߶��ߵĵ�ֱ���������ұߵ�ֵ�����ҹյ��ʱ������
            LostNum_RightLine++; //�Ҷ�����+1
        }
        //��ֹɨ�ߵ�������
        if(BinaryImage[row][CentreLine[row]]==IMAGE_BLACK && BinaryImage[row+BORDER_BIAS][CentreLine[row]]==IMAGE_BLACK)    //row�е������Ǻڣ�ɨ����������
        {
            Lost_CentreLine=row;    //��¼���ߵ㶪ʧ��������
            if(row>20)              //��ǰ20�в�������
                break;              //���Ѿ���20�к���ɨ�赽��������,ֱ��break������ͼ��ɨ�ߴ���
        }

        /*
         ** �ο��Ĵ����ж�����ר�ŵ����鵥����¼ÿһ���Ƿ��ж������ �����Ȳ��������ļ�¼
         ** Ĭ�϶��������LeftLine[]==0
         ** ������0���������Ϊû�ж���
         ** */


        Mid=CentreLine[row];      //����һ�ε�����ֵΪ��һ��ɨ�ߵ��м��
    }
}

/*
 ** ��������: �������ұ߽������õ��¹յ㣨ʮ�֡������������жϻ����ϣ�
 ** ��    ��: int starline:     ��ʼ��
 **           int endline:      ������
 **           int *LeftLine��     ��������
 **           int *RightLine��   ��������
 **           Point *InflectionL: ��߹յ�
 **           Point *InflectionR: �ұ߹յ�
 ** �� �� ֵ: ��
 ** ˵    ��: ��ָ�����������������õ��ĵ��������ȷ��ֱ�߽��в���
 ** ��    ��: LJF
 */
void GetDownInflection(int startline,int endline,int *LeftLine,int *RightLine,Point *InflectionL,Point *InflectionR)
{
    int i;

    for(i=startline;i<endline;i++)
    {
        //�������ߣ�����ȱ�С����С�ĵ㣬�����������ж���Ϊ�����У���߶���Ϊ0
        if(LeftLine[i]>LeftLine[i-1] && LeftLine[i]>LeftLine[i-3] && LeftLine[i]>LeftLine[i+1] && LeftLine[i]>LeftLine[i+3])
        {
            InflectionL->X=LeftLine[i];//����յ�ģ�x,y������
            InflectionL->Y=i;
        }
        //�������ߣ����������С�ĵ�����ұߵĹյ㣬�ұ��߶���ΪMT9V03X_W
        if(RightLine[i]<LeftLine[i-1] && RightLine[i]<LeftLine[i-3] && RightLine[i]<LeftLine[i+1] && RightLine[i]<LeftLine[i+3])
        {
            InflectionR->X=RightLine[i];//����յ�ģ�x,y������
            InflectionR->Y=i;
        }
    }
}

/*********************************************************************************
 ** ��������: ���������¹յ���Ѱ�������Ϲյ�
 ** ��    ��: Point InflectionL: ��߹յ�
 **           Point InflectionR: �ұ߹յ�
 **           Point *UpInflectionC: �м��Ϲյ�
 ** �� �� ֵ: ��
 ** ˵    ��: ��������������޸ģ�д�����ҹյ����棬���߻�Ҫ��һ��Ԫ�����͵Ĳ����������������Ҳ�ͬ�Ĺյ㣬����ֻ�������
 ** ��    ��: LJF
 **********************************************************************************/
void GetForkUpInflection(Point DownInflectionL,Point DownInflectionR,Point *UpInflectionC)
{
    int starline,i;
    UpInflectionC->X=(DownInflectionL.X+DownInflectionR.X)/2;//V���Ϲյ��������Ϊ���ҹյ��ֵ����Ҫ�޸ģ���һ������������
    starline=(DownInflectionL.Y+DownInflectionR.Y)/2;//��ʼ��Ϊ���ҹյ��еľ�ֵ
    //���������ҵ��Ǹ�����ĵ㼴Ϊ�Ϲյ�
    for(i=starline;i<MT9V03X_H;i++)
    {
        //ͼ��������[��][��]
        if(BinaryImage[i][UpInflectionC->X]==IMAGE_WHITE&&BinaryImage[i+1][UpInflectionC->X]==IMAGE_BLACK)
        {
            UpInflectionC->Y=i;//Y����������
        }
    }
}
