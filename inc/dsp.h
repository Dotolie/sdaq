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
//  File         : dsp.h
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------


#ifndef __DSP_H__
#define __DSP_H__

#include "runnable.h"
#include "thread.h"
#include "config.h"
#include "core.h"



using namespace std;



class CDsp: public Runnable
{
private:
	CCore *m_pCore;
	CConfig	*m_pConfig;
	float m_fAval;
	float m_fBval;
	float m_fCval;
	
public:
	CDsp(void *pInst);
	virtual ~CDsp();
	
	virtual void Run();
	virtual void Stop();

};

#endif  // __DSP_H__
