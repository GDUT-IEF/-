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
extern uint8 CrossLoop_flag;         //ʮ�ֻػ���־����
extern uint8 speed_case_1,speed_case_2,speed_case_3,speed_case_4,speed_case_5,speed_case_6,speed_case_7;

void Stop(void);
void LcdDrawPoint(Point Inflection,uint16 color);
void ImageProcess(void);//ͼ�����߼�����

#endif /* CODE_IMAGEPROCESS_H_ */
