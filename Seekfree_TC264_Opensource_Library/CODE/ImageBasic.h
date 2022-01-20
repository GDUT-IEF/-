/*
 **  ImageBasic.h
 **  作用：用来存放提取图像基本信息的模块，类似于扫线什么的
 **  Created on: 2021年12月7日
 **  Author: 30516
 */

#ifndef CODE_IMAGEBASIC_H_
#define CODE_IMAGEBASIC_H_

#include "Binarization.h"       //二值化之后的图像数组
#include "SEEKFREE_MT9V03X.h"   //为了要uint8这种定义,二值化算法中的某些数学计算,摄像头图像的全局变量

//宏定义
#define BORDER_BIAS 3
//全局变量
extern int Lost_CentreLine;                    //中线丢失的行坐标(扫线到赛道外)
extern int LostNum_LeftLine,LostNum_RightLine; //记录左右边界丢线数
//结构体
typedef struct Point
{
    int X;
    int Y;
}Point; //点坐标的结构体

void GetImagBasic(int *LeftLine, int *CentreLine, int *RightLine);  //扫线提取左中右三线
void GetDownInflection(int startline,int endline,int *LeftLine,int *RightLine,Point *InflectionL,Point *InflectionR);//根据左右边界线来得到下拐点（十字、三岔、环岛）
void GetForkUpInflection(Point DownInflectionL,Point DownInflectionR,Point *UpInflectionC);//搜寻出三岔上拐点

#endif /* CODE_IMAGEBASIC_H_ */
