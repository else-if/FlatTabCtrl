
// FlatTabCtrlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FlatTabCtrl.h"
#include "FlatTabCtrlDlg.h"
#include "afxdialogex.h"
#include "BGWorkerDlg.h"

#include <Gdiplus.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFlatTabCtrlDlg dialog



CFlatTabCtrlDlg::CFlatTabCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFlatTabCtrlDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFlatTabCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_FlatTabCtrl);
	DDX_Control(pDX, IDOK, m_OkButton);
	DDX_Control(pDX, IDCANCEL, m_CancelButton);
	DDX_Control(pDX, IDC_EDIT1, m_EditCtrl);
	DDX_Control(pDX, IDC_MY_STATIC, m_boringStatic);
	DDX_Control(pDX, IDC_COMBO1, m_ComboBox);
	DDX_Control(pDX, IDC_MYGROUPBOX, m_groupBox);
	DDX_Control(pDX, IDC_FRAME_STATIC, m_frameStatic);
	DDX_Control(pDX, IDC_EDIT2, m_EditCtrl1);	
	DDX_Control(pDX, IDC_BUTTON2, m_TTButton1);
	DDX_Control(pDX, IDC_BUTTON3, m_TTButton2);
	DDX_Control(pDX, IDC_COMBO2, m_CommonCombo);
	DDX_Control(pDX, IDC_COMBO3, m_SimpleComboBox);
	DDX_Control(pDX, IDC_COMBO4, m_DropDownComboBox);
	DDX_Control(pDX, IDC_COMBO5, m_DropDownListComboBox);
	DDX_Control(pDX, IDC_COMBO6, m_SimpleComboBox2);
	DDX_Control(pDX, IDC_COMBO7, m_DropDownComboBox2);
	DDX_Control(pDX, IDC_COMBO8, m_DropDownListComboBox2);
	DDX_Control(pDX, IDC_TT_STATIC1, m_TTStatic1);
	DDX_Control(pDX, IDC_TT_STATIC2, m_TTStatic2);
	DDX_Control(pDX, IDC_TT_STATIC3, m_TTStatic3);
	DDX_Control(pDX, IDC_TT_STATIC4, m_TTStatic4);
	DDX_Control(pDX, IDC_TT_STATIC5, m_TTStatic5);
	DDX_Control(pDX, IDC_TT_STATIC6, m_TTStatic6);
	DDX_Control(pDX, IDC_TT_STATIC7, m_TTStatic7);
	DDX_Control(pDX, IDC_TT_STATIC8, m_TTStatic8);
	DDX_Control(pDX, IDC_EDIT4, m_Edit1);
	DDX_Control(pDX, IDC_EDIT5, m_Edit2);
	DDX_Control(pDX, IDC_EDIT6, m_WideEdit1);
	DDX_Control(pDX, IDC_EDIT7, m_WideEdit2);	
	DDX_Control(pDX, IDC_TTGROUPBOX1, m_GrBox1);
	DDX_Control(pDX, IDC_TTGROUPBOX2, m_GrBox2);
}

BEGIN_MESSAGE_MAP(CFlatTabCtrlDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_STN_CLICKED(IDC_MY_STATIC, &CFlatTabCtrlDlg::OnStnClickedMyStatic)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CFlatTabCtrlDlg::OnTcnSelchangeTab1)
	ON_BN_CLICKED(IDC_BUTTON1, &CFlatTabCtrlDlg::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT2, &CFlatTabCtrlDlg::OnEnChangeEdit2)
	ON_BN_CLICKED(IDOK, &CFlatTabCtrlDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON2, &CFlatTabCtrlDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CFlatTabCtrlDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CFlatTabCtrlDlg message handlers

void CFlatTabCtrlDlg::bgWorker_DoWork(Object^ sender, DoWorkEventArgs^ e)
{
	BackgroundWorker^ worker = (BackgroundWorker^)sender;

	for (int i = 1; i <= 10; i++)
	{
		if (worker->CancellationPending == true)
		{
			e->Cancel = true;
			break;
		}
		else
		{
			// Perform a time consuming operation and report progress.
			System::Threading::Thread::Sleep(500);
			worker->ReportProgress(i * 10, System::String::Format("Räknat {0} av {1}", i, 100));
		}
	}
}

BOOL CFlatTabCtrlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//SetBackgroundColor(RGB(162, 162, 162));

	//m_FlatTabCtrl.BackgroundColor(RGB(162, 162, 162));
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here	

	m_FlatTabCtrl.InsertItem(0, _T("Test"));
	m_FlatTabCtrl.InsertItem(1, _T("Test1"));
	m_FlatTabCtrl.InsertItem(2, _T("Test2"));
	m_FlatTabCtrl.InsertItem(4, _T("Here is a very very long tab item header"));
	m_FlatTabCtrl.InsertItem(4, _T("Second very very long tab item header"));

	FillCombo(m_CommonCombo);

	FillCombo(m_SimpleComboBox);
	FillCombo(m_DropDownComboBox);
	FillCombo(m_DropDownListComboBox);	

	m_SimpleComboBox2.SetDrawingProperties(2, 10);
	m_DropDownComboBox2.SetDrawingProperties(2, 10);
	m_DropDownListComboBox2.SetDrawingProperties(2, 10);

	FillCombo(m_SimpleComboBox2);
	FillCombo(m_DropDownComboBox2);
	FillCombo(m_DropDownListComboBox2);
	
	
	m_TTStatic1.DrawBorders(true);
	m_TTStatic1.SetBackgroundColor(RGB(255, 255, 255));

	m_TTStatic2.SetDrawingProperties(2, 10, true);
	m_TTStatic2.SetBackgroundColor(RGB(255, 255, 255));
	
	m_TTStatic3.SetDrawingProperties(2, 10, true);
	m_TTStatic4.DrawBorders(true);

	m_TTStatic5.SetDrawingProperties(2, 10, false);

	m_TTStatic7.DrawBorders(true);
	m_TTStatic7.SetColorProperies(RGB(221, 232, 101), RGB(23, 55, 125), RGB(202, 81, 0));

	m_TTStatic8.SetDrawingProperties(2, 10, true);
	m_TTStatic8.SetColorProperies(RGB(221, 232, 101), RGB(23, 55, 125), RGB(202, 81, 0));

	//m_Edit1.SetDrawingProperties(1, 5);
	m_Edit1.SetWindowTextW(_T("Sample edit box"));
	m_Edit2.SetWindowTextW(_T("Sample edit box"));

	//m_WideEdit1.SetDrawingProperties(1, 5);
	m_WideEdit1.SetWindowTextW(_T("Wide edit box"));
	m_WideEdit2.SetWindowTextW(_T("Wide edit box"));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFlatTabCtrlDlg::FillCombo(CComboBox &Combo)
{
	for (int i = 0; i < 10; i++)
	{
		CString string;
		string.Format(_T("Item %d"), i);

		Combo.AddString(string);
	}
}

void CFlatTabCtrlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFlatTabCtrlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); // device context for painting

		CDialogEx::OnPaint();
	}

	


	m_CancelButton.LoadBitmaps(IDB_BITMAP1, 4, 10, 10, 20, 20);

}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFlatTabCtrlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFlatTabCtrlDlg::OnStnClickedMyStatic()
{
	// TODO: Add your control notification handler code here
}


void CFlatTabCtrlDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CFlatTabCtrlDlg::OnBnClickedButton1()
{
	CBGWorkerDlg dlg;
	dlg.m_bgWorker->DoWork += MAKE_DELEGATE(DoWorkEventHandler, bgWorker_DoWork);
	int dlgResult = dlg.DoModal();
}


void CFlatTabCtrlDlg::OnEnChangeEdit2()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CFlatTabCtrlDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//CDialogEx::OnOK();
}


void CFlatTabCtrlDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	CWnd* pBtnWnd = GetDlgItem(IDC_BUTTON3);

	if (pBtnWnd->IsWindowEnabled())
	{
		pBtnWnd->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON2)->SetWindowText(_T(" Enable ---> "));

		GetDlgItem(IDC_COMBO3)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO4)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO5)->EnableWindow(FALSE);

		GetDlgItem(IDC_TT_STATIC2)->EnableWindow(FALSE);
		GetDlgItem(IDC_TT_STATIC3)->EnableWindow(FALSE);
		GetDlgItem(IDC_TT_STATIC5)->EnableWindow(FALSE);
		GetDlgItem(IDC_TT_STATIC8)->EnableWindow(FALSE);

		GetDlgItem(IDC_EDIT5)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT7)->EnableWindow(FALSE);		
	}
	else
	{
		pBtnWnd->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON2)->SetWindowText(_T(" Disable ---> "));

		GetDlgItem(IDC_COMBO3)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO4)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO5)->EnableWindow(TRUE);

		GetDlgItem(IDC_TT_STATIC2)->EnableWindow(TRUE);
		GetDlgItem(IDC_TT_STATIC3)->EnableWindow(TRUE);
		GetDlgItem(IDC_TT_STATIC5)->EnableWindow(TRUE);
		GetDlgItem(IDC_TT_STATIC8)->EnableWindow(TRUE);

		GetDlgItem(IDC_EDIT5)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT7)->EnableWindow(TRUE);		
	}
}


void CFlatTabCtrlDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	CWnd* pBtnWnd = GetDlgItem(IDC_BUTTON2);

	if (pBtnWnd->IsWindowEnabled())
	{
		pBtnWnd->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON3)->SetWindowText(_T(" <--- Enable "));

		GetDlgItem(IDC_COMBO6)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO7)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO8)->EnableWindow(FALSE);

		GetDlgItem(IDC_TT_STATIC1)->EnableWindow(FALSE);
		GetDlgItem(IDC_TT_STATIC4)->EnableWindow(FALSE);
		GetDlgItem(IDC_TT_STATIC6)->EnableWindow(FALSE);
		GetDlgItem(IDC_TT_STATIC7)->EnableWindow(FALSE);

		GetDlgItem(IDC_EDIT4)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT6)->EnableWindow(FALSE);		
	}
	else
	{
		pBtnWnd->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON3)->SetWindowText(_T(" <--- Disable "));

		GetDlgItem(IDC_COMBO6)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO7)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO8)->EnableWindow(TRUE);

		GetDlgItem(IDC_TT_STATIC1)->EnableWindow(TRUE);
		GetDlgItem(IDC_TT_STATIC4)->EnableWindow(TRUE);
		GetDlgItem(IDC_TT_STATIC6)->EnableWindow(TRUE);
		GetDlgItem(IDC_TT_STATIC7)->EnableWindow(TRUE);

		GetDlgItem(IDC_EDIT4)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT6)->EnableWindow(TRUE);		
	}
}
