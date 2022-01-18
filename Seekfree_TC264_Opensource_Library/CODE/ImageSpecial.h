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

#define BLACK_WIDTH  3   //�����ߺ��߿����ֵ //�����жϸ����Ƿ�Ϊ�����ߺ���
#define BLACK_NUM    8   //�����ߺ���������ֵ //�����жϸ����Ƿ�Ϊ������
#define BLACK_TIMES  3   //������������ֵ     //�����жϸ�·���Ƿ�Ϊ������

uint8 StartLineFlag(Point InflectionL,Point InflectionR);    //������ʶ��

#endif /* CODE_IMAGESPECIAL_H_ */
