// TabSheet3.cpp : implementation file
//

#include "stdafx.h"
#include "FlatTabCtrl.h"
#include "TabSheet3.h"
#include "afxdialogex.h"


// CTabSheet3 dialog

IMPLEMENT_DYNAMIC(CTabSheet3, CDialog)

CTabSheet3::CTabSheet3(CWnd* pParent /*=NULL*/)
	: CDialog(CTabSheet3::IDD, pParent)
{

}

CTabSheet3::~CTabSheet3()
{    
}

void CTabSheet3::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST4, listCtrl);
    DDX_Control(pDX, IDC_LIST5, listctrl2);
}


BEGIN_MESSAGE_MAP(CTabSheet3, CDialog)
END_MESSAGE_MAP()


// CTabSheet3 message handlers


BOOL CTabSheet3::OnInitDialog()
{
    CDialog::OnInitDialog();

    int colCount = 3;
    int rowCount = 3;

    for (int i = 0; i < colCount; i++)
    {
        listCtrl.InsertColumn(i, CString("Col" + i));
        listCtrl.SetColumnWidth(i, 50);

        listctrl2.InsertColumn(i, CString("Col" + i));
        listctrl2.SetColumnWidth(i, 50);
    }

    listCtrl.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0,
        LVS_EX_SIMPLESELECT);

    for (int i = 0; i < rowCount; i++)
        for (int j = 0; j < colCount; j++)
        {
            CString text;
            text.Format(_T("item %d"), (i*colCount + j));

            LVITEM lv;
            lv.iItem = i;
            lv.iSubItem = j;
            lv.pszText = text.GetBuffer(text.GetLength());
            lv.mask = LVIF_TEXT;
            if (j == 0)
                listCtrl.InsertItem(&lv);
            else
                listCtrl.SetItem(&lv);

            if (i==0)
            {
                if (j == 0)
                    listctrl2.InsertItem(&lv);
                else
                    listctrl2.SetItem(&lv);
            }
        }

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
