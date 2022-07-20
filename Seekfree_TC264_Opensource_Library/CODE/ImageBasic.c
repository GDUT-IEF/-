#include "ImageBasic.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "LED.h"
#include "zf_gpio.h"

#define FINE_RIGHT_ANGLE_INFLECTION_DEBUG   1   //����ͼ��ڰ�������ֱ�ǹյ�Ԥ����ĺ궨��1������ 0���ر�

//��������
uint8 Mid=MT9V03X_W/2;                        //��ʼ��ɨ�ߵ��е�Ϊͼ���е�
uint8 Lost_Row=0;                             //���߶�ʧ��������(ɨ�ߵ�������)
uint8 LostNum_LeftLine=0,LostNum_RightLine=0; //��¼���ұ߽綪����

/*
 ** ��������: ɨ����ȡ���������ߵ�����
 ** ��    ��: *LeftLine����������
 **           *CentreLine����������
 **           *RightLine����������
 **           path��Ĭ��ɨ�߷���
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 */
void GetImagBasic(int *LeftLine, int *CentreLine, int *RightLine ,char path)
{
    uint8 row,cloum;              //��,��
    uint8 flag_l=0,flag_r=0;    //��¼�Ƿ���flag��flag=0������
    uint8 num=0;                //��¼����������ʧ�Ĵ���
    LostNum_LeftLine=0;LostNum_RightLine=0; //�Ѷ��ߵļ�����������

    //��ʼɨ��(��������,���м�������),Ϊ��ɨ�ߵ��Ͻ���,������BORDER_BIAS��������ɨ�߷�Χ��С��ͼ���С
    for(row=MT9V03X_H-1;row>0;row--) //�������ϣ���������ͼ��
    {
        //�������⣬���Ȱ�pathɨ�߷���Ѱ������
        if(BinaryImage[row][Mid]==IMAGE_BLACK)  //ɨ���е��Ǻ�ɫ�ģ��е��������⣩
        {
            num++;    //����������ʧ��+1
            if(path=='L')   //Ĭ������ɨ��
            {
                //�������ɨ�ߣ�Ѱ���ұ߽��
                for(cloum=Mid;cloum-BORDER_BIAS>0;cloum--)    //����ɨ
                {
                    if(BinaryImage[row][cloum]==IMAGE_WHITE && BinaryImage[row][cloum-BORDER_BIAS]==IMAGE_WHITE)  //�ҵ��׵㣨��������ɨ�������ڣ���-�ף�
                    {
                        RightLine[row]=cloum;    //��¼�ұ߽�㣨�����ҵ������ұ߽�㣩
                        flag_r=1;               //flag���޶��߱��
                        break;
                    }
                }
                //��������ɨ�ߵĽ���ж϶�ʧ������������߻����ұߴӶ������������ı�ɨ��
                if(flag_r==1)   //�ҵ����ұ߽磨��ʧ����������ߣ�
                {
                    //��������Ѱ����߽�
                    for(;cloum-BORDER_BIAS>0;cloum--)    //��������ɨ
                    {
                        if(BinaryImage[row][cloum]==IMAGE_BLACK && BinaryImage[row][cloum-BORDER_BIAS]==IMAGE_BLACK)    //�ҵ��ڵ㣺����������ɨ�������⣺��-�ڣ�
                        {
                            LeftLine[row]=cloum;   //��¼��߽��
                            flag_l=1;              //flag���޶��߱��
                            break;
                        }
                    }
                }
                else            //û���ҵ��ұ߽磨��ʧ���������ұߣ�
                {
                    for(cloum=Mid;cloum+BORDER_BIAS<MT9V03X_W-1;cloum++)    //����ɨ
                    {
                        if(BinaryImage[row][cloum]==IMAGE_WHITE && BinaryImage[row][cloum+BORDER_BIAS]==IMAGE_WHITE)
                        {
                            LeftLine[row]=cloum;   //��¼��߽�㣨�����ҵ�������߽�㣩
                            flag_l=1;              //flag���޶��߱��
                            break;
                        }
                    }
                    if(flag_l==1)   //�ҵ�����߽磨��ʧ���������ұߣ�
                    {
                        for(;cloum+BORDER_BIAS<MT9V03X_W-1;cloum++) //��������ɨ
                        {
                            if(BinaryImage[row][cloum]==IMAGE_BLACK && BinaryImage[row][cloum+BORDER_BIAS]==IMAGE_BLACK)
                            {
                                RightLine[row]=cloum;   //��¼�ұ߽��
                                flag_r=1;               //flag���޶��߱��
                                break;
                            }
                        }
                    }
                }
            }
            else if(path=='R')  //Ĭ������ɨ��
            {
                //�����ұ�ɨ�ߣ�Ѱ����߽��
                for(cloum=Mid;cloum+BORDER_BIAS<MT9V03X_W-1;cloum++)    //����ɨ
                {
                    //�ж���߽��
                    if(BinaryImage[row][cloum]==IMAGE_WHITE && BinaryImage[row][cloum+BORDER_BIAS]==IMAGE_WHITE)  //�ҵ��׵㣨��������ɨ�������ڣ���-�ף�
                    {
                        LeftLine[row]=cloum;    //��¼��߽�㣨�����ҵ�������߽�㣩
                        flag_l=1;               //flag���޶��߱��
                        break;
                    }
                }
                //��������ɨ�ߵĽ���ж϶�ʧ������������߻����ұߴӶ������������ı�ɨ��
                if(flag_l==1)   //�ҵ�����߽磨��ʧ���������ұߣ�
                {
                    //��������Ѱ���ұ߽�
                    for(;cloum+BORDER_BIAS<MT9V03X_W-1;cloum++) //��������ɨ
                    {
                        //�ж��ұ߽��
                        if(BinaryImage[row][cloum]==IMAGE_BLACK && BinaryImage[row][cloum+BORDER_BIAS]==IMAGE_BLACK)    //�ҵ��ڵ㣨��������ɨ�������⣺��-�ڣ�
                        {
                            RightLine[row]=cloum;   //��¼��߽��
                            flag_r=1;               //flag���޶��߱��
                            break;
                        }
                    }
                }
                else            //û���ҵ���߽磨��ʧ����������ߣ�
                {
                    for(cloum=Mid;cloum-BORDER_BIAS>0;cloum--)    //����ɨ
                    {
                        //�ж��ұ߽��
                        if(BinaryImage[row][cloum]==IMAGE_WHITE && BinaryImage[row][cloum-BORDER_BIAS]==IMAGE_WHITE)
                        {
                            RightLine[row]=cloum;   //��¼�ұ߽�㣨�����ҵ������ұ߽�㣩
                            flag_r=1;               //flag���޶��߱��
                            break;
                        }
                    }
                    if(flag_r==1)   //�ҵ����ұ߽磨��ʧ����������ߣ�
                    {
                        //��������Ѱ����߽�
                        for(;cloum-BORDER_BIAS>0;cloum--)   //��������ɨ
                        {
                            //�ж���߽��
                            if(BinaryImage[row][cloum]==IMAGE_BLACK && BinaryImage[row][cloum-BORDER_BIAS]==IMAGE_BLACK)  //�ж��ұ߽��
                            {
                                LeftLine[row]=cloum;    //��¼��߽��
                                flag_l=1;               //flag���޶��߱��
                                break;
                            }
                        }
                    }
                }
            }
        }
        //�������У�����ɨ��
        else
        {
            num=0;  //�������������ʧ��=0
            //���ɨ��
            for(cloum=Mid;cloum-BORDER_BIAS>0;cloum--)              //����ɨ
            {
                if(BinaryImage[row][cloum]==IMAGE_BLACK && BinaryImage[row][cloum-BORDER_BIAS]==IMAGE_BLACK)  //�ж���߽�㣨BORDER_BIAS��żȻ���أ�
                {
                    LeftLine[row]=cloum;    //��¼��߽��
                    flag_l=1;               //flag���޶��߱��
                    break;
                }
            }
            //�ұ�ɨ��
            for(cloum=Mid;cloum+BORDER_BIAS<MT9V03X_W-1;cloum++)    //����ɨ
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
            RightLine[row]=MT9V03X_W-1; //�ұ߽�����ͼ����ұ߽�
            LostNum_RightLine++;        //�Ҷ�����+1
        }
        CentreLine[row]=(LeftLine[row]+RightLine[row])/2;   //�������ߵ�
        //Ϊ��һ��ɨ����׼��
        Mid=CentreLine[row];    //����һ�ε�����ֵΪ��һ��ɨ�ߵ��м��
        flag_l=0;               //��߽綪��flag��0
        flag_r=0;               //�ұ߽綪��flag��0

//        //LCD����ͼ��
//        lcd_drawpoint(LeftLine[row],row,GREEN);
//        lcd_drawpoint(CentreLine[row],row,RED);
//        lcd_drawpoint(RightLine[row],row,BLUE);
//        systick_delay_ms(STM0,50);
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
        //2022��5��27�գ��Ӷ�һ��������������>=���ڵĵ㣬�����ȡɸѡ���ǲ���ֱ�ߣ����ڱȽ�Զ�ĵ�
        if(InflectionL->Y==0 && LeftLine[i]>=LeftLine[i-1] && LeftLine[i]>=LeftLine[i-3] && LeftLine[i]>=LeftLine[i+1] && LeftLine[i]>=LeftLine[i+3])
        {
            if(LeftLine[i]>LeftLine[i-5] && LeftLine[i]>LeftLine[i-7] && LeftLine[i]>LeftLine[i+5] && LeftLine[i]>LeftLine[i+7])
            {
                InflectionL->X=LeftLine[i];//����յ�ģ�x,y������
                InflectionL->Y=i;
                /*debug*/
//                lcd_showint32(0,6,InflectionL->X,3);
//                systick_delay_ms(STM0, 1000);
            }
        }

        //�������ߣ����������С�ĵ�����ұߵĹյ㣬�ұ��߶���ΪMT9V03X_W-1
        //���˸��ж�InflectionR->Y==0��Ϊ�˴������ϱ������ҵ���֮��Ͳ��ټ����������ˣ���������ʱ��Ľؾ�ͼƬ�Ͳ��øոպ���
        if(InflectionR->Y==0 && RightLine[i]<=RightLine[i-1] && RightLine[i]<=RightLine[i-3] && RightLine[i]<=RightLine[i+1] && RightLine[i]<=RightLine[i+3])
        {
            if(RightLine[i]<RightLine[i-5] && RightLine[i]<RightLine[i-7] && RightLine[i]<RightLine[i+5] && RightLine[i]<RightLine[i+7])
            {
                InflectionR->X=RightLine[i];//����յ�ģ�x,y������
                InflectionR->Y=i;
               /*��ӡ���ж�Ϊ�յ�������꣬���ڵ���*/
//                lcd_showint32(TFT_X_MAX-50,6,InflectionR->X,3);
//                systick_delay_ms(STM0, 1000);
            }
        }

        /*��ӡ���ж�Ϊ�յ�������꣬���ڵ���*/
//        lcd_drawpoint(RightLine[i],i,RED);
//        lcd_showint32(0,0,LeftLine[i],3);
//        systick_delay_ms(STM0, 800);
    }
}
#define UPINFLECTION_DOWM_MIN_THRESHOLD  15  //�Ϲյ������������������Ĳ�ֵ��С��ֵ
#define UPINFLECTION_UP_MAX_THRESHOLD  5  //�Ϲյ������������������Ĳ�ֵ�����ֵ
#define UPINFLECTION_COMPARE_INTERVAL 3 //�Ϲյ�����֮��Ƚϼ��

/************************************************************************
 ** ��������: �������ұ߽�õ��Ϲյ�
 ** ��    ��: char Choose��ѡ��������߻�������
 **           int startline����ʼ��
 **           int endline��������
 **           Point *UpInflection���Ϲյ�
 ** �� �� ֵ: ��
 ** ˵    ��: ��ʼ��ҪС�ڽ����У��������±���������
 ** ��    ��: LJF
 ***********************************************************************/
void GetUpInflection(char Choose,int startline,int endline,Point *UpInflection)
{
    int row=0;
    switch(Choose)
    {
        case 'L':
        {
            //�������±���
            for (row = startline; row < endline; row++)
            {
                //�����е�������-���������������ֵ������ABS��Ϊ���ٶȸ���
                if (LeftLine[row] - LeftLine[row+UPINFLECTION_COMPARE_INTERVAL] >= UPINFLECTION_DOWM_MIN_THRESHOLD
                 && LeftLine[row - UPINFLECTION_COMPARE_INTERVAL] - LeftLine[row + UPINFLECTION_COMPARE_INTERVAL] >= UPINFLECTION_DOWM_MIN_THRESHOLD
                 && LeftLine[row-1]-LeftLine[row]<=UPINFLECTION_UP_MAX_THRESHOLD)
                {
                    UpInflection->X = LeftLine[row]; UpInflection->Y = row;
                    /**************debug***********/
//                    lcd_drawpoint(UpInflection->X,UpInflection->Y,PURPLE);
//                    lcd_showint32(TFT_X_MAX-50,0,UpInflection->X,3);
//                    lcd_showint32(TFT_X_MAX-50,1,UpInflection->Y,3);
//                    systick_delay_ms(STM0, 800);
                    /*****************************/
                    break;
                }
                /**************debug***********/
//                lcd_showint32(0,0,LeftLine[row],3);
//                lcd_showint32(0,1,LeftLine[row+UPINFLECTION_COMPARE_INTERVAL],3);
//                systick_delay_ms(STM0, 800);
                /*****************************/
            }
            break;
        }
        case 'R':
        {
            for(row = startline; row < endline; row++)
            {
                //����������-�����е������������ֵ������ABS��Ϊ���ٶȸ���
                if (RightLine[row+UPINFLECTION_COMPARE_INTERVAL] - RightLine[row] >= UPINFLECTION_DOWM_MIN_THRESHOLD
                 && RightLine[row + UPINFLECTION_COMPARE_INTERVAL] - RightLine[row - UPINFLECTION_COMPARE_INTERVAL] >= UPINFLECTION_DOWM_MIN_THRESHOLD
                 && RightLine[row]-RightLine[row-1]<=UPINFLECTION_UP_MAX_THRESHOLD)
                {
                    UpInflection->X=RightLine[row];UpInflection->Y=row;
                   /**************debug***********/
//                   lcd_drawpoint(UpInflection->X,UpInflection->Y,PURPLE);
//                   lcd_showint32(TFT_X_MAX-50,0,UpInflection->X,3);
//                   lcd_showint32(TFT_X_MAX-50,1,UpInflection->Y,3);
//                   systick_delay_ms(STM0, 800);
                   /*****************************/
                    break;
                }
            }
            break;
        }
        default:break;
    }
}
/************************************************************************
 ** ��������: ����ͼ��ڰ�����Ѱ����ֱ�ǹյ�
 ** ��    ��: char Choose��ѡ�������ϻ�������
 **           Point DowmInflection����׼��
 **           Point *UpInflection���ҵ����Ϲյ�
 **           int RowThr:����ͼ���е���ֵ��������㵽����ͣ�£�
 **           int CloumnThr:����ͼ���е���ֵ
 ** �� �� ֵ: ��
 ** ˵    ��: ��ʼ��ҪС�ڽ����У��������±���������
 ** ��    ��: LJF
 ***********************************************************************/
void GetRightangleUPInflection(char Choose,Point DowmInflection,Point *UpInflection,int ROWTHR,int CLOUMNTHR)
{
    int row=0,cloumn=0;//��ʼ��,��
    UpInflection->X=0;UpInflection->Y=0;//���Ϲյ�����
    //���������Ұ�����
    for(row=DowmInflection.Y;row>ROWTHR;row--)
    {
#if FINE_RIGHT_ANGLE_INFLECTION_DEBUG
        lcd_drawpoint(DowmInflection.X, row, PURPLE);
#endif
        if(BinaryImage[row][DowmInflection.X]==IMAGE_WHITE&&BinaryImage[row-1][DowmInflection.X]==IMAGE_BLACK)
        {
            row=row-3;//������������
            switch(Choose)
            {
               case 'L':
               {
                   //�������ҵ�������
                   for(cloumn=DowmInflection.X;cloumn<CLOUMNTHR;cloumn++)
                   {
#if FINE_RIGHT_ANGLE_INFLECTION_DEBUG
                       lcd_drawpoint(cloumn, row, PURPLE);
#endif
                       if(BinaryImage[row][cloumn]==IMAGE_BLACK&&BinaryImage[row][cloumn+1]==IMAGE_WHITE)
                       {
                           UpInflection->X=cloumn;UpInflection->Y=row;
                           break;
                       }
                   }
                   break;
               }
               case 'R':
               {
                   //�������ҵ�������
                   for(cloumn=DowmInflection.X;cloumn>CLOUMNTHR;cloumn--)
                   {
                       if(BinaryImage[row][cloumn]==IMAGE_BLACK&&BinaryImage[row][cloumn-1]==IMAGE_WHITE)
                       {
                           UpInflection->X=cloumn;UpInflection->Y=row;
                           break;
                       }
                   }
                   break;
               }
               default:break;
            }
            break;//������ѭ��,û��Ҫ������ѭ����ȥ
        }
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
 ** ��������: ����������Ⱥ���
 ** ��    ��: int startline:Ҫ��������ʼ��
 **           int endline:Ҫ�����Ľ�����
 ** �� �� ֵ: ��
 ** ˵    ��: ������ǰ����ʹ�ã�������ȿ��Ը������ʱ�򵥱�Ѳ��
 ** ��    ��: LJF
 ***********************************************************************/
void MeasureWidth(int startline,int endline)
{
    int row=0,width=0;
    for(row=startline;row>endline;row--)
    {
        width=RightLine[row]-LeftLine[row];
        printf("%d\r\n",width);//�⵽����������еĺ�����ϵΪy=135-(119-nowline)*1.1
    }
}
