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
//  File         : core.cpp
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <string.h>
#include <string>


#include "object.h"
#include "taskMgr.h"
#include "timerMgr.h"
#include "core.h"
#include "INIReader.h"
#include "utils.h"
#include "version.h"
#include "server.h"
#include "dsp.h"
#include "ipsetup.h"


#define DEBUG
#include "debug.h"

#define VERSION_FILE_NAME		"/tmp/sdaq"


#define	TIMER_CORE_ID_0 		(TASK_ID_CORE + 1)
#define	TIMER_CORE_ID_1 		(TASK_ID_CORE + 2)


using namespace std;


extern CTimerMgr g_TimerMgr;
extern CTaskMgr  g_TaskMgr;


CCore::CCore() : CBase(__func__)
{
	DBG_I_C("create id=%p\r\n", GetId());

	
	g_TaskMgr.AddTask(TASK_ID_CORE, this);
}

CCore::~CCore() 
{
	g_TaskMgr.DelTask(TASK_ID_CORE);


	DBG_I_C("destroy id=%p\r\n", GetId() );
}

int CCore::MsgProc(int message, long wparam, long lparam)
{
	int nRet = 0;
	
	switch(message) {
	case MSG_CREATE:
		nRet = On_MSG_CREATE( wparam, lparam);
		break;
	case MSG_TIMER:
		nRet = On_MSG_TIMER( wparam, lparam);
		break;
	case MSG_QUIT:
		nRet = On_MSG_QUIT( wparam, lparam);
		break;
	default:
		break;
		}

	return nRet;
}

int CCore::On_MSG_CREATE( long wparam, long lparam )
{
	int nRet = -1;
	int nIdx = 0;
	CIpsetup iIpsetup;

	
	DBG_I_Y("w=%ld, l=%ld\r\n", wparam, lparam);

	SaveVersionNo();


	m_pConfig = new CConfig();
	nRet = m_pConfig->Load();

	for(int i=0;i<MAX_SERVER;i++) {
		if( m_pConfig->m_SeverCfg[i].d_nValid != 0) {
			DBG_I_G("m_SeverCfg[%d].d_nValid=%x\r\n", i, m_pConfig->m_SeverCfg[i].d_nValid);
			
			m_pServer[i] = new CServer((void*)&m_pConfig->m_SeverCfg[i]);
			m_pThreadServer[i] = new Thread( m_pServer[i] );
			m_pThreadServer[i]->Start();
			}
		}
	
	m_pAdcConfig = new CAdcConfig;
	nRet = m_pAdcConfig->Load();

	m_pSensor = new CSensor((void*)m_pAdcConfig);
	m_pThreadSensor = new Thread( m_pSensor );
	m_pThreadSensor->Start();
		
	m_pThreadDsp = new Thread( new CDsp(this));
	m_pThreadDsp->Start();



//	g_TimerMgr.addTimer(TASK_ID_CORE, 1000, TIMER_CORE_ID_0, 0);

_err:
		
	return nRet;
}

int CCore::On_MSG_TIMER( long wparam, long lparam )
{
	int nRet = 0;
	string str;

	
	DBG_I_C("uid=0x%lx,%s\r\n", wparam, GetDateTime2().c_str());
#if 0	
	switch(wparam) {
		case TIMER_CORE_ID_0:
			str = "<Msg><Command>NONSECSCHECK</Command><DataLayer>";
			str += "<TimeStamp>" + GetDateTime2() + "</TimeStamp>";
			str += "<EqpID>" + m_pConfig->m_SeverCfg.d_sEqpID + "</EqpID>";
			str += "<ModuleID>" + m_pConfig->m_SeverCfg.d_sModuleID + "</ModuleID>";
			str += "<ToolID>" + m_pConfig->m_SeverCfg.d_sToolID + "</ToolID>";
			str += "</DataLayer></Msg>\r\n";
			m_pServer->SendAll(str);	
			break;
		}
#endif		
	return 0;
}

int CCore::On_MSG_QUIT( long wparam, long lparam )
{
	DBG_I_P("w=%ld, l=%ld\r\n", wparam, lparam);

//	g_TimerMgr.delTimer( TIMER_CORE_ID_0);
//	g_TimerMgr.delTimer( TIMER_CORE_ID_1);


	m_pThreadDsp->Stop();
	m_pThreadSensor->Stop();

	for(int i=0;i<MAX_SERVER;i++) {
		if( m_pConfig->m_SeverCfg[i].d_nValid != 0) {
			m_pThreadServer[i]->Stop();
			delete m_pThreadServer[i];
			m_pThreadServer[i] = NULL;
			}
		}
	
	delete m_pThreadSensor;

	delete m_pConfig;

	delete m_pAdcConfig;

	return 0;
}

CConfig *CCore::GetConfig()
{
	return m_pConfig;
}


void CCore::SaveVersionNo()
{
	ofstream outFile;
	stringstream ss;

	ss << VERSION << std::endl;

	outFile.open(VERSION_FILE_NAME);
	outFile << ss.rdbuf();
	outFile.close();
}
  
