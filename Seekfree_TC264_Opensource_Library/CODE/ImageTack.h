/*
 * ImageTack.h
 *  Created on: 2021��12��22��
 *  Author: 30516
 *  Effect: ���ڴ�Ŵ�ͼ������ȡѭ��Ԫ�صĺ���
 */

#ifndef CODE_IMAGETACK_H_
#define CODE_IMAGETACK_H_

#include "Binarization.h"       //��ֵ��֮���ͼ������
#include "SEEKFREE_MT9V03X.h"   //Ϊ��Ҫuint8���ֶ���,��ֵ���㷨�е�ĳЩ��ѧ����,����ͷͼ���ȫ�ֱ���
#include "ImageBasic.h"         //��ȡͼ���������֮�������

extern float Bias;  //ƫ��

float Regression_Slope(int startline,int endline,int *CentreLine);//�������������ϳ����Ļع鷽��
void FillingLine(int *LeftLine, int *CentreLine, int *RightLine, Point StarPoint,Point EndPoint);//����������в���
float DifferentBias(int startline,int endline,int *CentreLine);


#endif /* CODE_IMAGETACK_H_ */
