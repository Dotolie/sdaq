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
//  File         : feature.cpp
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#include <float.h>
#include <iostream>
#include <sstream>

#include "feature.h"

#define DEBUG
#include "debug.h"




CFeature::CFeature() : CBase(__func__)
{
	DBG_I_C("create id=%p\r\n", GetId());
}

CFeature::~CFeature()
{
	DBG_I_C("destroy id=%p\r\n", GetId() );
}

int CFeature::processingWith(int nSRate, int nChSize, float **pfDatas)
{
	int nRet = 0;
	float fTempSum = 0.0f;
	float fTempMin = 0.0f;
	float fTempMax = 0.0f;
	float fTempAvg = 0.0f;
	
	for(int ch=0;ch<nChSize;ch++) {
		fTempSum = 0.0f;
		fTempAvg = 0.0f;
		fTempMin = FLT_MAX;
		fTempMax = FLT_MIN;
		for( int i=0;i<nSRate;i++) {
			fTempSum += pfDatas[ch][i];
			if( fTempMin > pfDatas[ch][i] ) fTempMin = pfDatas[ch][i];
			if( fTempMax < pfDatas[ch][i] ) fTempMax = pfDatas[ch][i];			
			}
		m_fFeature[ch][FT_MIN] = fTempMin;
		m_fFeature[ch][FT_MAX] = fTempMax;
		m_fFeature[ch][FT_P2P] = fTempMax - fTempMin;
		m_fFeature[ch][FT_SUM] = fTempSum;
		m_fFeature[ch][FT_AVG] = fTempAvg = fTempSum/nSRate;

//		DBG("ch=%d, %f,%f,%f,%f,%f\r\n", ch, m_fFeature[ch][0], m_fFeature[ch][1], m_fFeature[ch][2], m_fFeature[ch][3], m_fFeature[ch][4]);
		}
	
	return nRet;
}

const string CFeature::getFeatures()
{
	string szFeatures;
	ptSVID pSvid;
	long long lSvid;
	map<long long, ptSVID>::iterator iter;

	szFeatures = "{";
	for(iter=m_mSvid.begin(); iter!=m_mSvid.end(); iter++){
		lSvid = iter->first;
		pSvid = iter->second;
	
		if( pSvid->nCh <= MAX_CH && pSvid->nFeature != 0 && pSvid->nFeature <= FT_MAXNO) {
			if(iter != m_mSvid.begin()) szFeatures += "^";
			szFeatures += to_string(lSvid) + "=" + to_string(m_fFeature[pSvid->nCh-1][pSvid->nFeature-1]);
			}
//		DBG("svid=%ld, ch=%d, f=%d, %s, %f\r\n", lSvid, pSvid->nCh, pSvid->nFeature, pSvid->szName.c_str(), m_fFeature[pSvid->nCh][pSvid->nFeature-1]);
		}
	szFeatures += "}";
	
	return szFeatures;
}

vector<string> CFeature::split(string input, char delimiter)
{
    vector<string> answer;
    stringstream ss(input);
    string temp;
 
    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    }
 
    return answer;
}


int CFeature::setSVID(string szConfig)
{
	int nRet = 0;
	vector<string> vResults;
	vector<string> vContents;
	ptSVID pSvid;
	
	vResults = split(szConfig, '\n');

	for (int i=0;i<vResults.size();i++){
		DBG("i=%d, %s\r\n", i, vResults[i].c_str());
		vContents = split(vResults[i], '=');
		if (vContents.size() == 4) {
			pSvid = new tSVID;
			pSvid->nCh = stoi(vContents[2]);
			pSvid->nFeature = stoi(vContents[3]);
			pSvid->szName = vContents[0];
			m_mSvid.insert(pair<long long, tSVID*>(stoll(vContents[1]), pSvid));
//			printf("ch=%d, v0=%s, v1=%s, v2=%s, v3=%s\r\n", pSvid->nCh, vContents[0].c_str(), vContents[1].c_str(), vContents[2].c_str(), vContents[3].c_str());
			}
		}

	return nRet;
}


