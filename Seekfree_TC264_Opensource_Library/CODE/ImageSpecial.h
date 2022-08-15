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
#include "ImageTack.h"
#include "common.h"

//������ʶ��
#define G_LINEBIAS  0.8     //�ж�������һ�ߵ�ֱ��б��
#define G_ZEBRA_NUM 14      //�����߱�־������ֵ //������n�����ߣ���G_ZEBRA_NUM=2*n����������9�����ߣ�ȡƫСֵ
//�����ж����
#define C_LOSTLINE  35      //������������жϵĶ�����         //Խ������Խ�Ͻ�
#define C_LOSTNUM   50      //�ж��������ڵĶ�����
//ʮ�ֻػ�����
#define L_LOSTNUM   60

/*��������غ���*/
void OutGarage(void);       //����
int64 SobelTest(uint8 starline,uint8 endline,uint8 starcloumn,uint8 endcloumn);      //Sobel���Ӽ��������
uint8 ZebraIndentify(uint8 start_line,uint8 end_line,uint8* black_width);//�ڰ�����Ѱ�Ұ�����
void SeedGrowFindValley_Garage(char Choose,Point Seed,int endline,Point *UpInflectionC,char TRANSVERSE_THR);//��������������ҹȵ�
void SeedGrowFindPeak_Garage(char Choose,Point Seed,int endline,Point *PeakInflection,char TRANSVERSE_THR);//�������������ҵ�ɽ���������溯���෴
uint8 GarageLIdentify(char Choose,Point InflectionL,Point InflectionR);             //�󳵿�ʶ��
uint8 GarageLStatusIdentify(char Choose,Point InflectionL,Point InflectionR,uint8* GarageLFlag);  //�󳵿��״̬��ת��
uint8 LINGarageEntrance(Point InflectionL,Point InflectionR);//�󳵿����ʶ���ߺ���
uint8 LINGarageStatusIdentify(Point InflectionL,Point InflectionR,uint8* GarageLFlag);//��೵�����״̬��
uint8 RNINGarageIdentify(Point InflectionL,Point InflectionR);             //�ҳ��ⲻ���ʶ����
uint8 RNINGarageSpecial(Point InflectionL,Point InflectionR);//��б���ߺ���
uint8 RNINGarageStatusIdentify(Point InflectionL,Point InflectionR,uint8* GarageLFlag);//�����ұ߲����״̬��
uint8 RINGarageEntrance(Point InflectionL,Point InflectionR);//�ҳ������״̬ʶ����
uint8 RINGarageStatusIdentify(Point InflectionL,Point InflectionR,uint8* GarageLFlag);//�ҳ������״̬��
/*������غ���*/
uint8 CircleIslandBegin_L(void);
uint8 CircleIslandOverBegin_L(int *LeftLine);
uint8 CircleIslandEnd_L(void);
uint8 CircleIslandExit_L(Point InflectionL);
uint8 CircleIslandMid_L(void);
uint8 CircleIslandIdentify_L(int *LeftLine,Point InflectionL);   //�󻷵�״̬��
uint8 CircleIslandBegin_R(void);
uint8 CircleIslandOverBegin_R(int *RightLine);
uint8 CircleIslandEnd_R(void);
uint8 CircleIslandExit_R(Point InflectionR);
uint8 CircleIslandMid_R(void);
uint8 CircleIslandIdentify_R(int *RightLine,Point InflectionR);   //�һ���״̬��
/*ʮ�ֻػ���غ���*/
uint8 CrossLoopBegin_L(Point InflectionL,uint8 status);
uint8 CrossLoopEnd_L(uint8 status);
uint8 CrossLoopIdentify_L(Point InflectionL);    //��ʮ�ֻػ�״̬��
uint8 CrossLoopBegin_R(Point InflectionR,uint8 status);
uint8 CrossLoopEnd_R(uint8 status);
uint8 CrossLoopIdentify_R(Point InflectionR);    //��ʮ�ֻػ�״̬��
/*������غ���*/
void ForkFindSpecialPoint(int row,int cloumn,Point *LeftUpPoint);//�������⴦����ֹBias��Ϊɨ�߱���ʹ��Bias��С
void SeedGrowFindUpInflection(char Choose,Point Seed,int endline,Point *UpInflectionC);//�������������������ȵ�
void GetForkUpInflection(Point DownInflectionL,Point DownInflectionR,Point *UpInflectionC);     //��Ѱ�������Ϲյ�
uint8 ForkIdentify(int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR);   //����ʶ��
uint8 ForkTurnRIdentify(int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR);//�������ұ��ߵ�ʶ���ߴ�����
uint8 ForkFStatusIdentify(Point DownInflectionL,Point DownInflectionR,uint8 *NowFlag);           //��һ������״̬��ת�жϺ���
uint8 ForkSStatusIdentify(Point DownInflectionL,Point DownInflectionR,uint8 *NowFlag);           //�ڶ�������״̬��ת�жϺ���
/*ʮ����غ���*/
void GetCrossRoadsUpInflection(Point DownInflectionL,Point DownInflectionR,Point *UpInflectionL,Point *UpInflectionR);//ʮ��·����Ѱ�Ϲյ㺯��
uint8 CrossRoadsIdentify(Point DownInflectionL,Point DownInflectionR);      //ʮ��·��ʶ����
uint8 CrossRoadsStatusIdentify(Point DownInflectionL,Point DownInflectionR);//ʮ��״̬��
/*�����غ���*/
uint8 ZebraCrossingSearch(uint8 start_line,uint8 end_line);
void GarageInBegin(void);
uint8 GarageInEnd(void);
uint8 GarageInIdentify(void);
#endif /* CODE_IMAGESPECIAL_H_ */
