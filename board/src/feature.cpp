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
#include <math.h>

#include "feature.h"

#define DEBUG
#include "debug.h"




CFeature::CFeature() : CBase(__func__)
{
	DBG_I_C("create id=%p\r\n", GetId());

	m_szFeatName[FT_AVG] = "AVG";
	m_szFeatName[FT_MIN] = "MIN";
	m_szFeatName[FT_MAX] = "MAX";
	m_szFeatName[FT_RMS] = "RMS";
	m_szFeatName[FT_SUM] = "SUM";
	m_szFeatName[FT_P2P] = "P2P";
}

CFeature::~CFeature()
{
	DBG_I_C("destroy id=%p\r\n", GetId() );
}

int CFeature::preprocessingWith(int nSRate, int nChSize, float **pfDatas)
{
	int nRet = 0;
	float fSum = 0.0f;

	for(int ch=0;ch<nChSize;ch++) {
		fSum = 0.0f;
		for( int i=0;i<nSRate;i++) {
			fSum += pfDatas[ch][i];
			}
		m_fFeature[ch][FT_AVG] = fSum/nSRate;
		}


	for(int ch=0;ch<nChSize;ch++) {
		for( int i=0;i<nSRate;i++) {
			if( m_pfValParams[i].d_bEnableAvg )
				pfDatas[ch][i] = m_pfValParams[ch].d_fCvalue * ((m_pfValParams[ch].d_fAvalue*(pfDatas[ch][i] - m_fFeature[ch][FT_AVG]))-m_pfValParams[ch].d_fBvalue);
			else
				pfDatas[ch][i] = m_pfValParams[ch].d_fCvalue * ((m_pfValParams[ch].d_fAvalue*pfDatas[ch][i]) - m_pfValParams[ch].d_fBvalue);
			}
		}

	return nRet;
}

int CFeature::processingWith(int nSRate, int nChSize, float **pfDatas)
{
	int nRet = 0;
	float fTempSq = 0.0f;
	float fTempSum = 0.0f;
	float fTempMin = 0.0f;
	float fTempMax = 0.0f;

	preprocessingWith(nSRate, nChSize, pfDatas);
		
	for(int ch=0;ch<nChSize;ch++) {
		fTempSq = 0.0f;
		fTempSum = 0.0f;
		fTempMin = FLT_MAX;
		fTempMax = -FLT_MAX;
		for( int i=0;i<nSRate;i++) {
			fTempSum += pfDatas[ch][i];
			fTempSq = pfDatas[ch][i] * pfDatas[ch][i];
			if( fTempMin > pfDatas[ch][i] ) fTempMin = pfDatas[ch][i];
			if( fTempMax < pfDatas[ch][i] ) fTempMax = pfDatas[ch][i];			
			}
		m_fFeature[ch][FT_MIN] = fTempMin;
		m_fFeature[ch][FT_MAX] = fTempMax;
		m_fFeature[ch][FT_P2P] = fTempMax - fTempMin;
		m_fFeature[ch][FT_SUM] = fTempSum;
		m_fFeature[ch][FT_RMS] = sqrt(fTempSq/nSRate);

//		DBG("ch=%d, %f,%f,%f,%f,%f,%f\r\n", ch, m_fFeature[ch][0], m_fFeature[ch][1], m_fFeature[ch][2], m_fFeature[ch][3], m_fFeature[ch][4], m_fFeature[ch][5]);
		}
	
	return nRet;
}

const string CFeature::getFeatures(int nIdx)
{
	char szTemp[16];
	string szFeatures;
	ptSVID pSvid;
	long long lSvid;
	map<long long, ptSVID>::iterator iter;

	szFeatures = "{";
	for(iter=m_mSvid[nIdx].begin(); iter!=m_mSvid[nIdx].end(); iter++){
		lSvid = iter->first;
		pSvid = iter->second;
	
		if( pSvid->nCh <= MAX_CH && pSvid->nFeature < FT_MAXNO) {
			if(iter != m_mSvid[nIdx].begin()) szFeatures += "^";
			sprintf(szTemp, "%.3f", m_fFeature[pSvid->nCh-1][pSvid->nFeature]);
			szFeatures += to_string(lSvid) + "=" + szTemp;
			}
//		DBG("svid=%ld, ch=%d, f=%d, %s, %f\r\n", lSvid, pSvid->nCh, pSvid->nFeature, pSvid->szName.c_str(), m_fFeature[pSvid->nCh][pSvid->nFeature-1]);
		}
	szFeatures += "}";
	
	return szFeatures;
}
const string CFeature::getFeatureValues(int nIdx)
{
	char szTemp[16];
	string szFeatures;
	ptSVID pSvid;
	long long lSvid;
	map<long long, ptSVID>::iterator iter;

	for(iter=m_mSvid[nIdx].begin(); iter!=m_mSvid[nIdx].end(); iter++){
		lSvid = iter->first;
		pSvid = iter->second;
	
		if( pSvid->nCh <= MAX_CH && pSvid->nFeature < FT_MAXNO) {
			sprintf(szTemp, "%.3f", m_fFeature[pSvid->nCh-1][pSvid->nFeature]);
			szFeatures += string(szTemp) + ",";
			}
		}
	
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


int CFeature::setSVID(int nIdx, string szConfig)
{
	int nRet = 0;
	vector<string> vResults;
	vector<string> vContents;
	ptSVID pSvid;
	
	vResults = split(szConfig, '\n');

	for (int i=0;i<vResults.size();i++){
//		DBG("i=%d, %s\r\n", i, vResults[i].c_str());
		vContents = split(vResults[i], '=');
		if (vContents.size() == 4) {
			pSvid = new tSVID;
			pSvid->nCh = stoi(vContents[2]);
			pSvid->nFeature = stoi(vContents[3]);
			pSvid->szName = vContents[0];
			m_mSvid[nIdx].insert(pair<long long, tSVID*>(stoll(vContents[1]), pSvid));
//			printf("ch=%d, v0=%s, v1=%s, v2=%s, v3=%s\r\n", pSvid->nCh, vContents[0].c_str(), vContents[1].c_str(), vContents[2].c_str(), vContents[3].c_str());
			}
		}

	return nRet;
}

const string CFeature::getFeatNames(int nSvr)
{
	string szNames;
	ptSVID pSvid;
	long long lSvid;
	map<long long, ptSVID>::iterator iter;

	for(iter=m_mSvid[nSvr].begin(); iter!=m_mSvid[nSvr].end(); iter++){
		lSvid = iter->first;
		pSvid = iter->second;
	
		if( pSvid->nCh <= MAX_CH && pSvid->nFeature < FT_MAXNO) {
			szNames += m_szFeatName[pSvid->nFeature] + ",";
			}
		}
	szNames += "\n";

	return szNames;
}

void CFeature::setParam(void* pParam)
{
	m_pfValParams = (fVAL_PARAM_t*)pParam;
}

