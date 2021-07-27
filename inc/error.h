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
//  File         : error.h
//  Description  :
//  Author       : ywkim@mvtech.or.kr
//  Export       :
//  History      :
//
//------------------------------------------------------------------------------

#ifndef __ERROR_H__
#define __ERROR_H__


enum {
	ERR_NONE = 0,
	ERR_TASKMGR_CTRL_C,
	ERR_BIOT_START_FAIL,
	ERR_BIOT_DOWNLOAD_FAIL,
	ERR_CORE_CONFIG_FAIL,
	ERR_CORE_NE10_INIT_FAIL,

	ERR_SPI_OPEN_FAIL,
	ERR_SPI_CLOSE_FAIL,
	ERR_SPI_NOT_RESPOND,
	ERR_SPI_RESET_FAIL,
	ERR_SPI_NOT_OPEND,
	ERR_SPI_NAK,

	ERR_GPIO_OPEN_FAIL,
	ERR_GPIO_CLOSE_FAIL,

	ERR_DIFF_CHECKSUM,
};

#endif /* __ERROR_H__ */

