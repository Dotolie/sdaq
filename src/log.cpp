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
//  File         : log.cpp
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <dirent.h>
#include <algorithm>
#include <cstdio>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


#include "object.h"
#include "taskMgr.h"
#include "timerMgr.h"
#include "utils.h"
#include "struct.h"
#include "log.h"
#include "output.h"
#include "version.h"


#define DEBUG
#include "debug.h"


#define OUT_FOLDER_PATH		"/mnt/nand/BackUpLog/"
#define MAX_FILES			10


#define	TIMER_LOG_ID_0 		(TASK_ID_LOG + 1)
#define	TIMER_LOG_ID_1 		(TASK_ID_LOG + 2)



using namespace std;


extern CTimerMgr g_TimerMgr;
extern CTaskMgr  g_TaskMgr;



CLog::CLog() : CBase(__func__)
{
	DBG_I_C("create id=%p\r\n", GetId());

	InitLock();
	InitCond();

	for(int i=0; i<MAX_SERVER; i++) {
		m_pLog[i]=NULL;
		}

	m_nInp = 0;
	m_nOut = 0;
	m_nSize = 0;
	
	g_TaskMgr.AddTask(TASK_ID_LOG, this);
}

CLog::~CLog() 
{
	g_TaskMgr.DelTask(TASK_ID_LOG);

	for(int i=0; i<MAX_SERVER; i++) {
		if( m_pLog[i]!=NULL) {
			fclose(m_pLog[i]);
			}
		}

	DestCond();
	DestLock();

	DBG_I_C("destroy id=%p\r\n", GetId() );
}

int CLog::MsgProc(int message, long wparam, long lparam)
{
	int nRet = 0;
	
	switch(message) {
	case MSG_CREATE:
		nRet = On_MSG_CREATE( wparam, lparam);
		break;
	case MSG_TIMER:
		nRet = On_MSG_TIMER( wparam, lparam);
		break;
	case MSG_EVENT:
		nRet = On_MSG_EVENT( wparam, lparam);
		break;
	case MSG_QUIT:
		nRet = On_MSG_QUIT( wparam, lparam);
		break;
	default:
		break;
		}

	return nRet;
}

int CLog::On_MSG_CREATE( long wparam, long lparam )
{
	int nRet = 0;
	
	DBG_I_Y("w=%ld, l=%ld\r\n", wparam, lparam);

	
	m_pThread = new Thread( new COutput(this) );
	m_pThread->Start();
//	g_TimerMgr.addTimer(TASK_ID_LOG, 10000, TIMER_LOG_ID_0, 0);
		
	return nRet;
}

int CLog::On_MSG_TIMER( long wparam, long lparam )
{
	int nRet = 0;
	
	DBG_I_C("uid=%ld,%s\r\n", wparam, GetDateTime2().c_str());

	switch(wparam) {
		case 1:
			
			break;
		}
	
	return nRet;
}

int CLog::On_MSG_EVENT( long wparam, long lparam )
{
	int nRet = 0;
	
	DBG_I_C("uid=%ld,%s\r\n", wparam, GetDateTime2().c_str());

		
	return nRet;
}

int CLog::On_MSG_QUIT( long wparam, long lparam )
{
	DBG_I_P("w=%ld, l=%ld\r\n", wparam, lparam);

//	closeFile(0);
//	closeFile(1);
//	g_TimerMgr.delTimer( TIMER_LOG_ID_0);

	m_pThread->Stop();
	delete m_pThread;

	return 0;
}

int CLog::openRawFile()
{
	int nRet = 0;
	string sPath = OUT_FOLDER_PATH + string("RAW");
	string sFileName;
	string sPre = "RAW_";
	string sExt = ".csv";

	
	sFileName = sPath + "/" + sPre + GetDateTime() + sExt;

	getFileList(sPath, "csv");

	DBG_I_N("fNme=%s\r\n", sFileName.c_str() );
	
	m_pFile = fopen( sFileName.c_str(), "a+");
	if( m_pFile == NULL ) {
		nRet = -1;
		DBG_E_R("can't open, %s\r\n", sFileName.c_str());
		}

	return nRet;
}

int CLog::openFile(   int nIdx, string szFolder)
{
	int nRet = -1;
	string sPath = OUT_FOLDER_PATH + szFolder;
	string sFileName;
	string sPre = "LOG_";
	string sExt = ".log";

	sFileName =  sPath + "/" + sPre + GetDateTime5() + sExt;

	getFileList(sPath, "log");

	DBG_I_N("fName=%s, idx=%d\r\n", sFileName.c_str(), nIdx );
	
	m_ofs[nIdx].exceptions(ofstream::badbit | ofstream::failbit);

	try {
		m_ofs[nIdx].open(sFileName, ios::app );
		nRet = 1;
		}
	catch( ofstream::failure &e) {
		DBG_E_R("nIdx=%d, %s\r\n", nIdx, e.what());
		nRet = -1;
		}

	return nRet;
}

int CLog::writeString(   string szMsg ) 
{
	int nRet = 0;

	nRet = fwrite(szMsg.c_str(), 1, szMsg.length(), m_pFile);
	fflush(m_pFile);
	
//	DBG_I_N("nRet=%d, %s\r\n", nRet, szMsg.c_str());
		
	return nRet;
}

void CLog::writeString(  int nIdx, string &szMsg ) 
{

//	DBG_I_N("<%s>\r\n", szMsg.c_str());
	
	try {	
		m_ofs[nIdx] << szMsg;
		}
	catch( ofstream::failure &e) {
		DBG_E_R("nIdx=%d, %s\r\n", nIdx, e.what());
		}
		
}

void CLog::writeString(  int nIdx, string szMsg ) 
{
//	DBG_I_N("<%s>\r\n", szMsg.c_str());
	
	try {	
		m_ofs[nIdx] << szMsg;
		}
	catch( ofstream::failure &e) {
		DBG_E_R("nIdx=%d, %s\r\n", nIdx, e.what());
		}
		
}

int CLog::closeRawFile()
{
	int nRet = 0;

	nRet = fflush(m_pFile);
	nRet = fclose(m_pFile);

	DBG_I_N("nRet=%d\r\n", nRet);

	return nRet;
}

void CLog::closeFile(int nIdx)
{
	DBG_I_N("nIdx=%d\r\n", nIdx);

	try {
		m_ofs[nIdx].close();
		}
	catch( ofstream::failure &e) {
		DBG_E_R("nIdx=%d, %s\r\n", nIdx, e.what());
		}
}

void CLog::pushMsg( int nIdx, string szFolder, string szMsg )
{
	int nRet = 0;

	Lock();
	nRet = openFile( nIdx, szFolder);
	if( nRet ) {
		try {	
			m_ofs[nIdx] << "[" << GetDateTime2() << "]" << szMsg << endl;
			}
		catch( ofstream::failure &e) {
			DBG_E_R("nIdx=%d, %s\r\n", nIdx, e.what());
			}
		}
	closeFile( nIdx);
	Unlock();
}


int CLog::getFileList(string szFolder, string szExt )
{
	int nRet = -1;
	DIR *dir = NULL;
	struct dirent *diread = NULL;
	string szName;
	
	if ((dir = opendir(szFolder.c_str())) != nullptr) {
		while ((diread = readdir(dir)) != nullptr) {
			szName = diread->d_name;
			if( szName.find(szExt) != string::npos )
				m_vFiles.push_back(szName);
			}
		closedir (dir);

		sort(m_vFiles.begin(), m_vFiles.end(),[](string &s1, string &s2) { return s1 > s2; });
		nRet = m_vFiles.size();
#if 0
		for (int i=0;i<nRet;i++)
			DBG("i=%02d, %s\r\n", i, m_vFiles[i].c_str());
		cout << endl;
#endif

		if( nRet >= MAX_FILES) {
			for( int i=nRet;i>=MAX_FILES;i--) {
				string szFile = szFolder;
				szFile += "/" + m_vFiles[i-1];
				remove(szFile.c_str());
			
//				DBG_I_N("delete i=%02d, %s\r\n", i, szFile.c_str());			
				}
			}

		m_vFiles.clear();
		
		}
	else {
		if (mkdir(szFolder.c_str(), 0777) != -1) {
			DBG_I_Y("OK mkdir(%s)\r\n", szFolder.c_str());
			nRet = 0;
			}
	    else {
		    DBG_E_R("error! mkdir(%s)\r\n", szFolder.c_str());
	    	}
		}
	

	return nRet;
}

int CLog::getFileList(string szFolder, string szExt, int nMax )
{
	int nRet = -1;
	DIR *dir = NULL;
	struct dirent *diread = NULL;
	string szName;
	
	if ((dir = opendir(szFolder.c_str())) != nullptr) {
		while ((diread = readdir(dir)) != nullptr) {
			szName = diread->d_name;
			if( szName.find(szExt) != string::npos )
				m_vFiles.push_back(szName);
			}
		closedir (dir);

		sort(m_vFiles.begin(), m_vFiles.end(),[](string &s1, string &s2) { return s1 > s2; });
		nRet = m_vFiles.size();
#if 0
		for (int i=0;i<nRet;i++)
			DBG("i=%02d, %s\r\n", i, m_vFiles[i].c_str());
		cout << endl;
#endif

		if( nRet >= nMax) {
			for( int i=nRet;i>=nMax;i--) {
				string szFile = szFolder;
				szFile += "/" + m_vFiles[i-1];
				remove(szFile.c_str());
			
//				DBG_I_N("delete i=%02d, %s\r\n", i, szFile.c_str());			
				}
			}

		m_vFiles.clear();
		
		}
	else {
		if (mkdir(szFolder.c_str(), 0777) != -1) {
			DBG_I_Y("OK mkdir(%s)\r\n", szFolder.c_str());
			nRet = 0;
			}
	    else {
		    DBG_E_R("error! mkdir(%s)\r\n", szFolder.c_str());
	    	}
		}
	

	return nRet;
}

void CLog::setParam(  int nIdx, string &szEqpId, string &szData)
{
	m_szEqpId[nIdx] = szEqpId;
	m_szFeatNames[nIdx] = szData;
}

int CLog::IsFileExists(const char* filename)
{
    FILE *file;
    if (file = fopen(filename, "r"))
    {
        fclose(file);
        return 1;
    }

    return 0;
}

FILE* CLog::checkLogFile( int nSvr)
{
	int nExistFile = 0;
	string szDate = GetDateTime6();

	if( m_szFileDate[nSvr].compare(szDate) != 0 ) {
		if( m_pLog[nSvr] != NULL ) {
			DBG_I_C("fclose, svr=%d, now=%s, file=%s\r\n", nSvr, szDate.c_str(), m_szFileDate[nSvr].c_str());
			fclose(m_pLog[nSvr]);
			m_pLog[nSvr] = NULL;
			}
		}
	
	if( m_pLog[nSvr] == NULL ) {
		string sPath = OUT_FOLDER_PATH + m_szEqpId[nSvr];
		string sFileName;
		string sPre = m_szEqpId[nSvr] + "_" + to_string(nSvr+1) +"_";
		string sExt = ".csv";

		m_szFileDate[nSvr] = szDate;
		sFileName = sPath + "/" + sPre + szDate + sExt;
	
		getFileList(sPath, "csv", 7);

		nExistFile = IsFileExists(sFileName.c_str());

		m_pLog[nSvr] = fopen( sFileName.c_str(), "a+");
		if( m_pLog[nSvr] == NULL ) {
			DBG_E_R("can't open, %s\r\n", sFileName.c_str());
			}
		else {
			if( nExistFile != 1 ) {
				string szHead = makeHeader(nSvr);
				fwrite(szHead.c_str(), 1, szHead.length(), m_pLog[nSvr]);
				fflush(m_pLog[nSvr]);
				}
			}
		
		}

	return m_pLog[nSvr];
}

int CLog::writeLog( int nSvr, string szMsg)
{
	int nRet = 0;
	char szTemp[4096];
	FILE *pFile = NULL;

	pFile = checkLogFile(nSvr);

	if( pFile != NULL) {
		sprintf(szTemp, "%s\n", szMsg.c_str());
		fwrite(szTemp, 1, strlen(szTemp), pFile);
		fflush(pFile);
		}
	
//	DBG_I_N("svr=%d, msg=%s\r\n", nSvr, szMsg.c_str());

	return nRet;
}

int CLog::putDatas( float **pData)
{
	int nRet = 0;
	int i = 0;
	int j = 0;


	for(j=0;j<m_nCSize;j++) {
		memcpy(m_fRawData[m_nInp][j], pData[j], m_nSRate*4);
		}
	m_nInp++;
	m_nInp %= 10;
	
	m_nSize++;
	
	
	DBG_I_N("i=%d o=%d s=%d\r\n", m_nInp, m_nOut, m_nSize);

	return nRet;
}

const string CLog::makeHeader()
{
	int i = 0;
	string szTitle = "Version, Start Time, Channel Cnt, Sampling\n";
	string szVersion = "Ver";
	
	szVersion += VERSION;
	szVersion += ", ";
	szVersion += GetDateTime2();
	szVersion += ", " + to_string(m_nCSize);
	szVersion += ", " + to_string(m_nSRate);
	szVersion += "\n";


	string szContents = "";
	for(i=0;i<(m_nCSize-1);i++) szContents +="CH" + to_string(i+1) + ",";
	szContents += "CH" + to_string(i+1) + "\n";
	
	string szHeader = szTitle;
	szHeader += szVersion;
	szHeader += szContents;
	
	return szHeader;
}

const string CLog::makeHeader(int nSvr)
{
	string szTitle = "Version, Start Time, Channel Cnt, Sampling\n";
	string szVersion = "Ver";
	
	szVersion += VERSION;
	szVersion += ", ";
	szVersion += GetDateTime2();
	szVersion += ", " + to_string(m_nCSize);
	szVersion += ", " + to_string(m_nSRate);
	szVersion += "\n";

	string szHeader = szTitle;
	szHeader += szVersion;
	szHeader += m_szFeatNames[nSvr];
	
	return szHeader;
}

int CLog::writeRawData()
{
	int nRet = 0;
	int i = 0;
	int j = 0;
	int nLoop = 10;
	int nVal;
	int nMan;
	
	static char szTemp[32];
	static char szData[1024];	
	static int nIdx = 0;
	
	long now = GetMicrosecondCount();

	if( m_nSize > 0 ) {
	
		if( nIdx == 0 )	{ 
			openRawFile();
			writeString( makeHeader() );
			DBG_I_G("openfile\r\n");
			usleep(1000);
			}
#if 0
		for(i=0;i<m_nSRate;i++) {
			szData[0]=0;
			for(j=0;j<(m_nCSize-1);j++) {
				nVal = (int)m_fRawData[m_nOut][j][i];
				nMan = (int)(m_fRawData[m_nOut][j][i]*1000 - (nVal * 1000)); 
				sprintf(szTemp, "%d.%d,", nVal, nMan );
				strcat(szData, szTemp);
				}
			nVal = (int)m_fRawData[m_nOut][j][i];
			nMan = (int)(m_fRawData[m_nOut][j][i]*1000 - (nVal * 1000)); 
			sprintf(szTemp, "%d.%d\n", nVal, nMan );
			strcat(szData, szTemp);
			nRet = fwrite(szData, 1, strlen(szData), m_pFile);
			}
		fflush(m_pFile);
#else
		for(i=0;i<m_nSRate;i++) {
			for(j=0;j<(m_nCSize-1);j++) {
				nVal = (int)m_fRawData[m_nOut][j][i];
				nMan = (int)(m_fRawData[m_nOut][j][i]*1000 - (nVal * 1000)); 
				if( nMan < 0 ) nMan *= -1;
				fprintf(m_pFile, "%d.%d,", nVal, nMan );
//				printf("i=%d, j=%d, %f, %d, %d\r\n", i, j, m_fRawData[m_nOut][j][i], nVal, nMan );
				}

			nVal = (int)m_fRawData[m_nOut][j][i];
			nMan = (int)(m_fRawData[m_nOut][j][i]*1000 - (nVal * 1000)); 
			if( nMan < 0 ) nMan *= -1;
			fprintf(m_pFile, "%d.%d\n", nVal, nMan );
			}
#endif
		m_nOut++;
		m_nOut %= 60;
		
		m_nSize--;
		nIdx++;
		
		if( nIdx >= nLoop) {
			closeRawFile();
			DBG_I_Y("closefile : nIdx=%d\r\n", nIdx);
			nIdx = 0;
			}
		now = GetMicrosecondCount() - now;
		DBG_I_C("write elapsed time=%ld\r\n", now);

		}
	else {
		usleep(1000);
		}
	
	return nRet;
}

void CLog::setCHSize(int nSize)
{
	m_nCSize = nSize;
}

void CLog::setSampleRate(int nRate)
{
	m_nSRate = nRate;
}


