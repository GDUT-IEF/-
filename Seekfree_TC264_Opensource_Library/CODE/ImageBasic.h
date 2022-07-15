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
#define UPINFLECTION_THRESHOLD  30  //�Ϲյ�������Ĳ�ֵ��ֵ
#define UPINFLECTION_COMPARE_INTERVAL 3 //�Ϲյ�����֮��Ƚϼ��
//ȫ�ֱ���
extern uint8 Lost_Row;                    //���߶�ʧ��������(ɨ�ߵ�������)
extern uint8 LostNum_LeftLine,LostNum_RightLine; //��¼���ұ߽綪����
extern uint8 LeftLine[MT9V03X_H], CentreLine[MT9V03X_H], RightLine[MT9V03X_H];   //ɨ�ߴ�������������
//�ṹ��
typedef struct Point
{
    uint8 X;
    uint8 Y;
}Point; //������Ľṹ��

void GetImagBasic(uint8 *LeftLine, uint8 *CentreLine, uint8 *RightLine ,char path);  //ɨ����ȡ����������
void GetDownInflection(uint8 startline,uint8 endline,Point *InflectionL,Point *InflectionR);//�������ұ߽������õ��¹յ㣨ʮ�֡�����������
void GetUpInflection(char Choose,uint8 startline,uint8 endline,Point *UpInflection);//���ݱ��������ߵõ�������֮���������Ĳ�ֵ�������趨����ֵ���ж�Ϊ���Ϲյ�
void GetRightangleUPInflection(char Choose,Point DowmInflection,Point *UpInflection,uint8 ROWTHR,uint8 CLOUMNTHR);//���ݶ�ֵ��ͼ����ֱ���Ϲյ�
void Bin_Image_Filter(void);//ͼ��ʴ
void MeasureWidth(uint8 startline,uint8 endline);//�����������

#endif /* CODE_IMAGEBASIC_H_ */
