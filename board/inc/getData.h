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
//  File         : getData.h
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------


#ifndef __GETDATA_H__
#define __GETDATA_H__


#include "runnable.h"
#include "resource.h"
#include "sensor.h"


using namespace std;


enum {
	MODE_NORAML = 0,
	MODE_TEST_PATTERN,
};

class CGetData: public Runnable
{
protected:
	int m_nRate;
	bool m_bOpend;
	bool m_bSampling;

	CSensor *m_pRes;

protected:	
	int SetMode(int, int);

public:
	CGetData(CSensor *pRes);
	virtual ~CGetData();
	

	virtual void Run();
	virtual void Stop();

	int OpenDev();
	int CloseDev();
	int StartSampling();
	int StopSampling();
	void SaveVersionNo(char cMaj, char cMin);


};

#endif  // __GETDATA_H__
