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
//  File         : config.h
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------


#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>

#include "base.h"


#define DEBUG
#include "debug.h"



using namespace std;


typedef struct _CONFIG_t{
	int d_nType;
	int d_nSRate;
	int d_nVolt;
	int d_nMode;
	
	float d_fAval;
	float d_fBval;
	float d_fCval;

	int d_nInterval;
	int d_nPort;
	string d_sDestSubject;
	string d_sReplySubject;
	string d_sAppName;
	string d_sProcName;
	int d_nSeqNo;
	string d_sEqpID;
	string d_sModuleID;
	string d_sToolID;
	string d_sLocation;
	int d_nTRID;
	int d_nSMPLN;

	string d_sSVID;
} CONFIG_t;

class CConfig : public CBase
{
public:
	int m_nDebug;
	int m_nSampleRate;
	int m_nChSize;
	CONFIG_t m_DeviceCfg;	

public:
	CConfig();
	~CConfig();

	int Load();
	int Save(int);
	int SetDefault();
	int SavePeriod(int);
	int SetDefaultPeriod();
};
	


#endif	// __CONFIG_H__

