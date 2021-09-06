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
//  File         : ipsetup.cpp
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#include <iostream>
#include <fstream>

#include "struct.h"
#include "ipsetup.h"
#include "INIReader.h"

#define DEBUG
#include "debug.h"




#define IPSETUP_FILE_NAME		"./ipaddr.ini"

#define IP_WLAN0_FILE_NAME		"/etc/systemd/network/25-wireless.network"
#define IP_ETH0_FILE_NAME		"/etc/systemd/network/30-eth0.network"
#define IP_ETH1_FILE_NAME		"/etc/systemd/network/31-eth1.network"

//#define IP_WLAN0_FILE_NAME		"./25-wireless.network"
//#define IP_ETH0_FILE_NAME		"./30-eth0.network"
//#define IP_ETH1_FILE_NAME		"./31-eth1.network"

CIpsetup::CIpsetup() : CBase(__func__)
{
	DBG_I_C("create id=%p\r\n", GetId());
	
	Load();
}

CIpsetup::~CIpsetup()
{
	DBG_I_C("destroy id=%p\r\n", GetId() );
}

int CIpsetup::Load()
{
	int nRet=0;
	
	INIReader *pReader = new INIReader(IPSETUP_FILE_NAME);
	if( pReader->ParseError() >= 0 ) {
		m_IpCfg.d_sEth0 = pReader->Get("", "eth0", "");
		m_IpCfg.d_sEth1 = pReader->Get("", "eth1", "");
		m_IpCfg.d_sWlan0 = pReader->Get("", "wlan0", "");

		Save();

		remove( IPSETUP_FILE_NAME );

		DBG_I_G("save & remove ipaddr.ini\r\n");
		}
	else {
		DBG_I_G("can't find ipaddr.ini\r\n");
		}

	delete pReader;
	pReader = NULL;
	
	return nRet;
}


int CIpsetup::Save()
{
	ofstream outFile;

	if( m_IpCfg.d_sWlan0 != "") {
		stringstream ss;

		ss << "[Match]" << endl;
		ss << "Name=wlan0" << endl;
		ss << endl;
		ss << "[Network]" << endl;
		ss << "DHCP=no" << endl;
		ss << "Address=" << m_IpCfg.d_sWlan0 << endl;
		ss << endl;

		outFile.open(IP_WLAN0_FILE_NAME);
		outFile << ss.rdbuf();
		outFile.close();

		DBG_I_N("wlan0 = %s\r\n", m_IpCfg.d_sWlan0.c_str());
		}

	if( m_IpCfg.d_sEth0 != "") {
		stringstream ss;

		ss << "[Match]" << endl;
		ss << "Name=eth0" << endl;
		ss << endl;
		ss << "[Network]" << endl;
		ss << "DHCP=no" << endl;
		ss << "Address=" << m_IpCfg.d_sEth0 << endl;
		ss << endl;

		outFile.open(IP_ETH0_FILE_NAME);
		outFile << ss.rdbuf();
		outFile.close();

		DBG_I_N("eth0 = %s\r\n", m_IpCfg.d_sEth0.c_str());
		}
	
	if( m_IpCfg.d_sEth1 != "") {
		stringstream ss;

		ss << "[Match]" << endl;
		ss << "Name=eth1" << endl;
		ss << endl;
		ss << "[Network]" << endl;
		ss << "DHCP=no" << endl;
		ss << "Address=" << m_IpCfg.d_sEth1 << endl;
		ss << endl;

		outFile.open(IP_ETH1_FILE_NAME);
		outFile << ss.rdbuf();
		outFile.close();

		DBG_I_N("eth1 = %s\r\n", m_IpCfg.d_sEth1.c_str());
		}
	
	return 0;
}




