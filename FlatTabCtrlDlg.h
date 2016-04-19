
// FlatTabCtrlDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "BoringTabCtrl.h"
#include "afxwin.h"
#include "ttbutton.h"
#include "wbbutton.h"
#include "TTEdit.h"
#include "BoringStaticCtrl.h"
#include "BoringComboBox.h"
#include "BoringEditCtrl2.h"
#include "BoringEditCtrl.h"
#include "BoringButtonCtrl.h"
#include <msclr\event.h>

using namespace System;
using namespace System::ComponentModel;

// CFlatTabCtrlDlg dialog
class CFlatTabCtrlDlg : public CDialogEx
{
// Construction
public:
	CFlatTabCtrlDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FLATTABCTRL_DIALOG };

	BEGIN_DELEGATE_MAP(CFlatTabCtrlDlg)
		EVENT_DELEGATE_ENTRY(bgWorker_DoWork, Object^, DoWorkEventArgs^)
	END_DELEGATE_MAP()


protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support	

// Implementation
protected:
	HICON m_hIcon;

	void bgWorker_DoWork(Object^ sender, DoWorkEventArgs^ e);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CBoringTabCtrl m_FlatTabCtrl;

	CTTButton m_OkButton;
	CWBButton m_CancelButton;

	BoringEditCtrl2 m_EditCtrl;
	BoringStaticCtrl m_boringStatic;
	BoringStaticCtrl m_frameStatic;
	CEdit m_EditCtrl1;
	BoringButtonCtrl m_groupBox;
	afx_msg void OnStnClickedMyStatic();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	CBoringComboBox m_ComboBox;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnChangeEdit2();
};
