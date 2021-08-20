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
//  File         : output.h
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------


#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#include "struct.h"
#include "runnable.h"
#include "resource.h"
#include "log.h"


using namespace std;


 
class COutput: public CResource, public Runnable
{
private:
	CLog *m_pLog;
	
public:
	COutput();
	COutput(void* pParent);
	virtual ~COutput();
	

	virtual void Run();
	virtual void Stop();

};

#endif  // __OUTPUT_H__
