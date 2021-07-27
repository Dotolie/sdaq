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
//  File         : feature.h
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------


#ifndef __FEATURE_H__
#define __FEATURE_H__

#include <string>
#include <vector>
#include <map>

#include "base.h"
#include "struct.h"


#define DEBUG
#include "debug.h"


enum {
	FT_MIN = 0,
	FT_MAX,
	FT_P2P,
	FT_SUM,
	FT_AVG,

	FT_MAXNO,
};
	
using namespace std;


typedef struct _tsvid {
	int nCh;
	int nFeature;
	string szName;
} tSVID, *ptSVID;



class CFeature : public CBase
{
protected:
	int m_nChNo;
	int m_nSRate;
	float m_fFeature[MAX_CH][FT_MAXNO];
	map<long long, ptSVID> m_mSvid;
	vector<string> split(string input, char delimiter);
		
public:
	CFeature();
	~CFeature();

	int setSVID(string szConfig);
	int processingWith(int nSRate, int nChSize, float **pfDatas);
	const string getFeatures();
};
	


#endif	// __FEATURE_H__

