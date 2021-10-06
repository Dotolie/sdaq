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
//  File         : adcconfig.h
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------


#ifndef __ADCCONFIG_H__
#define __ADCCONFIG_H__

#include <string>

#include "base.h"
#include "struct.h"

#define DEBUG
#include "debug.h"



using namespace std;

typedef struct _ADC_CONFIG_t{
	int d_nType;
	int d_nSRate;
	int d_nVolt;
	int d_nMode;
} ADC_CONFIG_t;

class CAdcConfig : public CBase
{
public:
	int m_nDebug;
	int m_nSampleRate;
	int m_nChSize;
	ADC_CONFIG_t m_AdcCfg;
	fVAL_PARAM_t m_fValParams[MAX_CH];
	
public:
	CAdcConfig();
	~CAdcConfig();

	int Load();
	int Save(int);
	int SetDefault();
	int SavePeriod(int);
	int SetDefaultPeriod();
};
	


#endif	// __ADCCONFIG_H__

