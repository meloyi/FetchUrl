
// FetchUrlDlg.h : ͷ�ļ�
//

#pragma once

#include "stdafx.h"
#include "afxcmn.h"

#define KEMODE_UTF8	0x01

typedef struct _Urls
{
	CString strSUrl;
	CString strUrl;

	bool operator==(const struct _Urls &left) const 
	{
		return strUrl == left.strUrl;
	}
}Urls;

// CFetchUrlDlg �Ի���
class CFetchUrlDlg : public CDialog
{
// ����
public:
	CFetchUrlDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FETCHURL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	CListCtrl m_urlList;
	CArray<Urls> m_urls;
	string m_PageContent;
	int m_Page;

	static char *gb_to_utf8 ( char* src, char* dst, int len );
	static char *urlencode(const char *s, int len, int *new_length);
	CString KeywordEncode( CString strKeyword, int nMode );

	bool analyze_sbaidu( analyzeMode amode, 
		CString strEncodeKeyword /*= _T("")*/, int nCurrentPage=0 );

	bool findString(const Urls &left )
	{
		INT_PTR	nIndex;
		for( nIndex = 0; nIndex < m_urls.GetSize(); nIndex++)
		{
			if(m_urls.GetAt(nIndex) == left)
			{
				return true;
			}
		}
		return false;
	}

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedFetch();
};
