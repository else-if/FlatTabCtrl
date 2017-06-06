// TabBarDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FlatTabCtrl.h"
#include "TabBarDlg.h"
#include "afxdialogex.h"


// CTabBarDlg dialog

IMPLEMENT_DYNAMIC(CTabBarDlg, CDialogEx)

CTabBarDlg::CTabBarDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTabBarDlg::IDD, pParent)
{
}

CTabBarDlg::~CTabBarDlg()
{
}

void CTabBarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, 1000, m_BarTabCtrl);
}


BEGIN_MESSAGE_MAP(CTabBarDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CTabBarDlg message handlers


BOOL CTabBarDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	/*CRect clientRect;
	GetClientRect(clientRect);
	m_BarTabCtrl.Create(clientRect, this, 1000);*/

	if (!CMFCOutlookBarTabCtrl::IsAnimation())
		CMFCOutlookBarTabCtrl::EnableAnimation();

	m_TS1.Create(CTabSheet1::IDD, &m_BarTabCtrl);
	m_TS2.Create(CTabSheet2::IDD, &m_BarTabCtrl);
	m_TS3.Create(CTabSheet3::IDD, &m_BarTabCtrl);

	m_BarTabCtrl.AddTab(&m_TS1, _T("Tab# 1"));
	m_BarTabCtrl.AddTab(&m_TS2, _T("Tab# 2"));
	m_BarTabCtrl.AddTab(&m_TS3, _T("Tab# 3"));


	m_TS1_def.Create(CTabSheet1::IDD, &m_BarTabCtrl_def);
	m_TS2_def.Create(CTabSheet2::IDD, &m_BarTabCtrl_def);
	m_TS3_def.Create(CTabSheet3::IDD, &m_BarTabCtrl_def);

	m_BarTabCtrl_def.AddTab(&m_TS1_def, _T("Tab# 1"));
	m_BarTabCtrl_def.AddTab(&m_TS2_def, _T("Tab# 2"));
	m_BarTabCtrl_def.AddTab(&m_TS3_def, _T("Tab# 3"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


int CTabBarDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle(WS_CLIPCHILDREN, 0, SWP_NOREDRAW);

	CRect clientRect;
	GetClientRect(clientRect);

	CPoint centerPoint = clientRect.CenterPoint();

	CRect rect1(clientRect);
	rect1.right = centerPoint.x;
	rect1.DeflateRect(5, 5);
	m_BarTabCtrl.Create(rect1, this, 1000);

	CRect rect2(clientRect);
	rect2.left = centerPoint.x;
	rect2.DeflateRect(5, 5);
	m_BarTabCtrl_def.Create(rect2, this, 1001);
	m_BarTabCtrl.SetUseDefaultColors(true);

	return 0;
}


BOOL CTabBarDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect clientRect;
	GetClientRect(clientRect);
	pDC->FillSolidRect(clientRect, RGB(255, 255, 255));

	return TRUE/*CDialogEx::OnEraseBkgnd(pDC)*/;
}
