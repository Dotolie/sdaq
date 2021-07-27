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
//  File         : resource.h
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#ifndef __RESOURCE_H__
#define __RESOURCE_H__


#include <iostream>
#include "thread.h"


typedef int(*cb_resource_handler)(int , int, unsigned char*, size_t, size_t*, int, int);

class CResource
{
public:
	CResource():m_sync_cond(PTHREAD_COND_INITIALIZER), m_Mutex(PTHREAD_MUTEX_INITIALIZER){};
	~CResource(){};
	int InitLock()
	{
		return pthread_mutex_init(&m_Mutex, NULL);
	}
	int InitCond() 
	{
		return pthread_cond_init(&m_sync_cond, NULL);
	}
	int DestLock()
	{
		return pthread_mutex_destroy(&m_Mutex);
	}
	int DestCond() 
	{
		return pthread_cond_destroy(&m_sync_cond);
	}
	int Lock() 
	{
		return pthread_mutex_lock(&m_Mutex);
	}
	int Unlock()
	{
		return pthread_mutex_unlock(&m_Mutex);
	}
	int Signal() 
	{
		return pthread_cond_signal(&m_sync_cond);
	}
	int Wait() 
	{
		return pthread_cond_wait(&m_sync_cond, &m_Mutex);
	}


	
public:	
	pthread_cond_t		m_sync_cond;
	pthread_mutex_t 	m_Mutex;

	int m_nStatus;
	cb_resource_handler m_pCallback;

};

#endif  // __RESOURCE_H__

