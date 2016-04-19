// TabSheet1.cpp : implementation file
//

#include "stdafx.h"
#include "FlatTabCtrl.h"
#include "TabSheet1.h"
#include "afxdialogex.h"


// CTabSheet1 dialog

IMPLEMENT_DYNAMIC(CTabSheet1, CDialog)

CTabSheet1::CTabSheet1(CWnd* pParent /*=NULL*/)
	: CDialog(CTabSheet1::IDD, pParent)
{

}

CTabSheet1::~CTabSheet1()
{
}

void CTabSheet1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabSheet1, CDialog)
END_MESSAGE_MAP()


// CTabSheet1 message handlers
