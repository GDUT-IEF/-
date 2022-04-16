/*
 * ImageProcess.h
 *
 * Created on: 2022��3��29��
 * Author: 30516
 * Effect: Image element processing logic
 */

#ifndef CODE_IMAGEPROCESS_H_
#define CODE_IMAGEPROCESS_H_

#include "ImageBasic.h"     //ɨ�ߡ���Ѱ�����¹յ�
#include "ImageTack.h"      //����ƫ�����
#include "ImageSpecial.h"   //Ԫ��ʶ��

extern uint8 CrossRoads_flag;        //ʮ�ֱ�־����
extern uint8 Fork_flag;              //����ʶ��ı�־����
extern uint8 CircleIsland_flag;      //������־����

//���ذ�LED��ɫ����
#define LED_WHITE    P23_1
#define LED_GREEN    P22_1
#define LED_BLUE     P22_2
#define LED_RED      P22_3
#define LED_YELLOW   P21_2

void ImageProcess(void);//ͼ�����߼�����

#endif /* CODE_IMAGEPROCESS_H_ */
