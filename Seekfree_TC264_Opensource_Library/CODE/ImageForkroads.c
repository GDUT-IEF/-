/*
 * Forkroads.c
 * Created on: 2022��5��25��
 * Author: 30516
 * Effect: �������·����ص�Դ����
 * �����������ٶ�Ϊ260״̬��+-15���ӵ����ͬ���������250�Ƚ���
 */

#include "ImageSpecial.h"
#include "PID.h"
#include <stdlib.h> //abs������fabs��math.h
#include "LED.h"
#include "zf_gpio.h"
#include "Motor.h"//���������
#include "ICM20602.h"//�����ǲ�Ƕ�

#define L_FINDWHIDE_THRE  10 //Y�յ��м�����߰�ɫ����ֹͣ����ֵ
#define R_FINDWHIDE_THRE  150//Y�յ��м����ұ߰�ɫ����ֹͣ����ֵ
#define ROW_FINDWHIDE_THRE 110//Y�յ��е����޵���ֵ
#define CLOUMN_FINDWHIDE_THRE   10//������Ѱ�Ұ�ɫ����Ŀ�ȣ�����Ҳ����������������
#define SEED_R_TRANSVERSE_GROW_THRE    5//��������������������ߺ�����������ֵ��Χ��������Ĭ�Ϻ��������ĵ�һ��Ϊ�յ�
#define SEED_L_TRANSVERSE_GROW_THRE    10//����������������ֵ
#define FORK_INFLECTION_WIDTH  110//������debug,���յ���60�и������ҹյ�Ĳ�ֵ����ȫ��ʱ������û�ж�ʧ�Ĺյ��������һ���򵥵ı�����ϵ������ѭ��˼·��
#define FORK_DEBUG  0
#define FORK_LED_DEBUG  0   //״̬����LEDdebug

extern uint8 bias_startline,bias_endline;        //��̬ǰհ

/*********************************************************************************
 ** ��������: Ѱ����������϶��㣬��ֹ��Ϊɨ��ɨ����������һ��ʹ��Bias̫С
 ** ��    ��: uint8 row,uint8 cloumn:ȥ��Ѱ���ⲹ�������ϵ�Ļ�׼������
 **           Point *LeftUpPoint:��ߵ������
 ** �� �� ֵ: ��
 ** ˵    ��: �����������֮��Ӧ���ٲ�һ����ߵĴ�ֱ�ߺ��ұߴ��Ϲյ㲹�����Ͻǹյ�
 ** ��    ��: LJF
 **********************************************************************************/
typedef struct SeedGrowAqueue
{
    uint8 front;//��ͷ
    uint8 rear;//��β
}SeedGrowAqueue;//�ҵ��յ�֮���ٴΰ�����ɨ�ߵ����������Ķ���
uint8 const l_data[8] = {0,1,2,3,4,5,6,7};//�����ӱ�Ŷ���������
uint8 const r_data[8] = {4,3,2,1,0,7,6,5};//�����ӱ�Ŷ���������
void ForkSearchLineAgain(int *UpInflectionY,Point left_seed,Point right_seed)
{
    SeedGrowAqueue seed_queue;//���������ڱ��ת�Ƶ�ѭ������
    seed_queue.front = 0; seed_queue.rear = 7;
    //����������:�����������Ǵ�����Χ�˸������ǰ�ɫ
    int left_seed_grow_starcolumn=left_seed.X-1;
    for(;left_seed.X>left_seed_grow_starcolumn && left_seed.Y>bias_endline-1 && left_seed.Y<*UpInflectionY+1 && (seed_queue.front + 1) % 8 != seed_queue.rear;)
    {
        uint8 break_for_flag = 0;//�����ж��Ƿ��ҵ���ɫ������Ӷ���������Ų鿴����
        //�ж������Ƕ��߻����Ѿ�����ǰ�ߵĴ�Ƭ��ɫ����
        if (left_seed.X + 1 >= MT9V03X_W && BinaryImage[left_seed.Y][left_seed.X] == IMAGE_BLACK)//�����ӳ������ұߣ����һ��Ǻ�ɫ˵����һ���Ѿ�������ȫ��,��������
        {
            LeftLine[left_seed.Y] = 0;
            left_seed.Y--;
        }
        else if (left_seed.X - 1 <= 0 && (BinaryImage[left_seed.Y][left_seed.X] == IMAGE_WHITE|| BinaryImage[left_seed.Y-1][left_seed.X] == IMAGE_WHITE))//������һֱ����߽߱磬�����ǰ�ɫ��˵������������
        {
            LeftLine[left_seed.Y] = 0;
            left_seed.Y--;
        }
        else
        {
            for (; (seed_queue.front + 1) % 8 != seed_queue.rear; seed_queue.front++)
            {
                switch (l_data[seed_queue.front])
                {
                case 0:
                    if (BinaryImage[left_seed.Y][left_seed.X + 1] == IMAGE_BLACK)
                    {
                        left_seed.X++;
                        break_for_flag = 1;
                    }
                    break;
                case 1:
                    if (BinaryImage[left_seed.Y - 1][left_seed.X + 1] == IMAGE_BLACK)
                    {
                        left_seed.X++; left_seed.Y--;
                        break_for_flag = 1;
                    }
                    break;
                case 2:
                    if (BinaryImage[left_seed.Y - 1][left_seed.X] == IMAGE_BLACK)
                    {
                        left_seed.Y--;
                        break_for_flag = 1;
                    }
                    break;
                case 3:
                    if (BinaryImage[left_seed.Y - 1][left_seed.X - 1] == IMAGE_BLACK)
                    {
                        left_seed.X--; left_seed.Y--;
                        break_for_flag = 1;
                    }
                    break;
                case 4:
                    if (BinaryImage[left_seed.Y][left_seed.X - 1] == IMAGE_BLACK)
                    {
                        left_seed.X--;
                        break_for_flag = 1;
                    }
                    break;
                case 5:
                    if (BinaryImage[left_seed.Y + 1][left_seed.X - 1] == IMAGE_BLACK)
                    {
                        left_seed.X--; left_seed.Y++;
                        break_for_flag = 1;
                    }
                    break;
                case 6:
                    if (BinaryImage[left_seed.Y + 1][left_seed.X] == IMAGE_BLACK)
                    {
                        left_seed.Y++;
                        break_for_flag = 1;
                    }
                    break;
                case 7:
                    if (BinaryImage[left_seed.Y + 1][left_seed.X + 1] == IMAGE_BLACK)
                    {
                        left_seed.X++; left_seed.Y++;
                        break_for_flag = 1;
                    }
                    break;
                default:
                    break;
                }
                //�������ж�һ��ǰ���switch��û���ҵ���ɫ����
                if (break_for_flag == 1)
                {
                    char temp = seed_queue.front;
                    if (temp - 2 < 0)//�ж��Ƿ�-2��С��0����Ļ�����һ�δ�0����
                    {
                        seed_queue.front = 0;
                    }
                    else
                    {
                        seed_queue.front = temp - 2;
                    }
                    seed_queue.rear = (seed_queue.front + 7) % 8;//���ö�ͷ
                    break;//������ε�8����Ѱ�ң�������һ������8��������
                }
            }
            //ͨ������ұ��Ƿ��ǰ�ɫ���ж����Ƿ��Ǳ߽�㣬����߽����ܻ��ں�ɫ���򣬵������ӻ�Ҫ�������ұ��ߣ�ȴû�б�������
            if (BinaryImage[left_seed.Y][left_seed.X + 1] == IMAGE_WHITE)
            {
                LeftLine[left_seed.Y] = left_seed.X;
                CentreLine[left_seed.Y]=(left_seed.X+RightLine[left_seed.Y])/2;
            }
        }
    }
    //����������
    seed_queue.front = 0; seed_queue.rear = 7;//���ö�ͷ�Ͷ�β��������������иı伴��
    int right_seed_grow_startline=right_seed.Y;
    for(;right_seed.Y>bias_endline-1 && right_seed.Y<right_seed_grow_startline+1 && (seed_queue.front+1)%8!=seed_queue.rear;)
    {
        uint8 break_for_flag = 0;//�����ж��Ƿ��ҵ���ɫ������Ӷ���������Ų鿴����
        //�ж������Ƕ��߻����Ѿ�����ǰ�ߵĴ�Ƭ��ɫ����
        if (right_seed.X - 1 <= 0 && BinaryImage[right_seed.Y][right_seed.X] == IMAGE_BLACK)//�����ӳ�������ߣ����һ��Ǻ�ɫ˵����һ���Ѿ�������ȫ��,��������
        {
            RightLine[right_seed.Y] = MT9V03X_W-1;
            right_seed.Y--;
        }
        else if (right_seed.X + 1 >= MT9V03X_W && (BinaryImage[right_seed.Y][right_seed.X] == IMAGE_WHITE || BinaryImage[right_seed.Y - 1][right_seed.X] == IMAGE_WHITE))//������һֱ���ұ߽߱磬�����ǰ�ɫ��˵������������
        {
            RightLine[right_seed.Y] = MT9V03X_W - 1;
            right_seed.Y--;
        }
        else
        {
            for (; (seed_queue.front + 1) % 8 != seed_queue.rear; seed_queue.front++)
            {
                switch (r_data[seed_queue.front])
                {
                case 0:
                    if (BinaryImage[right_seed.Y][right_seed.X + 1] == IMAGE_BLACK)
                    {
                        right_seed.X++;
                        break_for_flag = 1;
                    }
                    break;
                case 1:
                    if (BinaryImage[right_seed.Y - 1][right_seed.X + 1] == IMAGE_BLACK)
                    {
                        right_seed.X++; right_seed.Y--;
                        break_for_flag = 1;
                    }
                    break;
                case 2:
                    if (BinaryImage[right_seed.Y - 1][right_seed.X] == IMAGE_BLACK)
                    {
                        right_seed.Y--;
                        break_for_flag = 1;
                    }
                    break;
                case 3:
                    if (BinaryImage[right_seed.Y - 1][right_seed.X - 1] == IMAGE_BLACK)
                    {
                        right_seed.X--; right_seed.Y--;
                        break_for_flag = 1;
                    }
                    break;
                case 4:
                    if (BinaryImage[right_seed.Y][right_seed.X - 1] == IMAGE_BLACK)
                    {
                        right_seed.X--;
                        break_for_flag = 1;
                    }
                    break;
                case 5:
                    if (BinaryImage[right_seed.Y + 1][right_seed.X - 1] == IMAGE_BLACK)
                    {
                        right_seed.X--; right_seed.Y++;
                        break_for_flag = 1;
                    }
                    break;
                case 6:
                    if (BinaryImage[right_seed.Y + 1][right_seed.X] == IMAGE_BLACK)
                    {
                        right_seed.Y++;
                        break_for_flag = 1;
                    }
                    break;
                case 7:
                    if (BinaryImage[right_seed.Y + 1][right_seed.X + 1] == IMAGE_BLACK)
                    {
                        right_seed.X++; right_seed.Y++;
                        break_for_flag = 1;
                    }
                    break;
                default:
                    break;
                }
                //�������ж�һ��ǰ���switch��û���ҵ���ɫ����
                if (break_for_flag == 1)
                {
                    char temp = seed_queue.front;
                    if (temp - 2 < 0)//�ж��Ƿ�-2��С��0����Ļ�����һ�δ�0����
                    {
                        seed_queue.front = 0;
                    }
                    else
                    {
                        seed_queue.front = temp - 2;
                    }
                    seed_queue.rear = (seed_queue.front + 7) % 8;//���ö�ͷ
                    break;//������ε�8����Ѱ�ң�������һ������8��������
                }
            }
            //ͨ������ұ��Ƿ��ǰ�ɫ���ж����Ƿ��Ǳ߽�㣬����߽����ܻ��ں�ɫ���򣬵������ӻ�Ҫ�������ұ��ߣ�ȴû�б�������
            if (BinaryImage[right_seed.Y][right_seed.X - 1] == IMAGE_WHITE)
            {
                RightLine[right_seed.Y] = right_seed.X;
                CentreLine[right_seed.Y]=(LeftLine[right_seed.Y]+right_seed.X)/2;
            }
        }
    }
    //����ҵ��˽�����ͣ��������ô�����Ϲյ�Ľ����б���ѭ����ƫ��
//    LcdDrawRow(left_seed.Y, PURPLE);
//    lcd_showint32(TFT_X_MAX-50, 2, left_seed.Y, 3);
    if(left_seed.Y<*UpInflectionY) *UpInflectionY=left_seed.Y;
}
/*********************************************************************************
 ** ��������: �������������������ȵ�Ѱ��Y�Ϲյ�
 ** ��    ��:char Choose��ѡ�����ڹȵ���߻����ұ�
 **          Point Seed
 **          int endline
 **          Point *UpInflectionC
 ** �� �� ֵ: ��
 ** ˵    ��: ��Ϊѡ���������ұߵĻ���������ȵ�����һ���������׵����ԣ���߲���Ҫ��transeverse�����ƣ��ܸ��õ��ҵ��Ǹ�Y�յ�
 ** ��    ��: LJF
 **********************************************************************************/
void SeedGrowFindUpInflection(char Choose,Point Seed,int endline,Point *UpInflectionC)
{
    char transversenum=0;//��¼�����Ƿ�һֱ�����ƶ�,���Ӻ��������Ĵ���
    Point tempSeed;//��ʱ������
    for(;Seed.Y<endline && Seed.X<MT9V03X_W-1 && Seed.X>0;)
    {
#if FORK_DEBUG
        lcd_drawpoint(Seed.X, Seed.Y, GREEN);
#endif
        switch(Choose)
        {
            case 'L':
                if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_BLACK && BinaryImage[Seed.Y][Seed.X+1]==IMAGE_BLACK)
                {
                    Seed.Y++,Seed.X++;
                    transversenum=0;
                }
                else if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_BLACK && BinaryImage[Seed.Y][Seed.X+1]==IMAGE_WHITE)
                {
                    Seed.Y++;
                    transversenum=0;
                }
                else if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_WHITE && BinaryImage[Seed.Y][Seed.X+1]==IMAGE_BLACK)
                {
                    Seed.X++;
                    if(transversenum==0)//�ж��Ƿ��ǵ�һ��������
                    {
                        tempSeed=Seed;
                    }
                    transversenum++;//˵�������ұ���
                }
                else if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_WHITE && BinaryImage[Seed.Y][Seed.X+1]==IMAGE_WHITE)
                {
                    if(transversenum!=0)//˵��֮ǰһֱ�����������ҵ��˹ȵ�
                    {
                        UpInflectionC->Y=tempSeed.Y,UpInflectionC->X=tempSeed.X;
                    }
                    else
                    {
                        UpInflectionC->Y=Seed.Y,UpInflectionC->X=Seed.X;
                    }
                    return;
                }
                //�����Ӻ��������Ĵ�����������ֵ
                if(transversenum>SEED_L_TRANSVERSE_GROW_THRE)
                {
                    UpInflectionC->Y=tempSeed.Y,UpInflectionC->X=tempSeed.X;
                    return;
                }
                break;
            case 'R':
                if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_BLACK && BinaryImage[Seed.Y][Seed.X-1]==IMAGE_BLACK)
                {
                    Seed.Y++,Seed.X--;
                    transversenum=0;
                }
                else if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_BLACK && BinaryImage[Seed.Y][Seed.X-1]==IMAGE_WHITE)
                {
                    Seed.Y++;
                    transversenum=0;
                }
                else if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_WHITE && BinaryImage[Seed.Y][Seed.X-1]==IMAGE_BLACK)
                {
                    Seed.X--;
                    if(transversenum==0)//�ж��Ƿ��ǵ�һ��������
                    {
                        tempSeed=Seed;
                    }
                    transversenum++;;//˵�����������
                }
                else if(BinaryImage[Seed.Y+1][Seed.X]==IMAGE_WHITE && BinaryImage[Seed.Y][Seed.X-1]==IMAGE_WHITE)
                {
                    if(transversenum!=0)//˵��֮ǰһֱ�����������ҵ��˹ȵ�
                    {
                        UpInflectionC->Y=tempSeed.Y,UpInflectionC->X=tempSeed.X;
                    }
                    else
                    {
                        UpInflectionC->Y=Seed.Y,UpInflectionC->X=Seed.X;
                    }
                    return;
                }
                //�����Ӻ��������Ĵ�����������ֵ
                if(transversenum>SEED_R_TRANSVERSE_GROW_THRE)
                {
                    UpInflectionC->Y=tempSeed.Y,UpInflectionC->X=tempSeed.X;
                    return;
                }
                break;
            default:break;
        }
        if(Seed.X<0 || Seed.X>MT9V03X_W-1 || Seed.Y<0 || Seed.Y>MT9V03X_H-1)
        {
            break;//����ҵ��߳�������Ļ���˳������������
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
    int row=0, cloumnL=0, cloumnR=0, cloumnnum=0;//cloumnnum��¼�ӻ�׼��ƫ���˶����к�ɫ����
    Point Seed;
    char Choose=0,flagL=0,flagR=0;//�ж����ڹȵ���߻����ұߵĺ���,�Լ��ж�����������û�а�ɫ�����FLAG
    UpInflectionC->X = 0; UpInflectionC->Y = 0;//�Ϲյ�����
    UpInflectionC->X = (DownInflectionL.X + DownInflectionR.X) / 2;//V���Ϲյ��������Ϊ���ҹյ��ֵ
    //��ֹ��Ϊ����Ѱ�յ�ʹ�����
    if(UpInflectionC->X<0) UpInflectionC->X=0;
    if(UpInflectionC->X>MT9V03X_W-1) UpInflectionC->X=MT9V03X_W-1;
    row = (DownInflectionL.Y + DownInflectionR.Y) / 2;//��ʼ��Ϊ���ҹյ��еľ�ֵ
    if(row<0) row=0;
    if(row>MT9V03X_H-1) row=MT9V03X_H-1;//�����������ǿ����׳��
    //�ж�һ�ο�ʼ�ĵ��Ƿ�Ϊ�׵㣬���������ʼ�ĵ�Ϊ�ڵ㣬һֱ�Ұ����ڿ����ҵ�һЩ���ҵĵط�
    if(BinaryImage[row][UpInflectionC->X]==IMAGE_BLACK)
        return;//�����ʼ����Ǻ�ɫ����ôֱ������
    for (; row > 20; row--)
    {
#if FORK_DEBUG
        lcd_drawpoint(UpInflectionC->X, row, PURPLE);
#endif
        if (BinaryImage[row][UpInflectionC->X] == IMAGE_WHITE && BinaryImage[row - 1][UpInflectionC->X] == IMAGE_BLACK)
        {
            //���������Ұ�ɫ����
            for (cloumnL = UpInflectionC->X; cloumnL > L_FINDWHIDE_THRE; cloumnL--)
            {
#if FORK_DEBUG
                lcd_drawpoint(cloumnL, row-1, YELLOW);
#endif
                if(BinaryImage[row - 1][cloumnL] == IMAGE_BLACK)
                {
                    cloumnnum++;
                }
                if (BinaryImage[row - 1][cloumnL] == IMAGE_BLACK && BinaryImage[row - 1][cloumnL - 1] == IMAGE_WHITE && BinaryImage[row - 1][cloumnL - 3] == IMAGE_WHITE)
                {
                    cloumnnum=0;
                    flagL = 1;
                    break;
                }
                //����1��ѭ�������˻�û�ҵ���ɫ���򣬻�����2����ɫ����̫���˴�������ֵ���������ҵ���ɫ���򲹵���Ҳ����
                if (cloumnL == L_FINDWHIDE_THRE + 1 || cloumnnum>CLOUMN_FINDWHIDE_THRE)
                {
                    cloumnnum=0;
                    Choose = 'R';//����Ҳ���˵���ڹȵ��ұ�
                    break;
                }
            }
            for (cloumnR = UpInflectionC->X; cloumnR < R_FINDWHIDE_THRE && Choose!='R'; cloumnR++)
            {
#if FORK_DEBUG
                lcd_drawpoint(cloumnR, row-1, YELLOW);
#endif
                if(BinaryImage[row - 1][cloumnR] == IMAGE_BLACK)
                {
                    cloumnnum++;
                }
                if (BinaryImage[row - 1][cloumnR] == IMAGE_BLACK && BinaryImage[row - 1][cloumnR + 1] == IMAGE_WHITE && BinaryImage[row - 1][cloumnL + 3] == IMAGE_WHITE)
                {
                    cloumnnum=0;
                    flagR = 1;
                    break;
                }
                if (cloumnR == R_FINDWHIDE_THRE - 1 || cloumnnum>CLOUMN_FINDWHIDE_THRE)
                {
                    cloumnnum=0;
                    Choose = 'L';//�ұ��Ҳ���˵���ڹȵ����
                    break;
                }
            }
            break;
        }
    }
    if (flagL == 0 || flagR == 0)//˵����һ����û�а�ɫ�����
    {
        if(Choose!=0)
        {
            Seed.X = UpInflectionC->X, Seed.Y = row - 1;
#if FORK_DEBUG
            for(int j=0;j<MT9V03X_W-1;j++)//����100��������
            {
                lcd_drawpoint(j, ROW_FINDWHIDE_THRE, PURPLE);
            }
#endif
            SeedGrowFindUpInflection(Choose, Seed, ROW_FINDWHIDE_THRE, UpInflectionC);
            //�ڴ��ٴ���֤һ���������ҵ��Ĺյ��ǲ�����Ĺյ㣬��ֹ����
            for (cloumnL = UpInflectionC->X; cloumnL > L_FINDWHIDE_THRE; cloumnL--)
            {
                if (BinaryImage[UpInflectionC->Y][cloumnL] == IMAGE_WHITE && BinaryImage[UpInflectionC->Y][cloumnL-3]==IMAGE_WHITE)
                {
                    break;
                }
                if (cloumnL == L_FINDWHIDE_THRE + 1 )
                {
                    //���Ŷ����˶��Ǻ�ɫ����ô�������ߵ������濴һ���Ƿ��а�
                    if(BinaryImage[UpInflectionC->Y+1][cloumnL]==IMAGE_WHITE)//����һ����Ҳ�ǰ�ɫ��ô��Ҳ��������յ�
                        break;
                    else
                    {
                        UpInflectionC->Y=0;//����Ҳ�����ɫ���ж�Ϊ����
                        break;
                    }
                }
            }
            for (cloumnR = UpInflectionC->X; cloumnR < R_FINDWHIDE_THRE; cloumnR++)
            {
                if (BinaryImage[UpInflectionC->Y][cloumnR] == IMAGE_WHITE && BinaryImage[UpInflectionC->Y][cloumnR+3] == IMAGE_WHITE)
                {
                    break;
                }
                if (cloumnR == R_FINDWHIDE_THRE - 1)
                {
                    //���Ŷ����˶��Ǻ�ɫ����ô�������ߵ������濴һ���Ƿ��а�
                    if(BinaryImage[UpInflectionC->Y+1][cloumnR]==IMAGE_WHITE)//����һ����Ҳ�ǰ�ɫ��ô��Ҳ��������յ�
                        break;
                    else
                    {
                        UpInflectionC->Y=0;//����Ҳ�����ɫ���ж�Ϊ����
                        break;
                    }
                }
            }
        }
    }
    else
    {
        UpInflectionC->Y = row - 1;
    }
}

/********************************************************************************************
 ** ��������: ʶ������
 ** ��    ��:
 **           int *LeftLine������
 **           int *RightLine:����
 **           Point *InflectionL:��߹յ�
 **           Point *InflectionR:�ұ߹յ�
 ** �� �� ֵ:  0��û��ʶ������
 **           1��ʶ������
 ** ��    ��: LJF
 ** ע    �⣺���ұ߽�������
 *********************************************************************************************/
uint8 ForkTurnRIdentify(int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR)
{
    Point UpInflectionC;//�Ϲյ㣬��߹յ㣬����϶��㣬������
    UpInflectionC.X=0;UpInflectionC.Y=0;
    //�����ҹյ����,���������յ�Ҫ��ͼ���°벿��
    if(DownInflectionL.X!=0 && DownInflectionR.X!=0 && DownInflectionL.X<120 && DownInflectionR.X>30)
    {
        //ȡ��������ҹյ��������жϣ�������������
        if(abs((DownInflectionL.Y-DownInflectionR.Y))<40)//���������յ������С��30���Ž����ж�
        {
            GetForkUpInflection(DownInflectionL, DownInflectionR, &UpInflectionC);//ȥ�����Ϲյ�
//            gpio_toggle(LED_BLUE);
        }
    }
    else if((DownInflectionL.X==0 && DownInflectionR.X==0) || (BinaryImage[MT9V03X_H-5][5]==IMAGE_WHITE && BinaryImage[MT9V03X_H-5][MT9V03X_W-5]==IMAGE_WHITE))//��������¹յ㲻���ڲ�������һ�γ��־Ͷ��ߵĻ��Ļ�,���Ǿ�ȥ���治�������ϵĹյ�
    {
        Point ImageDownPointL,ImageDownPointR;//�Ի�������½Ǻ����½���Ϊ���Ҳ��ߵĵ�
        //���Ϲյ����һ��Ԥ��ĵ㣬������д��Ϊ��Ļ�м�
        if(LeftLine[MT9V03X_H-1]!=0)//����������һ��û�ж���
            ImageDownPointL.X=LeftLine[MT9V03X_H-1]+10,ImageDownPointL.Y=MT9V03X_H-1;
        else ImageDownPointL.X=0,ImageDownPointL.Y=MT9V03X_H-1;
        if(RightLine[MT9V03X_H-1]!=MT9V03X_W-1)
            ImageDownPointR.X=RightLine[MT9V03X_H-1]-10,ImageDownPointR.Y=MT9V03X_H-1;
        else ImageDownPointR.X=MT9V03X_W-1,ImageDownPointR.Y=MT9V03X_H-1;
        //��Ѱ�Ϲյ�
        DownInflectionL=ImageDownPointL;
        GetForkUpInflection(ImageDownPointL, ImageDownPointR, &UpInflectionC);
        if(UpInflectionC.Y<40) UpInflectionC.Y=0;
//        gpio_toggle(LED_GREEN);
    }
    //�ұ߶��߳���60����յ���ڣ�������յ㲻�����ϰ�ƽ����ֹ����
    else if(LostNum_RightLine>55 && DownInflectionL.X>0 && DownInflectionL.X<90 && DownInflectionL.Y>40)
    {
        Point ImageDownPointR;//����յ�ԳƵĵ�ȥ���ߺ��ҹյ�
        //���Լ��趨���ҹյ�ȥ���Ϲյ�
//        ImageDownPointR.X=MT9V03X_W-1,ImageDownPointR.YDownInflectionL.Y=DownInflectionL.Y;
        //���õ���ѭ������˼����յ㣬������ȣ���б���Ϲյ��ұߵĶ�����һ��
        ImageDownPointR.X=DownInflectionL.X+(145-(119-DownInflectionL.Y)*1.1);ImageDownPointR.Y=DownInflectionL.Y;
        GetForkUpInflection(DownInflectionL, ImageDownPointR, &UpInflectionC);
        DownInflectionR=ImageDownPointR;
//        gpio_toggle(LED_RED);
    }
    //��߶��߳���60,�ҹյ����,�����ҹյ㲻�����ϰ�ƽ����ֹ����
    else if(LostNum_LeftLine>55 && DownInflectionR.X>60 && DownInflectionR.Y>40)
    {
        Point ImageDownPointL;//����յ�ԳƵĵ�ȥ���ߺ��ҹյ�
        //��յ�������һ��������ϵ��Խ�����ײ��˹յ���Խ��.��б������һ��
//        ImageDownPointL.X=DownInflectionR.X-(145-(119-DownInflectionL.Y)*1.1);ImageDownPointL.Y=DownInflectionR.Y;
        ImageDownPointL.X=DownInflectionR.X-FORK_INFLECTION_WIDTH;ImageDownPointL.Y=DownInflectionR.Y;
        GetForkUpInflection(ImageDownPointL, DownInflectionR, &UpInflectionC);
        //��б��ʱ����ʵ��յ����������Ļ�����½ǣ��������ڲ��ߵ�ʱ�������½�Ϊ���ߵ�
        ImageDownPointL.X=1;ImageDownPointL.Y=MT9V03X_H-1;
        DownInflectionL=ImageDownPointL;
//        gpio_toggle(LED_WHITE);
    }
    if(UpInflectionC.Y!=0)//ֱ�ӷ���Y���ɣ��ӿ��ٶȣ���ΪXĬ�Ͼͻḳֵ��
    {
        //����
        FillingLine('L',DownInflectionL,UpInflectionC);//��������˾��ڷ���֮ǰ����
        if(CentreLine[UpInflectionC.Y]>CentreLine[UpInflectionC.Y-3])//���ɨ��ɨ���˺�·���滮�෴��������ɨһ����
        {
            Point right_seed;
            right_seed.X=0;right_seed.Y=0;
            for(int column=UpInflectionC.X;column<MT9V03X_W-1;column++)
            {
                if(BinaryImage[column][UpInflectionC.Y]==IMAGE_WHITE&&BinaryImage[column+1][UpInflectionC.Y]==IMAGE_BLACK)
                {
                    right_seed.X=column+1;right_seed.Y=UpInflectionC.Y;
                }
                else if(column==MT9V03X_W-2)
                {
                    right_seed.X=MT9V03X_W-1;right_seed.Y=UpInflectionC.Y;
                }
            }
            ForkSearchLineAgain(&(UpInflectionC.Y),UpInflectionC,right_seed);
        }
        //ƫ���
//        if(UpInflectionC.Y<bias_endline)//starline<endline<Up.y��������ѭ��
//        {
//            ;
//        }
//        else if(UpInflectionC.Y<bias_startline && bias_endline<UpInflectionC.Y)//starline<UP.y<endline,������ʼ�е��Ϲյ�
//        {
//            bias_endline=UpInflectionC.Y;
//        }
//        else if(bias_startline<UpInflectionC.Y)//UP.y<starline<endline
//        {
//            bias_startline=DownInflectionL.Y;bias_endline=UpInflectionC.Y;
//        }
        return 1;
    }
    else
        return 0;
}
/********************************************************************************************
 ** ��������: ����״̬��ת�жϺ���
 ** ��    ��: Point InflectionL�����¹յ�
 **           Point InflectionR�����¹յ�
 ** �� �� ֵ:  0������δ����
 **           1�������ѽ���
 ** ��    ��: LJF
 *********************************************************************************************/
uint8 ForkFStatusIdentify(Point DownInflectionL,Point DownInflectionR,uint8 *ForkFlag)
{
    static uint8 StatusChange;//״̬ת�Ʊ���������֡���ӳ�
    uint8 NowFlag=0;//��ε�ʶ����
    NowFlag=ForkTurnRIdentify(LeftLine, RightLine, DownInflectionL, DownInflectionR);
    *ForkFlag=NowFlag;//��ʶ�����ͳ�ȥ
    //״̬����ʼ����
    switch(StatusChange)
    {
        //���״̬
        case 0:
        {
#if FORK_LED_DEBUG
            gpio_set(LED_RED, 0);
#endif
            if(NowFlag==1)
            {
#if FORK_LED_DEBUG
                gpio_set(LED_RED, 1);
#endif
                EncoderDistance(1, 1.7, 0, 0);//������Ϊ���л������������һ֡û�жϵ�����״̬����
                StatusChange=1;//ֻҪ��ʼʶ���������˵���Ѿ�����ڽ׶���
            }
            break;
        }
        //�������״̬
        case 1:
        {
#if FORK_LED_DEBUG
            gpio_set(LED_YELLOW, 0);
#endif
            if(NowFlag==0)
            {
#if FORK_LED_DEBUG
                gpio_set(LED_YELLOW, 1);
#endif
                base_speed+=15;
                StatusChange=2;
            }
            break;
        }
        //��;״̬
        case 2:
        {
#if FORK_LED_DEBUG
            gpio_set(LED_BLUE, 0);
#endif
            if(NowFlag==1)
            {
                if(encoder_dis_flag!=1)//���û���
                {
#if FORK_LED_DEBUG
                    gpio_set(LED_BLUE, 1);
#endif
                    base_speed-=15;//��Ϊ��û����������ٶȽ���ȥ
                    EncoderDistance(1, 1.7, 0, 0);//������Ϊ���л������������һ֡û�жϵ�����״̬����
                    StatusChange=1;//�����û�в�����Ҿ���Ϊ����һ�ε�ʶ������Ϊ���У��򷵻��������״̬
                }
                else
                {
#if FORK_LED_DEBUG
                    gpio_set(LED_BLUE, 1);
#endif
                    base_speed-=15;
                    StartIntegralAngle_Z(30);//�����ǿ�������׼��������
                    StatusChange=3;
                }
            }
            break;
        }
        //����
        case 3:
        {
#if FORK_LED_DEBUG
            gpio_set(LED_WHITE, 0);
#endif
            if(icm_angle_z_flag==1)
            {
#if FORK_LED_DEBUG
                gpio_set(LED_WHITE, 1);
#endif
                StatusChange=4;
            }
            break;
        }
        case 4:
        {
            if(NowFlag==0)
            {
                StatusChange=0;//����״̬����־����ʹ��״̬�����ٴ�ʹ��
                return 1;
            }
            break;
        }
        default:break;
    }
    return 0;
}

/********************************************************************************************
 ** ��������: ����״̬��ת�жϺ���
 ** ��    ��: Point InflectionL�����¹յ�
 **           Point InflectionR�����¹յ�
 ** �� �� ֵ:  0������δ����
 **           1�������ѽ���
 ** ��    ��: LJF
 *********************************************************************************************/
uint8 ForkSStatusIdentify(Point DownInflectionL,Point DownInflectionR,uint8 *ForkFlag)
{
    static uint8 StatusChange,numentrance,fork_encooder_flag,last_speed;//����ʶ��������ʱ״̬������������״̬�Ƿ���ת
    uint8 NowFlag=0;//��ε�ʶ����
    NowFlag=ForkTurnRIdentify(LeftLine, RightLine, DownInflectionL, DownInflectionR);
    *ForkFlag=NowFlag;//��ʶ�����ͳ�ȥ
    //״̬����ʼ����
    switch(StatusChange)
    {
        //���״̬
        case 0:
        {
#if FORK_LED_DEBUG
            gpio_set(LED_RED, 0);
#endif
            if(NowFlag==1)
            {
#if FORK_LED_DEBUG
                gpio_set(LED_RED, 1);
#endif
                last_speed=base_speed;
                StatusChange=1;//ֻҪ��ʼʶ���������˵���Ѿ�����ڽ׶���
            }
            break;
        }
        //��ڵ��µ�����״̬
        case 1:
        {
#if FORK_LED_DEBUG
            gpio_set(LED_YELLOW, 0);
#endif
            if(numentrance<10)
            {
                numentrance++;
                break;
            }
            if(NowFlag==0)
            {
#if FORK_LED_DEBUG
                gpio_set(LED_YELLOW, 1);
#endif
                EncoderDistance(1, 0.7, 0, 0);//�������,������¼���
                StatusChange=2;
            }
            break;
        }
        //�µ���;״̬
        case 2:
        {
#if FORK_LED_DEBUG
            gpio_set(LED_WHITE, 0);
#endif
            if(encoder_dis_flag==1)
            {
#if FORK_LED_DEBUG
                gpio_set(LED_WHITE, 1);
#endif
                base_speed=150;
                EncoderDistance(1, 1.2, 0, 0);//�������,�µ�
                StatusChange=3;
            }
            break;
        }
        //���������״̬
        case 3:
        {
#if FORK_LED_DEBUG
            gpio_set(LED_BLUE, 0);
#endif
            if(encoder_dis_flag==1)
            {
#if FORK_LED_DEBUG
                gpio_set(LED_BLUE, 1);
#endif
                base_speed=last_speed;//�ָ��ٶ�
                StatusChange=4;
            }
            break;
        }
        //�������״̬
        case 4:
        {
#if FORK_LED_DEBUG
            gpio_set(LED_GREEN, 0);
#endif
            if(NowFlag==1)
            {
#if FORK_LED_DEBUG
            gpio_set(LED_GREEN, 1);
#endif
                StartIntegralAngle_Z(30);//�����ǿ�������׼��������
                StatusChange=5;
            }
            break;
        }
        case 5:
        {
#if FORK_LED_DEBUG
            gpio_set(LED_RED, 0);
#endif
            if(icm_angle_z_flag==1)
            {
#if FORK_LED_DEBUG
            gpio_set(LED_RED, 1);
#endif
                StatusChange=6;
            }
            break;
        }
        case 6:
        {
#if FORK_LED_DEBUG
            gpio_set(LED_YELLOW, 0);
#endif
            if(NowFlag==0)
            {
#if FORK_LED_DEBUG
            gpio_set(LED_YELLOW, 1);
#endif
                return 1;
            }
            break;
        }
        default:break;
    }
    return 0;
}

