/*
 * Forkroads.c
 * Created on: 2022年5月25日
 * Author: 30516
 * Effect: 存放三岔路口相关的源代码
 * 三岔测试最高速度为200，路径不好是参数问题，速度200，舵机PID：KP14.25 KD40测的
 */

#include "ImageSpecial.h"
#include "PID.h"
#include <stdlib.h> //abs函数，fabs在math.h
#include "LED.h"

#define L_FINDWHIDE_THRE  10 //Y拐点中间找左边白色区域停止的阈值
#define R_FINDWHIDE_THRE  150//Y拐点中间找右边白色区域停止的阈值
#define FORK_INFLECTION_WIDTH  110//打开三岔debug,当拐点在60行附近左右拐点的差值，补全的时候，依据没有丢失的拐点的行数做一个简单的比例关系到单边循迹思路上
#define FORK_DEBUG  0

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
    int starline,i,cloumnL,cloumnR;
    UpInflectionC->X=0;UpInflectionC->Y=0;//上拐点置零
    UpInflectionC->X=(DownInflectionL.X+DownInflectionR.X)/2;//V型上拐点的列坐标为左右拐点均值，需要修改，不一定是正入三岔
    starline=(DownInflectionL.Y+DownInflectionR.Y)/2;//起始行为左右拐点行的均值
    //从下往上找到那个跳变的点即为上拐点
    for(i=starline;i>1;i--)
    {
#if FORK_DEBUG
        lcd_drawpoint(UpInflectionC->X, i, PURPLE);
#endif
        //图像数组是[高][宽]
        if(BinaryImage[i][UpInflectionC->X]==IMAGE_WHITE && BinaryImage[i-1][UpInflectionC->X]==IMAGE_BLACK)
        {
            for(cloumnL=UpInflectionC->X;cloumnL>L_FINDWHIDE_THRE;cloumnL--)
            {
#if FORK_DEBUG
                lcd_drawpoint(cloumnL, i-1, PURPLE);
#endif
                if(BinaryImage[i-1][cloumnL]==IMAGE_WHITE)
                    break;
                if(cloumnL==L_FINDWHIDE_THRE+1)//如果起始的列就小于了11，那么则不会return，会直接到后面的赋值
                    return;//遍历完了都没有找到白的即不是三岔，退出判断
            }
            for(cloumnR=UpInflectionC->X;cloumnR<R_FINDWHIDE_THRE;cloumnR++)
            {
#if FORK_DEBUG
                lcd_drawpoint(cloumnR, i-1, PURPLE);
#endif
                if(BinaryImage[i-1][cloumnR]==IMAGE_WHITE)
                    break;
                if(cloumnR==R_FINDWHIDE_THRE-1)
                    return;//遍历完了都没有找到白的即不是三岔，退出判断
            }
            UpInflectionC->Y=i;//Y坐标是行数
            return;
        }
    }
}

/********************************************************************************************
 ** 函数功能: 识别三岔
 ** 参    数:
 **           int *LeftLine：左线
 **           int *RightLine:右线
 **           Point *InflectionL:左边拐点
 **           Point *InflectionR:右边拐点
 ** 返 回 值:  0：没有识别到环岛
 **           1：正入三岔
 ** 作    者: LJF
 ** 注    意：1 . 目前仅仅是正入三岔的时候的函数，因为三岔前面都会有个弯道所以会出现车身斜的情况，此时的左右拐点并不一定都存在
 **           2.这个是进三岔的函数，出三岔时候应该重写一个，并在进入三岔后再开启出三岔的判断
 *********************************************************************************************/
uint8 ForkIdentify(int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR)
{
#if FORK_DEBUG
    lcd_showint32(0, 0, LostNum_RightLine, 3);
    lcd_showint32(0, 1, LostNum_LeftLine, 3);
    lcd_showint32(TFT_X_MAX-50, 0, DownInflectionL.X, 3);
    lcd_showint32(TFT_X_MAX-50, 1, DownInflectionL.Y, 3);
    lcd_showint32(TFT_X_MAX-50, 2, DownInflectionR.X, 3);
    lcd_showint32(TFT_X_MAX-50, 3, DownInflectionR.Y, 3);
#endif
    Point UpInflectionC;
    //当左右拐点存在,并且两个拐点要在图像下半部分
    if(DownInflectionL.X!=0 && DownInflectionL.Y>60 && DownInflectionR.X!=0 && DownInflectionR.Y>60)
    {
        //取消这个左右拐点行数的判断，增加运算速率
        if(abs((DownInflectionL.Y-DownInflectionR.Y))<40)//左右两个拐点的行数小于30，才进行判断
        {
            GetForkUpInflection(DownInflectionL, DownInflectionR, &UpInflectionC);//去搜索上拐点
            if(UpInflectionC.Y!=0)//直接访问Y即可，加快速度，因为X默认就会赋值了
            {
                FillingLine('R',DownInflectionR,UpInflectionC);//三岔成立了就在返回之前补线
                Bias=DifferentBias(DownInflectionR.Y,UpInflectionC.Y,CentreLine);//因为这里距离进入三岔还有一段距离，我怕打角太多，所以还是按照原来的方法
                return 1;//三个拐点存在三岔成立：正入三岔
            }
        }
        else
            return 0;
    }
    else if(DownInflectionL.X==0 && DownInflectionR.X==0)//如果左右下拐点不存在并且下面一段出现就丢线的话的话,我们就去看存不存在正上的拐点
    {
        Point ImageDownPointL,ImageDownPointR;//以画面的左下角和右下角作为左右补线的点
        ImageDownPointL.X=0,ImageDownPointL.Y=MT9V03X_H,ImageDownPointR.X=MT9V03X_W-1,ImageDownPointR.Y=MT9V03X_H;
        GetForkUpInflection(ImageDownPointL, ImageDownPointR, &UpInflectionC);
        if(UpInflectionC.Y!=0 && UpInflectionC.Y>40)//直接访问Y即可，加快速度，因为X默认就会赋值了
        {
            FillingLine('R',ImageDownPointR,UpInflectionC);//三岔成立了就在返回之前补线
            //在此处就对偏差进行计算，就可以避免仅有一部分中线被补线到的问题，同时外部使用一个标志变量识别到了之后这一次则不进行外面自定义的前瞻偏差计算
            //这一次是越过了三岔很接近冲出三岔的拐角，我们手动把补到的线计算出来的bias扩大
            Bias=DifferentBias(ImageDownPointR.Y,UpInflectionC.Y,CentreLine)*1.5;
            return 1;//三岔正入丢失左右拐点那一帧
        }
    }
    //右边丢线超过60，左拐点存在，并且左拐点不能在上半平屏防止误判
    else if(LostNum_RightLine>=60 && DownInflectionL.X!=0 && DownInflectionL.Y>60)
    {
        Point ImageDownPointR;//以左拐点对称的点去补线和找拐点
        //给自己设定的右拐点去找上拐点
//        ImageDownPointR.X=MT9V03X_W-1,ImageDownPointR.YDownInflectionL.Y=DownInflectionL.Y;
        ImageDownPointR.X=DownInflectionL.X+FORK_INFLECTION_WIDTH+DownInflectionL.Y/10,ImageDownPointR.Y=DownInflectionL.Y;//运用单边循迹法的思想给拐点，赛道宽度
        GetForkUpInflection(DownInflectionL, ImageDownPointR, &UpInflectionC);
        if(UpInflectionC.Y!=0)//直接访问Y即可，加快速度，因为X默认就会赋值了
        {
            FillingLine('R',ImageDownPointR,UpInflectionC);//三岔成立了就在返回之前补线
            Bias=DifferentBias(ImageDownPointR.Y,UpInflectionC.Y,CentreLine);//在此处就对偏差进行计算，就可以避免仅有一部分中线被补线到的问题，同时外部使用一个标志变量识别到了之后这一次则不进行外面自定义的前瞻偏差计算
            return 1;//三岔左斜入三岔
        }
    }
    //左边丢线超过60,右拐点存在,并且右拐点不能在上半平屏防止误判
    else if(LostNum_LeftLine>=60 && DownInflectionR.X!=0 && DownInflectionR.Y>60)
    {
        Point ImageDownPointL;//以左拐点对称的点去补线和找拐点
        //与拐点行数做一个比例关系，越靠近底部了拐点宽度越大
        ImageDownPointL.X=DownInflectionR.X-FORK_INFLECTION_WIDTH-DownInflectionL.X/10,ImageDownPointL.Y=DownInflectionR.Y;
        GetForkUpInflection(ImageDownPointL, DownInflectionR, &UpInflectionC);
        if(UpInflectionC.Y!=0)//直接访问Y即可，加快速度，因为X默认就会赋值了
        {
            FillingLine('R',DownInflectionR,UpInflectionC);//三岔成立了就在返回之前补线
            Bias=DifferentBias(DownInflectionR.Y,UpInflectionC.Y,CentreLine);//在此处就对偏差进行计算，就可以避免仅有一部分中线被补线到的问题，同时外部使用一个标志变量识别到了之后这一次则不进行外面自定义的前瞻偏差计算
            return 1;//三岔右斜入三岔
        }
    }
    return 0;
}

/********************************************************************************************
 ** 函数功能: 三岔状态跳转判断函数
 ** 参    数: Point InflectionL：左下拐点
 **           Point InflectionR：右下拐点
 ** 返 回 值:  0：三岔还未结束
 **           1：三岔已结束
 ** 作    者: LJF
 *********************************************************************************************/
uint8 ForkFStatusIdentify(Point DownInflectionL,Point DownInflectionR,uint8 NowFlag)
{
    static uint8 StatusChange,num1,num3,numspecial;//三岔识别函数的零食状态变量，用来看状态是否跳转

    if(numspecial<200)//防止很久都没有出现进入入口的状态，及时去判断出口
    {
        numspecial++;
    }
    else if(StatusChange<1)//判断状态有没有度过入口状态，若没有则强制跳过
    {
        StatusChange=2;
    }

    switch(StatusChange)
    {
        //入口状态
        case 0:
        {
            if(NowFlag==1)
            {
                StatusChange=1;//只要开始识别到了三岔就说明已经是入口阶段了
            }
            break;
        }
        //中途状态
        case 1:
        {
            if(num1<120)  //给足够长的时间让车走到三岔运行中
            {
                num1++;
                break;
            }
            else if(NowFlag==0)
            {
                StatusChange=2;//过了中间过度态之后跳转至检测出口
            }
            break;
        }
        //出口状态
        case 2:
        {
            if(NowFlag==1)
            {
                StatusChange=3;
            }
            break;
        }
        //确保已经出三岔了，否则三岔口就出三岔了，使得出三岔其实是扫线出的
        case 3:
        {
            if(num3<35)  //给足够长的时间让车走出三岔中
            {
                num3++;
                break;
            }
            else
            {
                return 1;
            }
        }
        default:break;
    }
    return 0;
}

/********************************************************************************************
 ** 函数功能: 三岔状态跳转判断函数
 ** 参    数: Point InflectionL：左下拐点
 **           Point InflectionR：右下拐点
 ** 返 回 值:  0：三岔还未结束
 **           1：三岔已结束
 ** 作    者: LJF
 *********************************************************************************************/
uint8 ForkSStatusIdentify(Point DownInflectionL,Point DownInflectionR,uint8 NowFlag)
{
    static uint8 StatusChange,num1,num3,numspecial;//三岔识别函数的零食状态变量，用来看状态是否跳转

    if(numspecial<255)//防止很久都没有出现进入入口的状态，及时去判断出口
    {
        numspecial++;
    }
    else if(StatusChange<1)//判断状态有没有度过入口状态，若没有则强制跳过
    {
        StatusChange=2;
    }

    switch(StatusChange)
    {
        //入口状态
        case 0:
        {
            if(NowFlag==1)
            {
                SteerK.P=18.25;//减小KP进入三岔
                StatusChange=1; //只要开始识别到了三岔就说明已经是入口阶段了
            }
            break;
        }
        //中途状态
        case 1:
        {
            if(num1<100)  //给足够长的时间让车走到三岔运行中
            {
                if(num1==50)
                {
                    SteerK.D=3;//进入直到之后D减小防止振荡
                    base_speed+=15; //进入三岔提速，确保是正常进入的三岔才会触发
                }
                num1++;
                break;
            }
            if(NowFlag==0)
            {
                StatusChange=2; //过了中间过度态之后跳转至检测出口
                base_speed-=5; //检测出口减速
            }
            break;
        }
        //出口状态
        case 2:
        {
            if(NowFlag==1)
            {
                StatusChange=3;
            }
            break;
        }
        //确保已经出三岔了，否则三岔口就出三岔了，使得出三岔其实是扫线出的
        case 3:
        {
            if(num3<50)
            {
                num3++;
                break;
            }
            else
            {
                SteerK.P=19.25;//还原KP
                return 1;
            }
        }
        default:break;
    }
    return 0;
}

