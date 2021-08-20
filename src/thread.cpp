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
//  Module      :
//  File           : thread.cpp
//  Description :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------


#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/resource.h>

#include "thread.h"

#define DEBUG
#include "debug.h"



Thread::Thread()
{
	m_coreNum = 0;
	m_nPriority = 0;
//	DBG_I_N("mutext_init !!! \r\n");
}
Thread::Thread( Runnable* pRunnable ): _runnable(pRunnable)
{
	m_coreNum = 0;
	m_nPriority = 0;
//	DBG_I_N("create (%s) Id=0x%p\r\n",_runnable->GetName(), _runnable->GetId());
}
Thread::~Thread()
{
//	DBG_I_N("destroy(%s) Id=0x%x\r\n", _runnable->GetName(), _runnable->GetId());
	
	delete _runnable;
	_runnable = NULL;
}
void Thread::Start() 
{
	pthread_create(&_thread, NULL, &Thread::Main, this );

//	DBG_I_N("_runnable=%s, pPt=0x%x\r\n", _runnable->GetName(), _runnable->GetId());

}

void Thread::Wait()
{
	void* pData;

	
	if (_runnable != 0) {
		if( _thread ) {
			pthread_join( _thread, &pData);
			}
		}
	
//	DBG_I_N("Thread:Wait, _thread=%x, ret=%d \r\n", _thread, nr);
}

void Thread::Run() {
	if (_runnable != 0) {
//		DBG_E_R("%s:Run Id=0x%x\r\n", _runnable->GetName(), _runnable->GetId());
		if(	m_coreNum > 0) {
			if (pthread_setaffinity_np(pthread_self(), sizeof(m_coreNum), (cpu_set_t *)&m_coreNum) <0) {
				DBG_E_R("pthread_setaffinity_np"); 
				}
			}
		else {
//			DBG_I_Y("non affinity\r\n");
			}

		if( m_nPriority != 0 ) {
			const pid_t tid = syscall(SYS_gettid);
			int ret = setpriority(PRIO_PROCESS, tid, m_nPriority);
//			DBG_I_N("tid of high priority thread %d, %d\n", tid, getpriority(PRIO_PROCESS, tid));
			}

		_runnable->Run();
	}

}

void Thread::Stop() {
	if (_runnable != 0) {
//		DBG_I_N("%s:stop Id=0x%x\r\n", _runnable->GetName(), _runnable->GetId());	
		_runnable->Stop();

		Wait();
	}
}

void* Thread::Main(void* pInst)
{
//	DBG_I_N("Thread:Main \r\n");
	
	Thread* pt = static_cast<Thread*>(pInst);
	pt->Run();

	return NULL;
}

void Thread::SetCoreNum(unsigned long core)
{
	m_coreNum = core;
}

void Thread::SetPriority(int nPri)
{
	m_nPriority = nPri;
}

