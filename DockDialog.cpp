// DockDialog.cpp : implementation file
//

#include "stdafx.h"
#include "FlatTabCtrl.h"
#include "DockDialog.h"
#include "afxdialogex.h"
#include "afxhtml.h"

// CDockDialog dialog

IMPLEMENT_DYNCREATE(CDlgMainFrame, CFrameWndEx)
//IMPLEMENT_SERIAL(CTTMultiPaneFrameWnd,CMultiPaneFrameWnd,VERSIONABLE_SCHEMA | 2)
IMPLEMENT_DYNAMIC(CDockDialog, CDialogEx)

CDockDialog::CDockDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDockDialog::IDD, pParent)
{

}

CDockDialog::~CDockDialog()
{
}

void CDockDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDockDialog, CDialogEx)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDockDialog message handlers


BOOL CDockDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//if(m_wndFrame->GetActiveView())
	//	((CHtmlView*)m_wndFrame->GetActiveView())->Navigate(_T("www.google.com"));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


int CDockDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	CDockingManager::SetDockingMode(DT_SMART);

	m_wndFrame = (CDlgMainFrame*)RUNTIME_CLASS(CDlgMainFrame)->CreateObject();
	CCreateContext cx;
	cx.m_pLastView = NULL;
	cx.m_pCurrentFrame = NULL;
	cx.m_pNewDocTemplate = NULL;
	cx.m_pCurrentDoc = NULL;
	cx.m_pNewViewClass = NULL;//RUNTIME_CLASS(CHtmlView);
	DWORD style = WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE | WS_TABSTOP;
	CRect rect(0, 0, 0, 0);
	GetClientRect(rect);
	if (!m_wndFrame->Create(NULL, NULL, style, rect, this, 0, 0, &cx))
		return -1;
	m_wndFrame->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	/*m_wndFrame->EnableDocking(CBRS_ALIGN_ANY);
	m_wndFrame->EnableAutoHidePanes(CBRS_ALIGN_ANY);*/


	m_wndFrame2 = (CDlgMainFrame*)RUNTIME_CLASS(CDlgMainFrame)->CreateObject();
	if (!m_wndFrame2->Create(NULL, NULL, style, rect, m_wndFrame, 0, 0, &cx))
		return -1;
	m_wndFrame2->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	m_wndFrame2->EnableDocking(CBRS_ALIGN_ANY);
	m_wndFrame2->EnableAutoHidePanes(CBRS_ALIGN_ANY);

	m_wndTreePane = new CDialogPane();
	m_wndTreePane->SetMainFrame(m_wndFrame2);
	style = WS_CHILD | CBRS_LEFT | CBRS_FLYBY | WS_VISIBLE;
	CString strTitle = _T("Tree Pane");
	CRect dockRect;
	GetClientRect(dockRect);
	dockRect.left += dockRect.Width() / 3;
	if (!m_wndTreePane->Create(strTitle, m_wndFrame2, dockRect, TRUE,
		10, style, AFX_CBRS_OUTLOOK_TABS))
	{
		if(m_wndTreePane);
		{
			delete m_wndTreePane;
			m_wndTreePane = NULL;
		}
		return -1;
	}
	m_wndTreePane->EnableDocking(CBRS_ALIGN_ANY);
	m_wndFrame2->DockPane((CBasePane*)m_wndTreePane, AFX_IDW_DOCKBAR_RIGHT);
	

	m_wndTreePane2 = new CDialogPane();
	m_wndTreePane2->SetMainFrame(m_wndFrame2);
	style = WS_CHILD | CBRS_LEFT | CBRS_FLYBY | WS_VISIBLE;
	strTitle = _T("2nd Tree Pane");
	GetClientRect(dockRect);
	dockRect.left += dockRect.Width() / 3;
	if (!m_wndTreePane2->Create(strTitle, m_wndFrame2, dockRect, TRUE,
		10, style, AFX_CBRS_OUTLOOK_TABS))
	{
		if (m_wndTreePane2);
		{
			delete m_wndTreePane2;
			m_wndTreePane2 = NULL;
		}
		return -1;
	}
	m_wndTreePane2->EnableDocking(CBRS_ALIGN_ANY);
	m_wndFrame2->DockPane((CBasePane*)m_wndTreePane2, AFX_IDW_DOCKBAR_TOP);



	m_wndFrame2->AdjustDockingLayout();

	return 0;
}


void CDockDialog::OnDestroy()
{
	CDialogEx::OnDestroy();

	//AFXSetTopLevelFrame(m_pOldTopLevelFrame);

	if (m_wndTreePane);
	{
		delete m_wndTreePane;
		m_wndTreePane = NULL;
	}

	if (m_wndTreePane2);
	{
		delete m_wndTreePane2;
		m_wndTreePane2 = NULL;
	}
}


void CDockDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	//CDialog::OnSize(nType, cx, cy);
	//m_wndFrame->MoveWindow(0,0, cx, cy);
}

BOOL CTTMultiPaneFrameWnd::Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, CCreateContext* pContext)
{
	return CreateEx(0, lpszWindowName, dwStyle, rect, pParentWnd, pContext);
}
BOOL CTTMultiPaneFrameWnd::CreateEx(DWORD dwStyleEx, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, CCreateContext* pContext)
{
	dwStyle |= WS_POPUP;

	if (pParentWnd != NULL && pParentWnd->GetExStyle() & WS_EX_LAYOUTRTL)
	{
		dwStyleEx |= WS_EX_LAYOUTRTL;
	}

	m_hParentWnd = pParentWnd != NULL ? pParentWnd->m_hWnd : NULL;

	if (!CWnd::CreateEx(dwStyleEx, GetGlobalData()->RegisterWindowClass(_T("Afx:MiniFrame")), lpszWindowName, dwStyle, rect, pParentWnd, 0, pContext))
	{
		return FALSE;
	}

	if (pParentWnd != NULL)
	{
		if (DYNAMIC_DOWNCAST(CFrameWnd, pParentWnd) == NULL)
		{
			TRACE0("Minframe parent must be derived from CFrameWnd. Creation failed.\n");
			return FALSE;
		}

		// register with dock manager
		CDockingManager* pDockManager = m_pDockManager != NULL ? m_pDockManager : afxGlobalUtils.GetDockingManager(pParentWnd);
		ASSERT_VALID(pDockManager);

		if (pDockManager == NULL)
		{
			TRACE0("Minframe parent must be connected to dock manager. Creation failed.\n");
			return FALSE;
		}

		pDockManager->AddMiniFrame(this);
	}

	m_dragFrameImpl.Init(this);
	return TRUE;
}


BEGIN_MESSAGE_MAP(CTTMultiPaneFrameWnd, CMultiPaneFrameWnd)
ON_WM_CREATE()
END_MESSAGE_MAP()


int CTTMultiPaneFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTTMultiPaneFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}
