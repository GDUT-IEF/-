/*
 **  ImageBasic.h
 **  ���ã����������ȡͼ�������Ϣ��ģ�飬������ɨ��ʲô��
 **  Created on: 2021��12��7��
 **  Author: 30516
 */

#ifndef CODE_IMAGEBASIC_H_
#define CODE_IMAGEBASIC_H_

#include "Binarization.h"       //��ֵ��֮���ͼ������
#include "SEEKFREE_MT9V03X.h"   //Ϊ��Ҫuint8���ֶ���,��ֵ���㷨�е�ĳЩ��ѧ����,����ͷͼ���ȫ�ֱ���
#include "SEEKFREE_18TFT.h"     //ʹ��LCD����
#include "zf_stm_systick.h"     //ʹ����ʱ����

//�궨��
#define BORDER_BIAS 1   //ɨ�����
#define INFLECTION_WIDTH  10    //�յ��������

//ȫ�ֱ���
extern uint8 Lost_Row;                    //���߶�ʧ��������(ɨ�ߵ�������)
extern uint8 LostNum_LeftLine,LostNum_RightLine; //��¼���ұ߽綪����
extern int LeftLine[MT9V03X_H], CentreLine[MT9V03X_H], RightLine[MT9V03X_H];   //ɨ�ߴ�������������
//�ṹ��
typedef struct Point
{
    int X;
    int Y;
}Point; //������Ľṹ��

void GetImagBasic(int *LeftLine, int *CentreLine, int *RightLine ,char path);  //ɨ����ȡ����������
void GetDownInflection(int startline,int endline,int *LeftLine,int *RightLine,Point *InflectionL,Point *InflectionR);//�������ұ߽������õ��¹յ㣨ʮ�֡�����������
void GetUpInflection(char Choose,int startline,int endline,Point *UpInflection);//���ݱ��������ߵõ�������֮���������Ĳ�ֵ�������趨����ֵ���ж�Ϊ���Ϲյ�
void GetRightangleUPInflection(char Choose,Point DowmInflection,Point *UpInflection,int ROWTHR,int CLOUMNTHR);//���ݶ�ֵ��ͼ����ֱ���Ϲյ�
void Bin_Image_Filter(void);//ͼ��ʴ
void MeasureWidth(int startline,int endline);//�����������
void GetImagBasic_Garage(int *LeftLine, int *CentreLine, int *RightLine ,char path);

#endif /* CODE_IMAGEBASIC_H_ */
