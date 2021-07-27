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
//  File         : iotBox.cpp
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_ether.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <iwlib.h>
#include <zmq.hpp>


#include "object.h"
#include "taskMgr.h"
#include "timerMgr.h"
#include "event.h"
#include "iotBox.h"
#include "version.h"
#include "INIReader.h"
#include "oled.h"
#include "utils.h"
#include "version.h"


#define DEBUG
#include "debug.h"


#define DEVICE_FILE_NAME		"./Configs/device.ini"
#define DEVICE_FILE_NAME_ENC	"./Configs/device.enc"

#define MOUNTTIME_FILE_NAME		"./Configs/mount_time.ini"
#define MOUNTTIME_FILE_NAME_ENC	"./Configs/mount_time.enc"

#define SENSORTIME_FILE_NAME	"./Configs/sensor_time.ini"
#define SENSORTIME_FILE_NAME_ENC "./Configs/sensor_time.enc"

//#define DSP_FILE_NAME			"./Configs/config_ssengine.ini"
//#define DSP_FILE_NAME_ENC		"./Configs/config_ssengine.enc"

#define IOTPUT_VERSION_FILE_NAME	"/tmp/iot-put"


#define MODEL_TYPE		"ITB_TYPE1"
#define MODEL_UID		"AC5D5C08142A"
#define SITE_ID			"C000000015"
#define SERVER_URI		"ssl://poc-brighticsiot.samsungsds.com:8001"
#define SITE_ID2		"S00001"
#define SERVER_URI2		"ssl://10.138.15.161:8011"


#define	TIMER_IOTBOX_ID_0 		(TASK_ID_IOTBOX + 1)


extern COled	g_Oled;
extern CTimerMgr g_TimerMgr;
extern CTaskMgr g_TaskMgr;

using namespace zmq;

CIotbox g_IotBox;


CIotbox::CIotbox() : CBase(__func__)
{
	DBG_I_N("create id=0x%p\r\n", this);

	
	g_TaskMgr.AddTask(TASK_ID_IOTBOX, this);

}

CIotbox::~CIotbox() 
{
	g_TaskMgr.DelTask(TASK_ID_IOTBOX);

	DBG_I_N("destroy \r\n");
}


int CIotbox::SetDefault()
{
	strcpy( m_Device.d_szVersion, VERSION );
	strcpy( m_Device.d_szModelType, MODEL_TYPE );
	strcpy( m_Device.d_szModelUid, getMAC() );

	strcpy( m_Device.d_szServerUri, SERVER_URI2 );
	strcpy( m_Device.d_szSiteId, SITE_ID2 );

	m_Device.d_bEth0Dhcp = true;
	m_Device.d_nEth0Ip =  inet_addr("192.168.0.10");
	m_Device.d_nEth0Gw = inet_addr("192.168.0.1");
	m_Device.d_nEth0Mask = inet_addr("255.255.0.0");
 
	m_Device.d_bWlan0Enable = false;
	strcpy( m_Device.d_szSsid, "send0" );
	strcpy( m_Device.d_szKey, "1234abcd" );
	
	return 0;
}

int CIotbox::SaveIniFile(DEVICE_t *pDev, int nEnc)
{
	struct sockaddr_in addr0, addr1;

	ofstream outFile;
	stringstream ss;

	ss << "; " << std::endl;
	ss << "; Generated device.ini file for default" << std::endl;
	ss << "; " << std::endl << std::endl;

	ss << "[system]" << std::endl;
//	ss << "version = " << pDev->d_szVersion << std::endl;
	ss << "model = " << pDev->d_szModelType << std::endl;
	ss << "uid = " << pDev->d_szModelUid << std::endl;
	ss << "siteid = " << pDev->d_szSiteId << std::endl;
	ss << "serverUri = " << pDev->d_szServerUri << std::endl << std::endl;

	ss << "#siteid = C000000015" << std::endl;
	ss << "#serverUri = ssl://poc-brighticsiot.samsungsds.com:8001" << std::endl << std::endl;
	
#if 0
	ss << "[ethernet]" << std::endl;
	ss << "dhcp = " << pDev->d_bEth0Dhcp << std::endl;
	addr0.sin_addr.s_addr = pDev->d_nEth0Ip;
	ss << "ip = " << inet_ntoa(addr0.sin_addr) << std::endl;
	addr1.sin_addr.s_addr = pDev->d_nEth0Gw;
	ss << "gw = " << inet_ntoa(addr1.sin_addr) << std::endl;
	addr0.sin_addr.s_addr = pDev->d_nEth0Mask;
	ss << "mask = " << inet_ntoa(addr0.sin_addr) << std::endl << std::endl;

	ss << "[wifi]" << std::endl;
	ss << "enable = " << pDev->d_bWlan0Enable << std::endl;
	ss << "ssid = " << pDev->d_szSsid << std::endl;
	ss << "key = " << pDev->d_szKey << std::endl;
#endif


	if( nEnc == 1 ) {
		string s = ss.str();
		int nSize = s.length();
		unsigned char* p = (unsigned char*)s.c_str();
		unsigned char* pEnc = (unsigned char*)malloc(nSize);

		enc_convert(p, pEnc, nSize);
		
		outFile.open(DEVICE_FILE_NAME_ENC, ios::binary);
		for(int i=0;i<nSize;i++)
			outFile << pEnc[i];
		outFile.close();

		free(pEnc);
		}
	else {
		outFile.open(DEVICE_FILE_NAME);
		outFile << ss.rdbuf();
		outFile.close();
		}

	DBG_I_C("\r\n");
	
	return 0;
}


int CIotbox::getMac ( char* pMac )
{
	int nRet = -1;
	int fd = -1;
	const char *iface = "wlan0";
	struct ifreq ifr;

	memset(&ifr, 0, sizeof(ifr));
	fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);

	if (0 == (nRet = ioctl(fd, SIOCGIFHWADDR, &ifr))) {
		char *pTemp;
		pTemp = (char *)ifr.ifr_hwaddr.sa_data;
		sprintf(pMac, "%.2x%.2x%.2x%.2x%.2x%.2x" , pTemp[0], pTemp[1], pTemp[2], pTemp[3], pTemp[4], pTemp[5]);
		//display mac address
		DBG_I_N("wifi Mac : %.2x%.2x%.2x%.2x%.2x%.2x\n" , pTemp[0], pTemp[1], pTemp[2], pTemp[3], pTemp[4], pTemp[5]);
	}

	::close(fd);

	return nRet;
}

int CIotbox::getLink ()
{
	int nRet = -1;
	int fd = -1;
	const char *iface = "eth0";
	struct ifreq ifr;
	struct ethtool_value eth;

	memset(&ifr, 0, sizeof(ifr));

	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
	ifr.ifr_data = (caddr_t)&eth;
	eth.cmd = ETHTOOL_GLINK;

	if (0 == (nRet = ioctl(fd, SIOCETHTOOL, &ifr)) ) {
		nRet = eth.data;
		DBG_I_N("nLink=%d\r\n", nRet);
	}

	::close(fd);

	return nRet;
}

int CIotbox::LedSet(int nVal)
{

	if( nVal ) {
//		if( m_Device.d_bWlan0Enable )
//			ioctl(m_nFd,IOCTL_GHIGH_SET,gLED_CPU );
//		ioctl(m_nFd,IOCTL_GHIGH_SET,gLED_WIFI1);
		}
	else {
//		if( m_Device.d_bWlan0Enable )
//			ioctl(m_nFd,IOCTL_GLOW_SET,gLED_CPU );
//		ioctl(m_nFd,IOCTL_GLOW_SET,gLED_WIFI1);
		}

	return 0;
}

int CIotbox::MsgProc(int message, long wparam, long lparam)
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

int CIotbox::On_MSG_CREATE( long wparam, long lparam )
{
	char strTemp[32] = {0,};
	INIReader *pReader = NULL;
//	struct sockaddr_in addr;

	DBG_I_Y("w=%d, l=%d\r\n", wparam, lparam);

	SaveVersionNo();

	if( getMac(strTemp) > -1 ) {
		strcpy(m_Device.d_szMac, strTemp);
		}
	else {
		strcpy(m_Device.d_szMac, "no mac");
		}

	m_nLink = getLink();

	pReader = new INIReader(DEVICE_FILE_NAME_ENC);
	if( pReader->ParseError() < 0 ) {
//		DBG_E_R("device.enc does't exist\r\n");
		SetDefault();

		pReader = new INIReader(DEVICE_FILE_NAME);	
		if( pReader->ParseError() < 0 ) {
			DBG_E_R("device.ini does't exist\r\n");	
			SaveIniFile( &m_Device, 0 );
			}
		}	

//	strcpy(m_Device.d_szVersion, pReader->Get("system", "version", VERSION).c_str());
	strcpy(m_Device.d_szModelType, pReader->Get("system", "model", MODEL_TYPE).c_str());
	strcpy(m_Device.d_szModelUid, pReader->Get("system", "uid", MODEL_UID).c_str());
	strcpy(m_Device.d_szSiteId, pReader->Get("system", "siteid", SITE_ID2).c_str());
	strcpy(m_Device.d_szServerUri, pReader->Get("system", "serverUri", SERVER_URI2).c_str());

	m_Device.d_bEth0Dhcp =  pReader->GetBoolean("ethernet", "dhcp", true);
	m_Device.d_nEth0Ip = inet_addr( pReader->Get("ethernet", "ip", "192.168.0.10").c_str());
	m_Device.d_nEth0Gw = inet_addr( pReader->Get("ethernet", "gw", "192.168.0.1").c_str());
	m_Device.d_nEth0Mask = inet_addr( pReader->Get("ethernet", "mask", "255.255.0.0").c_str());


	m_Device.d_bWlan0Enable = pReader->GetBoolean("wifi", "enable", false);
	strcpy(m_Device.d_szSsid, pReader->Get("wifi", "ssid", "send0").c_str());
	strcpy(m_Device.d_szKey, pReader->Get("wifi", "key", "1234abcd").c_str());	
	
	delete pReader;
	pReader = NULL;


	getSsid("wlan0");
	scanIp4wifi("wlan0");

	readMountTime();
	readSensorTime();

	g_Oled.DisplayFrame(m_Device.d_szModelType, m_Device.d_szModelUid);
	
	g_TimerMgr.addTimer(TASK_ID_IOTBOX, 1000, TIMER_IOTBOX_ID_0, 0);



	return 0;
}

int CIotbox::On_MSG_TIMER( long wparam, long lparam )
{
	int nRet = 0;
	char szBuf[6];

//	DBG_I_C("uid=%d, l=%d\r\n", wparam, lparam);

	switch(wparam) {
		case TIMER_IOTBOX_ID_0:
			getSsid("wlan0");
			g_Oled.UpdateWifi(getSSID());
			g_Oled.DisplayValues(0);
			break;
		}

	return nRet;
}

int CIotbox::On_MSG_EVENT( long wparam, long lparam )
{
	
	DBG_I_G("w=%d, l=%d\n", wparam, lparam);

	switch(wparam) {
		case EVENT_IOTBOX_GETIP:
			scanIp4wifi("wlan0");
			break;

		default:
			break;
		}
	
	return 0;
}

int CIotbox::On_MSG_QUIT( long wparam, long lparam )
{

	g_TimerMgr.delTimer(TIMER_IOTBOX_ID_0);

	DBG_I_P("w=%d, l=%d\r\n", wparam, lparam);
	
	return 0;
}

char* CIotbox::getVersion()
{
	return m_Device.d_szVersion;
}

char* CIotbox::getIp4wifi()
{
	return m_Device.d_szWifiIp;
}

char* CIotbox::getMAC()
{
	return m_Device.d_szMac;
}

char* CIotbox::getSSID()
{
	return m_Device.d_szSsid;
}

char* CIotbox::getBSSID()
{
	return m_Device.d_szBssid;
}

double CIotbox::getMountTime()
{
	return m_Device.d_dMountTime;
}

double CIotbox::getSensorTime()
{
	return m_Device.d_dSensorTime;
}

int CIotbox::scanIp4wifi(char *szName)
{
	struct ifaddrs *ifaddr, *ifa;
	int family, s;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		return -1;
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		family = ifa->ifa_addr->sa_family;

		if (family == AF_INET) {
			s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			if (s != 0) {
				DBG_E_R("getnameinfo() failed: %s\n", gai_strerror(s));
				strcpy(m_Device.d_szWifiIp, "not found");
				return -2;
				}
			if( strcmp(ifa->ifa_name, szName) == 0 ) {
				strcpy(m_Device.d_szWifiIp, host);
				DBG_I_N("wifi ip=%s\r\n", host);
				return 0;
				}
//			printf("<Interface>: %s \t <Address> %s\n", ifa->ifa_name, host);
			}
		}
	return -1;
}

int CIotbox::getSsid ( char* ifname )
{
	int nRet = -1;

	wireless_scan_head head;
	wireless_scan *result;
	iwrange range;
	int sock;
	wireless_config cfg;
	struct iwreq	wrq;

	/* Open socket to kernel */
	sock = iw_sockets_open();
#if 0
	/* Get some metadata to use for scanning */
	if (iw_get_range_info(sock, ifname, &range) < 0) {
			 printf("Error during iw_get_range_info. Aborting.\n");
			 return FAIL; 
	}

	/* Perform the scan */
	if (iw_scan(sock, ifname, range.we_version_compiled, &head) < 0) {
			 printf("Error during iw_scan. Aborting.\n");
			 return FAIL; 
	}

	/* Traverse the results */
	result = head.result;
	while (NULL != result) {
			 //printf("%s\n", result->b.essid);
			 result = result->next;
	}
#endif
	if ((nRet = iw_get_basic_config(sock, ifname, &cfg)) < 0) {
		DBG_E_R("Error iw_get_basic_config. Aborting.\n");
		strcpy(m_Device.d_szSsid, "not connected");
		return -1; 
	}
	/*
	printf("has_essid = %d\n", cfg.has_essid);
	printf("essid = %s\n", cfg.essid);
	printf("freq = %d\n", cfg.freq);
	printf("mode = %d\n", cfg.mode);
	*/
	memset( m_Device.d_szSsid, 0, sizeof(m_Device.d_szSsid));
	strncpy(m_Device.d_szSsid, cfg.essid, strlen(cfg.essid));
//	DBG_I_N("ESSID:%s\r\n", cfg.essid);


	/* Get AP address */
	if(iw_get_ext(sock, ifname, SIOCGIWAP, &wrq) >= 0) {
		char *APaddr = (char *)wrq.u.ap_addr.sa_data;
		sprintf(m_Device.d_szBssid, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\0", APaddr[0],APaddr[1],APaddr[2],APaddr[3],APaddr[4],APaddr[5]);
		}

//	DBG_I_N("BSSID:%s\r\n", m_Device.d_szBssid);

	iw_sockets_close(sock);
	
	
	return nRet;
}

int CIotbox::readMountTime()
{
	INIReader *pReader = NULL;
	double dNow = getTimeMS();

	pReader = new INIReader(MOUNTTIME_FILE_NAME_ENC);
	if( pReader->ParseError() < 0 ) {
//		DBG_E_R("mount_time.enc does't exist\r\n");

		pReader = new INIReader(MOUNTTIME_FILE_NAME);	
		if( pReader->ParseError() < 0 ) {
			DBG_E_R("mount_time.ini does't exist\r\n"); 
			SaveMountTimeFile( &m_Device, 0 );
			}
		}

	m_Device.d_dMountTime = pReader->GetReal("time", "mount", dNow);

	DBG_I_N("mount time:%f\r\n", m_Device.d_dMountTime);


	delete pReader;
	
	return 0;
}

int CIotbox::readSensorTime()
{
	INIReader *pReader = NULL;
	double dNow = getTimeMS();
	
	pReader = new INIReader(SENSORTIME_FILE_NAME_ENC);
	if( pReader->ParseError() < 0 ) {
//		DBG_E_R("sensor_time.enc does't exist\r\n");

		pReader = new INIReader(SENSORTIME_FILE_NAME);	
		if( pReader->ParseError() < 0 ) {
			DBG_E_R("sensor_time.ini does't exist\r\n"); 
			SaveSensorTimeFile( &m_Device, 0 );
			}
		}


	m_Device.d_dSensorTime = pReader->GetReal("time", "sensor", dNow);

	DBG_I_N("sensor time:%f\r\n", m_Device.d_dSensorTime );

	delete pReader;

	return 0;
}


int CIotbox::SaveMountTimeFile(DEVICE_t *pDev, int nEnc)
{
	struct sockaddr_in addr0, addr1;

	ofstream outFile;
	stringstream ss;
	double dNow = getTimeMS();
	string szTime = to_string((long)dNow);

	ss << "[time]" << std::endl;
	ss << "mount = " << szTime << std::endl;

	if( nEnc == 1 ) {
		string s = ss.str();
		int nSize = s.length();
		unsigned char* p = (unsigned char*)s.c_str();
		unsigned char* pEnc = (unsigned char*)malloc(nSize);

		enc_convert(p, pEnc, nSize);
		
		outFile.open(MOUNTTIME_FILE_NAME_ENC, ios::binary);
		for(int i=0;i<nSize;i++)
			outFile << pEnc[i];
		outFile.close();

		free(pEnc);
		}
	else {
		outFile.open(MOUNTTIME_FILE_NAME);
		outFile << ss.rdbuf();
		outFile.close();
		}

	DBG_I_C("\r\n");
	
	return 0;
}

int CIotbox::SaveSensorTimeFile(DEVICE_t *pDev, int nEnc)
{
	struct sockaddr_in addr0, addr1;

	ofstream outFile;
	stringstream ss;
	double dNow = getTimeMS();
	string szTime = to_string((long)dNow);

	ss << "[time]" << std::endl;
	ss << "sensor = " << szTime << std::endl;

	if( nEnc == 1 ) {
		string s = ss.str();
		int nSize = s.length();
		unsigned char* p = (unsigned char*)s.c_str();
		unsigned char* pEnc = (unsigned char*)malloc(nSize);

		enc_convert(p, pEnc, nSize);
		
		outFile.open(SENSORTIME_FILE_NAME_ENC, ios::binary);
		for(int i=0;i<nSize;i++)
			outFile << pEnc[i];
		outFile.close();

		free(pEnc);
		}
	else {
		outFile.open(SENSORTIME_FILE_NAME);
		outFile << ss.rdbuf();
		outFile.close();
		}

	DBG_I_C("\r\n");
	
	return 0;
}

void CIotbox::SaveVersionNo()
{
	ofstream outFile;
	stringstream ss;

	ss << VERSION << std::endl;

	outFile.open(IOTPUT_VERSION_FILE_NAME);
	outFile << ss.rdbuf();
	outFile.close();
}

