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
#define C_BIAS      2       //����Сë�̵�Ӱ�죬���߸���˿��
#define C_LOSTLINE  35      //������������жϵĶ�����         //Խ������Խ�Ͻ�
#define C_LINEBIAS  0.8     //�ж����������һ�ߵ�ֱ��б��     //ԽС����Խ�Ͻ�
#define C_LOSTNUM   50      //�ж��������ڵĶ�����
//Sobel���Ӽ��
#define FastABS(x) (x > 0 ? x : x * -1.0f)
#define BinaryImage(i, j)    BinaryImage[i][j]

/*��������غ���*/
uint8 GarageIdentify(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);      //������ʶ��
int64 SobelTest();      //Sobel���Ӽ��������
/*������غ���*/
uint8 CircleIslandBegin(int *LeftLine,int *RightLine);      //ʶ�𻷵����
uint8 CircleIslandEnd(); //ʶ�𻷵�����
uint8 CircleIslandIdentify(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);   //ʶ�𻷵�
/*������غ���*/
void GetForkUpInflection(Point DownInflectionL,Point DownInflectionR,Point *UpInflectionC);//��Ѱ�������Ϲյ�
uint8 ForkIdentify(int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR);//����ʶ��
/*ʮ����غ���*/
void GetCrossRoadsUpInflection(int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR,Point *UpInflectionL,Point *UpInflectionR);//��Ѱ��ʮ�ֵ��Ϲյ�
uint8 CrossRoadsIdentify(int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR);//ʮ��ʶ����

#endif /* CODE_IMAGESPECIAL_H_ */
