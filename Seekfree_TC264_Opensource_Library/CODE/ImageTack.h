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
extern float Slope; //б��

float Regression_Slope(int startline,int endline,int *CentreLine);//�������������ϳ����Ļع鷽��
void FillingLine(char Choose, Point StarPoint,Point EndPoint);//����������в���
float DifferentBias(int startline,int endline,int *CentreLine);
float DifferentBias_Circle(uint8 startline,uint8 endline,int *CentreLine);
float DifferentBias_Garage(int startline,int endline,int *CentreLine);//����ר��ѭ��ƫ��
void FillinLine_V2(char Choose,int startline,int endline,Point Point1,Point Point2);//�ȵ�һ�汾�����˿��Ը����Ѿ��е��߶�ȥ�Ʋ��δ֪���߶β���
void Unilaterally_Plan_CenterLine(char ManualorAuto ,char LorR,int startline,int endline);//�ж������Ƿ񵥱߶���ʹ��������Ϣʧ�棬��������һ��δ���ߵı�����ʧ��
uint8 SlopeUntie_X(Point point_1,Point point_2,uint8 y);

#endif /* CODE_IMAGETACK_H_ */
