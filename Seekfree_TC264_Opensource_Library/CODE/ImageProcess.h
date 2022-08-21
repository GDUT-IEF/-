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

#define PROCESS_SPEED_LEN   15
#define PROCESS_ENCODER_LEN 5

extern uint8 CrossRoads_flag;        //ʮ�ֱ�־����
extern uint8 Fork_flag;              //����ʶ��ı�־����
extern uint8 Circle_flag;
extern uint8 Garage_flag;
extern int16 speed_case_1,speed_case_2,speed_case_3,speed_case_4,speed_case_5,speed_case_6,speed_case_7;
extern uint8 bias_startline,bias_endline;        //��̬ǰհ
extern uint8 process_flag;

extern uint8 process_status[15];
extern uint16 process_speed[PROCESS_SPEED_LEN];
extern uint8 process_encoder[PROCESS_ENCODER_LEN];

void ImageProcess(void);//ͼ�����߼�����
void Stop(void);        //ͣ��
void LcdDrawPoint(Point Inflection,uint16 color);
void LcdDrawPoint_V2(uint8 row,uint8 column,uint16 color);
void LcdDrawRow(uint8 row,uint16 color);
void LcdDrawColumn(uint8 column,uint16 color);

#endif /* CODE_IMAGEPROCESS_H_ */
