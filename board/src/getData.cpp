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
//  File         : getData.cpp
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <poll.h>
#include <iostream>
#include <fstream>
#include <sstream>


#include "runnable.h"
#include "getData.h"
#include "error.h"
#include "utils.h"

#include "taskMgr.h"


#define DEBUG
#include "debug.h"


#define FPGA_VERSION_FILE_NAME	"/tmp/fpga"



extern CTaskMgr g_TaskMgr;


using namespace std;

CGetData::CGetData(CSensor *pRes) : Runnable(__func__)
{
	int nRet = ERR_NONE;
	m_pRes = pRes;

	m_pRes->m_nPageIn = 0;
	m_pRes->m_nPageOut = 0;
	m_pRes->m_nPageMax = 2;

	DBG_I_N("Create Id=0x%p\r\n", GetId() );
}

CGetData::~CGetData() 
{
	SetRunBit(false);

	DBG_I_N("destroy Id=0x%p\r\n", GetId() );
}

void CGetData::Stop()
{
	SetRunBit(false);

	DBG_I_N("m_bRun=false\r\n");
}

void CGetData::Run()
{
	int nRet = ERR_NONE;
	int nIdx = 0;
	int nSize = 0;
	int nLen = 0;
	int nTotalRx = 0;
	int nSeqNo = 0;
	int nSegIdx = 0;


	nRet = OpenDev();
	nRet = SetMode(m_pRes->m_nSampleRate, m_pRes->m_nMode);
	nRet = StartSampling();	

	SetRunBit(true);
	
	DBG_I_N("run : start \r\n");
	while(IsRun()) {
		for(int i=0;i<1024;i++) {
			for(int c=0;c<MAX_CH;c++) {
				m_pRes->m_fRawData[m_pRes->m_nPageIn][c][i] = i;
				}
			}
		m_pRes->Lock();
		m_pRes->m_nPageOut = m_pRes->m_nPageIn; 
		m_pRes->m_nPageIn++;
		m_pRes->m_nPageIn %= 2;

		m_pRes->Signal();
		m_pRes->Unlock();

		DBG("i=%d, Rate=%d, Read=%d ------- %s\r\n", nIdx, m_pRes->m_nSampleRate, nTotalRx, GetDateTime2().c_str());
		nIdx++;
		sleep(1);
		}

	nRet = StopSampling();
	nRet = CloseDev();

_err:
	
	DBG_I_N("end of loop \r\n");

}


int CGetData::OpenDev()
{
	int nRet = ERR_NONE;

	return nRet;
}

int CGetData::CloseDev()
{
	int nRet = ERR_NONE;

	return nRet;
}


int CGetData::SetMode(int nRate, int nMode)
{
	int nRet = ERR_NONE;


	return nRet;
}


int CGetData::StartSampling()
{
	int nRet = ERR_NONE;


	DBG_I_N("nRet=%d\r\n", nRet);

	return nRet;
}

int CGetData::StopSampling()
{
	int nRet = ERR_NONE;

	DBG_I_N("nRet=%d\r\n", nRet);

	return nRet;
}


void CGetData::SaveVersionNo(char cMaj, char cMin)
{
	ofstream outFile;
	stringstream ss;
	char szVer[6];

	sprintf(szVer, "%d.%d", cMaj, cMin);
	
	ss << szVer << endl;

	outFile.open(FPGA_VERSION_FILE_NAME);
	outFile << ss.rdbuf();
	outFile.close();
}

