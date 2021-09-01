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

#ifndef __SENSOR_H__
#define __SENSOR_H__

#include "struct.h"
#include "thread.h"
#include "resource.h"
#include "runnable.h"
#include "config.h"







class CSensor : public CResource, public Runnable
{
public:
	CSensor(void* pInst);
	~CSensor();

	virtual void Run();
	virtual void Stop();

	virtual int Read(float** pData);	
	virtual int Write(unsigned char* pData);

public:	
	int m_nPageMax;
	int m_nPageIn;
	int m_nPageOut;

	unsigned char m_cData[2048 * 5];
	short m_sRawData[MAX_PG][MAX_CH][MAX_RATE];
	float m_fRawData[MAX_PG][MAX_CH][MAX_RATE];

	typedef struct {
		short d_sOffset;
		float d_fScale;
	} PARAMS_t;

	PARAMS_t m_Parameter[MAX_CH];

private:
	CConfig	*m_pConfig;
	int	m_nFpga;

private:
	int	LoadOffsetFile();
	int	SaveOffsetFile(PARAMS_t *pParam, int nEnc);
	PARAMS_t* SetOffsetDefault();		

	int OpenDev();
	int CloseDev();
	int SetMode(int nRate, int nMode);
	int StartSampling();
	int StopSampling();
	
	int GpioSetValue(int gpio, int value);
	int fpgaSet(unsigned char cmd);
	
};

#endif  // __SENSOR_H__

