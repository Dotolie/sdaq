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

#include <iostream>
#include <math.h>
#include <unistd.h>

#include "sensor.h"
#include "INIReader.h"
#include "error.h"
#include "utils.h"
#include "signal512.h"

#define DEBUG
#include "debug.h"



		

#define OFFSET_FILE_NAME		"./cal.ini"



	
CSensor::CSensor(void *pInst) : Runnable(__func__)
{
	int nRet = 0;

	m_pConfig = (CConfig*)pInst;
	
	m_nPageMax = 2;
	m_nPageIn = 0;
	m_nPageOut = 0;

	InitLock();
	InitCond();

	
	nRet = LoadOffsetFile();
//	for(int i = 0;i<MAX_CH;i++) {
//		DBG("i=%d, offset=%d, scale=%d\r\n",i, m_Parameter[i].d_sOffset, m_Parameter[i].d_nScale);
//		}	

	DBG_I_C("Created id=%p\r\n", GetId());
}

CSensor::~CSensor()
{
	DestLock();
	DestCond();

	
	DBG_I_C("destroy id=%p\r\n", GetId());
}

int CSensor::Read(float** pfData)
{
	int nRet = 0;

	Lock(); 
	Wait();

	for(int i=0;i<MAX_CH;i++) {
		pfData[i] = m_fRawData[m_nPageOut][i];
		
	//	DBG("i=%d, pf=%p\r\n", i, pfData[i]);
		}

	Unlock();
	
	return 1;
}

int CSensor::Write(unsigned char* pData)
{
	int nRet = 0;
	
	*pData= 0;
	
	return nRet;
}
		
int CSensor::LoadOffsetFile()
{
	INIReader *pReader = new INIReader(OFFSET_FILE_NAME);
	
	if( pReader->ParseError() < 0 ) {
		DBG_E_R("cal.ini does't exist\r\n");
		SetOffsetDefault();
		SaveOffsetFile( m_Parameter, 0 );
		}	
	else {
		m_Parameter[ADC_CH0].d_sOffset	= pReader->GetInteger("ch0", "offset", 0);
		m_Parameter[ADC_CH0].d_nScale	= pReader->GetInteger("ch0", "scale", 100000);
		
		m_Parameter[ADC_CH1].d_sOffset	= pReader->GetInteger("ch1", "offset", 0);
		m_Parameter[ADC_CH1].d_nScale	= pReader->GetInteger("ch1", "scale", 100000);

		m_Parameter[ADC_CH2].d_sOffset 	= pReader->GetInteger("ch2", "offset", 0);
		m_Parameter[ADC_CH2].d_nScale	= pReader->GetInteger("ch2", "scale", 100000);

		m_Parameter[ADC_CH3].d_sOffset 	= pReader->GetInteger("ch3", "offset", 0);
		m_Parameter[ADC_CH3].d_nScale	= pReader->GetInteger("ch3", "scale", 100000);

		m_Parameter[ADC_CH4].d_sOffset	= pReader->GetInteger("ch4", "offset", 0);
		m_Parameter[ADC_CH4].d_nScale	= pReader->GetInteger("ch4", "scale", 100000);
		
		m_Parameter[ADC_CH5].d_sOffset	= pReader->GetInteger("ch5", "offset", 0);
		m_Parameter[ADC_CH5].d_nScale	= pReader->GetInteger("ch5", "scale", 100000);

		m_Parameter[ADC_CH6].d_sOffset 	= pReader->GetInteger("ch6", "offset", 0);
		m_Parameter[ADC_CH6].d_nScale	= pReader->GetInteger("ch6", "scale", 100000);

		m_Parameter[ADC_CH7].d_sOffset 	= pReader->GetInteger("ch7", "offset", 0);
		m_Parameter[ADC_CH7].d_nScale	= pReader->GetInteger("ch7", "scale", 100000);

		m_Parameter[ADC_CH8].d_sOffset	= pReader->GetInteger("ch8", "offset", 0);
		m_Parameter[ADC_CH8].d_nScale	= pReader->GetInteger("ch8", "scale", 100000);
		
		m_Parameter[ADC_CH9].d_sOffset	= pReader->GetInteger("ch9", "offset", 0);
		m_Parameter[ADC_CH9].d_nScale	= pReader->GetInteger("ch9", "scale", 100000);

		m_Parameter[ADC_CH10].d_sOffset = pReader->GetInteger("ch10", "offset", 0);
		m_Parameter[ADC_CH10].d_nScale	= pReader->GetInteger("ch10", "scale", 100000);

		m_Parameter[ADC_CH11].d_sOffset = pReader->GetInteger("ch11", "offset", 0);
		m_Parameter[ADC_CH11].d_nScale	= pReader->GetInteger("ch11", "scale", 100000);

		m_Parameter[ADC_CH12].d_sOffset	= pReader->GetInteger("ch12", "offset", 0);
		m_Parameter[ADC_CH12].d_nScale	= pReader->GetInteger("ch12", "scale", 100000);
		
		m_Parameter[ADC_CH13].d_sOffset	= pReader->GetInteger("ch13", "offset", 0);
		m_Parameter[ADC_CH13].d_nScale	= pReader->GetInteger("ch13", "scale", 100000);

		m_Parameter[ADC_CH14].d_sOffset = pReader->GetInteger("ch14", "offset", 0);
		m_Parameter[ADC_CH14].d_nScale	= pReader->GetInteger("ch14", "scale", 100000);

		m_Parameter[ADC_CH15].d_sOffset = pReader->GetInteger("ch15", "offset", 0);
		m_Parameter[ADC_CH15].d_nScale	= pReader->GetInteger("ch15", "scale", 100000);

		m_Parameter[ADC_CH16].d_sOffset	= pReader->GetInteger("ch16", "offset", 0);
		m_Parameter[ADC_CH16].d_nScale	= pReader->GetInteger("ch16", "scale", 100000);
		
		m_Parameter[ADC_CH17].d_sOffset	= pReader->GetInteger("ch17", "offset", 0);
		m_Parameter[ADC_CH17].d_nScale	= pReader->GetInteger("ch17", "scale", 100000);

		m_Parameter[ADC_CH18].d_sOffset = pReader->GetInteger("ch18", "offset", 0);
		m_Parameter[ADC_CH18].d_nScale	= pReader->GetInteger("ch18", "scale", 100000);

		m_Parameter[ADC_CH19].d_sOffset = pReader->GetInteger("ch19", "offset", 0);
		m_Parameter[ADC_CH19].d_nScale	= pReader->GetInteger("ch19", "scale", 100000);
		}

	delete pReader;

//	for(int ch=0;ch<MAX_CHANNEL;ch++) 
//		DBG_I_N("ch[%d]off=%d, ch[%d]scale=%lf\r\n",  ch, m_Parameter[ch].d_sOffset, ch, m_Parameter[ch].d_nScale);

	return 0;
}

int CSensor::SaveOffsetFile(PARAMS_t *pParam, int nEnc)
{
	ofstream outFile;
	stringstream ss;

	ss << ";" << std::endl;
	ss << "; Generated cal.ini file for default" << std::endl;
	ss << ";" << std::endl;
	ss << "; offset : -10 ~ 20" << std::endl;
	ss << "; scale  : -1000 ~ +1000" << std::endl;
	ss << ";" << std::endl << std::endl;

	ss << "[ch0]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH0].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH0].d_nScale  << std::endl;
	ss << std::endl;
	
	ss << "[ch1]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH1].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH1].d_nScale  << std::endl;
	ss << std::endl;

	ss << "[ch2]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH2].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH2].d_nScale  << std::endl;
	ss << std::endl;
	
	ss << "[ch3]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH3].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH3].d_nScale  << std::endl;
	ss << std::endl;

	ss << "[ch4]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH4].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH4].d_nScale  << std::endl;
	ss << std::endl;
	
	ss << "[ch5]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH5].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH5].d_nScale  << std::endl;
	ss << std::endl;

	ss << "[ch6]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH6].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH6].d_nScale  << std::endl;
	ss << std::endl;
	
	ss << "[ch7]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH7].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH7].d_nScale  << std::endl;
	ss << std::endl;

	ss << "[ch8]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH8].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH8].d_nScale  << std::endl;
	ss << std::endl;
	
	ss << "[ch9]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH9].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH9].d_nScale  << std::endl;
	ss << std::endl;

	ss << "[ch10]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH10].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH10].d_nScale  << std::endl;
	ss << std::endl;
	
	ss << "[ch11]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH11].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH11].d_nScale  << std::endl;
	ss << std::endl;

	ss << "[ch12]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH12].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH12].d_nScale  << std::endl;
	ss << std::endl;
	
	ss << "[ch13]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH13].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH13].d_nScale  << std::endl;
	ss << std::endl;

	ss << "[ch14]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH14].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH14].d_nScale  << std::endl;
	ss << std::endl;
	
	ss << "[ch15]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH15].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH15].d_nScale  << std::endl;
	ss << std::endl;

	ss << "[ch16]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH16].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH16].d_nScale  << std::endl;
	ss << std::endl;
	
	ss << "[ch17]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH17].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH17].d_nScale  << std::endl;
	ss << std::endl;

	ss << "[ch18]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH18].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH18].d_nScale  << std::endl;
	ss << std::endl;
	
	ss << "[ch19]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH19].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH19].d_nScale  << std::endl;
	ss << std::endl;



	outFile.open(OFFSET_FILE_NAME);
	outFile << ss.rdbuf();
	outFile.close();


	DBG_I_N("\r\n");
	
	return 0;
}

CSensor::PARAMS_t* CSensor::SetOffsetDefault()
{
	for( int i=0;i<MAX_CH;i++) {
		m_Parameter[i].d_sOffset	= 0;
		m_Parameter[i].d_nScale		= 100000;
		}

	return m_Parameter;
}

void CSensor::Stop()
{
	SetRunBit(false);

//	DBG_I_N("m_bRun=false\r\n");
}


void CSensor::Run()
{
	int nRet = 0;
	int nIdx = 0;
	int nMode = 0;
	int nSSize = m_pConfig->m_nSampleRate;
	float fAval = m_pConfig->m_DeviceCfg.d_fAval;
	float fBval = m_pConfig->m_DeviceCfg.d_fBval;
	float fCval = m_pConfig->m_DeviceCfg.d_fCval;

	nRet = OpenDev();
	nRet = SetMode(nSSize, nMode);
	nRet = StartSampling();	

	SetRunBit(true);
	
	DBG_I_N("run : start \r\n");
	while(IsRun()) {
		for(int i=0;i<nSSize;i++) {
			for(int c=0;c<MAX_CH;c++) {
				m_fRawData[m_nPageIn][c][i] = ((fAval*azfSig[(i+c)%512])-fBval)*fCval;
				}
			}
		usleep(1000000);
		
		Lock();
		m_nPageOut = m_nPageIn;
		m_nPageIn++;
		m_nPageIn %= 2;
		Signal();
		Unlock();

		DBG("--adc---------%s, idx=%d\r\n", GetDateTime3().c_str(), nIdx);

		usleep(1);	
		nIdx++;
		}

	nRet = StopSampling();
	nRet = CloseDev();

_err:
	
	DBG_I_N("end of loop \r\n");

}

int CSensor::OpenDev()
{
	int nRet = ERR_NONE;

	DBG_I_N("nRet=%d\r\n", nRet);

	return nRet;
}

int CSensor::CloseDev()
{
	int nRet = ERR_NONE;

	DBG_I_N("nRet=%d\r\n", nRet);

	return nRet;
}


int CSensor::SetMode(int nRate, int nMode)
{
	int nRet = ERR_NONE;

	DBG_I_N("nRet=%d\r\n", nRet);

	return nRet;
}


int CSensor::StartSampling()
{
	int nRet = ERR_NONE;


	DBG_I_N("nRet=%d\r\n", nRet);

	return nRet;
}

int CSensor::StopSampling()
{
	int nRet = ERR_NONE;

	DBG_I_N("nRet=%d\r\n", nRet);

	return nRet;
}

