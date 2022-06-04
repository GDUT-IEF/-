#ifndef CODE_BINARIZATION_H_
#define CODE_BINARIZATION_H_

#include "SEEKFREE_MT9V03X.h"//Ϊ��Ҫuint8���ֶ���,��ֵ���㷨�е�ĳЩ��ѧ����,����ͷͼ���ȫ�ֱ���

//************�궨��***************
#define IMAGE_BLACK 0//��ֵ���ĺڵ�
#define IMAGE_WHITE 255//��ֵ���İ׵㣬���ڶ�ֵ��ͼ���Լ�ɨ������
#define IMAGE_COMPRESS_W    80//ѹ��֮���ͼ����
#define IMAGE_COMPRESS_H    60//ѹ��֮���ͼ��߶�
#define IMAGECOMPRESS 0//�Ƿ���ͼ��ѹ���Ķ�ֵ����1���� 0���� ѹ��ͼ��֡�ʼǵõ���230
//*********************************

//************ȫ�ֱ���**************
extern uint8 CompressImage[IMAGE_COMPRESS_H][IMAGE_COMPRESS_W];//�洢ѹ��֮��ĻҶ�ͼ
#if IMAGECOMPRESS
extern uint8 BinaryImage[IMAGE_COMPRESS_H][IMAGE_COMPRESS_W];
#else
extern uint8 BinaryImage[MT9V03X_H][MT9V03X_W];
#endif
//**********************************


uint8 otsuThreshold(uint8 *image, uint16 width, uint16 height);     //�����ֵ����ֵ: ��� 3ms
uint8 GuDiThreshold(uint16 width, uint16 height);     //�����ֵ����ֵ���ȵ���Сֵ 2ms
uint8 OneDimensionalThreshold(uint16 width, uint16 height); //�����ֵ����ֵ 4ms
void adaptiveThreshold(uint8 *img_data, uint8 *output_data, int width, int height, int block, uint8 clip_value);//����Ӧ��ֵ��ֵ��
void Get_Compress_Image(void); //ѹ��ԭʼ�Ҷ�ͼ���ٽ��ж�ֵ�����ٶ���֮ǰ��һ��֮ǰ��ֵ��һ��ͼ5ms������1.5ms
void ImageBinary(void); //������ֵ��ֵ��ͼ��

#endif /* CODE_BINARIZATION_H_ */
