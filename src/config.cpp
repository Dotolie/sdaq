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
//  File         : config.cpp
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#include <iostream>
#include <fstream>

#include "struct.h"
#include "config.h"
#include "INIReader.h"

#define DEBUG
#include "debug.h"




#define CONFIG_FILE_NAME		"./FDC_CONFIG.cfg"



CConfig::CConfig() : CBase(__func__)
{
	DBG_I_C("create id=%p, %s\r\n", GetId());
}

CConfig::~CConfig()
{
	DBG_I_C("destroy id=%p\r\n", GetId() );
}

int CConfig::Load()
{
	int nRet=0;
	int nInterval = 0;
	string sSVID;
	
	INIReader *pReader = new INIReader(CONFIG_FILE_NAME);
	if( pReader->ParseError() < 0 ) {
		DBG_E_R("FDC_CONFIG.cfg does't exist\r\n");
		SetDefault();
		Save(0);
		}
	else {
		m_DeviceCfg.d_nInterval = pReader->GetInteger("COMMON", "Interval", 1000);
		m_DeviceCfg.d_nPort = pReader->GetInteger("COMMON", "Port", 9001);
		m_DeviceCfg.d_sDestSubject = pReader->Get("COMMON", "DestSubject", "");
		m_DeviceCfg.d_sReplySubject = pReader->Get("COMMON", "ReplySubject", "");
		m_DeviceCfg.d_sAppName = pReader->Get("COMMON", "AppName", "");
		m_DeviceCfg.d_sProcName = pReader->Get("COMMON", "ProcName", "");
		m_DeviceCfg.d_nSeqNo = pReader->GetInteger("COMMON", "SeqNo", 1);
		m_DeviceCfg.d_sEqpID = pReader->Get("COMMON", "EqpID", "EQ01");
		m_DeviceCfg.d_sModuleID = pReader->Get("COMMON", "ModuleID", "MOD01");
		m_DeviceCfg.d_sToolID = pReader->Get("COMMON", "ToolID", "TOOL01");
		m_DeviceCfg.d_sLocation = pReader->Get("COMMON", "Location", "H1S4");
		m_DeviceCfg.d_nTRID = pReader->GetInteger("COMMON", "TRID", 1);
		m_DeviceCfg.d_nSMPLN = pReader->GetInteger("COMMON", "SMPLN", 1);

		m_DeviceCfg.d_sSVID = pReader->Get("SVID", "SVID", "");

		m_DeviceCfg.d_nMode = pReader->GetReal("ADC", "Mode", 1);
		m_DeviceCfg.d_nSRate = pReader->GetReal("ADC", "SampleRate", 1);		
		m_DeviceCfg.d_fAval = pReader->GetReal("ADC", "Avalue", 5.5);
		m_DeviceCfg.d_fBval = pReader->GetReal("ADC", "Bvalue", 14.85);
		m_DeviceCfg.d_fCval = pReader->GetReal("ADC", "Cvalue", 10);		
		}

	switch(	m_DeviceCfg.d_nSRate ) {
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
			m_nChSize = 4;			
			break;
		default:
			m_nSampleRate = 1024;
			m_nChSize = MAX_CH;			
			break;
		}
#if 0
	DBG("Mode = %d\r\n", m_DeviceCfg.d_nMode);
	DBG("SampleRate = %d\r\n", m_DeviceCfg.d_nSRate);
	DBG("Avalue = %f\r\n", m_DeviceCfg.d_fAval);
	DBG("Bvalue = %f\r\n", m_DeviceCfg.d_fBval);
	DBG("Cvalue = %f\r\n", m_DeviceCfg.d_fCval);

	DBG("nInterval = %d\r\n", m_DeviceCfg.d_nInterval);
	DBG("Port = %d\r\n", m_DeviceCfg.d_nPort);
	DBG("DestSubject = %s\r\n", m_DeviceCfg.d_sDestSubject.c_str());
	DBG("ReplySubject = %s\r\n", m_DeviceCfg.d_sReplySubject.c_str());
	DBG("AppName = %s\r\n", m_DeviceCfg.d_sAppName.c_str());
	DBG("ProcName = %s\r\n", m_DeviceCfg.d_sProcName.c_str());
	DBG("SeqNo = %d\r\n", m_DeviceCfg.d_nSeqNo);
	DBG("EqpID = %s\r\n", m_DeviceCfg.d_sEqpID.c_str());
	DBG("ModuleId = %s\r\n", m_DeviceCfg.d_sModuleID.c_str());
	DBG("ToolID = %s\r\n", m_DeviceCfg.d_sToolID.c_str());
	DBG("Location = %s\r\n", m_DeviceCfg.d_sLocation.c_str());	
	DBG("TRID = %d\r\n", m_DeviceCfg.d_nTRID);
	DBG("SMPLN = %d\r\n", m_DeviceCfg.d_nSMPLN);
	
	DBG("SVID = %s\r\n", m_DeviceCfg.d_sSVID.c_str());
#endif
	delete pReader;
	pReader = NULL;
	
	return nRet;
}


int CConfig::Save(int nEnc)
{
	ofstream outFile;
	stringstream ss;
	string sName;
	
	ss << ";" << std::endl;
	ss << "; Generated FDC_CONFIG.cfg file for sdaq" << std::endl;
	ss << "; " << std::endl;

	ss << "[ADC]" << endl;
	ss << "Mode = " << m_DeviceCfg.d_nMode << endl;
	ss << "SampleRate = " << m_DeviceCfg.d_nSRate << endl;
	ss << "Avalue = " << m_DeviceCfg.d_fAval << endl;
	ss << "Bvalue = " << m_DeviceCfg.d_fBval << endl;
	ss << "Cvalue = " << m_DeviceCfg.d_fCval << endl;

	ss << "[COMMON]" << endl;
	ss << "Interval = "<< m_DeviceCfg.d_nInterval << endl;
	ss << "Port = " << m_DeviceCfg.d_nPort << endl;
	ss << "DestSubject = " << m_DeviceCfg.d_sDestSubject << endl;
	ss << "ReplySubject = " << m_DeviceCfg.d_sReplySubject << endl;
	ss << "AppName = " << m_DeviceCfg.d_sAppName << endl;
	ss << "ProcName = " << m_DeviceCfg.d_sProcName << endl;
	ss << "SeqNo = " << m_DeviceCfg.d_nSeqNo << endl;
	ss << "EqpID = " << m_DeviceCfg.d_sEqpID << endl;
	ss << "ModuleID = " << m_DeviceCfg.d_sModuleID << endl;
	ss << "TooID = " << m_DeviceCfg.d_sToolID << endl;
	ss << "Location = " << m_DeviceCfg.d_sLocation << endl;
	ss << "TRID = " << m_DeviceCfg.d_nTRID << endl;
	ss << "SMPLN = " << m_DeviceCfg.d_nSMPLN << endl;
	
	ss << "[SVID]" << endl;
	ss << "SVID = " << m_DeviceCfg.d_sSVID << endl;

	
	ss << endl;

	outFile.open(CONFIG_FILE_NAME);
	outFile << ss.rdbuf();
	outFile.close();


	DBG_I_N("\r\n");
	
	return 0;
}


int CConfig::SetDefault()
{
	m_DeviceCfg.d_nMode = 1;	// Realtime
	m_DeviceCfg.d_nSRate = 1;	// 1024Hz
	m_DeviceCfg.d_fAval = 5.5;	// ((Val x A)- B) x C
	m_DeviceCfg.d_fBval = 14.85;
	m_DeviceCfg.d_fCval = 10.0;

	m_DeviceCfg.d_nInterval = 1000;
	m_DeviceCfg.d_nPort = 9001;
	m_DeviceCfg.d_sDestSubject = "Photo";
	m_DeviceCfg.d_sReplySubject = "DAQ";
	m_DeviceCfg.d_sAppName = "sDaq";
	m_DeviceCfg.d_sProcName = "NONSECS";
	m_DeviceCfg.d_nSeqNo = 1;
	m_DeviceCfg.d_sEqpID = "EQ100";
	m_DeviceCfg.d_sModuleID = "MOD01";
	m_DeviceCfg.d_sToolID = "TOOL02";
	m_DeviceCfg.d_sLocation = "H1S4";
	m_DeviceCfg.d_nTRID = 1;
	m_DeviceCfg.d_nSMPLN = 1;

	m_DeviceCfg.d_sSVID = "L_GAP_SENSOR_A=1000001=1=1";
	
	DBG_I_N("\r\n");

	return 0;
}


