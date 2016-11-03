
// FetchUrlDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FetchUrl.h"
#include "FetchUrlDlg.h"
#include "Crawler.h"
#include <regex>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFetchUrlDlg �Ի���


CFetchUrlDlg::CFetchUrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFetchUrlDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFetchUrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_URL, m_urlList);
}

BEGIN_MESSAGE_MAP(CFetchUrlDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_FETCH, &CFetchUrlDlg::OnBnClickedFetch)
END_MESSAGE_MAP()


// CFetchUrlDlg ��Ϣ�������

BOOL CFetchUrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_urlList.InsertColumn(elhNo, "No", LVCFMT_CENTER, 40);
	m_urlList.InsertColumn(elhSubLink, "������", LVCFMT_LEFT, 200);
	m_urlList.InsertColumn(elhRealLink, "�����Ӷ�Ӧ����ʵ����", LVCFMT_LEFT, 200);
	m_urlList.SetExtendedStyle(LVS_EX_FULLROWSELECT| LVS_EX_GRIDLINES);// 	

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFetchUrlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CFetchUrlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFetchUrlDlg::OnBnClickedFetch()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//UpdateData();
	CString strKeyWord;
	GetDlgItemText(IDC_EDIT_KEYWORD, strKeyWord);

	CString strEncodeKeyword;
	char *chtemp, ch_utf8[1024];
	int len;

	gb_to_utf8(strKeyWord.GetBuffer(), ch_utf8, sizeof(ch_utf8));
	chtemp = urlencode(ch_utf8, strlen(ch_utf8), &len);
	strEncodeKeyword = chtemp;
	free(chtemp);

	analyze_sbaidu(	SEARCH, strEncodeKeyword, 0 );

	CCrawler Crawler;
	CrawlerArgs ca;
	ca.strUrl = m_urls.GetAt(0).strUrl;
	ca.strReferUrl = "https://www.baidu.com";
	//Crawler.Crawler_baidu(ca, m_PageContent, CONTENT);

	int nPage = 0;
	int i = 0;
	int iIndex = 0;
	m_Page = 3;
	m_urlList.DeleteAllItems();
	do 
	{
		//��ȡҳ�������
		if ( !Crawler.Crawler_baidu(ca, CONTENT, m_PageContent) )
		{
			break;
		}

		//�������ص�ҳ�沢��ȡҳ���������
		if ( analyze_sbaidu( SUBLINK, strEncodeKeyword ) )
		{
			//�����ظ�������
			//check_same_url(se, ha.m_urls);

			if ( m_urls.IsEmpty() )
			{
				break;
			}
			else 
			{	
				string strtmp;
				CString strIndex;
				for (i=0; i<m_urls.GetSize(); i++)
				{
					ca.strUrl = m_urls.GetAt(i).strUrl;
					Crawler.Crawler_baidu(ca, HEAD, strtmp);

					strIndex.Format("%d", iIndex);
					m_urlList.InsertItem(iIndex, strIndex);
					m_urlList.SetItemText(iIndex, elhNo, strIndex);
					m_urlList.SetItemText(iIndex, elhSubLink, ca.strUrl);
					m_urlList.SetItemText(iIndex, elhRealLink, ca.strRealUrl);
					m_urlList.SetCheck(iIndex);
					UpdateWindow();
					//UpdateData(FALSE);
					iIndex++;
				}
			}
		}
		else
		{
			break;
		}

		//��ȡ��һҳ������
		if ( ++nPage <= m_Page && 
			analyze_sbaidu(NEXTPAGE, strEncodeKeyword, nPage ) &&
			m_urls.GetSize() >= 1 )
		{
			ca.strReferUrl = ca.strUrl;
			ca.strUrl = m_urls.GetAt(0).strUrl;
		}
		else
		{
			break;
		}

	} while (true);

}

bool CFetchUrlDlg::analyze_sbaidu( analyzeMode amode, CString strEncodeKeyword, int nCurrentPage)
{
	m_urls.RemoveAll();

	CString strRetUrl;
	static CString strSearchPage = "http://www.baidu.com/s?wd=%s";
	static CString strNextPage = "http://www.baidu.com/s?wd=%s&pn=%d&tn=baiduhome_pg&ie=utf-8&usm=4";	// ��һҳ
	static CString strSubPage = "http://www\\.baidu\\.com/link\\?url=.[^\"]+";
	//static CString strSubPage2 = "http://www\\.baidu\\.com/link\\?url=.[^\"]+";
	//static CString strNextPage = "http://tieba.baidu.com/f/search/res?isnew=1&kw=&qw=%s&rn=10&un=&sm=1&sd=&ed=&pn=%d";
	//static CString strSubPage = "/p/\\d+\\?pid=\\d+&cid=\\d+#\\d+";
	CString strEndPageId = "\">��һҳ";

	char utf8[128], *putf8;

	Urls url;

	if ( amode == SEARCH )
	{
		url.strUrl.Format(strSearchPage, strEncodeKeyword);
		m_urls.Add(url);
	}
	// ��ȡҳ��������
	else if ( amode == SUBLINK)
	{
		const std::tr1::regex pattern(strSubPage.GetBuffer());
		const std::tr1::sregex_token_iterator end;
		std::string	content = m_PageContent;
		CString strUrl;
		int n = 0;

		for (std::tr1::sregex_token_iterator i(content.begin(), content.end(), pattern);
			i != end; ++i)
		{
			url.strSUrl = strUrl = (*i).str().c_str();

			url.strUrl = strUrl;
			if ( !findString(url) )
				m_urls.Add(url);	
		}

		// ��������û����ҳ������
		if ( m_urls.IsEmpty() )
			return false;
	}
	else if ( amode == NEXTPAGE )		// ��ȡ��һҳ 
	{
		putf8 = gb_to_utf8(strEndPageId.GetBuffer(), utf8, sizeof(utf8));

		//�鿴�Ƿ������һҳ��������ӹ�����һҳ��Url
		if ( m_PageContent.find(putf8) != string::npos )
		{
			url.strUrl.Format(strNextPage, strEncodeKeyword, nCurrentPage * 10 - 10);
			m_urls.Add(url);
		}
	}

	return true;
}

char * CFetchUrlDlg::gb_to_utf8 ( char* src, char* dst, int len )
{
	int ret = 0;
	WCHAR* strA;
	int i= MultiByteToWideChar ( CP_ACP, 0 , src, -1, NULL, 0 );
	if( i<=0 ){
		return NULL;
	}
	strA = (WCHAR *)malloc( i*2 );
	MultiByteToWideChar ( CP_ACP , 0 , src, -1, strA , i);
	i= WideCharToMultiByte(CP_UTF8,0,strA,-1,NULL,0,NULL,NULL);
	if( len >= i ){
		ret = WideCharToMultiByte (CP_UTF8,0,strA,-1,dst,i,NULL,NULL);
		dst[i] = 0;
	}
	if( ret<=0 ){
		free( strA ); return NULL;
	}
	free( strA );
	return dst;
}

char *CFetchUrlDlg::urlencode(const char *s, int len, int *new_length)
{
	//printf( s);
	//printf("\n");

	const char  *from, *end;

	from = s;
	end = s + len;
	u_char *start, *to;

	start = to = (unsigned char *) malloc (3 * len + 1);

	unsigned char hexchars[] = "0123456789abcdef";

	char c;

	while (from < end)
	{
		c = *from++;

		if (c == ' ')
		{
			*to++ = '+';
		}
		else if ((c < '0' && c != '-' && c != '.')
			|| (c < 'A' && c > '9')
			|| (c > 'Z' && c < 'a' && c != '_') || (c > 'z'))
		{
			to[0] = '%';
			to[1] = hexchars[(c & 0xf0) >> 4];
			to[2] = hexchars[(c & 0x0f)];
			to += 3;
		}
		else
		{
			*to++ = c;
		}
	}
	*to = 0;
	if (new_length)
	{
		*new_length = to - start;
	}
	return (char *) start;

}

CString CFetchUrlDlg::KeywordEncode( CString strKeyword, int nMode )
{
	CString strEncodeKeyword;
	char *chtemp, ch_utf8[1024];
	int len;

	if ( nMode == KEMODE_UTF8 )
	{
		gb_to_utf8(strKeyword.GetBuffer(), ch_utf8, sizeof(ch_utf8));
		chtemp = urlencode(ch_utf8, strlen(ch_utf8), &len);
		strEncodeKeyword = chtemp;
		free(chtemp);
	}
	else
	{
		chtemp = urlencode(strKeyword.GetBuffer(), strKeyword.GetLength(), &len);
		strEncodeKeyword = chtemp;
		free(chtemp);
	}

	//
	// ����ؼ����������ģ��򽫽��תΪ��д
	// 
	if ( strEncodeKeyword.Find('%') != -1 )
		return strEncodeKeyword.MakeUpper();

	return strEncodeKeyword;
}