#pragma once

#include "afxcmn.h"
#include "afxwin.h"
#include "resource.h"
#include "TTTabCtrl.h"
#include "ViewsHolderDlg.h"
#include "anchor.h"
#include <list>

// CmainDialog dialog

class CMainDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CMainDialog)

public:
	CMainDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMainDialog();

// Dialog Data
	enum { IDD = IDD_MAINDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	int count;

	CDlgAnchor m_dlgAnchor;

	CViewsHolderDlg* m_pViewsDlg;
	list<CStatic*> m_ViewsList;

	void AddNewView(bool isStatic = false);
	bool m_bScroll;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedAddview();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton2();
	CSliderCtrl m_SliderB;
	CSliderCtrl m_SliderG;
	CSliderCtrl m_SliderR;
	int m_Blue;
	int m_Green;
	int m_Red;
	
	int m_iEditRed;
	int m_iEditGreen;
	int m_iEditBlue;
	CEdit m_RedEdit;
	CEdit m_GreenEdit;
	CEdit m_BlueEdit;
	
	afx_msg void OnUpdateEditRed();
	afx_msg void OnUpdateEditGreen();
	afx_msg void OnUpdateEditBlue();
private:
	void UpdateColor();
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
