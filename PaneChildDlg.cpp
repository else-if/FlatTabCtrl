// PaneChildDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FlatTabCtrl.h"
#include "PaneChildDlg.h"
#include "afxdialogex.h"
#include "afxDockablepane.h"


// CPaneChildDlg dialog

IMPLEMENT_DYNAMIC(CPaneChildDlg, CDialogEx)

CPaneChildDlg::CPaneChildDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPaneChildDlg::IDD, pParent)
{

}

CPaneChildDlg::~CPaneChildDlg()
{
}

void CPaneChildDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPaneChildDlg, CDialogEx)
END_MESSAGE_MAP()


// CPaneChildDlg message handlers

/**********************************************************************/
IMPLEMENT_DYNAMIC(CDialogPane, CDockablePane)
IMPLEMENT_SERIAL(CTTPaneFrameWnd, CPaneFrameWnd, VERSIONABLE_SCHEMA | 2)
BEGIN_MESSAGE_MAP(CDialogPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

CDialogPane::CDialogPane()
{
	m_customImpl = NULL;
	m_pMiniFrameRTC = RUNTIME_CLASS(CTTPaneFrameWnd);
}
CDialogPane::~CDialogPane()
{
	*&m_dragFrameImpl = m_OrigImpl;

	if (m_customImpl)
	{
		delete m_customImpl;
		m_customImpl = NULL;
	}
}

int CDialogPane::OnCreate(LPCREATESTRUCT lp)
{
	*&m_OrigImpl = m_dragFrameImpl;
	m_customImpl = new CTTDragFrameImpl(m_pMainFrame);
	*&m_dragFrameImpl = *m_customImpl;

	if (CDockablePane::OnCreate(lp) == -1)
		return -1;
	if (!m_wndDlg.Create(CPaneChildDlg::IDD, this))
		return -1;
	m_wndDlg.ShowWindow(SW_SHOWDEFAULT);

	CWnd* pWnd = CWnd::FromHandle(lp->hwndParent);
	
	return  0;
}
void CDialogPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	m_wndDlg.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
}
