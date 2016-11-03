
// FetchUrlDlg.h : 头文件
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

// CFetchUrlDlg 对话框
class CFetchUrlDlg : public CDialog
{
// 构造
public:
	CFetchUrlDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FETCHURL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

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

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedFetch();
};
