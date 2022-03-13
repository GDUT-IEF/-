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
#define S_BLACK_WIDTH  3    //�����ߺ��߿����ֵ //�����жϸ����Ƿ�Ϊ�����ߺ���
#define S_BLACK_NUM    8    //�����ߺ���������ֵ //�����жϸ����Ƿ�Ϊ������
#define S_BLACK_TIMES  3    //������������ֵ     //�����жϸ�·���Ƿ�Ϊ������
//�����ж����
#define C_BIAS  2           //��������Сë�̵�Ӱ�죬���߸���˿��

/*��������غ���*/
uint8 StartLineFlag(int *LeftLine,int *RightLine);      //������ʶ��

/*������غ���*/
uint8 CircleIslandBegin(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);
uint8 CircleIslandEnd(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);
uint8 CrossLoopEnd(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);
/*������غ���*/
void GetForkUpInflection(Point DownInflectionL,Point DownInflectionR,Point *UpInflectionC);//��Ѱ�������Ϲյ�
uint8 ForkIdentify(int startline,int endline,int *LeftLine,int *RightLine,Point *InflectionL,Point *InflectionR,Point *InflectionC);//����ʶ��
/*ʮ����غ���*/
void GetCrossRoadsUpInflection(int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR,Point *UpInflectionL,Point *UpInflectionR);//��Ѱ��ʮ�ֵ��Ϲյ�

#endif /* CODE_IMAGESPECIAL_H_ */
