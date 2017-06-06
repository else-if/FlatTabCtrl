#pragma once
#include "afxcmn.h"


// CTestDialog dialog

class CTestDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CTestDialog)

public:
	CTestDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTestDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_list;

	void AddTip(int id, CString& str);
	void ClearTips();
};
