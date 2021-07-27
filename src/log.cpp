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
//  File         : log.cpp
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>


#include "object.h"
#include "taskMgr.h"
#include "timerMgr.h"
#include "utils.h"
#include "struct.h"
#include "log.h"


#define DEBUG
#include "debug.h"


#define OUT_FOLDER_PATH		"./"



#define	TIMER_LOG_ID_0 		(TASK_ID_LOG + 1)
#define	TIMER_LOG_ID_1 		(TASK_ID_LOG + 2)



using namespace std;


extern CTimerMgr g_TimerMgr;
extern CTaskMgr  g_TaskMgr;

CLog g_Log;


CLog::CLog() : CBase(__func__)
{
	DBG_I_C("create id=%p\r\n", GetId());

	
	g_TaskMgr.AddTask(TASK_ID_LOG, this);
}

CLog::~CLog() 
{
	g_TaskMgr.DelTask(TASK_ID_LOG);


	DBG_I_C("destroy id=%p\r\n", GetId() );
}

int CLog::MsgProc(int message, long wparam, long lparam)
{
	int nRet = 0;
	
	switch(message) {
	case MSG_CREATE:
		nRet = On_MSG_CREATE( wparam, lparam);
		break;
	case MSG_TIMER:
		nRet = On_MSG_TIMER( wparam, lparam);
		break;
	case MSG_EVENT:
		nRet = On_MSG_EVENT( wparam, lparam);
		break;
	case MSG_QUIT:
		nRet = On_MSG_QUIT( wparam, lparam);
		break;
	default:
		break;
		}

	return nRet;
}

int CLog::On_MSG_CREATE( long wparam, long lparam )
{
	int nRet = 0;
	int nIdx = 0;

	
	DBG_I_Y("w=%d, l=%d\r\n", wparam, lparam);


//	g_TimerMgr.addTimer(TASK_ID_LOG, 10000, TIMER_LOG_ID_0, 0);

_err:
		
	return nRet;
}

int CLog::On_MSG_TIMER( long wparam, long lparam )
{
	int nRet = 0;
	
	DBG_I_C("uid=%d,%s\r\n", wparam, GetDateTime2().c_str());

	switch(wparam) {
		case 1:
			
			break;
		}
	
	return 0;
}

int CLog::On_MSG_EVENT( long wparam, long lparam )
{
	int nRet = 0;
	
	DBG_I_C("uid=%d,%s\r\n", wparam, GetDateTime2().c_str());

		
	return 0;
}

int CLog::On_MSG_QUIT( long wparam, long lparam )
{
	DBG_I_P("w=%d, l=%d\r\n", wparam, lparam);

//	closeFile(0);
	closeFile(1);
//	g_TimerMgr.delTimer( TIMER_LOG_ID_0);


	return 0;
}


void CLog::openFile(   int nIdx, string szFolder, string &szEqpId, string &szLocation)
{
	string sFileName;
	string sPre = szEqpId + "_" + szLocation +"_";
	string sExt = ".csv";

	sFileName = szFolder + sPre + GetDateTime() + sExt;


	DBG_I_N("fNme=%s, nIdx=%d\r\n", sFileName.c_str(), nIdx );
	
	m_ofs[nIdx].exceptions(ofstream::badbit | ofstream::failbit);

	try {
		m_ofs[nIdx].open(sFileName);
		
		m_ofs[nIdx].setf(ios::fixed);
		m_ofs[nIdx].setf(ios::showpoint);	
		m_ofs[nIdx].precision(3);	
		}
	catch( ofstream::failure &e) {
		DBG_E_R("nIdx=%d, %s\r\n", nIdx, e.what());
		}
	
}

int CLog::openFile(   int nIdx, string szFolder)
{
	int nRet = -1;
	string sFileName;
	string sPre = "LOG_";
	string sExt = ".txt";

	sFileName = szFolder + sPre + GetDateTime5() + sExt;


	DBG_I_N("fNme=%s, idx=%d\r\n", sFileName.c_str(), nIdx );
	
	m_ofs[nIdx].exceptions(ofstream::badbit | ofstream::failbit);

	try {
		m_ofs[nIdx].open(sFileName, ios::app );
		nRet = 1;
		}
	catch( ofstream::failure &e) {
		DBG_E_R("nIdx=%d, %s\r\n", nIdx, e.what());
		nRet = -1;
		}

	return nRet;
}

void CLog::writeData(   int nIdx,  int nSize, float **pData)
{
	int j;

	try {
		for(int i=0;i<nSize;i++) {
			for(j=0;j<(MAX_CH-1);j++) m_ofs[nIdx] << pData[j][i] << ",";
			m_ofs[nIdx] << pData[j][i] << endl;
			}
		}
	catch( ofstream::failure &e) {
		DBG_E_R("nIdx=%d, %s\r\n", nIdx, e.what());
		}
}

void CLog::writeString(  int nIdx, string &szMsg ) 
{

	DBG_I_N("<%s>\r\n", szMsg.c_str());
	
	try {	
		m_ofs[nIdx] << szMsg;
		}
	catch( ofstream::failure &e) {
		DBG_E_R("nIdx=%d, %s\r\n", nIdx, e.what());
		}
		
}

void CLog::writeString(  int nIdx, string szMsg ) 
{
	DBG_I_N("<%s>\r\n", szMsg.c_str());
	
	try {	
		m_ofs[nIdx] << szMsg;
		}
	catch( ofstream::failure &e) {
		DBG_E_R("nIdx=%d, %s\r\n", nIdx, e.what());
		}
		
}

void CLog::closeFile(int nIdx)
{
	DBG_I_N("nIdx=%d\r\n", nIdx);

	try {
		m_ofs[nIdx].close();
		}
	catch( ofstream::failure &e) {
		DBG_E_R("nIdx=%d, %s\r\n", nIdx, e.what());
		}
}

void CLog::pushMsg( int nIdx, string szFolder, string szMsg )
{
	int nRet = 0;
	
	nRet = openFile( nIdx, szFolder);
	if( nRet ) {
		try {	
			m_ofs[nIdx] << "[" << GetDateTime2() << "]" << szMsg << endl;
			}
		catch( ofstream::failure &e) {
			DBG_E_R("nIdx=%d, %s\r\n", nIdx, e.what());
			}
		}
	closeFile( nIdx);
}

