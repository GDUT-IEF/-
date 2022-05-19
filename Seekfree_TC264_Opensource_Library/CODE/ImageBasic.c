#include "ImageBasic.h"
#include <math.h>
#include <stdlib.h>

//��������
int LeftLine[MT9V03X_H]={0}, CentreLine[MT9V03X_H]={0}, RightLine[MT9V03X_H]={0};   //ɨ�ߴ�������������
int Mid=MT9V03X_W/2;                        //��ʼ��ɨ�ߵ��е�Ϊͼ���е�
int Lost_Row=0;                             //���߶�ʧ��������(ɨ�ߵ�������)
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
    int row,cloum;              //��,��
    uint8 flag_l=0,flag_r=0;    //��¼�Ƿ���flag��flag=0������
    uint8 num=0;                //��¼����������ʧ�Ĵ���
    LostNum_LeftLine=0;LostNum_RightLine=0; //�Ѷ��ߵļ�����������
    //��ʼɨ��(��������,���м�������),Ϊ��ɨ�ߵ��Ͻ���,������BORDER_BIAS��������ɨ�߷�Χ��С��ͼ���С
    for(row=MT9V03X_H-1;row>0;row--) //ͼ���ԭ�������Ͻ�
    {
        //�������if��Ϊ�˽��ɨ��һ��ʼ��������������⣬���������һ�����
        if(BinaryImage[row][Mid]==IMAGE_BLACK)  //ɨ���е��Ǻ�ɫ�ģ��е��������⣩
        {
            num++;    //���߶�ʧ����+1
            //�������ɨ�ߣ�Ѱ���ұ߽��
            for(cloum=Mid;(cloum-BORDER_BIAS)>0;cloum--)    //�����ɨ��
            {
                if(BinaryImage[row][cloum]==IMAGE_WHITE && BinaryImage[row][cloum-BORDER_BIAS]==IMAGE_WHITE)  //�ж��ұ߽�㣨��������ɨ��������Ӧ���Ǻڱ�׵Ĺ��̣�
                {
                    RightLine[row]=cloum;    //��¼�ұ߽�㣨�����ҵ������ұ߽�㣩
                    flag_r=1;               //flag���޶��߱��
                    break;
                }
            }
            //��������ɨ�ߵĽ���ж϶�ʧ������������߻����ұߴӶ������������ı�ɨ��
            if(flag_r==1)   //����ɨ���ҵ����ұ߽�û�ж��ߣ���ʧ����������ߣ�
            {
                for(;(cloum-BORDER_BIAS)>0;cloum--)    //���������ɨ���ҵ���������߽�
                {
                    if(BinaryImage[row][cloum]==IMAGE_BLACK && BinaryImage[row][cloum+BORDER_BIAS]==IMAGE_BLACK)    //�ж���߽�㣨��������ɰױ�ڣ�
                    {
                        LeftLine[row]=cloum;   //��¼��߽��
                        flag_l=1;              //flag���޶��߱��
                        break;
                    }
                }
            }
            //����ɨ��û���ҵ��ұ߽�㣬��ô����ɨ��Ѱ����߽��
            else
            {
                for(cloum=Mid;(cloum+BORDER_BIAS)<MT9V03X_W;cloum++)    //���ұ�ɨ��
                {
                    if(BinaryImage[row][cloum]==IMAGE_WHITE && BinaryImage[row][cloum+BORDER_BIAS]==IMAGE_WHITE)  //�ж���߽��
                    {
                        LeftLine[row]=cloum;   //��¼��߽�㣨�����ҵ�������߽�㣩
                        flag_l=1;              //flag���޶��߱��
                        break;
                    }
                }
                if(flag_l==1)   //����ɨ���ҵ�����߽�û�ж��ߣ���ʧ���������ұߣ�
                {
                    for(;(cloum+BORDER_BIAS)<MT9V03X_W;cloum++)    //�������ұ�ɨ�ߣ�Ѱ���ұ߽��
                    {
                        if(BinaryImage[row][cloum]==IMAGE_BLACK && BinaryImage[row][cloum+BORDER_BIAS]==IMAGE_BLACK)  //�ж��ұ߽��
                        {
                            RightLine[row]=cloum;   //��¼�ұ߽��
                            flag_r=1;               //flag���޶��߱��
                            break;
                        }
                    }
                }
            }
        }
        //���������ϣ�����ɨ��
        else
        {
            num=0;  //���߶�ʧ����=0
            //���ɨ��
            for(cloum=Mid;(cloum-BORDER_BIAS)>0;cloum--)
            {
                if(BinaryImage[row][cloum]==IMAGE_BLACK && BinaryImage[row][cloum-BORDER_BIAS]==IMAGE_BLACK)  //�ж���߽�㣨BORDER_BIAS��żȻ���أ�
                {
                    LeftLine[row]=cloum;    //��¼��߽��
                    flag_l=1;               //flag���޶��߱��
                    break;
                }
            }
            //�ұ�ɨ��
            for(cloum=Mid;(cloum+BORDER_BIAS)<MT9V03X_W;cloum++)
            {
                if(BinaryImage[row][cloum]==IMAGE_BLACK && BinaryImage[row][cloum+BORDER_BIAS]==IMAGE_BLACK)  //�ж��ұ߽�㣨BORDER_BIAS��żȻ���أ�
                {
                    RightLine[row]=cloum;   //��¼�ұ߽��
                    flag_r=1;               //flag���޶��߱��
                    break;
                }
            }
        }
        //1.29��������д�����ݴ���
        if(flag_l==0)   //��߽綪��
        {
            LeftLine[row]=0;            //��߽�����ͼ�����߽�
            LostNum_LeftLine++;         //������+1
        }
        if(flag_r==0)   //�ұ߽綪��
        {
            RightLine[row]=MT9V03X_W-1;   //�ұ߽�����ͼ����ұ߽�
            LostNum_RightLine++;        //�Ҷ�����+1
        }
        CentreLine[row]=(LeftLine[row]+RightLine[row])/2;   //��¼���ߵ�

        Mid=CentreLine[row];    //����һ�ε�����ֵΪ��һ��ɨ�ߵ��м�㣬����һ�ε�����ֵ�պ��ڱ�Ե�ϣ���һ�ε�ɨ�߻��������ȫ�ܵ����ߵ����
        flag_l=0;               //��߽綪��flag��0
        flag_r=0;               //�ұ߽綪��flag��0
        //�������һ��ɨ�ߵ���������жϣ�ǰ30�в�������ֻ��������������������⴦��
//        if(row<90&&num!=0)  //��һ�е��������������
//        {
//            if(abs(CentreLine[row]-CentreLine[row+1])>60)  //�����������ߵ�ƫ�����
//            {
//                Lost_Row=row;
//            }
//        }
//        if(row<90&&num>=5)   //����������ʧ5�����ж�Ϊɨ���Ѿ����������⣬��ֹɨ��
//        {
//            Lost_Row=row;
//        }
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
 ** ˵    ��: - ��ָ�����������������õ��ĵ��������ȷ��ֱ�߽��в���
 **           - ����ͼ��ԭ�������Ͻǣ�������ʼ���Ǵ��ڽ����У�����������������ϱ���
 ** ��    ��: LJF
 */
void GetDownInflection(int startline,int endline,int *LeftLine,int *RightLine,Point *InflectionL,Point *InflectionR)
{
    int i=0;
    InflectionL->X=0;InflectionL->Y=0;InflectionR->X=0;InflectionR->Y=0;//���ҹյ�����

    for(i=startline;i>endline;i--)
    {
        //�������ߣ�����ȱ�С����С�ĵ㣬�����������ж���Ϊ�����У���߶���Ϊ0
        /*ע�⣺��������ж������Ǻ����ڵ�1,3��ֵ�ԱȵĻ���������̫С�Ӷ��������ȵ��������ڵĻ�����Ӱ���жϣ�������Ҫ�Ĵ�Ƚϵ�����*/
        //���˸��ж�InflectionL->Y==0��Ϊ�˴������ϱ������ҵ���֮��Ͳ��ټ����������ˣ���������ʱ��Ľؾ�ͼƬ�Ͳ��øոպ���
        if(InflectionL->Y==0 && LeftLine[i]>LeftLine[i-3] && LeftLine[i]>LeftLine[i-5] && LeftLine[i]>LeftLine[i+3] && LeftLine[i]>LeftLine[i+5])
        {
            InflectionL->X=LeftLine[i];//����յ�ģ�x,y������
            InflectionL->Y=i;
            /*��ӡ���ж�Ϊ�յ�������꣬���ڵ���*/
//            lcd_showint32(0,6,InflectionL->X,3);
//            systick_delay_ms(STM0, 1000);
        }
        //�������ߣ����������С�ĵ�����ұߵĹյ㣬�ұ��߶���ΪMT9V03X_W
        //���˸��ж�InflectionR->Y==0��Ϊ�˴������ϱ������ҵ���֮��Ͳ��ټ����������ˣ���������ʱ��Ľؾ�ͼƬ�Ͳ��øոպ���
        if(InflectionR->Y==0 && RightLine[i]<RightLine[i-3] && RightLine[i]<RightLine[i-5] && RightLine[i]<RightLine[i+3] && RightLine[i]<RightLine[i+5])
        {
            InflectionR->X=RightLine[i];//����յ�ģ�x,y������
            InflectionR->Y=i;
            /*��ӡ���ж�Ϊ�յ�������꣬���ڵ���*/
//            lcd_showint32(TFT_X_MAX-50,6,InflectionR->X,3);
//            systick_delay_ms(STM0, 1000);
        }
        /*��ӡ���ж�Ϊ�յ�������꣬���ڵ���*/
//        lcd_drawpoint(RightLine[i],i,RED);
//        lcd_showint32(0,0,RightLine[i],3);
//        systick_delay_ms(STM0, 800);
    }
}

/*---------------------------------------------------------------
 ����    ����Bin_Image_Filter
 ����    �ܡ��������
 ����    ������
 ���� �� ֵ����
 ��ע�����
 ----------------------------------------------------------------*/
void Bin_Image_Filter(void)
{
    for (int nr=1; nr < MT9V03X_H-1; nr++)
    {
        for (int nc=1; nc < MT9V03X_W-1; nc++)
        {
            if ((BinaryImage[nr][nc] == IMAGE_BLACK)
                    &&(BinaryImage[nr-1][nc]+BinaryImage[nr+1][nc]+BinaryImage[nr][nc+1]+BinaryImage[nr][nc-1]>510))
            {
                BinaryImage[nr][nc] = IMAGE_WHITE;
            }
            else if ((BinaryImage[nr][nc] == IMAGE_WHITE)
                    && (BinaryImage[nr-1][nc]+BinaryImage[nr+1][nc]+BinaryImage[nr][nc+1]+BinaryImage[nr][nc-1]<510))
            {
                BinaryImage[nr][nc] = IMAGE_BLACK;
            }
        }
    }
}

/************************************************************************
 ** ��������: �������Եɨ��
 ** ��    ��: ��
 ** �� �� ֵ: ��
 ** ˵    ��: ʹ������������ȫ�ֱ���
 ** ��    ��: LJF
 ***********************************************************************/
void EdgeTrack(void)
{
    int num=0,row=MT9V03X_H,cloum=MT9V03X_W/2;//������ı�ţ��У���
    uint8 left_seed=0,right_seed=0;//�������������Ƿ��ҵ��ı�־����
    Point seed;//������ӵĵ�
    for(row=MT9V03X_H;row>0;row--)//�д�������
    {
        //������Ӳ����ھ�Ҫ����
        if(left_seed==0)
        {
            for(cloum=MT9V03X_W/2;cloum>0;cloum--)//�д��м������ҵ��������
            {
                if(BinaryImage[row][cloum]==IMAGE_BLACK)
                {
                    LeftLine[row]=cloum;//��¼��߽������λ��
                    seed.X=cloum;seed.Y=row;//���ӱ��
                    left_seed=1;//�����ߵ������Ѿ����ҵ�
                    break;//����ѭ��
                }
                else
                    left_seed=0;//���������һ������Ļ��ᣬ��ֹ��߲��ֵ����ұ�û�в��ֵ���Ȼ�����²���ʱ��������
            }
        }
        //��������
        for(num=0;num<5;num++)
        {
            switch(num)
            {
            case 0:
                if(BinaryImage[seed.Y][seed.X+1]==IMAGE_BLACK)
                {
                    left_seed=0;//������ʧ
                    num=5;//Ϊ������forѭ��
                }
                break;
            case 1:
                if(BinaryImage[seed.Y-1][seed.X+1]==IMAGE_BLACK)
                {
                    seed.X+=1;seed.Y-=1;//�ҵ�����һ���ڵ�����һ���ڵ�Ϊ����������
                    row--;
                    LeftLine[row]=seed.X;//���ҵ�����߽��������������
                    num=5;//Ϊ������forѭ��
                }
                break;
            case 2:
               if(BinaryImage[seed.Y-1][seed.X]==IMAGE_BLACK)
               {
                   seed.Y-=1;//�ҵ�����һ���ڵ�����һ���ڵ�Ϊ����������
                   row--;
                   LeftLine[row]=seed.X;//���ҵ�����߽��������������
                   num=5;//Ϊ������forѭ��
               }
               break;
            case 3:
                if(BinaryImage[seed.Y-1][seed.X-1]==IMAGE_BLACK)
                 {
                    seed.X-=1;seed.Y-=1;//�ҵ�����һ���ڵ�����һ���ڵ�Ϊ����������
                    row--;
                    LeftLine[row]=seed.X;//���ҵ�����߽��������������
                    num=5;//Ϊ������forѭ��
                 }
                 break;
            case 4:
                if(BinaryImage[seed.Y][seed.X-1]==IMAGE_BLACK)
                 {
                    left_seed=0;//������ʧ
                    num=5;//Ϊ������forѭ��
                 }
                 break;
            //��Ȼ�ǰ������ǳ����˹յ�֮��Ͳ�ɨ�ˣ�˵���˴��ǹյ㣬��Ҫ���²�����һ��
            default:break;
            }
        }
    }
}
