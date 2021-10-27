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
//  File         : log.h
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------


#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "base.h"
#include "object.h"
#include "thread.h"
#include "struct.h"
#include "resource.h"


using namespace std;


enum {
	LOG_SERVER = 0,
	LOG_DSP,

	LOG_MAX,
};


class CLog: public CObject, CBase, CResource
{
public:
	FILE *m_pFile;
	ofstream	m_ofs[LOG_MAX];
	vector<string>	m_vFiles;

	int 	m_nSRate;
	int		m_nCSize;
	string	m_szPath;
	string	m_szEqpId[MAX_SERVER];
	string	m_szFeatNames[MAX_SERVER];
	long int	m_lFileDate[MAX_SERVER];
	FILE	*m_pLog[MAX_SERVER];

	int		m_nInp;
	int		m_nOut;
	int		m_nSize;
	float 	m_fRawData[10][20][65536];
	
public:
	CLog();
	~CLog();

	virtual int MsgProc(int message, long wparam, long lparam);

	int openRawFile();
	int openFile( int nIdx, string szFolder );	

	void writeString( int nIdx, string &szMsg );
	void writeString( int nIdx, string szMsg );	
	int writeString( string szMsg );
	int writeLog( int, string szMsg);
	int writeRawData();

	void closeFile( int nIdx );
	int closeRawFile();

	void setCHSize(int);
	void setSampleRate(int);
	void setParam( int, string &szEqpId, string &szLocation);
	
	int putDatas( float **pData);
	void pushMsg( int nIdx, string szFolder, string szMsg );	

	
private:
	int On_MSG_CREATE( long, long );
	int On_MSG_QUIT( long, long);
	int On_MSG_EVENT( long, long);
	int On_MSG_TIMER( long, long );

	int getFileList(string szFolder, string szExt);
	int getFileList(string szFolder, string szExt, int );	
	const string makeHeader();
	const string makeHeader(int);	
	FILE* checkLogFile( int nSvr);
	int IsFileExists(const char* filename);

private:
	Thread *m_pThread;
	
};

#endif  // __LOG_H__
