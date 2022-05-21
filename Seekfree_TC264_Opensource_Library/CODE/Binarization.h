#ifndef CODE_BINARIZATION_H_
#define CODE_BINARIZATION_H_

#include "SEEKFREE_MT9V03X.h"//Ϊ��Ҫuint8���ֶ���,��ֵ���㷨�е�ĳЩ��ѧ����,����ͷͼ���ȫ�ֱ���

//************�궨��***************
#define IMAGE_BLACK 0//��ֵ���ĺڵ�
#define IMAGE_WHITE 255//��ֵ���İ׵㣬���ڶ�ֵ��ͼ���Լ�ɨ������
//*********************************

//************ȫ�ֱ���**************
extern uint8 BinaryImage[MT9V03X_H][MT9V03X_W];
//**********************************


uint8 otsuThreshold(uint8 *image, uint16 width, uint16 height);     //�����ֵ����ֵ: ��� 3ms
uint8 GuDiThreshold(uint16 width, uint16 height);     //�����ֵ����ֵ���ȵ���Сֵ 2ms
uint8 OneDimensionalThreshold(uint16 width, uint16 height); //�����ֵ����ֵ 4ms

void ImageBinary(void);                                             //������ֵ��ֵ��ͼ���޸�ԭ������

#endif /* CODE_BINARIZATION_H_ */
