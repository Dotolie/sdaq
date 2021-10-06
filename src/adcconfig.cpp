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
//  File         : adcconfig.cpp
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#include <iostream>
#include <fstream>

#include "struct.h"
#include "adcconfig.h"
#include "INIReader.h"

#define DEBUG
#include "debug.h"



#define ADC_CONF_FILE_NAME		"./adc.ini"


CAdcConfig::CAdcConfig() : CBase(__func__)
{
	DBG_I_C("create id=%p\r\n", GetId());
}

CAdcConfig::~CAdcConfig()
{
	DBG_I_C("destroy id=%p\r\n", GetId() );
}

int CAdcConfig::Load()
{
	int nRet=0;
	int nInterval = 0;
	string sSVID;
	
	INIReader *pReader = new INIReader(ADC_CONF_FILE_NAME);
	if( pReader->ParseError() < 0 ) {
		DBG_E_R("adc.ini does't exist\r\n");
		SetDefault();
		Save(0);
		}
	else {
		m_AdcCfg.d_nType = pReader->GetInteger("ADC", "Type", 1);
		m_AdcCfg.d_nSRate = pReader->GetInteger("ADC", "SampleRate", 1);
		m_AdcCfg.d_nVolt = pReader->GetInteger("ADC", "VoltRange", 4);
		m_AdcCfg.d_nMode = pReader->GetInteger("ADC", "Mode", 1);

		for(int i=0;i<MAX_CH;i++) {
			string szSection = "ch" + to_string(i+1);
			m_fValParams[i].d_bEnableAvg = pReader->GetBoolean(szSection, "EnableAvg", false);
			m_fValParams[i].d_fAvalue = pReader->GetReal(szSection, "Avalue", 1);
			m_fValParams[i].d_fBvalue = pReader->GetReal(szSection, "Bvalue", 0);
			m_fValParams[i].d_fCvalue = pReader->GetReal(szSection, "Cvalue", 1);		
			}
		}

	switch(	m_AdcCfg.d_nSRate ) {
		case SRATE_1024:
			m_nSampleRate = 1024;
			m_nChSize = MAX_CH;
			break;
		case SRATE_2048:
			m_nSampleRate = 2048;
			m_nChSize = MAX_CH;
			break;
		case SRATE_4096:
			m_nSampleRate = 4096;
			m_nChSize = MAX_CH;	
			break;
		case SRATE_8192:
			m_nSampleRate = 8192;
			m_nChSize = MAX_CH;
			break;
		case SRATE_65536:
			m_nSampleRate = 65536;
			m_nChSize = MAX_HSPEED_CH;			
			break;
		default:
			m_nSampleRate = 1024;
			m_nChSize = MAX_CH;			
			break;
		}
#if 0
	DBG("Mode = %d\r\n", m_AdcCfg.d_nMode);
	DBG("SampleRate = %d\r\n", m_AdcCfg.d_nSRate);
	DBG("Type = %d\r\n", m_AdcCfg.d_nType);
	DBG("VoltRange = %d\r\n", m_AdcCfg.d_nVolt);
#endif

	delete pReader;
	pReader = NULL;
	
	return nRet;
}


int CAdcConfig::Save(int nEnc)
{
	ofstream outFile;
	stringstream ss;
	string sName;
	
	ss << ";" << endl;
	ss << "; Generated adc.ini file for sdaq" << endl;
	ss << "; " << endl;
	ss << "; Type : 1 (diffrential only)" << endl;
	ss << "; SampleRate : 1 (1024), 2 (2048), 3 (4096), 4 (8192), 5 (65536) Hz" << endl;
	ss << "; VoltRange : 1 (6to6), 4 (12to12), 7 (24to24)" << endl;
	ss << "; Mode : 1 (measure), 2 (logging), 3 (meas + log)" << endl;
	ss << "; " << endl;
	ss << "; EnableAvg :0 (x'=x), 1 (x'=x-Avg)" << endl;
	ss << "; f(x) = Cvalue*((Avalue*x)-Bvalue)" << endl;
	ss << "; " << endl;
	ss << endl;

	ss << "[ADC]" << endl;
	ss << "Type = " << m_AdcCfg.d_nType << endl;
	ss << "SampleRate = " << m_AdcCfg.d_nSRate << endl;
	ss << "VoltRange = " << m_AdcCfg.d_nVolt << endl;
	ss << "Mode = " << m_AdcCfg.d_nMode << endl;
	ss << endl;

	for(int i=0;i<MAX_CH;i++) {
		string szSection = "[ch" + to_string(i+1) + "]";
		ss << szSection << endl;
		ss << "EnableAvg = " << m_fValParams[i].d_bEnableAvg << endl;
		ss << "Avalue = " << m_fValParams[i].d_fAvalue << endl;
		ss << "Bvalue = " << m_fValParams[i].d_fBvalue << endl;
		ss << "Cvalue = " << m_fValParams[i].d_fCvalue << endl;
		ss << endl;
		}


	outFile.open(ADC_CONF_FILE_NAME);
	outFile << ss.rdbuf();
	outFile.close();


	DBG_I_N("\r\n");
	
	return 0;
}


int CAdcConfig::SetDefault()
{
	m_AdcCfg.d_nType = 1;	// diffrential only
	m_AdcCfg.d_nSRate = 1;	// 1024Hz	
	m_AdcCfg.d_nVolt = 4;	// 12to12
	m_AdcCfg.d_nMode = 1;	// Realtime

	for(int i=0;i<MAX_CH;i++) {
		m_fValParams[i].d_bEnableAvg = false;
		m_fValParams[i].d_fAvalue = 1;
		m_fValParams[i].d_fBvalue = 0;
		m_fValParams[i].d_fCvalue = 1;
		}
	
	DBG_I_N("\r\n");

	return 0;
}


