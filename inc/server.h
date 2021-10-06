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
//  File         : server.h
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------


#ifndef __SERVER_H__
#define __SERVER_H__

#include "struct.h"
#include "runnable.h"
#include "resource.h"
#include "config.h"
#include "ServerSocket.h"

using namespace std;


struct udata
{
    int fd;
    char name[80];
};
 
class CServer: public CResource, public Runnable
{
protected:
	CONFIG_t *m_pDeviceCfg;
	int m_nFds[1024];
	CServerSocket *m_pClient_socket[1024];
	
public:
	CServer();
	CServer(void*);
	virtual ~CServer();
	

	virtual void Run();
	virtual void Stop();

	int SendInitMsg(int nFd);
	int SendAll(string &szMsg);
	int SendFeaturesAll(string &szEqpid, int nTrid, string &szFeature);

};

#endif  // __SERVER_H__
