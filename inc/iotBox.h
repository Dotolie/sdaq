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
//  File         : iotbox.h
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------


#ifndef __IOTBOX_H__
#define __IOTBOX_H__

#include "base.h"
#include "object.h"
#include "thread.h"


#define UGPIO_NAME    		"/dev/ugpio"

#define UGPIO_MINOR   		253
#define IOCTL_MAGIC			UGPIO_MINOR
#define IOCTL_GHIGH_SET		_IO(IOCTL_MAGIC,2)
#define IOCTL_GLOW_SET		_IO(IOCTL_MAGIC,3)

#define gWDT			1
#define gLED_PWR1		2
#define gLED_PWR2		3
#define gLED_WIFI1		4
#define gLED_WIFI2		5
#define gLED_CPU		6	
#define gETH_PWR		7	
#define gWIFI_PWR		8



typedef struct  
{
	char d_szVersion[8];
	char d_szModelUid[256];
	char d_szServerUri[256];
	char d_szModelType[256];
	char d_szSiteId[256];

	char d_szMac[24];
	bool d_bEth0Dhcp;
	uint32_t d_nEth0Ip;
	uint32_t d_nEth0Gw;
	uint32_t d_nEth0Mask;

	bool d_bWlan0Enable;
	char d_szSsid[16];
	char d_szBssid[24];
	char d_szKey[16];

	char d_szWifiIp[16];
	
	double d_dMountTime;
	double d_dSensorTime;
} __attribute__((packed)) DEVICE_t, *DEVICE_PTR_t;




using namespace std;



class CIotbox: public CObject, CBase
{
public:
	DEVICE_t	m_Device;
	
private:
	int m_nFd;
	int m_nLink;
	
public:
	CIotbox();
	~CIotbox();

	int SaveIniFile(DEVICE_t *pDev, int nEnc);
	int SetDefault();
	int getMac( char*);
	int getLink();
	int LedSet(int);
	char* getVersion();
	char* getIp4wifi();
	char* getMAC();
	char* getSSID();
	char* getBSSID();	
	double getMountTime();
	double getSensorTime();
	int SaveMountTimeFile(DEVICE_t *pDev, int nEnc);	
	int SaveSensorTimeFile(DEVICE_t *pDev, int nEnc);
	void SaveVersionNo();
	
	virtual int MsgProc(int message, long wparam, long lparam);

private:		
	int On_MSG_CREATE( long, long );
	int On_MSG_TIMER( long, long );
	int On_MSG_EVENT( long , long );
	int On_MSG_QUIT( long, long );	

	int scanIp4wifi(char*);
	int getSsid(char *);	

	int readMountTime();
	int readSensorTime();
};

#endif  // __IOTBOX_H__
