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
#include "utils.h"


#define DEBUG
#include "debug.h"


#define	MODE_REALTIME	0x01
#define MODE_LOGGING	0x02
#define MODE_MIXED		0x03


extern CTaskMgr g_TaskMgr;
extern CLog		g_Log;



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
	long lElaps = 0;
	bool bWDI = false;
	float *pfData[MAX_CH];
	int nSSize = m_pCore->m_pAdcConfig->m_nSampleRate;
	int nChSize = m_pCore->m_pAdcConfig->m_nChSize;
	CFeature sFeature;
	
	sFeature.setParam( (void*)m_pCore->m_pAdcConfig->m_fValParams);

	g_Log.setCHSize( nChSize );
	g_Log.setSampleRate( nSSize );
	
	for(int i=0;i<MAX_SERVER;i++) {
		if( m_pConfig->m_SeverCfg[i].d_nValid != 0) {
			sFeature.setSVID( i, m_pConfig->m_SeverCfg[i].d_sSVID);
			string szNames = sFeature.getFeatNames(i);
			g_Log.setParam( i, m_pConfig->m_SeverCfg[i].d_sEqpID, szNames);
			}
		}
	
	DBG_I_N("run : sRate=%d\r\n", nSSize );

	SetRunBit(true);

	while( IsRun() ) {
		if( m_pCore->m_pSensor != NULL ) {
			DBG("1-dsp---------%s, sRate=%d\r\n", GetDateTime3().c_str(), nSSize);			

			m_pCore->m_pSensor->Read(pfData);
			if( !IsRun()) 
				break;
			
			if( bWDI )	{ WDI(0);CPU_LED_R(0);}
			else 		{ WDI(1);CPU_LED_R(1);}
			bWDI = !bWDI;

#if 0
			for(int i=0;i<nSSize/100;i++) {
				DBG("%04d : ", i);
				for(int j=0;j<nChSize/4;j++) {
					DBG("%6.3f, ", pfData[j][i]);
					}
				DBG("\r\n");
				}
#endif			
			
//			DBG("2-dsp---------%s, sRate=%d\r\n", GetDateTime3().c_str(), nSSize);

			if( m_pCore->m_pAdcConfig->m_AdcCfg.d_nMode == MODE_LOGGING) {
				lElaps = GetMicrosecondCount();
				g_Log.putDatas(pfData);
				lElaps = GetMicrosecondCount() - lElaps;
//				DBG_I_C("Log elapsed time=%ld\r\n", lElaps);
				}

			if( m_pCore->m_pAdcConfig->m_AdcCfg.d_nMode == MODE_REALTIME || m_pCore->m_pAdcConfig->m_AdcCfg.d_nMode == MODE_MIXED ) {
				lElaps = GetMicrosecondCount();
				
				sFeature.processingWith(nSSize, nChSize, pfData);
				for(int i=0;i<MAX_SERVER;i++) {
					if( m_pConfig->m_SeverCfg[i].d_nValid != 0) {
						string szFeatrues =	sFeature.getFeatures(i);
						m_pCore->m_pServer[i]->SendFeaturesAll(m_pConfig->m_SeverCfg[i].d_sEqpID, m_pConfig->m_SeverCfg[i].d_nTRID, szFeatrues);
						if( m_pCore->m_pAdcConfig->m_AdcCfg.d_nMode == MODE_MIXED ) {
							string szValues = sFeature.getFeatureValues(i);
							g_Log.writeLog(i, szValues);
							}
						}
					}
				lElaps = GetMicrosecondCount() - lElaps;
				DBG_I_C("features elapsed time=%ld\r\n", lElaps);
				
//				DBG("%s\r\n", szFeatrues.c_str());
				}

			ret++;
			usleep(10);
			}
		else {
			usleep(1000000);
			DBG("no m_pSensor\r\n");
			}
	}

	DBG_I_N("end of loop\r\n");
}



