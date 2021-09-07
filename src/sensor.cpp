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
#include <fcntl.h>
#include <sys/ioctl.h>

#include "sensor.h"
#include "INIReader.h"
#include "error.h"
#include "utils.h"
//#include "signal512.h"

#define DEBUG
#include "debug.h"



#define OFFSET_FILE_NAME		"./cal.ini"
#define	FPGA_DEV				"/dev/fpga_spi"

#define	TOTAL_ADC				5
#define NUM_ADC_CH				8

#define GPIO_NR(bank, nr)		(((bank) - 1) * 32 + (nr))
#define DIR_INPUT				0
#define DIR_OUTPUT				1

#define AD_CMD0					GPIO_NR(3, 16)		//EIM_D16		80
#define AD_CMD1					GPIO_NR(3, 17)		//EIM_D17		81
#define AD_CMD2					GPIO_NR(3, 18)		//EIM_D18		82
#define AD_CMD3					GPIO_NR(3, 19)		//EIM_D19		83

#define FPGASPI_READ			_IO('F', 1)
#define FPGASPI_WRITE			_IO('F', 2)
#define FPGASPI_REGREAD			_IO('F', 3)
#define FPGASPI_REGWRITE		_IO('F', 4)
#define FPGASPI_GET_DATA		_IO('F', 5)
#define FPGASPI_SET_FREQ		_IO('F', 6)

#define ADC_MAX_RANGE			0x07
#define ADC_6REF_RANGE			0x04
#define ADC_START				0x80
#define ADC_DIFF_MODE			0x08
#define ADC_SINGLE_MODE			0x00


enum {
	FPGA_1KHZ_SAMPLE = 0,
 	FPGA_2KHZ_SAMPLE,
 	FPGA_4KHZ_SAMPLE,
 	FPGA_8KHZ_SAMPLE,
	FPGA_16KHZ_SAMPLE,
	FPGA_32KHZ_SAMPLE,
	FPGA_64KHZ_SAMPLE,
};

enum {
	FPGA_CMD_ADC0 = 0x0,
 	FPGA_CMD_ADC1 = 0x1,
 	FPGA_CMD_ADC2 = 0x2,
	FPGA_CMD_ADC3 = 0x3,
 	FPGA_CMD_ADC4 = 0x4,
	FPGA_CMD_ADC0_BYPASS = 0x5,
	FPGA_CMD_ADC1_BYPASS = 0x6,
	FPGA_CMD_ADC2_BYPASS = 0x7,
	FPGA_CMD_ADC3_BYPASS = 0x8,
	FPGA_CMD_ADC4_BYPASS = 0x9,
	FPGA_CMD_ADC0_SAMPLE = 0xa,
	FPGA_CMD_ADC1_SAMPLE = 0xb,
	FPGA_CMD_ADC2_SAMPLE = 0xc,
	FPGA_CMD_ADC3_SAMPLE = 0xd,
	FPGA_CMD_ADC4_SAMPLE = 0xe,
	FPGA_CMD_ADC_START = 0xf,
};

enum {
	ADC_SINGLE = 0,
	ADC_DIFF = 1,
};

struct spi_info{
	unsigned int 	channel;
	unsigned char *	inbuff;
	unsigned char *	outbuff;
	unsigned int 	size;
};

CSensor::CSensor(void *pInst) : Runnable(__func__)
{
	int nRet = 0;

	m_pConfig = (CConfig*)pInst;
	
	m_nPageMax = 2;
	m_nPageIn = 0;
	m_nPageOut = 0;

	InitLock();
	InitCond();

	m_nFpga = -1;

	
	nRet = LoadOffsetFile();
//	for(int i = 0;i<MAX_CH;i++) {
//		DBG("i=%d, offset=%d, scale=%d\r\n",i, m_Parameter[i].d_sOffset, m_Parameter[i].d_fScale);
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

	for(int i=0;i<m_pConfig->m_nChSize;i++) {
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
		m_Parameter[ADC_CH0].d_fScale	= pReader->GetReal("ch0", "scale", 1);
		
		m_Parameter[ADC_CH1].d_sOffset	= pReader->GetInteger("ch1", "offset", 0);
		m_Parameter[ADC_CH1].d_fScale	= pReader->GetReal("ch1", "scale", 1);

		m_Parameter[ADC_CH2].d_sOffset 	= pReader->GetInteger("ch2", "offset", 0);
		m_Parameter[ADC_CH2].d_fScale	= pReader->GetReal("ch2", "scale", 1);

		m_Parameter[ADC_CH3].d_sOffset 	= pReader->GetInteger("ch3", "offset", 0);
		m_Parameter[ADC_CH3].d_fScale	= pReader->GetReal("ch3", "scale", 1);

		m_Parameter[ADC_CH4].d_sOffset	= pReader->GetInteger("ch4", "offset", 0);
		m_Parameter[ADC_CH4].d_fScale	= pReader->GetReal("ch4", "scale", 1);
		
		m_Parameter[ADC_CH5].d_sOffset	= pReader->GetInteger("ch5", "offset", 0);
		m_Parameter[ADC_CH5].d_fScale	= pReader->GetReal("ch5", "scale", 1);

		m_Parameter[ADC_CH6].d_sOffset 	= pReader->GetInteger("ch6", "offset", 0);
		m_Parameter[ADC_CH6].d_fScale	= pReader->GetReal("ch6", "scale", 1);

		m_Parameter[ADC_CH7].d_sOffset 	= pReader->GetInteger("ch7", "offset", 0);
		m_Parameter[ADC_CH7].d_fScale	= pReader->GetReal("ch7", "scale", 1);

		m_Parameter[ADC_CH8].d_sOffset	= pReader->GetInteger("ch8", "offset", 0);
		m_Parameter[ADC_CH8].d_fScale	= pReader->GetReal("ch8", "scale", 1);
		
		m_Parameter[ADC_CH9].d_sOffset	= pReader->GetInteger("ch9", "offset", 0);
		m_Parameter[ADC_CH9].d_fScale	= pReader->GetReal("ch9", "scale", 1);

		m_Parameter[ADC_CH10].d_sOffset = pReader->GetInteger("ch10", "offset", 0);
		m_Parameter[ADC_CH10].d_fScale	= pReader->GetReal("ch10", "scale", 1);

		m_Parameter[ADC_CH11].d_sOffset = pReader->GetInteger("ch11", "offset", 0);
		m_Parameter[ADC_CH11].d_fScale	= pReader->GetReal("ch11", "scale", 1);

		m_Parameter[ADC_CH12].d_sOffset	= pReader->GetInteger("ch12", "offset", 0);
		m_Parameter[ADC_CH12].d_fScale	= pReader->GetReal("ch12", "scale", 1);
		
		m_Parameter[ADC_CH13].d_sOffset	= pReader->GetInteger("ch13", "offset", 0);
		m_Parameter[ADC_CH13].d_fScale	= pReader->GetReal("ch13", "scale", 1);

		m_Parameter[ADC_CH14].d_sOffset = pReader->GetInteger("ch14", "offset", 0);
		m_Parameter[ADC_CH14].d_fScale	= pReader->GetReal("ch14", "scale", 1);

		m_Parameter[ADC_CH15].d_sOffset = pReader->GetInteger("ch15", "offset", 0);
		m_Parameter[ADC_CH15].d_fScale	= pReader->GetReal("ch15", "scale", 1);

		m_Parameter[ADC_CH16].d_sOffset	= pReader->GetInteger("ch16", "offset", 0);
		m_Parameter[ADC_CH16].d_fScale	= pReader->GetReal("ch16", "scale", 1);
		
		m_Parameter[ADC_CH17].d_sOffset	= pReader->GetInteger("ch17", "offset", 0);
		m_Parameter[ADC_CH17].d_fScale	= pReader->GetReal("ch17", "scale", 1);

		m_Parameter[ADC_CH18].d_sOffset = pReader->GetInteger("ch18", "offset", 0);
		m_Parameter[ADC_CH18].d_fScale	= pReader->GetReal("ch18", "scale", 1);

		m_Parameter[ADC_CH19].d_sOffset = pReader->GetInteger("ch19", "offset", 0);
		m_Parameter[ADC_CH19].d_fScale	= pReader->GetReal("ch19", "scale", 1);
		}

	delete pReader;

//	for(int ch=0;ch<MAX_CHANNEL;ch++) 
//		DBG_I_N("ch[%d]off=%d, ch[%d]scale=%lf\r\n",  ch, m_Parameter[ch].d_sOffset, ch, m_Parameter[ch].d_fScale);

	return 0;
}

int CSensor::SaveOffsetFile(PARAMS_t *pParam, int nEnc)
{
	ofstream outFile;
	stringstream ss;

	ss << ";" << std::endl;
	ss << "; Generated cal.ini file for default" << std::endl;
	ss << ";" << std::endl;
	ss << "; offset : -500 ~ 500" << std::endl;
	ss << "; scale  : -1000.0 ~ +1000.0" << std::endl;
	ss << ";" << std::endl << std::endl;

	ss << "[ch0]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH0].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH0].d_fScale  << std::endl;
	ss << std::endl;
	
	ss << "[ch1]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH1].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH1].d_fScale  << std::endl;
	ss << std::endl;

	ss << "[ch2]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH2].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH2].d_fScale  << std::endl;
	ss << std::endl;
	
	ss << "[ch3]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH3].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH3].d_fScale  << std::endl;
	ss << std::endl;

	ss << "[ch4]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH4].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH4].d_fScale  << std::endl;
	ss << std::endl;
	
	ss << "[ch5]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH5].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH5].d_fScale  << std::endl;
	ss << std::endl;

	ss << "[ch6]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH6].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH6].d_fScale  << std::endl;
	ss << std::endl;
	
	ss << "[ch7]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH7].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH7].d_fScale  << std::endl;
	ss << std::endl;

	ss << "[ch8]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH8].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH8].d_fScale  << std::endl;
	ss << std::endl;
	
	ss << "[ch9]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH9].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH9].d_fScale  << std::endl;
	ss << std::endl;

	ss << "[ch10]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH10].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH10].d_fScale  << std::endl;
	ss << std::endl;
	
	ss << "[ch11]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH11].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH11].d_fScale  << std::endl;
	ss << std::endl;

	ss << "[ch12]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH12].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH12].d_fScale  << std::endl;
	ss << std::endl;
	
	ss << "[ch13]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH13].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH13].d_fScale  << std::endl;
	ss << std::endl;

	ss << "[ch14]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH14].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH14].d_fScale  << std::endl;
	ss << std::endl;
	
	ss << "[ch15]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH15].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH15].d_fScale  << std::endl;
	ss << std::endl;

	ss << "[ch16]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH16].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH16].d_fScale  << std::endl;
	ss << std::endl;
	
	ss << "[ch17]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH17].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH17].d_fScale  << std::endl;
	ss << std::endl;

	ss << "[ch18]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH18].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH18].d_fScale  << std::endl;
	ss << std::endl;
	
	ss << "[ch19]" << std::endl;
	ss << "offset = " 	<< pParam[ADC_CH19].d_sOffset << std::endl;
	ss << "scale = " 	<< pParam[ADC_CH19].d_fScale  << std::endl;
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
		m_Parameter[i].d_fScale		= 1.0f;
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
	int nOffset = 0;
	int nBuf_idx = 0;
	int nType = 1; 	// m_pConfig->m_DeviceCfg.d_nType; (only diff type)
	char cVolt = m_pConfig->m_DeviceCfg.d_nVolt;
	
	int nSSize = m_pConfig->m_nSampleRate;			// 1024, 2048, 4096, 8192, 65536
	int nPSize = nSSize/64;							//   16,   32,   64,  128,  1024
	int nChSize = m_pConfig->m_nChSize;				//   20,   20,   20,   20,     5
	int nSRate = m_pConfig->m_DeviceCfg.d_nSRate; 	//     1,   2,    3,    4,     5
	float fAval = m_pConfig->m_DeviceCfg.d_fAval;
	float fBval = m_pConfig->m_DeviceCfg.d_fBval;
	float fCval = m_pConfig->m_DeviceCfg.d_fCval;

	short *psData = (short*)m_cData;
	struct spi_info sit;

	sit.channel = 0;
	sit.inbuff = 0;
	sit.outbuff = m_cData;

	nRet = OpenDev();
	if( nRet != ERR_NONE ) goto _err;
	nRet = SetMode( nSRate , nType, cVolt);
	if( nRet != ERR_NONE ) goto _err;	
	nRet = StartSampling();	
	if( nRet != ERR_NONE ) goto _err;

	SetRunBit(true);
	
	DBG_I_N("run : start \r\n");
	
	while(IsRun()) {
		nRet = ioctl(m_nFpga, FPGASPI_GET_DATA, &sit);
		nOffset = nIdx % 64;
#if 0
		for (int j = 0; j < nPSize; j++) {
			for (int k = 0; k < 5; k++) {
				if( nSRate == SRATE_65536) 	nBuf_idx = ( 2 * j) + (2048 * k);
				else 						nBuf_idx = (16 * j) + (2048 * k);

				m_fRawData[m_nPageIn][k*4  ][j + nOffset*nPSize] = 	(float)((short)(m_cData[nBuf_idx    ] << 8 | m_cData[nBuf_idx + 1]) - m_Parameter[k*4  ].d_sOffset)*m_Parameter[k*4  ].d_fScale;
				m_fRawData[m_nPageIn][k*4+1][j + nOffset*nPSize] = 	(float)((short)(m_cData[nBuf_idx + 2] << 8 | m_cData[nBuf_idx + 3]) - m_Parameter[k*4+1].d_sOffset)*m_Parameter[k*4+1].d_fScale;
				m_fRawData[m_nPageIn][k*4+2][j + nOffset*nPSize] = 	(float)((short)(m_cData[nBuf_idx + 4] << 8 | m_cData[nBuf_idx + 5]) - m_Parameter[k*4+2].d_sOffset)*m_Parameter[k*4+2].d_fScale;
				m_fRawData[m_nPageIn][k*4+3][j + nOffset*nPSize] = 	(float)((short)(m_cData[nBuf_idx + 6] << 8 | m_cData[nBuf_idx + 7]) - m_Parameter[k*4+3].d_sOffset)*m_Parameter[k*4+3].d_fScale;
				}
			}
#else
		for(int i=0;i<nPSize;i++) {
			for(int c=0;c<nChSize;c++) {
				m_fRawData[m_nPageIn][c][i + nOffset*nPSize] = 
					((fAval * (psData[i*8 + (c%4) + (c/4)*1024] - m_Parameter[c].d_sOffset) * m_Parameter[c].d_fScale) + fBval) * fCval;
//				DBG("i=%d, i=%d, c=%d, d[%d][%d] = s[%d]\r\n", (i + nOffset*nPSize), i, c, c, (i + nOffset*nPSize), (i*8 + (c%4) + (c/4)*1024));
				}
			}
#endif
		nIdx++;

		if( (nIdx%64) == 0) {
			Lock();
			m_nPageOut = m_nPageIn;
			m_nPageIn++;
			m_nPageIn %= 2;
			Signal();
			Unlock();
			
			DBG("--adc---------%s, idx=%d\r\n", GetDateTime3().c_str(), nIdx);
			}
		usleep(1);
		}

	nRet = StopSampling();
	nRet = CloseDev();
	nRet = fpgaUnsetAll();

_err:
	
	DBG_I_N("end of loop \r\n");

}

int CSensor::OpenDev()
{
	int nRet = -ERR_OPEN_FAIL;

	if( m_nFpga < 0 ) {
		if( (m_nFpga = open( FPGA_DEV, O_RDWR)) >= 0 ) {
			nRet = ERR_NONE;
			DBG_I_N("nRet=%d\r\n", nRet);			
			}
		else {
			nRet = -ERR_OPEN_FAIL;	
			DBG_E_R("open fail, m_nFpga=%d\r\n", m_nFpga);
			}
		}
	else {
		nRet = -ERR_ALREADY_OPENED_FAIL;		
		DBG_E_R("already opened, m_nFpga=%d\r\n", m_nFpga);
		}
	
	return nRet;
}

int CSensor::CloseDev()
{
	int nRet = -ERR_NOT_OPEND;

	if( m_nFpga >= 0) {
		close(m_nFpga);
		m_nFpga = -1;
		nRet = ERR_NONE;
		DBG_I_N("nRet=%d\r\n", nRet);	
		}
	else {
		nRet = -ERR_NOT_OPEND;
		DBG_E_R("err not opend nRet=%d\r\n", nRet);
		}

	return nRet;
}


int CSensor::SetMode(int nRate, int nType, char cVolt)
{
	int nRet = -ERR_NOT_OPEND;
	int nFreq = 0;
	int nFpagFreq = 0;
	struct spi_info sit = {0};
	unsigned char wData[8] = {0};
	unsigned char rData[8] = {0};

	if( m_nFpga > -1 ) {
//		fpgaSet(FPGA_CMD_ADC0_BYPASS);

		switch(nRate) {
			case 5:	// 64k
				nFreq = 64;
				nFpagFreq = FPGA_64KHZ_SAMPLE;
				break;
			case 4: // 8k
				nFreq = 8;
				nFpagFreq = FPGA_8KHZ_SAMPLE;
				break;
			case 3:	// 4k
				nFreq = 4;
				nFpagFreq = FPGA_4KHZ_SAMPLE;				
				break;
			case 2:	// 2k
				nFreq = 2;
				nFpagFreq = FPGA_2KHZ_SAMPLE;				
				break;
			case 1:	// 1k
			default:
				nFreq = 1;
				nFpagFreq = FPGA_1KHZ_SAMPLE;			
				break;
			}
		//max1300 bypass init
		sit.channel = 0;
		sit.inbuff = wData;
		sit.outbuff = rData;
		sit.size = 1;

		for (int i = 0; i < TOTAL_ADC; i++) {
			fpgaSet(FPGA_CMD_ADC0_BYPASS + i);
	
			for(int j = 0; j < NUM_ADC_CH; j+=2) {
				//mode & channel configuration
				wData[0] = (j << 4) | ADC_START | (cVolt & 0x07);
				if( nType == MODE_DIFF) wData[0] |= ADC_DIFF_MODE;				
				nRet = ioctl(m_nFpga, FPGASPI_REGWRITE, &sit);
				if (nRet) {
					DBG_E_R("err! ADC mode set nRet=\n", nRet);
					return -1;
					}
				}
			}

		//fpga conversion 8byte
		sit.channel = 0;
		sit.size = 8;

		for (int i = 0; i < NUM_ADC_CH; i++)
			if( nType == MODE_DIFF ) {
				if( i < 4)	wData[i] = 0x80 | ((i*2) << 4);
				else		wData[i] = 0;
				}
			else {
				wData[i] = 0x80 | (i << 4);
				}
	
		for (int i = 0; i < TOTAL_ADC; i++) {
			fpgaSet(FPGA_CMD_ADC0 + i);
			nRet = ioctl(m_nFpga, FPGASPI_WRITE, &sit);
			if (nRet) {
				DBG_E_R("err! ADC start : nRet=\n", nRet);
				return -1;
				}
			}

		
		nRet = ioctl(m_nFpga, FPGASPI_SET_FREQ, &nFreq);
		if (nRet) {
			DBG_E_R("err! Set Freq : nRet=%d\n", nRet);
			return -1;
		}

		//fpga sample 2byte
		wData[0] = 0;
		wData[1] = nFpagFreq;
		sit.size = 2;
		
		for (int i = 0; i < TOTAL_ADC; i++) {
			fpgaSet(FPGA_CMD_ADC0_SAMPLE + i);
			nRet = ioctl(m_nFpga, FPGASPI_WRITE, &sit);
			if (nRet) {
				DBG_E_R("err! set sample rate : nRet=%d\n", nRet);
				return -1;
				}
			}

		nRet = ERR_NONE;
		
		DBG_I_N("SRate=%d, Freq=%d, fpga=%d, type=%d, volt=0x%02x, nRet=%d\r\n", nRate, nFreq, nFpagFreq, nType, cVolt, nRet);
		}
	else {
		nRet = -ERR_NOT_OPEND;
		DBG_E_R("device not found, m_nFpga=%d\r\n", m_nFpga);
		}

	return nRet;
}


int CSensor::StartSampling()
{
	int nRet = ERR_NONE;

	fpgaSet(FPGA_CMD_ADC_START);

	DBG_I_N("nRet=%d\r\n", nRet);

	return nRet;
}

int CSensor::StopSampling()
{
	int nRet = ERR_NONE;

	fpgaSet(FPGA_CMD_ADC0_BYPASS);

	DBG_I_N("nRet=%d\r\n", nRet);

	return nRet;
}


int CSensor::GpioSetValue(int gpio, int value)
{
	int fd, fd2, fd3;
	char buf[255]; 

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (fd < 0) {
		DBG_E_R("fail export %d", gpio);
		return -1;
		}

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%d", gpio); 
	write(fd, buf, strlen(buf));
	close(fd);

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "/sys/class/gpio/gpio%d/direction", gpio);

	fd2 = open(buf, O_WRONLY);
	if (fd2 < 0) {
		DBG_E_R("fail direction %d", gpio);
		return -1;
		}

	write(fd2, "out", 3);
	close(fd2);

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);

	fd3 = open(buf, O_WRONLY);
	if (fd3 < 0) {
		DBG_E_R("fail value %d", gpio);
		return -1;
	}

	if (value)
		write(fd3, "1", 1);
	else
		write(fd3, "0", 1); 

	close(fd3);

	return 0;
}

int CSensor::GpioUnexport(int gpio)
{
	int fd, fd2, fd3;
	char buf[255]; 

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd < 0) {
		DBG_E_R("fail unexport %d", gpio);
		return -1;
		}

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%d", gpio); 
	write(fd, buf, strlen(buf));
	close(fd);

	return 0;
}

int CSensor::fpgaSet(unsigned char cmd)
{
	int ret;

	ret = GpioSetValue(AD_CMD0, cmd & 0x01);
	if (ret)
		return -1;
	ret = GpioSetValue(AD_CMD1, cmd & 0x02);
	if (ret)
		return -1;
	ret = GpioSetValue(AD_CMD2, cmd & 0x04);
	if (ret)
		return -1;
	ret = GpioSetValue(AD_CMD3, cmd & 0x08);
	if (ret)
		return -1;
	
	return 0;
}

int CSensor::fpgaUnsetAll()
{
	int ret;

	ret = GpioUnexport(AD_CMD0);
	ret = GpioUnexport(AD_CMD1);
	ret = GpioUnexport(AD_CMD2);
	ret = GpioUnexport(AD_CMD3);
	
	return 0;
}


