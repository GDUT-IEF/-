/*
 **  ImageBasic.h
 **  ���ã����������ȡͼ�������Ϣ��ģ�飬������ɨ��ʲô��
 **  Created on: 2021��12��7��
 **  Author: 30516
 */

#ifndef CODE_IMAGEBASIC_H_
#define CODE_IMAGEBASIC_H_

#include "Binarization.h"//��ֵ��֮���ͼ������
#include "SEEKFREE_MT9V03X.h"//Ϊ��Ҫuint8���ֶ���,��ֵ���㷨�е�ĳЩ��ѧ����,����ͷͼ���ȫ�ֱ���

//�궨��
#define BORDER_BIAS 3


void GetImagBasic(int *LeftLine, int *CentreLine, int *RightLine);  //ɨ����ȡ����������

#endif /* CODE_IMAGEBASIC_H_ */
