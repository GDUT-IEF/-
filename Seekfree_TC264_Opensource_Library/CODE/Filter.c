/*
 * Filter.c
 *
 *  Created on: 2022��3��18��
 *      Author: yue
 */

#include "Filter.h"


/*
 *******************************************************************************************
 ** ��������: һ���ͺ��˲��㷨
 ** ��    ��: value����Ҫ�����˲���ֵ
 ** �� �� ֵ: �˲����ֵ
 ** ��    ��: WBN
 ********************************************************************************************
 */
float FirstOrderLagFilter(float value)
{
    static float last_value;  //��һ���˲����

    value=FIRST_LAG_P*value+(1-FIRST_LAG_P)*last_value; //һ���ͺ��˲�
    last_value=value;   //����˴��˲����Ϊ��һ���˲����

    return value;
}


