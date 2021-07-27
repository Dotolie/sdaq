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

#include <iostream>
#include <fstream>
#include <vector>

#include "base.h"
#include "object.h"


using namespace std;


enum {
	LOG_SERVER = 0,
	LOG_DSP,

	LOG_MAX,
};


class CLog: public CObject, CBase
{
public:
	ofstream	m_ofs[LOG_MAX];
	vector<string>	m_vFiles;
	
public:
	CLog();
	~CLog();

	virtual int MsgProc(int message, long wparam, long lparam);

	void openFile( int nIdx, string szFolder, string &szEqpId, string &szLocation );
	int openFile( int nIdx, string szFolder );	
	void pushMsg( int nIdx, string szFolder, string szMsg );
	void writeData( int nIdx, int nSRate, int nChSize, float **pData);
	void writeString( int nIdx, string &szMsg );
	void writeString( int nIdx, string szMsg );	
	void closeFile( int nIdx );


	
private:
	int On_MSG_CREATE( long, long );
	int On_MSG_QUIT( long, long);
	int On_MSG_EVENT( long, long);
	int On_MSG_TIMER( long, long );

	int getFileList(string szFolder, string szExt);
		
};

#endif  // __LOG_H__
