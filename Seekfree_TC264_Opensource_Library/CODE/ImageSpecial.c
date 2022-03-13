/*
 * ImageSpecial.c
  *  �����ϸ�������Ԫ�ص�ʶ��
  *  ���ļ��ĺ���ֻʵ��ʶ���Ƿ��и�Ԫ�صĳ����Լ����ֵ�λ�ã����ڸ�Ԫ�س����˶��ٴ��Լ���Ӧ��ʩ�����ڸ��ļ���Χ��
 *  Created on: 2022��1��17��
 *      Author: yue
 */

#include "ImageSpecial.h"

/*
 *******************************************************************************************
 ** ��������: ʶ��������
 ** ��    ��: *LeftLine��  ��������
 **           *RightLine����������
 ** �� �� ֵ: 0��û��ʶ��������
 **           1��ʶ���������ҳ����ڳ����
 **           2��ʶ���������ҳ����ڳ��Ҳ�
 ** ��    ��: WBN
 ** ע    �⣺1 . Ĭ���ڽ���������ʶ���·�ζ���ֱ�ߣ�����ͷ�Ѿ�����
 **           2.����û��ʵ��ͼ���ο���ֻ���ȼ���һ������״̬������������ǡ�ò�������ͼ��
 ********************************************************************************************
 */
uint8 StartLineFlag(int *LeftLine,int *RightLine)
{
    /*
     ** �������ߵĵط����г��⣬��������ԭ�򣬳����п��ܳ����ڳ�����߻��ұߣ�����Ҳ�ȶ���������������жϲ��ֱ���
     ** �Գ��������Ϊ�����ӹյ㿪ʼ��Ϊɨ�ߵ�ԭ�㣬�������ϣ���������ɨ��
     ** �̶�һ�У���������ɨ���ҵ�һ���ڵ㣬�̶����У���������ɨ����¼�������ֵĺڵ���������ߵĿ�ȣ������ÿ�ȴ�����ֵBLACK_WIDTH����
     ** ��ȷ������һ�����ߡ�����ɨ��һ�У����������㹻��������ĺ��ߣ������趨����ֵBLACK_NUM��������Ϊ��һ�з��ϰ����ߡ����ո����ۻ�
     ** ���������е�ɨ�裬���������������㹻�ࣨ�����趨����ֵBLACK_TIMES��������Ϊ��һ��ͼƬ�д��ڰ����ߣ�����·����������
     **/

    int row,cloum;          //��,��
    int Black_width=0;      //�̶��У�����ɨ���Ǽ�¼ÿ�κڵ�ĸ�������һ�����ߵĿ�ȣ�
    int Black_num=0;        //��¼�к��ߵ���������Ϊ�жϸ����Ƿ�Ϊ�����ߵ�����
    int Black_times=0;      //��¼��������ߵ�����������Ϊ�жϸ�·���Ƿ�Ϊ�����ߵ�����

    Point InflectionL, InflectionR; //�¹յ�
    InflectionL.X=0;
    InflectionL.Y=0;
    InflectionR.X=0;
    InflectionR.Y=0;
    GetDownInflection(0,MT9V03X_H,LeftLine,RightLine,&InflectionL,&InflectionR);    //��ȡ�¹յ�


    if(InflectionL.X!=0&&InflectionL.Y!=0)    //�յ㣨���⣩�����
    {
        for(row=InflectionL.X,cloum=InflectionL.Y;row<MT9V03X_H;row++)        //����յ㿪ʼ�̶��У���������ɨ
        {
            if(BinaryImage[row][cloum]==IMAGE_BLACK)    //�ҵ���һ���ڵ�
            {
                for(;cloum<MT9V03X_W;cloum++)                                 //�̶��У���������ɨ
                {
                    if(BinaryImage[row][cloum]==IMAGE_BLACK)    //ɨ���ڵ�
                    {
                        Black_width++;   //���߿��+1
                    }
                    else                                        //ɨ���׵�
                    {
                        if(Black_width>=S_BLACK_WIDTH)    //�жϺ��߿���Ƿ�������ֵ
                        {
                            Black_num++; //�к�������+1
                        }
                        Black_width=0;   //��һ�ΰ׵��жϺ����ú��߿��
                    }
                    if(Black_num>=S_BLACK_NUM)    //����������ߵ���ֵ����һ�У�
                    {
                        Black_times++;  //��������ߵ�����+1
                        break;
                    }
                }
                Black_num=0;    //��һ�е�ɨ�߽����������к�����
            }
            if(Black_times>=S_BLACK_TIMES)    //�����������·�ε���ֵ
            {
                return 1;
            }
        }
        return 0;
    }

    if(InflectionR.X!=0&&InflectionR.Y!=0)    //�յ㣨���⣩���ұ�
    {
        for(row=InflectionL.X,cloum=InflectionL.Y;row<MT9V03X_H;row++)        //���ҹյ㿪ʼ�̶��У���������ɨ
        {
            if(BinaryImage[row][cloum]==IMAGE_BLACK)    //�ҵ���һ���ڵ�
            {
                for(;cloum>0;cloum--)                                         //�̶��У���������ɨ
                {
                    if(BinaryImage[row][cloum]==IMAGE_BLACK)    //ɨ���ڵ�
                    {
                        Black_width++;   //���߿��+1
                    }
                    else                                        //ɨ���׵�
                    {
                        if(Black_width>=S_BLACK_WIDTH)    //�жϺ��߿���Ƿ�������ֵ
                        {
                            Black_num++; //�к�������+1
                        }
                        Black_width=0;   //��һ�ΰ׵��жϺ����ú��߿��
                    }
                    if(Black_num>=S_BLACK_NUM)    //����������ߵ���ֵ����һ�У�
                    {
                        Black_times++;  //��������ߵ�����+1
                        break;
                    }
                }
                Black_num=0;    //��һ�е�ɨ�߽����������к�����
            }
            if(Black_times>=S_BLACK_TIMES)    //�����������·�ε���ֵ
            {
                return 2;
            }
        }
        return 0;
    }

    return 0;
}

/*
 *******************************************************************************************
 ** ��������: ʶ�𻷵����
 ** ��    ��: LeftLine����������
 **           RightLine����������
 **           InflectionL����յ�
 **           InflectionR���ҹյ�
 ** �� �� ֵ: 0��û��ʶ�𵽻���
 **           1��ʶ�𵽻������ڳ������
 **           2��ʶ�𵽻������ڳ����Ҳ�
 ** ��    ��: WBN
 ** ע    �⣺����Ĺյ���ȷ��������ͼ�����ڹյ���յ�����ݾ�Ϊ0
 ********************************************************************************************
 */
uint8 CircleIslandBegin(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    int row;            //��
    Point Inflection;   //�Ϲյ㣬���ڲ���

    if(InflectionL.X!=0&&InflectionL.Y!=0)  //�յ㣨�����������
    {
        for(row=InflectionL.Y;row-1-C_BIAS>0;row--)      //����յ㿪ʼ��ǰ��ɨ��
        {
//            if(BinaryImage[row][InflectionL.X]==IMAGE_WHITE&&BinaryImage[row-1][InflectionL.X]==IMAGE_BLACK)  //ֱ��ʹ�ö�ֵ����ͼ��ȥѰ���Ϲյ�
            if(LeftLine[row]==0&&LeftLine[row-1]!=0)  //���ж��߶���һ�в�����
            {
                //��¼�Ϲյ�
                Inflection.Y=row-1-C_BIAS;
                Inflection.X=InflectionL.X+10;  //����ɨ�ߵ����ԣ�����Ĵ������ͻ������ұ߲�һ��
                FillingLine(InflectionL,Inflection);                        //���ߴ���
                //Debug����ӡ���ߺ��ͼ��
//                lcd_displayimage032(BinaryImage[0],MT9V03X_W,MT9V03X_H);    //��ֵ�����ͼ��
//                systick_delay_ms(STM0, 1000);

                return 1;
            }
        }
    }
    if(InflectionR.X!=0&&InflectionR.Y!=0)    //�յ㣨���������ұ�
    {
        for(row=InflectionR.Y;row-1-C_BIAS>0;row--)      //���ҹյ㿪ʼ��ǰ��ɨ��
        {
            if(RightLine[row]==MT9V03X_W-1&&RightLine[row-1]!=MT9V03X_W-1)  //���ж��߶���һ�в�����
            {
                //��¼�Ϲյ�
                Inflection.Y=row-1-C_BIAS;
                Inflection.X=RightLine[row-1-C_BIAS];
                FillingLine(InflectionR,Inflection);                        //���ߴ���
                //Debug����ӡ���ߺ��ͼ��
//                lcd_displayimage032(BinaryImage[0],MT9V03X_W,MT9V03X_H);    //��ֵ�����ͼ��
//                systick_delay_ms(STM0, 1000);

                return 2;
            }
        }
    }
    return 0;
}

uint8 CircleIslandEnd(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{

    return 0;
}

/*
 *******************************************************************************************
 ** ��������: ʶ��ʮ�ֻػ�����
 ** ��    ��: LeftLine����������
 **           RightLine����������
 **           InflectionL����յ�
 **           InflectionR���ҹյ�
 ** �� �� ֵ: 0��û��ʶ��ʮ�ֻػ�����
 **           1��ʶ��ʮ�ֻػ�����
 ** ��    ��: WBN
 ** ע    �⣺����Ĺյ���ȷ��������ͼ�����ڹյ���յ�����ݾ�Ϊ0
 ********************************************************************************************
 */
uint8 CrossLoopEnd(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    int row;  //��

    if(InflectionL.X!=0&&InflectionL.Y!=0)  //�յ������
    {
        for(row=InflectionL.Y;row>0;row--)
        {
            if(LeftLine[row]==0&&LeftLine[row-1]!=0)  //���ж��߶���һ�в�����
            {
                return 1;
            }
        }
    }
    if(InflectionR.X!=0&&InflectionR.Y!=0)  //�յ����ұ�
    {
        for(row=InflectionR.Y;row>0;row--)
        {
            if(RightLine[row]==MT9V03X_W-1&&RightLine[row-1]!=MT9V03X_W-1)  //���ж��߶���һ�в�����
            {
                return 1;
            }
        }
    }

    return 0;
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
    UpInflectionC->X=0;UpInflectionC->Y=0;//�Ϲյ�����
    UpInflectionC->X=(DownInflectionL.X+DownInflectionR.X)/2;//V���Ϲյ��������Ϊ���ҹյ��ֵ����Ҫ�޸ģ���һ������������
    starline=(DownInflectionL.Y+DownInflectionR.Y)/2;//��ʼ��Ϊ���ҹյ��еľ�ֵ
    //���������ҵ��Ǹ�����ĵ㼴Ϊ�Ϲյ�
    for(i=starline;i>0;i--)
    {
        //ͼ��������[��][��]
        if(BinaryImage[i][UpInflectionC->X]==IMAGE_WHITE && BinaryImage[i-1][UpInflectionC->X]==IMAGE_BLACK)
        {
            UpInflectionC->Y=i;//Y����������
            /*��ӡ�Ϲյ����꣬���ڲ���*/
//            lcd_showint32(0,0,UpInflectionC->Y,3);
//            lcd_showint32(TFT_X_MAX-50,0,UpInflectionC->X,3);
//            systick_delay_ms(STM0, 1000);
            return;
        }
    }
}

/********************************************************************************************
 ** ��������: ʶ������
 ** ��    ��: int startline:�û���������ʼ��
 **           int endline:�û������Ľ����У���ʾ��ǰ���ε�ʶ�𣬸����ٶȲ�ͬ���е�����
 **           int *LeftLine������
 **           int *RightLine:����
 **           Point *InflectionL:��߹յ�
 **           Point *InflectionR:�ұ߹յ�
 **           Point *InflectionC:�м�յ�
 ** �� �� ֵ: 0��û��ʶ�𵽻���
 **           1��ʶ������
 ** ��    ��: LJF
 ** ע    �⣺1 . Ŀǰ���������������ʱ��ĺ�������Ϊ����ǰ�涼���и�������Ի���ֳ���б���������ʱ�����ҹյ㲢��һ��������
 **           2.����ǽ�����ĺ�����������ʱ��Ӧ����дһ�������ڽ���������ٿ�����������ж�
 *********************************************************************************************/
uint8 ForkIdentify(int startline,int endline,int *LeftLine,int *RightLine,Point *InflectionL,Point *InflectionR,Point *InflectionC)
{
    GetDownInflection(startline, endline, LeftLine, RightLine, InflectionL, InflectionR);//��ȡ���ҹյ�
    if(InflectionL->X!=0 && InflectionR->X!=0 && LeftLine[InflectionL->Y-5]!=0)//�����ҹյ����,�����ҹյ㲻��̫����ֶ������
    {
        GetForkUpInflection(*InflectionL, *InflectionR, InflectionC);//ȥ�����Ϲյ�
        if(InflectionC->X!=0)
        {
            //����������Ϲյ㻹���������������жϣ����粻����������ʲô�ģ����Ҫ��������ж���
            //�������������̫���˻�������У������ʮ�ֳ�����ʱ��Ϳ��ܻ�����ͬ���ľ���������
            //�����������Ԫ�ػ����ԭ��,��һ����ʮ�ֵı�־
            return 1;//�����յ�����������
            /*����Ҫд��������ѡ���ʶ����ѡ���������ұߴӶ����в���*/
        }
    }
    return 0;
}

/*********************************************************************************
 ** ��������: ���������¹յ���Ѱ��ʮ��·�ڵ������Ϲյ�
 ** ��    ��: Point InflectionL: ��߹յ�
 **           Point InflectionR: �ұ߹յ�
 **           Point *UpInflectionC: ����Ϲյ�
 **           Point *UpInflectionC: �ұ��Ϲյ�
 ** �� �� ֵ: ��
 ** ˵    ��: ��
 ** ��    ��: LJF
 **********************************************************************************/
void GetCrossRoadsUpInflection(int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR,Point *UpInflectionL,Point *UpInflectionR)
{
    int row=0;//��ʼ��
    UpInflectionL->X=DownInflectionL.X+5;UpInflectionL->Y=0;//���Ϲյ�����
    UpInflectionR->X=DownInflectionR.X-5;UpInflectionR->Y=0;//���Ϲյ�����

    for(row=DownInflectionL.Y;row>0;row--)
    {
        if(BinaryImage[row][UpInflectionL->X]==IMAGE_WHITE && BinaryImage[row-1][UpInflectionL->X]==IMAGE_BLACK)  //�ɰ׵�������
        {
            //��¼�Ϲյ�
            UpInflectionL->Y=row-1;
            FillingLine(DownInflectionL,*UpInflectionL);                        //���ߴ���
            break;//��¼��֮����˳�ѭ��
        }
    }

    for(row=DownInflectionR.Y;row>0;row--)
    {
        if(BinaryImage[row][UpInflectionR->X]==IMAGE_WHITE && BinaryImage[row-1][UpInflectionR->X]==IMAGE_BLACK)  //�ɰ׵�������
        {
            //��¼�Ϲյ�
            UpInflectionR->Y=row-1;
            FillingLine(DownInflectionR,*UpInflectionR);                        //���ߴ���
            break;//��¼��֮����˳�ѭ��
        }
    }
}
