// TabSheet2.cpp : implementation file
//

#include "stdafx.h"
#include "FlatTabCtrl.h"
#include "TabSheet2.h"
#include "afxdialogex.h"


// CTabSheet2 dialog

IMPLEMENT_DYNAMIC(CTabSheet2, CDialog)

CTabSheet2::CTabSheet2(CWnd* pParent /*=NULL*/)
	: CDialog(CTabSheet2::IDD, pParent)
{

}

CTabSheet2::~CTabSheet2()
{
}

void CTabSheet2::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDOK, m_OkBtn);
    DDX_Control(pDX, IDCANCEL, m_CancelBtn);
}


BEGIN_MESSAGE_MAP(CTabSheet2, CDialog)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CTabSheet2 message handlers


BOOL CTabSheet2::OnEraseBkgnd(CDC* pDC)
{
	CRect clientRect;
	GetClientRect(clientRect);
	pDC->FillSolidRect(clientRect, RGB(255, 255, 255));

	return TRUE;
}
