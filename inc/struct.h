//------------------------------------------------------------------------------
//
//  Copyright (C) 2008 MVTech Co., Ltd. All Rights Reserved
//  MVTech Co.,Ltd. Proprietary & Confidential
//
//  Reproduction in whole or in part is prohibited without the written consent 
//  of the copyright owner. MVTech reserves the right to make changes 
//  without notice at any time. MVTech makes no warranty, expressed, 
//  implied or statutory, including but not limited to any implied warranty of 
//  merchantability or fitness for any particular purpose, or that the use will
//  not infringe any third party patent,  copyright or trademark. 
//  MVTech must not be liable for any loss or damage arising from its use.
//
//  Module       :
//  File         : struct.h
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------


#ifndef __STRUCT_H__
#define __STRUCT_H__



#define MAX_PG		2
#define MAX_CH		20
#define MAX_RATE	65536

enum {
	SRATE_1024 = 1,
	SRATE_2048,
	SRATE_4096,
	SRATE_8192,
	SRATE_65536,

	SRATE_MAX,
};

enum {
	ADC_CH0 = 0,
	ADC_CH1,
	ADC_CH2,
	ADC_CH3,
	ADC_CH4,
	ADC_CH5,
	ADC_CH6,
	ADC_CH7,
	ADC_CH8,
	ADC_CH9,
	ADC_CH10,
	ADC_CH11,
	ADC_CH12,
	ADC_CH13,
	ADC_CH14,
	ADC_CH15,
	ADC_CH16,
	ADC_CH17,
	ADC_CH18,
	ADC_CH19,

	ADC_MAX,
};


#define WDI(x)					system("echo " #x " > /sys/class/leds/wdi/brightness")
#define PWR_LED_G(x)			system("echo " #x " > /sys/class/leds/pwr_led1/brightness")
#define PWR_LED_R(x)			system("echo " #x " > /sys/class/leds/pwr_led2/brightness")
#define CPU_LED_G(x)			system("echo " #x " > /sys/class/leds/cpu_led1/brightness")
#define CPU_LED_R(x)			system("echo " #x " > /sys/class/leds/cpu_led2/brightness")
#define WIF_LED_G(x)			system("echo " #x " > /sys/class/leds/wifi_led1/brightness")
#define WIF_LED_R(x)			system("echo " #x " > /sys/class/leds/wifi_led2/brightness")



#endif /* __STRUCT_H__ */

