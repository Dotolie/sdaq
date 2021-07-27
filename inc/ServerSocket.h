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
//  File         : ServerSocket.h
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#ifndef __SERVERSOCKET_H__
#define __SERVERSOCKET_H__

#include "Socket.h"


class CServerSocket : public CSocket
{
public:
	CServerSocket ( int port );
	CServerSocket ();
	virtual ~CServerSocket();

	const CServerSocket& operator << ( const std::string& ) const;
	const CServerSocket& operator >> ( std::string& ) const;	
  
	void accept ( CServerSocket& );
	void accept ( CServerSocket*  );
};


#endif // __SERVERSOCKET_H__
