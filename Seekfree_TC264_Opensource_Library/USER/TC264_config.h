/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：三群：824575535
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		isr
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看doc内version文件 版本说明
 * @Software 		ADS v1.2.2
 * @Target core		TC264D
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-3-23
 ********************************************************************************************************************/

#ifndef _TC264_config_h
#define _TC264_config_h

#define AURIX_MCU_FREQUENCY         200*1000*1000//设置单片机频率为200M 200M(默认频率)

//-------------------------------------------------------------------------------------------------------------------
//  PRINTF_ENABLE 为 0               不使用printf串口打印函数及不输出调试信息，则不会初始化相应串口
//  PRINTF_ENABLE 为 1               使用printf串口打印函数及输出调试信息，则会初始化相应串口，这里定义为1之后，无需自己手动初始化串口
//-------------------------------------------------------------------------------------------------------------------
#define PRINTF_ENABLE               1


#if(1 == PRINTF_ENABLE)

//-------------------------------------------------------------------------------------------------------------------
//printf串口打印函数所使用的串口、串口波特率、串口引脚宏定义
//-------------------------------------------------------------------------------------------------------------------
#define DEBUG_UART                  UART_2
#define DEBUG_UART_BAUD             115200
#define DEBUG_UART_TX_PIN           UART0_TX_P14_0
#define DEBUG_UART_RX_PIN           UART0_RX_P14_1

//-------------------------------------------------------------------------------------------------------------------
//  DEBUG_INFO 为 0                  不输出断言信息
//  DEBUG_INFO 为 1                  输出断言信息，断言信息包含错误所在文件及所在行  需要PRINTF宏定义为1才能有效输出
//-------------------------------------------------------------------------------------------------------------------
#define DEBUG_PRINTF                1

#endif





#endif
