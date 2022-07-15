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
int64 SobelTest(uint8 starline,uint8 endline,uint8 starcloumn,uint8 endcloumn);      //Sobel���Ӽ��������
uint8 GarageLIdentify(char Choose,Point InflectionL,Point InflectionR);             //�󳵿�ʶ��
uint8 GarageRIdentify(char Choose,Point InflectionL,Point InflectionR);             //�ҳ���ʶ��
uint8 GarageLStatusIdentify(char Choose,Point InflectionL,Point InflectionR,uint8* GarageLFlag);  //�󳵿��״̬��ת��
uint8 GarageROStatusIdentify(Point InflectionL,Point InflectionR,uint8* GarageLFlag);//�����ұ߲����״̬��
uint8 GarageRIStatusIdentify(Point InflectionL,Point InflectionR,uint8* GarageLFlag);//�����ұ����״̬��
void OutGarage(void);       //����
/*������غ���*/
uint8 CircleIslandBegin_L(void);
uint8 CircleIslandOverBegin_L(int *LeftLine,int *RightLine);
uint8 CircleIslandEnd_L(Point InflectionL,Point InflectionR);
uint8 CircleIslandExit_L(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);
uint8 CircleIslandOverExit_L(int *LeftLine,int *RightLine);
uint8 CircleIslandMid_L(int *LeftLine,int *RightLine);
uint8 CircleIslandInside_L(void);
uint8 CircleIslandIdentify_L(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);   //�󻷵�״̬��
uint8 CircleIslandBegin_R(void);
uint8 CircleIslandOverBegin_R(int *LeftLine,int *RightLine);
uint8 CircleIslandEnd_R(Point InflectionL,Point InflectionR);
uint8 CircleIslandExit_R(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);
uint8 CircleIslandOverExit_R(int *LeftLine,int *RightLine);
uint8 CircleIslandMid_R(int *LeftLine,int *RightLine);
uint8 CircleIslandInside_R(void);
uint8 CircleIslandIdentify_R(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);   //�һ���״̬��
/*ʮ�ֻػ���غ���*/
uint8 CrossLoopBegin_L(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);
uint8 CrossLoopOverBegin_L(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);
uint8 CrossLoopEnd_L(void);
uint8 CrossLoopIdentify_L(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);    //��ʮ�ֻػ�״̬��
uint8 CrossLoopBegin_R(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);
uint8 CrossLoopOverBegin_R(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);
uint8 CrossLoopEnd_R(void);
uint8 CrossLoopIdentify_R(int *LeftLine,int *RightLine,Point InflectionL,Point InflectionR);    //��ʮ�ֻػ�״̬��
/*������غ���*/
void SeedGrowFindUpInflection(char Choose,Point Seed,int endline,Point *UpInflectionC);//�������������������ȵ�
void GetForkUpInflection(Point DownInflectionL,Point DownInflectionR,Point *UpInflectionC);     //��Ѱ�������Ϲյ�
uint8 ForkIdentify(int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR);   //����ʶ��
uint8 ForkFStatusIdentify(Point DownInflectionL,Point DownInflectionR,uint8 *NowFlag);           //��һ������״̬��ת�жϺ���
uint8 ForkSStatusIdentify(Point DownInflectionL,Point DownInflectionR,uint8 *NowFlag);           //�ڶ�������״̬��ת�жϺ���
/*ʮ����غ���*/
void GetCrossRoadsUpInflection(Point DownInflectionL,Point DownInflectionR,Point *UpInflectionL,Point *UpInflectionR);//ʮ��·����Ѱ�Ϲյ㺯��
uint8 CrossRoadsIdentify(Point DownInflectionL,Point DownInflectionR);      //ʮ��·��ʶ����
uint8 CrossRoadsStatusIdentify(Point DownInflectionL,Point DownInflectionR);//ʮ��״̬��
#endif /* CODE_IMAGESPECIAL_H_ */
