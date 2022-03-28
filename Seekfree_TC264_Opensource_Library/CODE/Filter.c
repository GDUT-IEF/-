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

/*
 *******************************************************************************************
 ** ��������: �����ͺ��˲��㷨
 ** ��    ��: value����Ҫ�����˲���ֵ
 ** �� �� ֵ: �˲����ֵ
 ** ��    ��: WBN
 ********************************************************************************************
 */
int16 SecondOrderLagFilter_L(int16 value)
{
    static int16 last_value,last_2_value;

    value=0.2*value+0.4*last_value+0.4*last_2_value;     //�����ͺ��˲�

    last_2_value=last_value;
    last_value=value;

    return value;
}

/*
 *******************************************************************************************
 ** ��������: �����ͺ��˲��㷨
 ** ��    ��: value����Ҫ�����˲���ֵ
 ** �� �� ֵ: �˲����ֵ
 ** ��    ��: WBN
 ********************************************************************************************
 */
int16 SecondOrderLagFilter_R(int16 value)
{
    static int16 last_value,last_2_value;

    value=0.2*value+0.4*last_value+0.4*last_2_value;     //�����ͺ��˲�

    last_2_value=last_value;
    last_value=value;

    return value;
}
