/*
 * Filter.h
 *
 *  Created on: 2022��3��18��
 *      Author: yue
 */

#ifndef CODE_FILTER_H_
#define CODE_FILTER_H_

#include "common.h"

#define FIRST_LAG_P 0.8     //һ���ͺ��˲�ϵ����ԽС�ͺ�Ч��Խǿ


float FirstOrderLagFilter(float value); //һ���ͺ��˲��㷨
int16 SecondOrderLagFilter_L(int16 value);
int16 SecondOrderLagFilter_R(int16 value);


#endif /* CODE_FILTER_H_ */
