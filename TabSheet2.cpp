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
}


BEGIN_MESSAGE_MAP(CTabSheet2, CDialog)
END_MESSAGE_MAP()


// CTabSheet2 message handlers
