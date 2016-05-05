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
    DDX_Control(pDX, IDCANCEL, m_CancelBtn);
    DDX_Control(pDX, IDOK, m_OkBtn);
    DDX_Control(pDX, IDCANCEL2, m_btn1);
    DDX_Control(pDX, IDCANCEL3, m_btn2);
    DDX_Control(pDX, IDCANCEL4, m_btn3);
    DDX_Control(pDX, IDCANCEL5, m_btn4);
    DDX_Control(pDX, IDCANCEL6, m_btn5);
    DDX_Control(pDX, IDCANCEL7, m_btn6);
    DDX_Control(pDX, IDCANCEL9, m_btn7);
    DDX_Control(pDX, IDCANCEL8, m_btn8);
}


BEGIN_MESSAGE_MAP(CTabSheet1, CDialog)
END_MESSAGE_MAP()


// CTabSheet1 message handlers
