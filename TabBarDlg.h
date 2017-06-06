#pragma once

#include "TTBarTabCtrl.h"
#include "TabSheet1.h"
#include "TabSheet2.h"
#include "TabSheet3.h"

// CTabBarDlg dialog

class CTabBarDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTabBarDlg)

public:
	CTabBarDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTabBarDlg();

	CTTBarTabCtrl m_BarTabCtrl;
	CTabSheet1 m_TS1;
	CTabSheet2 m_TS2;
	CTabSheet3 m_TS3;

	CTTBarTabCtrl m_BarTabCtrl_def;
	CTabSheet1 m_TS1_def;
	CTabSheet2 m_TS2_def;
	CTabSheet3 m_TS3_def;

// Dialog Data
	enum { IDD = IDD_TABBARDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
