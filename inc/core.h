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
//  File         : core.h
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------


#ifndef __CORE_H__
#define __CORE_H__

#include "base.h"
#include "object.h"
#include "thread.h"
#include "config.h"
#include "server.h"
#include "sensor.h"


using namespace std;



class CCore: public CObject, CBase
{
public:
	CConfig	*m_pConfig;
	CServer *m_pServer;
	CSensor *m_pSensor;

private:
	Thread 	*m_pThreadDsp;
	Thread 	*m_pThreadServer;
	Thread 	*m_pThreadSensor;	
	
public:
	CCore();
	~CCore();

	virtual int MsgProc(int message, long wparam, long lparam);

	CConfig *GetConfig();
	void SaveVersionNo();
	
private:
	int On_MSG_CREATE( long, long );
	int On_MSG_QUIT( long, long);
	int On_MSG_TIMER( long, long );
};

#endif  // __CORE_H__
