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
//  File         : dsp.cpp
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>
#include <time.h>


#include "stdafx.h"
#include "msg.h"
#include "object.h"
#include "event.h"
#include "taskMgr.h"
#include "base.h"
#include "dsp.h"
#include "core.h"
#include "utils.h"
#include "version.h"
#include "feature.h"
#include "log.h"



#define DEBUG
#include "debug.h"


#define	MODE_REALTIME	0x01
#define MODE_LOGGING	0x02


#define OUT_FOLDER_PATH		"./"

extern CTaskMgr g_TaskMgr;
extern CLog		g_Log;


long int GetMicrosecondCount()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec*1000000 + tv.tv_usec;
}

#define GET_TIME(time, code) { \
        (time) = GetMicrosecondCount(); \
        code \
        (time) = GetMicrosecondCount() - (time);\
    }



CDsp::CDsp(void   *pInst) : Runnable(__func__)
{
	m_pCore = (CCore*)pInst;
	m_pConfig = m_pCore->m_pConfig;


	DBG_I_C("create id=%p\r\n", GetId() );
}

CDsp::~CDsp()
{
	SetRunBit(false);

	DBG_I_C("destroy id=%p\r\n", GetId() );
}

void CDsp::Stop()
{
	SetRunBit(false);
	
	m_pCore->m_pSensor->Signal();
	
//	DBG_I_N("m_bRun=false\r\n");
}

void CDsp::Run()
{
	int ret = 0;
	int nIdx = 0;
	int nLoop = 10;
	float *pfData[MAX_CH];
	int nSSize = m_pConfig->m_nSampleRate;
	CFeature sFeature;

	sFeature.setSVID(m_pConfig->m_DeviceCfg.d_sSVID);
	
	DBG_I_N("run : sRate=%d\r\n", nSSize );

	SetRunBit(true);

	while( IsRun() ) {
		if( m_pCore->m_pSensor != NULL ) {
			DBG("1-dsp---------%s, sRate=%d\r\n", GetDateTime3().c_str(), nSSize);			

			m_pCore->m_pSensor->Read(pfData);
			if( !IsRun()) 
				break;

#if 1
			for(int i=0;i<nSSize/100;i++) {
				DBG("%04d : ", i);
				for(int j=0;j<MAX_CH/4;j++) {
					DBG("%6.3f, ", pfData[j][i]);
					}
				DBG("\r\n");
				}
#endif			
			
			DBG("2-dsp---------%s, sRate=%d\r\n", GetDateTime3().c_str(), nSSize);

			if( m_pConfig->m_DeviceCfg.d_nMode & MODE_REALTIME) {
				sFeature.processingWith(nSSize, pfData);
				string szFeatrues =	sFeature.getFeatures();
				m_pCore->m_pServer->SendFeaturesAll(m_pConfig->m_DeviceCfg.d_sEqpID, m_pConfig->m_DeviceCfg.d_nTRID, szFeatrues);
				DBG("%s\r\n", szFeatrues.c_str());
				}
			
			if( m_pConfig->m_DeviceCfg.d_nMode & MODE_LOGGING) {
				if( nIdx == 0)	{
					g_Log.openFile(LOG_DSP, OUT_FOLDER_PATH, m_pConfig->m_DeviceCfg.d_sEqpID, m_pConfig->m_DeviceCfg.d_sLocation);
					g_Log.writeString(LOG_DSP, makeHeader());
					}

				g_Log.writeData(LOG_DSP, nSSize, pfData);

				nIdx++;
				if( nIdx == nLoop ) {
					nIdx = 0;
					g_Log.closeFile(LOG_DSP);
					}
				}
			ret++;
			usleep(10000);
			}
		else {
			usleep(1000000);
			DBG("no m_pSensor\r\n");
			}
	}




	DBG_I_N("end of loop\r\n");
}

const string CDsp::makeHeader()
{
	int i;
	int nSSize	= m_pConfig->m_nSampleRate;	

	string szTitle = "Version, Start Time, Channel Cnt, Sampling\n";

	string szVersion = "Ver";
	szVersion += VERSION;
	szVersion += ", ";
	szVersion += GetDateTime2();
	szVersion += ", 20, ";
	szVersion += to_string(nSSize);
	szVersion += "\n";


	string szContents = "";
	for(i=0;i<(MAX_CH-1);i++) szContents +="CH" + to_string(i+1) + ",";
	szContents += "CH" + to_string(i+1) + "\n";
	
	string szHeader = szTitle;
	szHeader += szVersion;
	szHeader += szContents;
	
	return szHeader;
}


