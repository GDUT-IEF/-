/*
 * ICM20602.c
 *
 *  Created on: 2022��5��19��
 *      Author: yue
 */

#include "ICM20602.h"
#include "SEEKFREE_IIC.h"
#include "SEEKFREE_ICM20602.h"
#include "zf_ccu6_pit.h"
#include "Filter.h"

/*
 ** ��������: ��ȡICM20602Z����ٶ�
 ** ��    ��: ��
 ** �� �� ֵ: Z����ٶ�
 ** ��    ��: WBN
 ** ע    �⣺����ɷ�װ���޸ģ�ֱ��ֻ��ICM��Ӧ�Ĵ����ж�ȡZ����ٶȵ�ֵ����ʡ����Ҫ���˷�
 */
int16 GetICM20602Gyro_Z(void)
{
    uint8 dat[2];
    int16 gyro_z;   //Z����ٶ�

    simiic_read_regs(ICM20602_DEV_ADDR, ICM20602_GYRO_ZOUT_H, dat, 2, SIMIIC);
    gyro_z = (int16)(((uint16)dat[0]<<8 | dat[1]));

    return gyro_z;
}

/*
 ** ��������: ��ICM20602Z����нǶȻ���
 ** ��    ��: flag���Ƿ��������   1���������    0�������ϴεĻ���
 ** �� �� ֵ: Z����ֵĽǶ�
 ** ��    ��: WBN
 */
float GetICM20602Angle_Z(uint8 flag)
{
    static float my_angle_z;   //Z��Ƕ�

    if(flag==1) //����֮ǰ�Ļ���
    {
        my_angle_z=0;
        return 0;
    }

    int16 my_gyro_z=GetICM20602Gyro_Z();                //��ȡZ����ٶ�
    my_gyro_z=kalman1_filter(&kalman_gyro, my_gyro_z);  //�˲�
    my_angle_z+=0.00012480f*my_gyro_z;                  //����

    return my_angle_z;
}

/*
 ** ��������: �ӵ�ǰλ�ÿ�����ICM20602����Ŀ�����
 ** ��    ��: target_angle��Ŀ��Ƕ�
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 */
void StartIntegralAngle_Z(float target_angle)
{
    icm_target_angle_z=target_angle;        //����Ŀ��Ƕ�
    icm_angle_z_flag=0;                     //����Ŀ��flag=0
    GetICM20602Angle_Z(1);                  //��������
    pit_enable_interrupt(CCU6_1, PIT_CH0);  //�����ж�
}

/*
 ** ��������: ��ICM20602������̬����ó�ŷ����
 ** ��    ��: ��
 ** �� �� ֵ: ��
 ** ��    ��: WBN
 */
void GetICM20602Eulerian(void)
{
    get_icm20602_accdata();  //��ȡ���ٶȼƵ�ֵ
    get_icm20602_gyro();   //��ȡ�����ǵ�ֵ
    IMU_quaterToEulerianAngles(); //����ŷ����
}
