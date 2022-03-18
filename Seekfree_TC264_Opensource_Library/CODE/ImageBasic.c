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
    int row,cloum;              //��,��
    uint8 flag_l=0,flag_r=0;    //��¼�Ƿ���flag��flag=0������
    LostNum_LeftLine=0;LostNum_RightLine=0;//�Ѷ��ߵļ�����������
    //��ʼɨ��(��������,���м�������),Ϊ��ɨ�ߵ��Ͻ���,������BORDER_BIAS��������ɨ�߷�Χ��С��ͼ���С
    for(row=MT9V03X_H;row>0;row--) //ͼ���ԭ�������Ͻ�
    {
        //�������if��Ϊ�˽��ɨ��һ��ʼ��������������⣬���������һ�����
        if(BinaryImage[row][Mid]==IMAGE_BLACK)  //ɨ���е��Ǻ�ɫ�ģ��е��������⣩
        {
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

//        //��ֹɨ�ߵ������⣬�����������
//        if(BinaryImage[row][CentreLine[row]]==IMAGE_BLACK && BinaryImage[row+BORDER_BIAS][CentreLine[row]]==IMAGE_BLACK)    //row�е������Ǻڣ�ɨ����������
//        {
//            Lost_CentreLine=row;    //��¼���ߵ㶪ʧ��������
//            if(row>20)              //��ǰ20�в�������
//                break;              //���Ѿ���20�к���ɨ�赽��������,ֱ��break������ͼ��ɨ�ߴ���
//        }

        Mid=CentreLine[row];    //����һ�ε�����ֵΪ��һ��ɨ�ߵ��м�㣬����һ�ε�����ֵ�պ��ڱ�Ե�ϣ���һ�ε�ɨ�߻��������ȫ�ܵ����ߵ����
        flag_l=0;               //��߽綪��flag��0
        flag_r=0;               //�ұ߽綪��flag��0
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

/*
 ** ��������: �������ұ߽��ߺ��¹յ����õ��Ϲյ�
 ** ��    ��: *LeftLine��  ��������
 **           *RightLine����������
 **           *DownInflectionL: ���¹յ�
 **           *DownInflectionR: ���¹յ�
 **           *UpInflectionL��    ���Ϲյ�
 **           *UpInflectionR��    ���Ϲյ�
 ** �� �� ֵ: ��
 ** ˵    ��: ��һЩ����Ԫ��ʶ���������Ҫ�õ��Ϲյ㣬����������Ԫ�ض�Ҫ�õ��ģ����Խ��Ϲյ�ֿ�������Ϊһ��������ʵ�֣������ڲ�ͬ��
 **           Ԫ��ʶ���н��е���
 ** ��    ��: WBN
 */
void GetUpInflection(int *LeftLine,int *RightLine,Point *DownInflectionL,Point *DownInflectionR,Point *UpInflectionL,Point *UpInflectionR)
{
    /*
     ** �򵥴ֱ��ķ�����
     ** ��ߵ��Ϲյ����ұ߽��ߣ��ұߵ��Ϲյ�����߽��ߣ�
     ** ���¹յ����ʱ����Ӧ���Ϲյ���һ�����ڵģ�ֻ���Ƿ���������ͷ��ȡ��ͼ���г��ֵ����⣻�������������һ������������ͼ���У��Ϲյ��
     ** �����ڶ�Ӧ�¹յ�����X�е�λ�ã�����ֻ��Ҫ��һ���Ƿ�����Խ����ж�
     * */

    //��ߵ��Ϲյ����ұ߽���
    if(DownInflectionL->Y+INFLECTION_WIDTH<=MT9V03X_H)
    {
        UpInflectionL->X=RightLine[DownInflectionL->Y+INFLECTION_WIDTH];
        UpInflectionL->Y=DownInflectionL->Y+INFLECTION_WIDTH;
    }
    //�ұߵ��Ϲյ�����߽���
    if(DownInflectionR->Y+INFLECTION_WIDTH<=MT9V03X_H)
    {
        UpInflectionR->X=LeftLine[DownInflectionR->Y+INFLECTION_WIDTH];
        UpInflectionR->Y=DownInflectionR->Y+INFLECTION_WIDTH;
    }


    /*
         ** ��һ�ַ�����
         ** ����Ѱ���¹յ�ķ�����Ѱ��һ������ֵͻ��ĵ�
         * */
//    int row;    //��
//    //�������ߣ�����յ㿪ʼɨ
//    for(row=DownInflectionL->Y;row<MT9V03X_H;row++)  //��ɨ����������ɨ
//    {
//        if()    //��ߵ��Ϲյ���������
//        {
//            UpInflectionL->X=RightLine[row];
//            UpInflectionL->Y=row;
//        }
//    }
//    //�������ߣ����ҹյ㿪ʼɨ
//    for(row=DownInflectionR->Y;row<MT9V03X_H;row++)  //��ɨ����������ɨ
//    {
//
//        if()    //�ұߵ��Ϲյ���������
//        {
//            UpInflectionR->X=LeftLine[row];
//            UpInflectionR->Y=row;
//        }
//    }
}
