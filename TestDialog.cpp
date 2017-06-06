// TestDialog.cpp : implementation file
//

#include "stdafx.h"
#include "FlatTabCtrl.h"
#include "TestDialog.h"
#include "afxdialogex.h"


// CTestDialog dialog

IMPLEMENT_DYNAMIC(CTestDialog, CDialogEx)

CTestDialog::CTestDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestDialog::IDD, pParent)
{

}

CTestDialog::~CTestDialog()
{
}

void CTestDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(CTestDialog, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CTestDialog::OnNMDblclkList1)
END_MESSAGE_MAP()


// CTestDialog message handlers


void CTestDialog::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	

	*pResult = 0;
}

void CTestDialog::AddTip(int id, CString& str)
{
	m_list.InsertItem(id, str);
	UpdateData();
}

void CTestDialog::ClearTips()
{
	m_list.DeleteAllItems();
	UpdateData();
}
