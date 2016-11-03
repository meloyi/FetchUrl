#include "stdafx.h"
#include "Crawler.h"

#ifdef _DEBUG
#pragma comment(lib, "libcurld_imp.lib")
#pragma comment(lib, "libcurld.lib")
#else
#pragma comment(lib, "libcurl_imp.lib")
#pragma comment(lib, "libcurl.lib")
#endif

CCrawler::CCrawler()
{
	m_status = -1;

	m_curl = curl_easy_init();
}

CCrawler::~CCrawler()
{
	if ( m_curl )
	{
		curl_easy_cleanup(m_curl);
		m_curl = NULL;
	}
}

// ����HTTP SERVER����ͷ����Ϣ...
// ������ص�ͷ����Ϣ���ж��У������ö��
static size_t header_callback_baidu(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	CString *pstr = (CString *)userdata;
	if ( strncmp((const char *)ptr, "Location: ", strlen("Location: ")) == 0 )
	{
		CString tmp;
		tmp = CString((const char *)ptr);
		tmp.Replace( "Location: " , "" );
		*pstr = tmp;
	}

	return size * nmemb;
}

// ��ȡ����
static size_t write_callback( char *ptr, size_t size, size_t nmemb, void *userdata)
{
	//PCrawlerWriteData pcwd = static_cast<PCrawlerWriteData>(userdata);
	//size_t len = size * nmemb;
	string& buffer =	*(static_cast<string*>(userdata));
	size_t len = size * nmemb;
	for(size_t i = 0; i<len; ++i)
	{
		buffer += *ptr;
		++ptr;
	}
	return len;
}

bool CCrawler::Crawler_baidu(CrawlerArgs &ca, getMode fMode, string &strContent )
{
	CString strRefer;
	//CrawlerWriteData	cwd;
	int delay = 0,curtime = 0,failret = -10;
	int nRedirectTimes = 0;
	CString strRealUrl;

	if(!m_curl)
	{
		return false;
	}

	// ����URL
	curl_easy_setopt(m_curl, CURLOPT_URL, ca.strUrl);

	// �����SSL��վ������SSL��վ��֤�������֤
	curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYHOST, 0L);

	//�Զ���ת
	curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1);

	// ��ʱ����
	curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, CRALWER_TIMEOUT);

	//curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1); 

		// ����ͷ
	curl_slist *chunk = NULL;
	chunk = curl_slist_append( chunk, "Accept: text/html, application/xhtml+xml, */*" );  
	chunk = curl_slist_append( chunk, "Accept-Language: zh-CN" ); 
	chunk = curl_slist_append( chunk, "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/535.7 (KHTML, like Gecko)" ); 
	chunk = curl_slist_append( chunk, "Accept-Encoding: gzip, deflate" ); 
	chunk = curl_slist_append( chunk, "Connection: keep-alive" );

	if ( fMode == CONTENT )
	{
		strRefer.Format(_T("Referer: %s"), ca.strReferUrl);
		chunk = curl_slist_append( chunk, strRefer.GetBuffer() );

		// ���ö�ȡHTTP�������ݵĻص�����
		curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_callback);

		// ���ûص������Ĳ�������write_callback���������һ������
		curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &strContent);
	}
	else if ( fMode == HEAD )
	{
		/* we want the headers to this file handle */ 
		curl_easy_setopt(m_curl, CURLOPT_HEADERFUNCTION, header_callback_baidu);	

		//���ûص������Ĳ�������header_callback_baidu���������һ������
		curl_easy_setopt(m_curl, CURLOPT_HEADERDATA, &(ca.strRealUrl));
	}

	curl_easy_setopt( m_curl, CURLOPT_HTTPHEADER, chunk );

	//curl_easy_setopt(m_curl, CURLOPT_HTTP_CONTENT_DECODING, 1);
	curl_easy_setopt(m_curl, CURLOPT_ENCODING, "gzip");

	curl_easy_setopt( m_curl, CURLOPT_COOKIEFILE, "cookiefile.txt" );
	curl_easy_setopt( m_curl, CURLOPT_COOKIEJAR, "cookiefile.txt" );

	//cwd.strPath = ca.strPath;

	// ����ѡ������ã����Բο���http://curl.haxx.se/libcurl/c/curl_easy_setopt.html


//retry:
	// ��������
	CURLcode curlRes = curl_easy_perform(m_curl);
	//char *url;
	bool bRet = false;

// 	do 
// 	{
// 		if ( curlRes == CURLE_OK || CURLE_HTTP_RETURNED_ERROR == curlRes )
// 		{
// 			curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &m_status);
// 
// 			if ( m_status != CRAWLER_STATUSOK )
// 			{
// 				if ( m_status == 302 || m_status == 301 )
// 				{
// 					if ( (CURLE_OK == curl_easy_getinfo(m_curl, CURLINFO_REDIRECT_URL, &url)) && url )
// 					{
// 						log.warn("[%d-%d] redirect url: %s", GetCurrentProcessId(), GetCurrentThreadId(), url);
// 						m_redirect_url = url;
// 
// 						curl_easy_setopt(m_curl, CURLOPT_URL, url);
// 						
// 						if ( nRedirectTimes < 3 )
// 						{
// 							nRedirectTimes++;
// 							goto retry;
// 						}
// 					}
// 				}
// 				break;
// 			}
// 		}
// 		else
// 		{ 
// 			m_status = (int)curlRes;
// 
// 			// ��������վ���ܷ��ʣ����߳�ʱ��
// 			bRet = false;
// 			break;
// 		}
// 
// 		bRet = true;
// 	} while (FALSE);

	if( curlRes == CURLE_OK )
	{
		//ca.strUrl = strRealurl;
		//ca.strReferUrl = _T("");
		bRet = true;
	}

	return bRet; 
}