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
//  File         : ipsetup.h
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------


#ifndef __IPSETUP_H__
#define __IPSETUP_H__

#include <string>

#include "base.h"


#define DEBUG
#include "debug.h"



using namespace std;


typedef struct _IPCONFIG_t{
	string d_sEth0;
	string d_sEth1;
	string d_sWlan0;
} IPCONFIG_t;


class CIpsetup : public CBase
{
public:
	IPCONFIG_t m_IpCfg;	

public:
	CIpsetup();
	~CIpsetup();

	int Load();
	int Save();
};
	


#endif	// __IPSETUP_H__

