// CmainDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "afxdialogex.h"
#include "MainDialog.h"
#include "SimpleView.h"

// CmainDialog dialog

IMPLEMENT_DYNAMIC(CMainDialog, CDialogEx)

CMainDialog::CMainDialog(CWnd* pParent /*=NULL*/)
: CDialogEx(CMainDialog::IDD, pParent)
{
	count = 0;
}

CMainDialog::~CMainDialog()
{
	if (m_pViewsDlg)
	{
		delete m_pViewsDlg;
		m_pViewsDlg = NULL;
	}

	for each (CStatic* pWnd in m_ViewsList)
	{
		if (pWnd)
		{
			delete pWnd;
			pWnd = NULL;
		}
	}

	m_ViewsList.clear();
}

void CMainDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_B, m_SliderB);
	DDX_Control(pDX, IDC_SLIDER_G, m_SliderG);
	DDX_Control(pDX, IDC_SLIDER_R, m_SliderR);
	m_SliderB.SetRange(0, 255);
	m_SliderG.SetRange(0, 255);
	m_SliderR.SetRange(0, 255);
	DDX_Slider(pDX, IDC_SLIDER_B, m_Blue);
	DDX_Slider(pDX, IDC_SLIDER_G, m_Green);
	DDX_Slider(pDX, IDC_SLIDER_R, m_Red);
	DDX_Text(pDX, IDC_EDIT_RED, m_iEditRed);
	DDX_Text(pDX, IDC_EDIT_GREEN, m_iEditGreen);
	DDX_Text(pDX, IDC_EDIT_BLUE, m_iEditBlue);
	DDX_Control(pDX, IDC_EDIT_RED, m_RedEdit);
	DDX_Control(pDX, IDC_EDIT_GREEN, m_GreenEdit);
	DDX_Control(pDX, IDC_EDIT_BLUE, m_BlueEdit);
}

void CMainDialog::AddNewView(bool isStatic)
{
	CRect emptyRect;
	emptyRect.SetRectEmpty();

	CString caption;
	caption.Format(_T("View #%d caption!"), m_ViewsList.size() + 1);

	CSimpleView* sv = new CSimpleView(m_bScroll);
	sv->SetName(caption);
	sv->Create(caption, WS_CHILD /*| WS_BORDER*/ | SS_NOTIFY, emptyRect, m_pViewsDlg);
	sv->SetStatic(isStatic);

	m_bScroll = !m_bScroll;

	m_ViewsList.push_back(sv);

	if (isStatic)
		m_pViewsDlg->AddStaticView(sv, (count++ % 2 == 0 ? StaticViewAlign::Top : StaticViewAlign::Bottom));
	else
		m_pViewsDlg->AddView(sv/*, false, false, 1, false*/);
}

BEGIN_MESSAGE_MAP(CMainDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CMainDialog::OnBnClickedButton1)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_ADDVIEW, &CMainDialog::OnBnClickedAddview)
	ON_BN_CLICKED(IDC_BUTTON2, &CMainDialog::OnBnClickedButton2)
	ON_EN_UPDATE(IDC_EDIT_RED, &CMainDialog::OnUpdateEditRed)
	ON_EN_UPDATE(IDC_EDIT_GREEN, &CMainDialog::OnUpdateEditGreen)
	ON_EN_UPDATE(IDC_EDIT_BLUE, &CMainDialog::OnUpdateEditBlue)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CmainDialog message handlers

void CMainDialog::OnBnClickedButton1()
{
	if (!m_pViewsDlg->IsWindowVisible())
	{
		CRect cliRect;
		GetClientRect(&cliRect);

		cliRect.left += 150;

		m_pViewsDlg->MoveWindow(&cliRect);

		m_pViewsDlg->ShowWindow(SW_SHOW);
		
		m_dlgAnchor.Add(m_pViewsDlg->GetSafeHwnd(), ANCHOR_ALL);

		SCROLLINFO info;
		info.cbSize = sizeof(SCROLLINFO);
		info.fMask = SIF_ALL;
		info.nMin = 0;
		info.nMax = 20;
		info.nPage = 0;
		info.nPos = 0;
		info.nTrackPos = 0;
		m_pViewsDlg->SetScrollInfo(SB_HORZ, &info);

		GetDlgItem(IDC_BUTTON1)->SetWindowText(_T("Hide views"));
	}
	else
	{
		m_pViewsDlg->ShowWindow(SW_HIDE);
		m_dlgAnchor.Remove(m_pViewsDlg->GetSafeHwnd());
		GetDlgItem(IDC_BUTTON1)->SetWindowText(_T("Show views"));
	}
}

void CMainDialog::OnBnClickedAddview()
{
	AddNewView();	
}

int CMainDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_pViewsDlg)
	{
		m_pViewsDlg = new CViewsHolderDlg(this);
		m_pViewsDlg->Create(CViewsHolderDlg::IDD, this);
	}

	/*for (int id = 1; id < 5; id++)
		AddNewView();*/

	AddNewView();
	AddNewView();

	m_dlgAnchor.Init(GetSafeHwnd());

	

	return 0;
}

void CMainDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	m_dlgAnchor.OnSize(FALSE);
}

BOOL CMainDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	OnBnClickedButton1();

	m_Red = 255;
	m_Green = 205;
	m_Blue = 82;

	m_iEditRed = m_Red;
	m_iEditGreen = m_Green;
	m_iEditBlue = m_Blue;

	UpdateData(FALSE);

	return TRUE; 
}

void CMainDialog::OnBnClickedButton2()
{
	AddNewView(true);
}


void CMainDialog::OnUpdateEditRed()
{
	UpdateData();
	if (m_iEditRed < 0)
		m_iEditRed = 0;
	else if (m_iEditRed > 255)
		m_iEditRed = 255;
	m_Red = m_iEditRed;
	UpdateData(FALSE);

	UpdateColor();
}


void CMainDialog::OnUpdateEditGreen()
{
	// TODO:  Add your control notification handler code here
	UpdateData();
	if (m_iEditGreen < 0)
		m_iEditGreen = 0;
	else if (m_iEditGreen > 255)
		m_iEditGreen = 255;
	m_Green = m_iEditGreen;
	UpdateData(FALSE);

	UpdateColor();
}


void CMainDialog::OnUpdateEditBlue()
{
	// TODO:  Add your control notification handler code here
	UpdateData();
	if (m_iEditBlue < 0)
		m_iEditBlue = 0;
	else if (m_iEditBlue > 255)
		m_iEditBlue = 255;
	m_Blue = m_iEditBlue;
	UpdateData(FALSE);

	UpdateColor();
}

void CMainDialog::UpdateColor()
{
	m_pViewsDlg->UpdateColor(RGB(m_Red, m_Green, m_Blue));
}


void CMainDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateData();
	m_iEditRed = m_Red;
	m_iEditGreen = m_Green;
	m_iEditBlue = m_Blue;
	UpdateData(FALSE);

	UpdateColor();

	__super::OnHScroll(nSBCode, nPos, pScrollBar);
}
