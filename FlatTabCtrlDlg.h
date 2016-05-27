
// FlatTabCtrlDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "TTTabCtrl.h"
#include "afxwin.h"
#include "ttbutton.h"
#include "wbbutton.h"
#include "TTEdit.h"
#include "TTEdit2.h"
#include "BoringStaticCtrl.h"
#include "BoringComboBox.h"
#include "BoringEditCtrl2.h"
#include "BoringEditCtrl.h"
#include "BoringButtonCtrl.h"
#include "ttbutton2.h"
#include "TTComboBox.h"
#include "TTStatic.h"
#include <msclr\event.h>
#include "TTGroupBox.h"
#include "TTStatic2.h"
#include <vector>
#include "anchor.h"
#include "TTListCtrl.h"

using namespace System;
using namespace System::ComponentModel;

// CFlatTabCtrlDlg dialog
class CFlatTabCtrlDlg : public CDialogEx
{
// Construction
public:
	CFlatTabCtrlDlg(CWnd* pParent = NULL);	// standard constructor
    virtual ~CFlatTabCtrlDlg();

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

    std::vector<CDialog*> m_vPages;

	void bgWorker_DoWork(Object^ sender, DoWorkEventArgs^ e);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

    CDlgAnchor m_dlgAnchor;
    LOGFONT m_lf;

	CTTTabCtrl m_FlatTabCtrl;

	CTTButton m_OkButton;
	CWBButton m_CancelButton;

	CTTButton m_TTButton1;
	CTTButton2 m_TTButton2;

	/*CComboBox*/ CTTComboBox m_SimpleComboBox;
    /*CComboBox*/ CTTComboBox m_DropDownComboBox;
    /*CComboBox*/ CTTComboBox m_DropDownListComboBox;

    /*CComboBox*/ CTTComboBox m_SimpleComboBox2;
    /*CComboBox*/ CTTComboBox m_DropDownComboBox2;
    /*CComboBox*/ CTTComboBox m_DropDownListComboBox2;

	CComboBox m_CommonCombo;

	//CFont m_NewFont;

	/*CStatic*/ CTTStatic m_TTStatic1;
	/*CStatic*/ CTTStatic m_TTStatic2;
	/*CStatic*/ CTTStatic m_TTStatic3;
	/*CStatic*/ CTTStatic m_TTStatic4;
	/*CStatic*/ CTTStatic m_TTStatic5;
	/*CStatic*/ CTTStatic m_TTStatic6;
	/*CStatic*/ CTTStatic m_TTStatic7;
	/*CStatic*/ CTTStatic m_TTStatic8;
	/*CStatic*/ CTTStatic m_TTStatic9;

	/*CEdit */CTTEdit m_Edit1;
	/*CEdit*/ CTTEdit m_Edit2;
	/*CEdit*/ CTTEdit m_WideEdit1;
	/*CEdit*/ CTTEdit m_WideEdit2;
	/*CEdit*/ CTTEdit m_MultiLineEdit;
	
	CStatic/*CTTGroupBox*/ m_GrBox1;
	CTTGroupBox m_GrBox2;
	CTTGroupBox m_GrBox3;

    CTTButton m_TTPushButton;

    CListCtrl /*CTTListCtrl*/ m_TTListCtrl;
    /*CListCtrl*/ CTTListCtrl m_TTListCtrl2;
    CListCtrl /*CTTListCtrl*/ m_TTListCtrl3;

	void FillCombo(CComboBox &Combo);
    void FillList(CListCtrl &list, int colCount, int rowcount);

	BoringEditCtrl2 m_EditCtrl;
	BoringStaticCtrl m_boringStatic;
	BoringStaticCtrl m_frameStatic;
	CEdit m_EditCtrl1;
	BoringButtonCtrl m_groupBox;
	afx_msg void OnStnClickedMyStatic();
	afx_msg void OnTabChanged(NMHDR *pNMHDR, LRESULT *pResult);
	CBoringComboBox m_ComboBox;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedCheck1();
    afx_msg void OnTabChanging(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSize(UINT nType, int cx, int cy);
};
