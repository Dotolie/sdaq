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
//  File         : server.cpp
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>

#include "stdafx.h"
#include "utils.h"
#include "log.h"
#include "SocketException.h"
#include "server.h"



#define DEBUG
#include "debug.h"


#define EPOLL_SIZE 		20
#define MAXLINE 		1024

#define CONNECTED		1
#define DISCONNECT		-1


#define LOG_FOLDER_PATH		"./"

#define MSG_INIT 	"<Msg><Command>FDCNONSECS_FIRSTREPORT</Command><DataLayer><EqpID></EqpID><ModuleID></ModuleID></DataLayer></Msg>";



extern CLog		g_Log;


CServer::CServer() : Runnable(__func__)
{
	InitLock();
	InitCond();

	DBG_I_C("create id=%p\r\n", GetId());	
}

CServer::CServer(void* pConfig) : Runnable(__func__)
{
	InitLock();
	InitCond();

	m_pConfig = (CConfig*)pConfig;

	DBG_I_C("create id=%p\r\n", GetId());	
}

CServer::~CServer() 
{
	SetRunBit(false);

	DestLock();
	DestCond();
	
	DBG_I_C("destroy id=%p\r\n", GetId() );
}

void CServer::Stop()
{
	SetRunBit(false);
	
//	DBG_I_N("m_bRun=false\r\n");
}

void CServer::Run()
{
	int nRet = 0;
	int i;
	int readn;
	int eventn;
	int epollfd;
	
	char buf[MAXLINE];
	struct udata *user_data;
	struct epoll_event ev, *events;
	string szMsg ="";
	
	SetRunBit(true);
	
	DBG_I_N("run : start \r\n");


	CServerSocket server( m_pConfig->m_DeviceCfg.d_nPort );		
	events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

	g_Log.pushMsg( LOG_SERVER, LOG_FOLDER_PATH, "start server");
	
	if((epollfd = epoll_create(100)) == -1) {
		DBG_E_R("fail epoll create\r\n");
	
		goto _err;
		}
	
	ev.events = EPOLLIN;
	ev.data.fd = server.m_nSock;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, server.m_nSock, &ev);
	memset(m_nFds, -1, sizeof(int) * EPOLL_SIZE);

	try
	{
		while( IsRun() )
		{
			eventn = epoll_wait(epollfd, events, EPOLL_SIZE, 1000); 
			if(eventn == -1)
			{
				DBG_I_N("epoll exit\r\n");
				goto _err;
			}
			for(i=0; i < eventn; i++)
			{		
				if(events[i].data.fd == server.m_nSock)
				{					
					CServerSocket *pSock = new CServerSocket ; 
					server.accept( pSock );

					m_nFds[pSock->m_nSock] = CONNECTED;
					m_pClient_socket[pSock->m_nSock] = pSock;

					user_data = (struct udata *)malloc(sizeof(user_data));
					user_data->fd = pSock->m_nSock;
					strcpy(user_data->name, inet_ntoa(pSock->m_addr.sin_addr));

					szMsg = "connect from : ";
					szMsg += user_data->name;
					g_Log.pushMsg( LOG_SERVER, LOG_FOLDER_PATH, szMsg);
					
					SendInitMsg(pSock->m_nSock);
					DBG_I_N("connect[%s:%d] fd=%d\r\n", user_data->name, ntohs(pSock->m_addr.sin_port), user_data->fd );

					ev.events = EPOLLIN;
					ev.data.ptr = user_data;

					epoll_ctl(epollfd, EPOLL_CTL_ADD, pSock->m_nSock, &ev);
				}
				else
				{
					user_data = (struct udata*)events[i].data.ptr;
					memset(buf, 0x00, MAXLINE);
					readn = read(user_data->fd, buf, MAXLINE);
					if(readn <= 0)
					{
						DBG_I_N("disconnect[%s] fd=%d\r\n", user_data->name, user_data->fd);

						szMsg = "disconnect   : ";
						szMsg += user_data->name;
						g_Log.pushMsg( LOG_SERVER, LOG_FOLDER_PATH, szMsg);

						epoll_ctl(epollfd, EPOLL_CTL_DEL, user_data->fd, events);

						m_nFds[user_data->fd] = DISCONNECT;
						delete m_pClient_socket[user_data->fd];
						close(user_data->fd);
						free(user_data);
					}
					else
					{
						szMsg = user_data->name;
						szMsg += ":";
						szMsg += buf;
						
//						SendAll(szMsg);
						DBG_I_N("RcvMsg:%s", szMsg.c_str());
					}
				}
			}	  
			usleep(1000);
		}

		g_Log.pushMsg( LOG_SERVER, "./", "stop server");

	}
	catch ( SocketException& e) {
		DBG_E_R("%s\r\n", e.description().c_str());
		}


_err:
	SetRunBit(false);


	DBG_I_N("end of loop \r\n");
}

int CServer::SendAll(string &szMsg)
{
	int nRet = 0;

	DBG("SendAll:%s\r\n", szMsg.c_str());
	
	Lock();
	for(int j=0;j<EPOLL_SIZE;j++) {
		if( (m_nFds[j] == CONNECTED )) {
			*m_pClient_socket[j] << szMsg;
			}
		}
	Unlock();
	
	return nRet;
}

int CServer::SendInitMsg(int nFd)
{
	int nRet = 0;

	*m_pClient_socket[nFd] << MSG_INIT;;

	return nRet;
}


int CServer::SendFeaturesAll(string &szEqpid, int nTrid, string &szFeature)
{
	int nRet = 0;
	string szMsg;
	
	szMsg = "NONSECS_TOOLDATA HDR=(null,null,null) TIME_STAMP=";
	szMsg += GetDateTime3();
	szMsg += " EQPID=" + szEqpid;
	szMsg += " TRID=" + to_string(nTrid);
	szMsg += " LOTID=(NULL) RECPID=(NULL) SENSOR_VALUES=";
	szMsg += szFeature;

//	DBG("SendFeatureAll:%s\r\n", szMsg.c_str());	
	
	Lock();
	for(int j=0;j<EPOLL_SIZE;j++) {
		if( (m_nFds[j] == CONNECTED )) {
			*m_pClient_socket[j] << szMsg;
			}
		}
	Unlock();
	
	return nRet;
}

