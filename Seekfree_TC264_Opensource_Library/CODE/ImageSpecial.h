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
#define ZebraTresholeL 1500  //���������Ե���ֵ
#define ZebraTresholeR 2000  //���������Գ������ұߵ���ֵ
#define SobelLTestStop 3     //��������߹رյ���ֵ
//ʮ�ֻػ�����
#define L_LOSTNUM   60


/*��������غ���*/
int64 SobelTest(void);      //Sobel���Ӽ��������
uint8 GarageIdentify(char Direction,Point InflectionL,Point InflectionR);            //������ʶ��
uint8 GarageLStatusIdentify(Point InflectionL,Point InflectionR,uint8 GarageLFlag);  //�󳵿��״̬��ת��
void OutGarage(void);       //����
/*������غ���*/
uint8 CircleIslandBegin_L(int *LeftLine,int *RightLine);
uint8 CircleIslandOverBegin_L(int *LeftLine,int *RightLine);
uint8 CircleIslandEnd_L(void);
uint8 CircleIsFlag_1_L(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);
uint8 CircleIsFlag_2_L(int *LeftLine,int *RightLine);
uint8 CircleIsFlag_3_L(void);
uint8 CircleIslandIdentify_L(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);   //�󻷵�״̬��
uint8 CircleIslandBegin_R(int *LeftLine,int *RightLine);
uint8 CircleIslandOverBegin_R(int *LeftLine,int *RightLine);
uint8 CircleIslandEnd_R(void);
uint8 CircleIsFlag_1_R(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);
uint8 CircleIsFlag_2_R(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);
uint8 CircleIsFlag_3_R(void);
uint8 CircleIslandIdentify_R(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);   //�һ���״̬��
uint8 DealGarageLSpecial(void);     //�󳵿��������������б���
/*������غ���*/
void GetForkUpInflection(Point DownInflectionL,Point DownInflectionR,Point *UpInflectionC);     //��Ѱ�������Ϲյ�
uint8 ForkIdentify(int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR);   //����ʶ��
uint8 ForkFStatusIdentify(Point DownInflectionL,Point DownInflectionR,uint8 NowFlag);           //��һ������״̬��ת�жϺ���
uint8 ForkSStatusIdentify(Point DownInflectionL,Point DownInflectionR,uint8 NowFlag);           //�ڶ�������״̬��ת�жϺ���
/*ʮ����غ���*/
uint8 CrossLoopEnd_F(void); //��һ��ʮ�ֻػ�����
uint8 CrossLoopEnd_S(void); //�ڶ���ʮ�ֻػ�����
uint8 CrossLoopBegin_F(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);   //��һ��ʮ�ֻػ����
uint8 CrossLoopBegin_S(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);   //�ڶ���ʮ�ֻػ����
uint8 CrossLoop_F(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);        //��һ��ʮ�ֻػ�״̬��

#endif /* CODE_IMAGESPECIAL_H_ */
