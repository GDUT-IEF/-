#include "Binarization.h"
#include "headfile.h"
#include <stdlib.h>
#include "zf_assert.h"

#define IMAGECOMPRESS 0//�Ƿ���ͼ��ѹ���Ķ�ֵ����1���� 0���� ѹ��ͼ��֡�ʼǵõ���230

uint8 CompressImage[IMAGE_COMPRESS_H][IMAGE_COMPRESS_W]={0};//�洢ѹ��֮��ĻҶ�ͼ
#if IMAGECOMPRESS
uint8 BinaryImage[IMAGE_COMPRESS_H][IMAGE_COMPRESS_W]={0};//ѹ��ͼ��֮��Ķ�ֵ��ͼ��
#else
uint8 BinaryImage[MT9V03X_H][MT9V03X_W]={0};//��ֵ��ͼ��
#endif

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
/***************************************************************************
 ** ��������: �ȵ���Сֵ��ֵ���㷨
 ** ��    ��: uint16 width : ͼ����
 **           uint16 height: ͼ��߶�
 ** �� �� ֵ: ��ֵ����ֵ
 ** ��    ��: ʦ��
 ** ע    �⣺����������ͷ�ɼ��ĻҶ�ͼ���ȫ�ֱ���
 *************************************************************************/
uint8 GuDiThreshold(uint16 width, uint16 height)     //�����ֵ����ֵ���ȵ���Сֵ
{
    uint16 graynum[256] = {0};
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            uint8 pix = mt9v03x_image[i][j];
            graynum[pix]++;
        }
    }
    uint8 Max[2] = {0};
    uint8 index[2] = {0};
    for (int i = 0; i < 2; ++i)
    {
        for (uint16 j = 0; j < 256; ++j)
        {
            if (i == 0)
            {
                if (graynum[j] > Max[i])
                {
                    Max[i] = graynum[j];
                    index[i] = j;
                }
            }
            else
            {
                if (graynum[j] > Max[i] && graynum[j] != Max[0])
                {
                    Max[i] = graynum[j];
                    index[i] = j;
                }
            }
        }
    }
    if (index[0] > index[1])
    {
        uint8 temp = 0;
        temp = index[0];
        index[0] = index[1];
        index[1] = temp;
    }

    uint8 Min = 255, index_Min = 0;
    for (uint8 i = index[0]; i < index[1]; ++i)
    {
        if (graynum[i] < Min)
        {
            Min = graynum[i];
            index_Min = i;
        }
    }

    return index_Min;
}

/*
 *******************************************************************************************
 ** ��������: ����һάMeans�Ķ�ֵ����ֵ����
 ** ��    ��: width��ͼ����
 **           height��ͼ��߶�
 ** �� �� ֵ: ��ֵ����ֵ
 ** ��    ��: WBN
 ********************************************************************************************
 */
uint8 OneDimensionalThreshold(uint16 width, uint16 height)
{
    int row,cloum;
    int G1,G2;
    int g1,g2;
    uint8 threshold=160,threshold_last=0;   //��ֵ����һ����ֵ����ʼ��Ϊ��ͬ��ֵ����һ����ֵ����Ϊ����趨��

    while(abs(threshold-threshold_last)>10)   //ֻ�е��������μ������ֵ���ʱ�Ż�����while
    {
        //��ʼ������
        G1=0;G2=0;
        g1=0;g2=0;
        //����G1��G2�ķ���
        for(row=0;row<height;row++)
        {
            for(cloum=0;cloum<width;cloum++)
            {
                if(mt9v03x_image[row][cloum]>threshold)
                {
                    G1+=mt9v03x_image[row][cloum];
                    g1++;
                }
                else
                {
                    G2+=mt9v03x_image[row][cloum];
                    g2++;
                }
            }
        }
        //��������ֵ�ļ���
        threshold_last=threshold;       //������һ�ε���ֵ
        threshold=((G1/g1)+(G2/g2))/2;  //��ֵ=��G1ƽ��ֵ+G2ƽ��ֵ��/ 2
    }
    return threshold;
}

//���ݳ����������ô�򷨻�ȵ���Сֵ�õ���ֵ����ֵȻ����ݻҶ�ͼ�õ��ڰ�ͼ��
void ImageBinary()
{
#if IMAGECOMPRESS
    //ѹ��ͼ��Ķ�ֵ��
    uint8 Image_Threshold = otsuThreshold(CompressImage[0],IMAGE_COMPRESS_W,IMAGE_COMPRESS_H);//ʹ�ô�򷨵õ���ֵ����ֵ
    for (int i = 0; i < IMAGE_COMPRESS_H; ++i)
    {
        for (int j = 0; j < IMAGE_COMPRESS_W; ++j)
        {
            if (CompressImage[i][j] <= Image_Threshold)//���ж�ֵ��֮ǰֻ�ǵõ���ֵ
                Binary2Image[i][j] = IMAGE_BLACK;//0�Ǻ�ɫ  //ͼ��ԭ�㲻��
            else
                Binary2Image[i][j] = IMAGE_WHITE;//1�ǰ�ɫ  //ͼ��ԭ�㲻��
        }
    }
#else
//    systick_start(STM1);
//    uint8 Image_Threshold = GuDiThreshold(MT9V03X_W,MT9V03X_H);//ʹ�ùȵ���Сֵ�õ���ֵ����ֵ
    uint8 Image_Threshold = otsuThreshold(mt9v03x_image[0],MT9V03X_W,MT9V03X_H);//ʹ�ô�򷨵õ���ֵ����ֵ
//    uint8 Image_Threshold = OneDimensionalThreshold(MT9V03X_W,MT9V03X_H);//ʹ��һάmeans���õ���ֵ����ֵ
//    lcd_showint32(60, 0, systick_getval_us(STM1), 5);
//    lcd_showuint8(8, 0, Image_Threshold);
    for (int i = 0; i < MT9V03X_H; ++i)
    {
        for (int j = 0; j < MT9V03X_W; ++j)
        {
            if (mt9v03x_image[i][j] <= Image_Threshold)//���ж�ֵ��֮ǰֻ�ǵõ���ֵ
                BinaryImage[i][j] = IMAGE_BLACK;//0�Ǻ�ɫ  //ͼ��ԭ�㲻��
            else
                BinaryImage[i][j] = IMAGE_WHITE;//1�ǰ�ɫ  //ͼ��ԭ�㲻��
        }
    }
#endif
}

/********************************************************************************************
 ** ��������: ����Ӧ��ֵ��ֵ��ͼ��
 ** ��    ��: uint8* img_data���Ҷ�ͼ��
 **           uint8* output_data����ֵ��ͼ��
 **           int width��ͼ����
 **           int height��ͼ��߶�
 **           int block���ָ�ֲ���ֵ�ķ����С����7*7
 **           uint8 clip_value: �ֲ���ֵ��ȥ�ľ���ֵһ��Ϊ��2~5��
 ** �� �� ֵ: ��
 ** ��    ��: �Ϻ�����16�����ܳ������Ӿ���SJTUAuTop
 **           https://zhuanlan.zhihu.com/p/391051197
 ** ע    �⣺adaptiveThreshold(mt9v03x_image[0],BinaryImage[0],MT9V03X_W,MT9V03X_H,5,1);//����ûd�ø����һ��
 *********************************************************************************************/
void adaptiveThreshold(uint8 *img_data, uint8 *output_data, int width, int height, int block, uint8 clip_value)
{
//  assert(block % 2 == 1); // block����Ϊ����
  int half_block = block / 2;
  for(int y=half_block; y<height-half_block; y++)
  {
    for(int x=half_block; x<width-half_block; x++)
    {
      // ����ֲ���ֵ
      int thres = 0;
      for(int dy=-half_block; dy<=half_block; dy++)
      {
        for(int dx=-half_block; dx<=half_block; dx++)
        {
          thres += img_data[(x+dx)+(y+dy)*width];
        }
      }
      thres = thres / (block * block) - clip_value;
      // ���ж�ֵ��
      output_data[x+y*width] = img_data[x+y*width]>thres ? 255 : 0;
    }
  }
}

/*************************************************************************
 *  �������ƣ�void Get_Use_Image (void)
 *  ����˵����������ͷ�ɼ���ԭʼͼ�����ŵ�����ʶ�������С
 *  ����˵������
 *  �������أ���
 *  �޸�ʱ�䣺2022��6��4��
 *  ��    ע�� IMAGE_COMPRESS_H:Ϊͼ��ѹ��֮��ĸ߶�120/2=60
 *             IMAGE_COMPRESS_W:Ϊͼ��ѹ��֮��ĸ߶�160/2=80
 *             ֡��230��������ѹ����֮���ֵ�������ҹյ�ɼ�ͼ���ʱ��Ϊ2.3~2.5ms��δѹ����8.0ms��֡��400��ʱ��Ϊ5ms
 *             ˳����ѹ����ʱ������˸���ֵ�˲����ԻҶ�ͼ�˲�������ص���Ա��Ǹ���ȥ�����ֵ
 *             ��������ԭ��û̫������Ϊ���������ӣ�����ɾȥ����ʱ����0.2ms
 *************************************************************************/
void Get_Compress_Image(void)
{
    short i = 0, j = 0, row = 0, line = 0;

    for (i = 0; i < MT9V03X_H; i += 2)          //���۸� 120 / 2  = 60��
    {
        for (j = 0; j <= MT9V03X_W; j += 2)     //���ۿ�188 / 2  = 94��
        {
            CompressImage[row][line]=mt9v03x_image[i][j];
//            CompressImage[row][line] = (mt9v03x_image[i][j]+mt9v03x_image[i][j-1])/2;
            line++;
        }
        line = 0;
        row++;
    }
}
