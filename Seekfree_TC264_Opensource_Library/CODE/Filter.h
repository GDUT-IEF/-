/*
 * Filter.h
 *
 *  Created on: 2022年3月18日
 *      Author: yue
 */

#ifndef CODE_FILTER_H_
#define CODE_FILTER_H_


#define FIRST_LAG_P 0.6     //一阶滞后滤波系数，越小滞后效果越强

float FirstOrderLagFilter(float value); //一阶滞后滤波算法


#endif /* CODE_FILTER_H_ */
