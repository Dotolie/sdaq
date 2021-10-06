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
//  File         : output.cpp
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#include <unistd.h>

#include "output.h"



#define DEBUG
#include "debug.h"



COutput::COutput() : Runnable(__func__)
{
	InitLock();
	InitCond();

	DBG_I_C("create id=%p\r\n", GetId());	
}

COutput::COutput(void* pParent) : Runnable(__func__)
{
	InitLock();
	InitCond();

	m_pLog = (CLog*)pParent;


	DBG_I_C("create id=%p\r\n", GetId());	
}

COutput::~COutput() 
{
	SetRunBit(false);

	DestLock();
	DestCond();
	
	DBG_I_C("destroy id=%p\r\n", GetId() );
}

void COutput::Stop()
{
	SetRunBit(false);
	
//	DBG_I_N("m_bRun=false\r\n");
}

void COutput::Run()
{
//	int nIdx = 0;

	SetRunBit(true);
	
	while(IsRun()) {

		m_pLog->writeData(0);

		}

	DBG_I_N("end of loop \r\n");
}

