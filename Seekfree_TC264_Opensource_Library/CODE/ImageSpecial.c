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
 ** 参    数: *LeftLine：  左线数组
 **           *RightLine：右线数组
 ** 返 回 值: 0：没有识别到起跑线
 **           1：识别到起跑线且车库在车左侧
 **           2：识别到起跑线且车库在车右侧
 ** 作    者: WBN
 ** 注    意：1 . 默认在进行起跑线识别的路段都是直线，即车头已经摆正
 **           2.由于没有实物图做参考，只能先假设一个理想状态：整条起跑线恰好布满整个图像
 ********************************************************************************************
 */
uint8 StartLineFlag(int *LeftLine,int *RightLine)
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

    Point InflectionL, InflectionR; //下拐点
    InflectionL.X=0;
    InflectionL.Y=0;
    InflectionR.X=0;
    InflectionR.Y=0;
    GetDownInflection(0,MT9V03X_H,LeftLine,RightLine,&InflectionL,&InflectionR);    //获取下拐点


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
 ** 注    意：传入的拐点需确保：若该图不存在拐点则拐点的数据均为0
 ********************************************************************************************
 */
uint8 CircleIslandBegin(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{
    /*
     ** 这个可能的情况比较多，比较难处理，后面再写
     ** 当识别到前面有环岛时，主要是对环岛进行一个屏蔽的补线，避免车子误判为拐弯而拐进环岛的出口
     * */
    int row;          //行
    Point Inflection;
    if(InflectionL.X!=0&&InflectionL.Y!=0)  //拐点（环岛）在左边
        {
            for(row=InflectionL.Y-10;row>0;row--)      //从左拐点开始向前行扫线
            {
                if(LeftLine[row]==MT9V03X_W&&LeftLine[row-1]!=MT9V03X_W)  //该行丢线而下一行不丢线
                {
                    return 1;
                }
            }
        }
    if(InflectionR.X!=0&&InflectionR.Y!=0)    //拐点（环岛）在右边
    {
        for(row=InflectionR.Y;row-1>0;row--)      //从右拐点开始向前行扫线
        {
            if(RightLine[row]==MT9V03X_W&&RightLine[row-1]!=MT9V03X_W)  //该行丢线而下一行不丢线
            {

//                lcd_drawpoint(RightLine[row-1],row,GREEN);
//
//                Inflection.Y=row-1;
//                Inflection.X=RightLine[row-1];
//                FillingLine(InflectionR,Inflection);


                return 2;
            }
        }
    }
    return 0;
}

uint8 CircleIslandEnd(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR)
{

}

/*********************************************************************************
 ** 函数功能: 根据左右下拐点搜寻出三岔上拐点
 ** 参    数: Point InflectionL: 左边拐点
 **           Point InflectionR: 右边拐点
 **           Point *UpInflectionC: 中间上拐点
 ** 返 回 值: 无
 ** 说    明: 这个函数最后面会修改，写入在找拐点里面，或者还要加一个元素类型的参数，根据类型来找不同的拐点，这里只针对三岔
 ** 作    者: LJF
 **********************************************************************************/
void GetForkUpInflection(Point DownInflectionL,Point DownInflectionR,Point *UpInflectionC)
{
    int starline,i;
    UpInflectionC->X=(DownInflectionL.X+DownInflectionR.X)/2;//V型上拐点的列坐标为左右拐点均值，需要修改，不一定是正入三岔
    starline=(DownInflectionL.Y+DownInflectionR.Y)/2;//起始行为左右拐点行的均值
    //从下往上找到那个跳变的点即为上拐点
    for(i=starline;i>0;i--)
    {
        //图像数组是[高][宽]
        if(BinaryImage[i][UpInflectionC->X]==IMAGE_WHITE && BinaryImage[i-1][UpInflectionC->X]==IMAGE_BLACK)
        {
            UpInflectionC->Y=i;//Y坐标是行数
            /*打印上拐点坐标，用于测试*/
//            lcd_showint32(0,0,UpInflectionC->Y,3);
//            lcd_showint32(TFT_X_MAX-50,0,UpInflectionC->X,3);
//            systick_delay_ms(STM0, 1000);
            return;
        }
    }
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
