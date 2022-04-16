/*
 * ImageSpecial.h
 *
 *  Created on: 2022��1��17��
 *      Author: yue
 */

#ifndef CODE_IMAGESPECIAL_H_
#define CODE_IMAGESPECIAL_H_

#include "Binarization.h"       //��ֵ��֮���ͼ������
#include "SEEKFREE_MT9V03X.h"   //Ϊ��Ҫuint8���ֶ���,��ֵ���㷨�е�ĳЩ��ѧ����,����ͷͼ���ȫ�ֱ���
#include "ImageBasic.h"         //��ȡͼ���������֮�������
#include "ImageTack.h"

//������ʶ��
#define G_LINEBIAS  0.8     //�ж�������һ�ߵ�ֱ��б��
#define G_ZEBRA_NUM 14      //�����߱�־������ֵ //������n�����ߣ���G_ZEBRA_NUM=2*n����������9�����ߣ�ȡƫСֵ
//�����ж����
#define C_LOSTLINE  35      //������������жϵĶ�����         //Խ������Խ�Ͻ�
#define C_LOSTNUM   50      //�ж��������ڵĶ�����
//Sobel���Ӽ��
#define FastABS(x) (x > 0 ? x : x * -1.0f)
#define BinaryImage(i, j)    BinaryImage[i][j]
#define ZebraTresholeL 2300  //���������Ե���ֵ
#define ZebraTresholeR 2000  //���������Գ������ұߵ���ֵ
#define SobelLTestStop 2     //��������߹رյ���ֵ
//ʮ�ֻػ�����
#define L_LOSTNUM   60

extern uint8 SobelLCount;             //���������
extern uint8 SobelRCount;             //�ұ�������

/*��������غ���*/
int64 SobelTest();      //Sobel���Ӽ��������
uint8 GarageIdentify(char Direction,Point InflectionL,Point InflectionR);      //������ʶ��
/*������غ���*/
uint8 CircleIslandIdentify_L(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);   //�󻷵�״̬��
uint8 CircleIslandIdentify_R(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);   //�һ���״̬��
uint8 DealGarageLSpecial();     //�󳵿��������������б���
/*������غ���*/
void GetForkUpInflection(Point DownInflectionL,Point DownInflectionR,Point *UpInflectionC);//��Ѱ�������Ϲյ�
uint8 ForkIdentify(int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR);//����ʶ��
/*ʮ����غ���*/
void GetCrossRoadsUpInflection(int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR,Point *UpInflectionL,Point *UpInflectionR);//��Ѱ��ʮ�ֵ��Ϲյ�
uint8 CrossRoadsIdentify(int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR);//ʮ��ʶ����
uint8 CrossLoopEnd_F();
uint8 CrossLoopEnd_S();
#endif /* CODE_IMAGESPECIAL_H_ */
