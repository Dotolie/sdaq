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

#define CONFIG_FILE_NAME_1		"./FDC_CONFIG1.cfg"
#define CONFIG_FILE_NAME_2		"./FDC_CONFIG2.cfg"
#define CONFIG_FILE_NAME_3		"./FDC_CONFIG3.cfg"
#define CONFIG_FILE_NAME_4		"./FDC_CONFIG4.cfg"
#define CONFIG_FILE_NAME_5		"./FDC_CONFIG5.cfg"


static char *szFName[MAX_HSPEED_CH] ={ CONFIG_FILE_NAME_1, 
										CONFIG_FILE_NAME_2,
										CONFIG_FILE_NAME_3,
										CONFIG_FILE_NAME_4,
										CONFIG_FILE_NAME_5 };


CConfig::CConfig() : CBase(__func__)
{
	DBG_I_C("create id=%p\r\n", GetId());
}

CConfig::~CConfig()
{
	DBG_I_C("destroy id=%p\r\n", GetId() );
}

int CConfig::Load()
{
	int nRet=0;
	int nIdx=0;
	int nInterval = 0;
	string sSVID;

	for(int i=0;i<MAX_HSPEED_CH;i++) {
		INIReader *pReader = new INIReader(szFName[i]);
		if( pReader->ParseError() < 0 ) {
			DBG_E_R("%s does't exist\r\n", szFName[i]);
			m_SeverCfg[i].d_nValid = 0x0;
			SetDefault(i);
			Save(i);
			}
		else {
			m_SeverCfg[i].d_nValid = 0xaf01af01;
			m_SeverCfg[i].d_nInterval = pReader->GetInteger("FDC", "Interval", 1000);
			m_SeverCfg[i].d_nPort = pReader->GetInteger("FDC", "Port", 9001);
			m_SeverCfg[i].d_sDestSubject = pReader->Get("FDC", "DestSubject", "");
			m_SeverCfg[i].d_sReplySubject = pReader->Get("FDC", "ReplySubject", "");
			m_SeverCfg[i].d_sAppName = pReader->Get("FDC", "AppName", "");
			m_SeverCfg[i].d_sProcName = pReader->Get("FDC", "ProcName", "");
			m_SeverCfg[i].d_nSeqNo = pReader->GetInteger("FDC", "SeqNo", 1);
			m_SeverCfg[i].d_sEqpID = pReader->Get("FDC", "EqpID", "EQ01");
			m_SeverCfg[i].d_sModuleID = pReader->Get("FDC", "ModuleID", "MOD01");
			m_SeverCfg[i].d_sToolID = pReader->Get("FDC", "ToolID", "TOOL01");
			m_SeverCfg[i].d_sLocation = pReader->Get("FDC", "Location", "H1S4");
			m_SeverCfg[i].d_nTRID = pReader->GetInteger("FDC", "TRID", 1);
			m_SeverCfg[i].d_nSMPLN = pReader->GetInteger("FDC", "SMPLN", 1);

			m_SeverCfg[i].d_sSVID = pReader->Get("SVID", "SVID", "");
#if 0
			DBG("nCh = %d\r\n", i );
			DBG("nValid = 0x%x\r\n", m_SeverCfg[i].d_nValid);
			DBG("nInterval = %d\r\n", m_SeverCfg[i].d_nInterval);
			DBG("Port = %d\r\n", m_SeverCfg[i].d_nPort);
			DBG("DestSubject = %s\r\n", m_SeverCfg[i].d_sDestSubject.c_str());
			DBG("ReplySubject = %s\r\n", m_SeverCfg[i].d_sReplySubject.c_str());
			DBG("AppName = %s\r\n", m_SeverCfg[i].d_sAppName.c_str());
			DBG("ProcName = %s\r\n", m_SeverCfg[i].d_sProcName.c_str());
			DBG("SeqNo = %d\r\n", m_SeverCfg[i].d_nSeqNo);
			DBG("EqpID = %s\r\n", m_SeverCfg[i].d_sEqpID.c_str());
			DBG("ModuleId = %s\r\n", m_SeverCfg[i].d_sModuleID.c_str());
			DBG("ToolID = %s\r\n", m_SeverCfg[i].d_sToolID.c_str());
			DBG("Location = %s\r\n", m_SeverCfg[i].d_sLocation.c_str());	
			DBG("TRID = %d\r\n", m_SeverCfg[i].d_nTRID);
			DBG("SMPLN = %d\r\n", m_SeverCfg[i].d_nSMPLN);
			
			DBG("SVID = %s\r\n", m_SeverCfg[i].d_sSVID.c_str());
#endif
			}
		
		delete pReader;
		pReader = NULL;
		}

	return nRet;
}


int CConfig::Save(int nIdx)
{
	ofstream outFile;
	stringstream ss;
	string sName;
	
	ss << ";" << endl;
	ss << "; Generated FDC_CONFIG.cfg file for sdaq" << endl;
	ss << "; " << endl;
	ss << "; SVID = (label)=(uid)=(chNo)=(featNo)" << endl;
	ss << "; chNO : 1 ~ 20 " << endl;
	ss << "; featNo : 1 (min), 2 (max), 3 (avg), 4 (sum), 5 (p2p)" << endl;
	ss << "; " << endl;
	ss << endl;

	ss << "[FDC]" << endl;
	ss << "Interval = "<< m_SeverCfg[nIdx].d_nInterval << endl;
	ss << "Port = " << m_SeverCfg[nIdx].d_nPort << endl;
	ss << "DestSubject = " << m_SeverCfg[nIdx].d_sDestSubject << endl;
	ss << "ReplySubject = " << m_SeverCfg[nIdx].d_sReplySubject << endl;
	ss << "AppName = " << m_SeverCfg[nIdx].d_sAppName << endl;
	ss << "ProcName = " << m_SeverCfg[nIdx].d_sProcName << endl;
	ss << "SeqNo = " << m_SeverCfg[nIdx].d_nSeqNo << endl;
	ss << "EqpID = " << m_SeverCfg[nIdx].d_sEqpID << endl;
	ss << "ModuleID = " << m_SeverCfg[nIdx].d_sModuleID << endl;
	ss << "TooID = " << m_SeverCfg[nIdx].d_sToolID << endl;
	ss << "Location = " << m_SeverCfg[nIdx].d_sLocation << endl;
	ss << "TRID = " << m_SeverCfg[nIdx].d_nTRID << endl;
	ss << "SMPLN = " << m_SeverCfg[nIdx].d_nSMPLN << endl;
	ss << endl;
	
	ss << "[SVID]" << endl;
	ss << "SVID = " << m_SeverCfg[nIdx].d_sSVID << endl;
	ss << endl;

	outFile.open(CONFIG_FILE_NAME);
	outFile << ss.rdbuf();
	outFile.close();


	DBG_I_N("\r\n");
	
	return 0;
}


int CConfig::SetDefault(int nIdx)
{
	m_SeverCfg[nIdx].d_nInterval = 1000;
	m_SeverCfg[nIdx].d_nPort = 9001;
	m_SeverCfg[nIdx].d_sDestSubject = "Photo";
	m_SeverCfg[nIdx].d_sReplySubject = "DAQ";
	m_SeverCfg[nIdx].d_sAppName = "sDaq";
	m_SeverCfg[nIdx].d_sProcName = "NONSECS";
	m_SeverCfg[nIdx].d_nSeqNo = 1;
	m_SeverCfg[nIdx].d_sEqpID = "EQ100";
	m_SeverCfg[nIdx].d_sModuleID = "MOD01";
	m_SeverCfg[nIdx].d_sToolID = "TOOL01";
	m_SeverCfg[nIdx].d_sLocation = "H1S1";
	m_SeverCfg[nIdx].d_nTRID = 1;
	m_SeverCfg[nIdx].d_nSMPLN = 1;

	m_SeverCfg[nIdx].d_sSVID = "L_GAP_SENSOR_A=1000000011=1=1";
	
	DBG_I_N("\r\n");

	return 0;
}


