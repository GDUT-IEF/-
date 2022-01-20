/*
 * ImageSpecial.c
  *  赛道上各种特殊元素的识别
  *  该文件的函数只实现识别是否有该元素的出现以及出现的位置，至于该元素出现了多少次以及对应措施，不在该文件范围内
 *  Created on: 2022年1月17日
 *      Author: yue
 */

#include "ImageSpecial.h"

//环岛判断flag
uint8 Flag_CircleBegin=0;   //发现环岛
uint8 Flag_CircleIn=0;      //环岛入口

/*
 *******************************************************************************************
 ** 函数功能: 识别起跑线
 ** 参    数: InflectionL：左拐点
 **           InflectionR：右拐点
 ** 返 回 值: 0：没有识别到起跑线
 **           1：识别到起跑线且车库在车左侧
 **           2：识别到起跑线且车库在车右侧
 ** 作    者: WBN
 ** 注    意：1 . 默认在进行起跑线识别的路段都是直线，即车头已经摆正
 **           2.由于没有实物图做参考，只能先假设一个理想状态：整条起跑线恰好布满整个图像
 ********************************************************************************************
 */
uint8 StartLineFlag(Point InflectionL,Point InflectionR)
{
    /*
     ** 有起跑线的地方就有车库，由于赛道原因，车库有可能出现在车的左边或右边，这里也先对这两种情况进行判断并分别处理；
     ** 以车库在左边为例，从拐点开始作为扫线的原点，从下往上，从左往右扫；
     ** 固定一列，从下往上扫，找到一个黑点，固定该行，从左往右扫，记录连续出现的黑点个数（黑线的宽度），若该宽度大于阈值BLACK_WIDTH，可
     ** 以确定这是一条黑线。继续扫这一列，若发现了足够多的这样的黑线（大于设定的阈值BLACK_NUM），则认为这一行符合斑马线。按照该理论回
     ** 到起点继续行的扫描，若像这样的行数足够多（大于设定的阈值BLACK_TIMES），则认为这一幅图片中存在斑马线，即该路段是起跑线
     **/

    int row,cloum;          //行,列
    int Black_width=0;      //固定行，横向扫线是记录每段黑点的个数（即一条黑线的宽度）
    int Black_num=0;        //记录行黑线的数量，作为判断该行是否为斑马线的依据
    int Black_times=0;      //记录满足斑马线的行数，并作为判断该路段是否为斑马线的依据

    if(InflectionL.X!=0&&InflectionL.Y!=0)    //拐点（车库）在左边
    {
        for(row=InflectionL.X,cloum=InflectionL.Y;row<MT9V03X_H;row++)        //从左拐点开始固定列，从下往上扫
        {
            if(BinaryImage[row][cloum]==IMAGE_BLACK)    //找到了一个黑点
            {
                for(;cloum<MT9V03X_W;cloum++)                                 //固定行，从左往右扫
                {
                    if(BinaryImage[row][cloum]==IMAGE_BLACK)    //扫到黑点
                    {
                        Black_width++;   //黑线宽度+1
                    }
                    else                                        //扫到白点
                    {
                        if(Black_width>=S_BLACK_WIDTH)    //判断黑线宽度是否满足阈值
                        {
                            Black_num++; //行黑线数量+1
                        }
                        Black_width=0;   //在一次白点判断后重置黑线宽度
                    }
                    if(Black_num>=S_BLACK_NUM)    //若满足斑马线的阈值（这一行）
                    {
                        Black_times++;  //满足斑马线的行数+1
                        break;
                    }
                }
                Black_num=0;    //这一行的扫线结束，重置行黑线数
            }
            if(Black_times>=S_BLACK_TIMES)    //若满足斑马线路段的阈值
            {
                return 1;
            }
        }
        return 0;
    }

    if(InflectionR.X!=0&&InflectionR.Y!=0)    //拐点（车库）在右边
    {
        for(row=InflectionL.X,cloum=InflectionL.Y;row<MT9V03X_H;row++)        //从右拐点开始固定列，从下往上扫
        {
            if(BinaryImage[row][cloum]==IMAGE_BLACK)    //找到了一个黑点
            {
                for(;cloum>0;cloum--)                                         //固定行，从右往左扫
                {
                    if(BinaryImage[row][cloum]==IMAGE_BLACK)    //扫到黑点
                    {
                        Black_width++;   //黑线宽度+1
                    }
                    else                                        //扫到白点
                    {
                        if(Black_width>=S_BLACK_WIDTH)    //判断黑线宽度是否满足阈值
                        {
                            Black_num++; //行黑线数量+1
                        }
                        Black_width=0;   //在一次白点判断后重置黑线宽度
                    }
                    if(Black_num>=S_BLACK_NUM)    //若满足斑马线的阈值（这一行）
                    {
                        Black_times++;  //满足斑马线的行数+1
                        break;
                    }
                }
                Black_num=0;    //这一行的扫线结束，重置行黑线数
            }
            if(Black_times>=S_BLACK_TIMES)    //若满足斑马线路段的阈值
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
 ** 函数功能: 识别环岛但没有到达环岛入口
 ** 参    数: LeftLine：左线数组
 **           RightLine：右线数组
 **           InflectionL：左拐点
 **           InflectionR：右拐点
 ** 返 回 值: 0：没有识别到环岛
 **           1：识别到环岛且在车身左侧
 **           2：识别到环岛且在车身右侧
 ** 作    者: WBN
 ** 注    意：环岛的识别分为两部分，一是识别到环岛但并为到达入口，二是识别到了环岛入口
 ********************************************************************************************
 */
uint8 CircleIsland_Begin(int LeftLine,int RightLine,Point InflectionL,Point InflectionR)
{
    /*
     ** 这个可能的情况比较多，比较难处理，后面再写
     ** 当识别到前面有环岛时，主要是对环岛进行一个屏蔽的补线，避免车子误判为拐弯而拐进环岛的出口
     * */

    if(InflectionL.X!=0&&InflectionL.Y!=0)    //拐点（环岛）在左边
    {

    }
    return 0;
}

/*
 *******************************************************************************************
 ** 函数功能: 识别环岛入口
 ** 参    数: LeftLine：左线数组
 **           RightLine：右线数组
 **           InflectionL：左拐点
 **           InflectionR：右拐点
 ** 返 回 值: 0：没有识别到环岛
 **           1：识别到环岛入口且在车身左侧
 **           2：识别到环岛入口且在车身右侧
 ** 作    者: WBN
 ** 注    意：该函数的调用应该在识别到环岛之后开始，并在进入环岛后关闭
 ********************************************************************************************
 */
uint8 CircleIsland_In(int LeftLine,int RightLine,Point InflectionL,Point InflectionR)
{
    /*
     **     这里先只取一种情况：车子刚好在环岛圆环的中心位置，即出口已经不在图像中，图像中有的只是环岛的入口
     **     对于该种情况的判断方式：图像的中有一条直行和一条拐弯的路，那么就会出现一边是有完整的线，另一边出现丢线（转弯路口是两边丢线）
     **     第二种判断方式：一边丢线一边不丢线，这里先写了两种判断方式大概的代码
     * */

    int row,cloum;              //行,列
    int upline[MT9V03X_W]={0};  //下边界线

    if(InflectionL.X!=0&&InflectionL.Y!=0)    //拐点（环岛入口）在左边
    {
        for(cloum=InflectionL.Y;cloum>0;cloum--)    //从拐点列坐标开始，右往左扫
        {
            for(row=InflectionL.X;row<MT9V03X_H;row++)   //从拐点行坐标开始，下往上扫
            {
                if(BinaryImage[row][cloum]==IMAGE_WHITE)    //扫到白点
                {
                    upline[cloum-InflectionL.Y]=row;    //记录上边界线：以数据的位置为X轴，数据的值为Y轴画图
                    break;
                }
            }
        }
        int left=0,mid=0,right=0;   //将边界线分为左中右三段，分别求出这三段的均值
        for(cloum=InflectionL.Y;cloum>2*InflectionL.Y/3;cloum--)    //右
        {
            right+=upline[cloum];
        }
        for(;cloum>InflectionL.Y/3;cloum--)                         //中
        {
            mid+=upline[cloum];
        }
        for(;cloum>0;cloum--)                                       //左
        {
            left+=upline[cloum];
        }
        if(left<mid&&right<mid)
        {
            return 1;
        }
    }

    if(InflectionR.X!=0&&InflectionR.Y!=0)    //拐点（环岛入口）在右边
    {
        if(LostNum_LeftLine<C_LOST1&&LostNum_RightLine>C_LOST2) //左丢线数小于阈值，右丢线数大于阈值
        {
            return 2;
        }
    }
    return 0;
}

/********************************************************************************************
 ** 函数功能: 识别三岔
 ** 参    数: int startline:用户决定的起始行
 **           int endline:用户决定的结束行（表示对前几段的识别，根据速度不同进行调整）
 **           int *LeftLine：左线
 **           int *RightLine:右线
 **           Point *InflectionL:左边拐点
 **           Point *InflectionR:右边拐点
 **           Point *InflectionC:中间拐点
 ** 返 回 值: 0：没有识别到环岛
 **           1：识别到三岔
 ** 作    者: LJF
 ** 注    意：1 . 目前仅仅是正入三岔的时候的函数，因为三岔前面都会有个弯道所以会出现车身斜的情况，此时的左右拐点并不一定都存在
 **           2.这个是进三岔的函数，出三岔时候应该重写一个，并在进入三岔后再开启出三岔的判断
 *********************************************************************************************/
uint8 ForkIdentify(int startline,int endline,int *LeftLine,int *RightLine,Point *InflectionL,Point *InflectionR,Point *InflectionC)
{
    GetDownInflection(startline, endline, LeftLine, RightLine, InflectionL, InflectionR);//获取左右拐点
    if(InflectionL->X!=0&&InflectionR->X!=0)//当左右拐点存在
    {
        GetForkUpInflection(*InflectionL, *InflectionR, InflectionC);//去搜索上拐点
        if(InflectionC->X!=0)
        {
            //关于三岔的上拐点还会有其他的条件判断，比如不超过多少行什么的，这个要具体才能判断了
            //三岔成立的条件太简单了会存在误判，比如从十字出来的时候就可能会遇到同样的具有三个点
            //这里我想采用元素互斥的原则,给一个非十字的标志
            return 1;//三个拐点存在三岔成立
            /*还需要写根据左右选择标识符来选择往左还是右边从而进行补线*/
        }
    }
    return 0;
}
