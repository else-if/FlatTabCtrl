#pragma once
#include "afxframewndex.h"
#include "afxmultipaneframewnd.h"
#include "PaneChildDlg.h"

class CDlgMainFrame : public CFrameWndEx
{
	DECLARE_DYNCREATE(CDlgMainFrame)
public:
	BOOL OnCreateClient(LPCREATESTRUCT, CCreateContext* pContext)
	{
		//m_Impl.m_pDummy
		return true;
	}
};

class CTTMultiPaneFrameWnd : public CMultiPaneFrameWnd
{
	//DECLARE_SERIAL(CTTMultiPaneFrameWnd)
public:
	CTTMultiPaneFrameWnd() { d = 123; };
	virtual ~CTTMultiPaneFrameWnd();

public:
	/*virtual */BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, CCreateContext* pContext = NULL) /*override*/;
	virtual BOOL CreateEx(DWORD dwStyleEx, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, CCreateContext* pContext = NULL) override;

	int d;
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


// CDockDialog dialog

class CDockDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CDockDialog)
	DECLARE_MESSAGE_MAP()
public:
	CDockDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDockDialog();

// Dialog Data
	enum { IDD = IDD_DOCKDIALOG };

private:
	CDlgMainFrame* m_wndFrame;
	CDlgMainFrame* m_wndFrame2;
	CDialogPane* m_wndTreePane;
	CDialogPane* m_wndTreePane2;

	CFrameWnd* m_pOldTopLevelFrame;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
