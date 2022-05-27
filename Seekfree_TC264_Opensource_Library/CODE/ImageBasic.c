#include "ImageBasic.h"
#include <math.h>
#include <stdlib.h>

//变量定义
int LeftLine[MT9V03X_H]={0}, CentreLine[MT9V03X_H]={0}, RightLine[MT9V03X_H]={0};   //扫线处理左中右三线
int Mid=MT9V03X_W/2;                        //初始化扫线的中点为图像中点
int Lost_Row=0;                             //中线丢失的行坐标(扫线到赛道外)
int LostNum_LeftLine=0,LostNum_RightLine=0; //记录左右边界丢线数

/*
 ** 函数功能: 扫线提取左中右三线的坐标(坐标在数组中的位置代表其行坐标,坐标在数组中的位置代表其列坐标)
 ** 参    数: int *LeftLine：     左线数组
 **           int *CentreLine： 中线数组
 **           int *RightLine：   右线数组
 ** 返 回 值: 无
 ** 作    者: LJF
 */

void GetImagBasic(int *LeftLine, int *CentreLine, int *RightLine)
{
    int row,cloum;              //行,列
    uint8 flag_l=0,flag_r=0;    //记录是否丢线flag，flag=0：丢线
    uint8 num=0;                //记录中线连续丢失的次数
    LostNum_LeftLine=0;LostNum_RightLine=0; //把丢线的计数变量清零
    //开始扫线(从下往上,从中间往两边),为了扫线的严谨性,我们做BORDER_BIAS的误差处理，即扫线范围会小于图像大小
    for(row=MT9V03X_H-1;row>0;row--) //图像的原点在左上角
    {
        //下面这个if是为了解决扫线一开始就在赛道外的问题，帮助重新找回赛道
        if(BinaryImage[row][Mid]==IMAGE_BLACK)  //扫线中点是黑色的（中点在赛道外）
        {
            num++;    //中线丢失次数+1
            //先向左边扫线，寻找右边界点
            for(cloum=Mid;(cloum-BORDER_BIAS)>0;cloum--)    //向左边扫线
            {
                if(BinaryImage[row][cloum]==IMAGE_WHITE && BinaryImage[row][cloum-BORDER_BIAS]==IMAGE_WHITE)  //判断右边界点（从赛道外扫到赛道内应该是黑变白的过程）
                {
                    RightLine[row]=cloum;    //记录右边界点（向左找到的是右边界点）
                    flag_r=1;               //flag做无丢线标记
                    break;
                }
            }
            //根据上面扫线的结果判断丢失的赛道是在左边还是右边从而决定继续向哪边扫线
            if(flag_r==1)   //向左扫线找到了右边界没有丢线（丢失的赛道在左边）
            {
                for(;(cloum-BORDER_BIAS)>0;cloum--)    //继续向左边扫线找到真正的左边界
                {
                    if(BinaryImage[row][cloum]==IMAGE_BLACK && BinaryImage[row][cloum+BORDER_BIAS]==IMAGE_BLACK)    //判断左边界点（正常情况由白变黑）
                    {
                        LeftLine[row]=cloum;   //记录左边界点
                        flag_l=1;              //flag做无丢线标记
                        break;
                    }
                }
            }
            //向左扫线没有找到右边界点，那么向右扫线寻找左边界点
            else
            {
                for(cloum=Mid;(cloum+BORDER_BIAS)<MT9V03X_W;cloum++)    //向右边扫线
                {
                    if(BinaryImage[row][cloum]==IMAGE_WHITE && BinaryImage[row][cloum+BORDER_BIAS]==IMAGE_WHITE)  //判断左边界点
                    {
                        LeftLine[row]=cloum;   //记录左边界点（向右找到的是左边界点）
                        flag_l=1;              //flag做无丢线标记
                        break;
                    }
                }
                if(flag_l==1)   //向右扫线找到了左边界没有丢线（丢失的赛道在右边）
                {
                    for(;(cloum+BORDER_BIAS)<MT9V03X_W;cloum++)    //继续向右边扫线，寻找右边界点
                    {
                        if(BinaryImage[row][cloum]==IMAGE_BLACK && BinaryImage[row][cloum+BORDER_BIAS]==IMAGE_BLACK)  //判断右边界点
                        {
                            RightLine[row]=cloum;   //记录右边界点
                            flag_r=1;               //flag做无丢线标记
                            break;
                        }
                    }
                }
            }
        }
        //在赛道中上，正常扫线
        else
        {
            num=0;  //中线丢失次数=0
            //左边扫线
            for(cloum=Mid;(cloum-BORDER_BIAS)>0;cloum--)
            {
                if(BinaryImage[row][cloum]==IMAGE_BLACK && BinaryImage[row][cloum-BORDER_BIAS]==IMAGE_BLACK)  //判断左边界点（BORDER_BIAS防偶然因素）
                {
                    LeftLine[row]=cloum;    //记录左边界点
                    flag_l=1;               //flag做无丢线标记
                    break;
                }
            }
            //右边扫线
            for(cloum=Mid;(cloum+BORDER_BIAS)<MT9V03X_W;cloum++)
            {
                if(BinaryImage[row][cloum]==IMAGE_BLACK && BinaryImage[row][cloum+BORDER_BIAS]==IMAGE_BLACK)  //判断右边界点（BORDER_BIAS防偶然因素）
                {
                    RightLine[row]=cloum;   //记录右边界点
                    flag_r=1;               //flag做无丢线标记
                    break;
                }
            }
        }
        //1.29晚上重新写的数据处理
        if(flag_l==0)   //左边界丢线
        {
            LeftLine[row]=0;            //左边界点等于图像的左边界
            LostNum_LeftLine++;         //左丢线数+1
        }
        if(flag_r==0)   //右边界丢线
        {
            RightLine[row]=MT9V03X_W-1;   //右边界点等于图像的右边界
            LostNum_RightLine++;        //右丢线数+1
        }
        CentreLine[row]=(LeftLine[row]+RightLine[row])/2;   //记录中线点

        Mid=CentreLine[row];    //以上一次的中线值为下一次扫线的中间点，若上一次的中线值刚好在边缘上，下一次的扫线会出现中线全跑到中线的情况
        flag_l=0;               //左边界丢线flag置0
        flag_r=0;               //右边界丢线flag置0
        //在这里加一个扫线到赛道外的判断，前30行不做处理，只保留该行坐标而不做特殊处理
//        if(row<90&&num!=0)  //这一行的起点是在赛道外
//        {
//            if(abs(CentreLine[row]-CentreLine[row+1])>60)  //上下两个中线点偏差过大
//            {
//                Lost_Row=row;
//            }
//        }
//        if(row<90&&num>=5)   //中线连续丢失5次则判定为扫线已经到了赛道外，终止扫线
//        {
//            Lost_Row=row;
//        }
    }
}

/*
 ** 函数功能: 根据左右边界线来得到下拐点（十字、三岔、环岛的判断会用上）
 ** 参    数: int starline:     起始行
 **           int endline:      结束行
 **           int *LeftLine：     左线数组
 **           int *RightLine：   右线数组
 **           Point *InflectionL: 左边拐点
 **           Point *InflectionR: 右边拐点
 ** 返 回 值: 无
 ** 说    明: - 用指针带入进来函数，最后得到的点可以两点确定直线进行补线
 **           - 由于图像原点在左上角，所以起始行是大于结束行，左右线数组从下往上遍历
 ** 作    者: LJF
 */
void GetDownInflection(int startline,int endline,int *LeftLine,int *RightLine,Point *InflectionL,Point *InflectionR)
{
    int i=0;
    InflectionL->X=0;InflectionL->Y=0;InflectionR->X=0;InflectionR->Y=0;//左右拐点置零

    for(i=startline;i>endline;i--)
    {
        //遍历左线，求出先变小大后变小的点，多加三个点的判断是为了误判，左边丢线为0
        /*注意：这里如果判断条件是和相邻的1,3的值对比的话，会区间太小从而如果有相等的列数存在的话，会影响判断，所以需要改大比较的区间*/
        //加了个判断InflectionL->Y==0是为了从下往上遍历，找到了之后就不再继续往上找了，这样遍历时候的截距图片就不用刚刚好了
        //2022年5月27日：加多一层条件，即：比>=相邻的点，随后再取筛选它是不是直线：大于比较远的点
        if(InflectionL->Y==0 && LeftLine[i]>=LeftLine[i-1] && LeftLine[i]>=LeftLine[i-3] && LeftLine[i]>=LeftLine[i+1] && LeftLine[i]>=LeftLine[i+3])
        {
            if(LeftLine[i]>LeftLine[i-5] && LeftLine[i]>LeftLine[i-7] && LeftLine[i]>LeftLine[i+5] && LeftLine[i]>LeftLine[i+7])
            {
                InflectionL->X=LeftLine[i];//存入拐点的（x,y）坐标
                InflectionL->Y=i;
                /*debug*/
//                lcd_showint32(0,6,InflectionL->X,3);
//                systick_delay_ms(STM0, 1000);
            }
        }

        //遍历右线，求出列数最小的点就是右边的拐点，右边线丢线为MT9V03X_W-1
        //加了个判断InflectionR->Y==0是为了从下往上遍历，找到了之后就不再继续往上找了，这样遍历时候的截距图片就不用刚刚好了
        if(InflectionR->Y==0 && RightLine[i]<=RightLine[i-1] && RightLine[i]<=RightLine[i-3] && RightLine[i]<=RightLine[i+1] && RightLine[i]<=RightLine[i+3])
        {
            if(RightLine[i]<RightLine[i-5] && RightLine[i]<RightLine[i-7] && RightLine[i]<RightLine[i+5] && RightLine[i]<RightLine[i+7])
            {
                InflectionR->X=RightLine[i];//存入拐点的（x,y）坐标
                InflectionR->Y=i;
               /*打印被判断为拐点的列坐标，用于调试*/
//                lcd_showint32(TFT_X_MAX-50,6,InflectionR->X,3);
//                systick_delay_ms(STM0, 1000);
            }
        }

        /*打印被判断为拐点的列坐标，用于调试*/
//        lcd_drawpoint(RightLine[i],i,RED);
//        lcd_showint32(0,0,LeftLine[i],3);
//        systick_delay_ms(STM0, 800);
    }
}

/*---------------------------------------------------------------
 【函    数】Bin_Image_Filter
 【功    能】过滤噪点
 【参    数】无
 【返 回 值】无
 【注意事项】
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
 ** 函数功能: 八领域边缘扫线
 ** 参    数: 无
 ** 返 回 值: 无
 ** 说    明: 使用了三线数组全局变量
 ** 作    者: LJF
 ***********************************************************************/
void EdgeTrack(void)
{
    int num=0,row=MT9V03X_H,cloum=MT9V03X_W/2;//八邻域的标号，行，列
    uint8 left_seed=0,right_seed=0;//左右两边种子是否被找到的标志变量
    Point seed;//存放种子的点
    for(row=MT9V03X_H;row>0;row--)//行从下往上
    {
        //如果种子不存在就要播种
        if(left_seed==0)
        {
            for(cloum=MT9V03X_W/2;cloum>0;cloum--)//列从中间往左，找到左边种子
            {
                if(BinaryImage[row][cloum]==IMAGE_BLACK)
                {
                    LeftLine[row]=cloum;//记录左边界的行列位置
                    seed.X=cloum;seed.Y=row;//种子标记
                    left_seed=1;//标记左边的种子已经被找到
                    break;//跳出循环
                }
                else
                    left_seed=0;//给左边种子一个置零的机会，防止左边播种到了右边没有播种到，然后重新播种时出现误判
            }
        }
        //遍历八域
        for(num=0;num<5;num++)
        {
            switch(num)
            {
            case 0:
                if(BinaryImage[seed.Y][seed.X+1]==IMAGE_BLACK)
                {
                    left_seed=0;//种子消失
                    num=5;//为了跳出for循环
                }
                break;
            case 1:
                if(BinaryImage[seed.Y-1][seed.X+1]==IMAGE_BLACK)
                {
                    seed.X+=1;seed.Y-=1;//找到了下一个黑点以下一个黑点为八领域中心
                    row--;
                    LeftLine[row]=seed.X;//把找到的左边界存入左线数组中
                    num=5;//为了跳出for循环
                }
                break;
            case 2:
               if(BinaryImage[seed.Y-1][seed.X]==IMAGE_BLACK)
               {
                   seed.Y-=1;//找到了下一个黑点以下一个黑点为八领域中心
                   row--;
                   LeftLine[row]=seed.X;//把找到的左边界存入左线数组中
                   num=5;//为了跳出for循环
               }
               break;
            case 3:
                if(BinaryImage[seed.Y-1][seed.X-1]==IMAGE_BLACK)
                 {
                    seed.X-=1;seed.Y-=1;//找到了下一个黑点以下一个黑点为八领域中心
                    row--;
                    LeftLine[row]=seed.X;//把找到的左边界存入左线数组中
                    num=5;//为了跳出for循环
                 }
                 break;
            case 4:
                if(BinaryImage[seed.Y][seed.X-1]==IMAGE_BLACK)
                 {
                    left_seed=0;//种子消失
                    num=5;//为了跳出for循环
                 }
                 break;
            //虽然是八领域但是出现了拐点之后就不扫了，说明此处是拐点，需要重新播种下一行
            default:break;
            }
        }
    }
}
