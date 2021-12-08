#include "Binarization.h"

uint8 BinaryImage[MT9V03X_H][MT9V03X_W]={0};

/*
 *  @brief  ��򷨶�ֵ��0.8ms����ʵ�ʲ���4ms��TC264�У�
 *  @date:   2018-10
 *  @since      v1.2
 *  *image ��ͼ���ַ
 *  width:  ͼ���
 *  height��ͼ���
 *  @author     ZС��
 */
uint8 otsuThreshold(uint8 *image, uint16 width, uint16 height)
{
    #define GrayScale 256
    int pixelCount[GrayScale] = {0};//ÿ���Ҷ�ֵ��ռ���ظ���
    float pixelPro[GrayScale] = {0};//ÿ���Ҷ�ֵ��ռ�����ر���
    int i,j;
    int Sumpix = width * height;   //�����ص�
    uint8 threshold = 0;
    uint8* data = image;  //ָ���������ݵ�ָ��

    //ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            pixelCount[(int)data[i * width + j]]++;  //������ֵ��Ϊ����������±�
          //   pixelCount[(int)image[i][j]]++;    ������ָ�������
        }
    }
    float u = 0;
    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / Sumpix;   //����ÿ������������ͼ���еı���
        u += i * pixelPro[i];  //��ƽ���Ҷ�
    }

    float maxVariance=0.0;  //�����䷽��
    float w0 = 0, avgValue  = 0;  //w0 ǰ������ ��avgValue ǰ��ƽ���Ҷ�
    for(int i = 0; i < 256; i++)     //ÿһ��ѭ������һ��������䷽����� (����for����Ϊ1��)
    {
        w0 += pixelPro[i];  //���赱ǰ�Ҷ�iΪ��ֵ, 0~i �Ҷ�������ռ����ͼ��ı�����ǰ������
        avgValue  += i * pixelPro[i];

        float variance = pow((avgValue/w0 - u), 2) * w0 /(1 - w0);    //��䷽��
        if(variance > maxVariance)
        {
            maxVariance = variance;
            threshold = (uint8)i;
        }
    }

    return threshold;
}

//���ݳ����������ô�򷨻�ȵ���Сֵ�õ���ֵ����ֵȻ����ݻҶ�ͼ�õ��ڰ�ͼ��
void ImageBinary()
{
    uint8 Image_Threshold = otsuThreshold(mt9v03x_image[0],MT9V03X_W,MT9V03X_H);
    //�����е�һά����������Ϊ�˽�ͼ��ԭ���Ϊ���½ǣ�ԭͼ������Ϊ���Ͻǣ�
    for (int i = 0; i < MT9V03X_H; ++i)
    {
        for (int j = 0; j < MT9V03X_W; ++j)
        {
            if (mt9v03x_image[i][j] <= Image_Threshold)//���ж�ֵ��֮ǰֻ�ǵõ���ֵ
                BinaryImage[MT9V03X_H - 1 - i][j] = 0;//0�Ǻ�ɫ
            else
                BinaryImage[MT9V03X_H - 1 - i][j] = 255;//1�ǰ�ɫ
        }
    }
}

