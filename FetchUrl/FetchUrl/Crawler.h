// ***************************************************************
//  Crawler.h   version:  1.0   date: 28/12/2012
//  Author:		WangShuiBing(shbwang@86nsn.org)
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2010 - 2015 SZWA  CO.,Ltd - All Rights Reserved
// ***************************************************************
// 
//  ChangeLog:
//
// ***************************************************************

#ifndef _CRAWLER_H
#define _CRAWLER_H

#include <curl/curl.h>

#define CRALWER_TIMEOUT	60

#define CRAWLER_STATUS_REDIRECT	302
#define CRAWLER_STATUSOK			200

typedef enum _eCrawlerMethod
{
	CRAWLER_METHOD_GET = 1,
	CRAWLER_METHOD_POST,
	CRAWLER_METHOD_UNKNOWN
}eCrawlerMethod;

typedef struct _CralwerWriteData
{
	CString	strPath;
	HANDLE	hFile;
	int		nStatus;

	_CralwerWriteData()
	{
		strPath = "";
		hFile = INVALID_HANDLE_VALUE;
		nStatus = 0;
	}
}CrawlerWriteData, *PCrawlerWriteData;

typedef struct _CrawlerArgs
{
	CString strReferUrl;
	CString	strUrl;
	CString strRealUrl;

	_CrawlerArgs()
	{
		strReferUrl = "";
		strUrl = "";
		strRealUrl = "";
	}
}CrawlerArgs, *PCrawlerArgs;

class CCrawler
{
public:
	
	CCrawler();
	~CCrawler();
	bool Crawler_baidu(CrawlerArgs &ca, getMode fMode, string &strContent);

	/**
	 \brief	服务器返回的状态值
	 */
	int	m_status;

	CString m_redirect_url;
private:

	CrawlerArgs	m_ca;
	CURL *m_curl;

};

#endif