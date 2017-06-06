
// FlatTabCtrlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FlatTabCtrl.h"
#include "FlatTabCtrlDlg.h"
#include "afxdialogex.h"
#include "BGWorkerDlg.h"

#include <Gdiplus.h>
#include "TabSheet1.h"
#include "TabSheet2.h"
#include "TabSheet3.h"
#include "ManagedImageProcessor.h"
#include "NativeImageProcessor.h"
#include <algorithm>
#include <shlwapi.h> 
#include "ColorsCommon.h"

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
	/*m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	img.Load(_T("H:\\1.png"));
	hBmp = img.Detach();*/

	m_pFont = ::new Gdiplus::Font((const WCHAR *)_T("Segoe UI"), 15 * 0.75, Gdiplus::FontStyle::FontStyleRegular, Gdiplus::Unit::UnitPixel);

	m_NormalBkgndBrush.CreateSolidBrush(RGB(255, 255, 255));
	m_DisabledBkgndBrush.CreateSolidBrush(RGB(240, 240, 240));
	pTestDlg = NULL;
}

CFlatTabCtrlDlg::~CFlatTabCtrlDlg()
{
    m_vPages[0]->DestroyWindow();
    m_vPages[1]->DestroyWindow();
    m_vPages[2]->DestroyWindow();

    delete m_vPages[0];
    delete m_vPages[1];
    delete m_vPages[2];

	::delete m_pFont;

    m_vPages.clear();

	if (pTestDlg != NULL)
	{
		delete pTestDlg;
		pTestDlg = NULL;
	}

	if (m_pac != NULL)
	{
		//m_pac->Unbind();
		/*delete m_pac;
		m_pac = NULL;*/
		m_pac->Release();
	}

	/*if (pViewHolderDlg)
		delete pViewHolderDlg;*/
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
    DDX_Control(pDX, IDC_TT_STATIC9, m_TTStatic9);
	DDX_Control(pDX, IDC_EDIT4, m_Edit1);
	DDX_Control(pDX, IDC_EDIT5, m_Edit2);
	DDX_Control(pDX, IDC_EDIT6, m_WideEdit1);
	DDX_Control(pDX, IDC_EDIT7, m_WideEdit2);	
	DDX_Control(pDX, IDC_TTGROUPBOX1, m_GrBox1);
	DDX_Control(pDX, IDC_TTGROUPBOX2, m_GrBox2);
	DDX_Control(pDX, IDC_TTGROUPBOX3, m_GrBox3);
	DDX_Control(pDX, IDC_EDIT8, m_MultiLineEdit);
    DDX_Control(pDX, IDC_CHECK1, m_TTPushButton);
    DDX_Control(pDX, IDC_LIST1, m_TTListCtrl);
    DDX_Control(pDX, IDC_LIST2, m_TTListCtrl2);
    DDX_Control(pDX, IDC_LIST3, m_TTListCtrl3);
    DDX_Control(pDX, IDC_RICHEDIT21, m_TTRichEdit1);
	DDX_Control(pDX, IDC_COMBO10, m_SimpleComboBox3);
	DDX_Control(pDX, IDC_COMBO11, m_DropDownComboBox3);
	DDX_Control(pDX, IDC_COMBO12, m_DropDownListComboBox3);
}

BEGIN_MESSAGE_MAP(CFlatTabCtrlDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_STN_CLICKED(IDC_MY_STATIC, &CFlatTabCtrlDlg::OnStnClickedMyStatic)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CFlatTabCtrlDlg::OnTabChanged)
	ON_BN_CLICKED(IDC_BUTTON1, &CFlatTabCtrlDlg::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT2, &CFlatTabCtrlDlg::OnEnChangeEdit2)
	ON_BN_CLICKED(IDOK, &CFlatTabCtrlDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON2, &CFlatTabCtrlDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CFlatTabCtrlDlg::OnBnClickedButton3)
    ON_BN_CLICKED(IDC_CHECK1, &CFlatTabCtrlDlg::OnBnClickedCheck1)
    ON_NOTIFY(TCN_SELCHANGING, IDC_TAB1, &CFlatTabCtrlDlg::OnTabChanging)
    ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_EN_SETFOCUS(IDC_EDIT4, &CFlatTabCtrlDlg::OnSetfocusEdit4)
	ON_EN_KILLFOCUS(IDC_EDIT4, &CFlatTabCtrlDlg::OnKillfocusEdit4)
	ON_WM_MOVE()
	ON_EN_CHANGE(IDC_EDIT4, &CFlatTabCtrlDlg::OnEnChangeEdit4)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
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

	//CoInitialize(NULL);
	//SHAutoComplete(GetDlgItem(IDC_EDIT4)->GetSafeHwnd(), SHACF_DEFAULT);
	
	CSimpleArray<CString> itemsList;
	for (int i = 0; i < 10; i++)
	{
		CString cStr;
		cStr.Format(_T("Directory path #%d"), i);

		itemsList.Add(cStr);
	}

	m_pac = new CTTAutoComplete(itemsList);
	m_pac->Bind(GetDlgItem(IDC_EDIT4)->GetSafeHwnd(),
		ACO_AUTOSUGGEST | ACO_UPDOWNKEYDROPSLIST | ACO_NOPREFIXFILTERING);

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

    m_vPages.push_back(new CTabSheet1(this));
    m_vPages.push_back(new CTabSheet2(this));
    m_vPages.push_back(new CTabSheet3(this));
    
    m_vPages[0]->Create(CTabSheet1::IDD, &m_FlatTabCtrl);
    m_vPages[1]->Create(CTabSheet2::IDD, &m_FlatTabCtrl);
    m_vPages[2]->Create(CTabSheet3::IDD, &m_FlatTabCtrl);

    ::EnableThemeDialogTexture(m_vPages[0]->GetSafeHwnd(), ETDT_ENABLETAB);
    ::EnableThemeDialogTexture(m_vPages[1]->GetSafeHwnd(), ETDT_ENABLETAB);
    ::EnableThemeDialogTexture(m_vPages[2]->GetSafeHwnd(), ETDT_ENABLETAB);

	RECT r;
    TCITEM tci;

    m_FlatTabCtrl.GetClientRect(&r);

    tci.mask = TCIF_TEXT;
    tci.iImage = -1;
    tci.pszText = L"Page One";
    m_FlatTabCtrl.InsertItem(0, &tci);
    tci.pszText = L"Page Two";
    m_FlatTabCtrl.InsertItem(1, &tci);    
    tci.pszText = L"Page Three";
    m_FlatTabCtrl.InsertItem(2, &tci);

    OnTabChanged(NULL, NULL);


	FillCombo(m_CommonCombo);

	FillCombo(m_SimpleComboBox);
	FillCombo(m_DropDownComboBox);
	FillCombo(m_DropDownListComboBox);	

	//m_SimpleComboBox.SetDrawingProperties(2, 10);
	//m_DropDownComboBox.SetDrawingProperties(2, 10);
	//m_DropDownListComboBox.SetDrawingProperties(2, 10);

	FillCombo(m_SimpleComboBox2);
	FillCombo(m_DropDownComboBox2);
	FillCombo(m_DropDownListComboBox2);
	
	m_SimpleComboBox3.SetMultiSelectionMode();
	m_DropDownComboBox3.SetMultiSelectionMode();
	m_DropDownListComboBox3.SetMultiSelectionMode();

	FillCombo(m_SimpleComboBox3);
	FillCombo(m_DropDownComboBox3);
	FillCombo(m_DropDownListComboBox3);
	
	m_TTStatic1.DrawBorders(true);
    m_TTStatic1.DrawBackground(true);
	m_TTStatic1.SetBackgroundColor(RGB(255, 255, 255));

	m_TTStatic2.SetDrawingProperties(2, 10, true, true);
	m_TTStatic2.SetBackgroundColor(RGB(255, 255, 255));
	
	m_TTStatic3.SetDrawingProperties(2, 10, true);
	m_TTStatic4.DrawBorders(true);

	m_TTStatic5.SetDrawingProperties(2, 10, false);

	m_TTStatic7.DrawBorders(true);
    m_TTStatic7.DrawBackground(true);
	m_TTStatic7.SetColorProperties(RGB(221, 232, 101), RGB(23, 55, 125), RGB(202, 81, 0));

	m_TTStatic8.SetDrawingProperties(2, 10, true, true);
	m_TTStatic8.SetColorProperties(RGB(221, 232, 101), RGB(23, 55, 125), RGB(202, 81, 0));


    //m_Edit2.SetDrawingProperties(2, 10);
	m_Edit1.SetWindowTextW(_T("Sample edit box"));
	m_Edit2.SetWindowTextW(_T("Sample edit box"));

	//m_WideEdit2.SetDrawingProperties(2, 10);
	m_WideEdit1.SetWindowTextW(_T("Wide edit box"));
	m_WideEdit2.SetWindowTextW(_T("Wide edit box"));

	m_MultiLineEdit.SetWindowTextW(_T("multy\r\nline\r\ntext"));

    m_lf.lfWeight = -1;

    FillList(m_TTListCtrl, 10, 100);
    FillList(m_TTListCtrl2, 1, 8);
    FillList(m_TTListCtrl3, 5, 6);

    CWnd* pWnd = GetDlgItem(IDC_LIST3);
    CRect cRect;
    pWnd->GetWindowRect(&cRect);

    ScreenToClient(&cRect);
    ::MoveWindow(pWnd->GetSafeHwnd(), cRect.left, cRect.top, 300, cRect.Height(), TRUE);

	m_Edit2.SetWindowTextW(_T("Text for test"));

    m_dlgAnchor.Init(GetSafeHwnd());
	m_dlgAnchor.Add(m_Edit2.GetSafeHwnd(), ANCHOR_BOTTOMRIGHT);
	m_dlgAnchor.Add(m_TTListCtrl.GetSafeHwnd(), ANCHOR_TOPLEFT | ANCHOR_BOTTOMRIGHT);
	m_dlgAnchor.Add(m_TTListCtrl3.GetSafeHwnd(), ANCHOR_TOPLEFT | ANCHOR_BOTTOMRIGHT);
    m_dlgAnchor.Add(m_TTButton1.GetSafeHwnd(), ANCHOR_TOPLEFT | ANCHOR_BOTTOMRIGHT);
    m_dlgAnchor.Add(m_TTButton2.GetSafeHwnd(), ANCHOR_BOTTOMRIGHT);
	m_dlgAnchor.Add(m_TTStatic1.GetSafeHwnd(), ANCHOR_TOPLEFT | ANCHOR_BOTTOMRIGHT);
    m_dlgAnchor.Add(m_TTStatic6.GetSafeHwnd(), ANCHOR_BOTTOMRIGHT);
	m_dlgAnchor.Add(m_MultiLineEdit.GetSafeHwnd(), ANCHOR_TOPLEFT | ANCHOR_BOTTOMRIGHT);
    m_dlgAnchor.Add(m_TTRichEdit1.GetSafeHwnd(), ANCHOR_TOPLEFT | ANCHOR_BOTTOMRIGHT);
    
    /*CRect rc;
    GetWindowRect(rc);
    CRgn wRgn;
    wRgn.CreateRoundRectRgn(0, 0, rc.Width(), rc.Height(), m_curWndCornRadius, m_curWndCornRadius);
    SetWindowRgn(wRgn, TRUE);*/

	//m_TTButton1.SetBitmap(hBmp);
	/*if (hBmp == NULL)
		TRACE("Something wrong");

	if (m_TTButton1.GetBitmap() == NULL)
		m_TTButton1.SetBitmap(hBmp);*/

	/*if (m_TTButton2.GetBitmap() == NULL)
		TRACE("Something wrong");*/

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFlatTabCtrlDlg::FillList(CListCtrl &list, int colCount, int rowcount)
{
    for (int i = 0; i < colCount; i++)
    {
        list.InsertColumn(i, CString("Col" + i));
        list.SetColumnWidth(i, 60);
    }

    list.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0,
        LVS_EX_SIMPLESELECT);

    for (int i = 0; i < rowcount; i++)
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
                list.InsertItem(&lv);
            else
                list.SetItem(&lv);
        }
}

void CFlatTabCtrlDlg::FillCombo(CComboBox &Combo)
{
	for (int i = 0; i < 10; i++)
	{
        CString string;
		string.Format(_T("Item %d"), i);

		if (!::IsWindow(Combo.GetSafeHwnd()))
			TRACE("Not window\n");
		Combo.AddString(string); 
	}

   Combo.AddString(_T("And here is an item with a large amount of text only for test reason"));

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

		CRect r(10, 450, 140, 480);
		CTTEdit::DrawEditControlFrame(&dc, &r, &r);
		//dc.FillSolidRect(r, RGB(255, 0, 0));
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

BOOL testfunc(TestClassA*& a)
{
	TestClassB* b = new TestClassB();
	b->i = 10;
	b->j = 100;
	b->p = 999;

	a = b;

	return TRUE;
}

void CFlatTabCtrlDlg::OnBnClickedButton2()
{
	/*if (!pViewHolderDlg)
	{
		pViewHolderDlg = new CViewsHolderDlg(this);
		pViewHolderDlg->Create(CViewsHolderDlg::IDD, this);
		
		CRect emptyRect;
		emptyRect.SetRectEmpty();

		CString caption;
		caption.Format(_T("Here would be a View #1!"));
		m_View1.Create(caption, WS_CHILD | WS_BORDER | SS_NOTIFY, emptyRect, pViewHolderDlg);
		pViewHolderDlg->AddView(&m_View1);

		caption.Format(_T("Here would be a View #2!"));
		m_View2.Create(caption, WS_CHILD | WS_BORDER | SS_NOTIFY, emptyRect, pViewHolderDlg);
		pViewHolderDlg->AddView(&m_View2);

		caption.Format(_T("Here would be a View #3!"));
		m_View3.Create(caption, WS_CHILD | WS_BORDER | SS_NOTIFY, emptyRect, pViewHolderDlg);
		pViewHolderDlg->AddView(&m_View3);
	}
	
	if (!pViewHolderDlg->IsWindowVisible())
	{
		pViewHolderDlg->ShowWindow(SW_SHOW);
		pViewHolderDlg->ShowViews();
	}*/

	/*Gdiplus::Graphics g(this->GetDC()->GetSafeHdc());	
	Gdiplus::PointF pt(100.0f, 420.0f);

	CString s = _T("7.50");

	Gdiplus::SolidBrush br(Gdiplus::Color::Black);
	g.DrawString(s.GetBuffer(0), s.GetLength(), m_pFont, pt, &br);*/

	/*Gdiplus::SolidBrush br(Gdiplus::Color::Red);
	Gdiplus::Rect r(100, 420, 100, 100);
	g.FillRectangle(&br, r);*/

	return;
	/*CFuncPointerHolder fp{};
	fp.f = &testfunc;
	fp.InvokeFunc();

	return;*/

	/*m_curWndCornRadius--;
    CRect rc;
    GetWindowRect(rc);
    CRgn wRgn;
    wRgn.CreateRoundRectRgn(0, 0, rc.Width(), rc.Height(), m_curWndCornRadius, m_curWndCornRadius);
    SetWindowRgn(wRgn, TRUE);
    TRACE("Radius %d\n", m_curWndCornRadius);
    return;*/

    /*m_TTStatic1.EnableScrollBarCtrl(SB_HORZ, TRUE);
    m_TTStatic2.EnableScrollBarCtrl(SB_VERT, TRUE);*/

    TRACE("-----------------------------------------------------------------------------------\n");
    CWnd* pWnd = GetDlgItem(IDC_EDIT8);
    CRect cRect;
    pWnd->GetWindowRect(&cRect);
    
	ScreenToClient(&cRect);
    //::MoveWindow(pWnd->GetSafeHwnd(), cRect.left, cRect.top, cRect.Width() + 2, cRect.Height() + 2, TRUE);

	cRect.InflateRect(10, 10);
	CDC* pDC = this->GetDC();
	pDC->FillSolidRect(&cRect, RGB(255, 0, 0));
	ReleaseDC(pDC);

	cRect.right = cRect.left + 20;
	//cRect.left -= 10;
	cRect.bottom = cRect.top + 20;
	//cRect.top -= 10;

	InvalidateRect(cRect);

    return;
	
	/*Invalidate(TRUE);

	return;*/

	// TODO: Add your control notification handler code here
    CButton* pBtnWnd = (CButton*)GetDlgItem(IDC_BUTTON3);

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
		GetDlgItem(IDC_TTGROUPBOX3)->EnableWindow(FALSE);

        GetDlgItem(IDC_LIST1)->EnableWindow(FALSE);
        GetDlgItem(IDC_LIST2)->EnableWindow(FALSE);
        GetDlgItem(IDC_LIST3)->EnableWindow(FALSE);
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
		GetDlgItem(IDC_TTGROUPBOX3)->EnableWindow(TRUE);

        GetDlgItem(IDC_LIST1)->EnableWindow(TRUE);
        GetDlgItem(IDC_LIST2)->EnableWindow(TRUE);
        GetDlgItem(IDC_LIST3)->EnableWindow(TRUE);
	}
}


void CFlatTabCtrlDlg::OnBnClickedButton3()
{
	GetDlgItem(IDC_COMBO7)->EnableWindow(!GetDlgItem(IDC_COMBO7)->IsWindowEnabled());

	return;
	/*ManagedImageProcessor^ m = gcnew ManagedImageProcessor();
	NativeImageProcessor n(m);*/
	CImage img;
	img.Load(_T("H:\\test.png"));

	int bpp = img.GetBPP();

	//get pixel format:
	HBITMAP hbmp = img.Detach();
	Gdiplus::Bitmap* bmpTemp = Gdiplus::Bitmap::FromHBITMAP(hbmp, 0);
	Gdiplus::PixelFormat pixel_format = bmpTemp->GetPixelFormat();
	if (bpp == 32)
		pixel_format = PixelFormat32bppARGB;
	img.Attach(hbmp);

	//Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromHBITMAP((HBITMAP)img, NULL);
	Gdiplus::Bitmap bmp(img.GetWidth(), img.GetHeight(), img.GetPitch(), pixel_format, static_cast<BYTE*>(img.GetBits()));

	CDC* pDC = GetDC();

	pDC->FillSolidRect(CRect(150, 425, 300, 500), RGB(255, 0, 0));

	Gdiplus::Graphics graphics(pDC->GetSafeHdc());
	graphics.DrawImage(&bmp,
		Gdiplus::RectF(150, 425, 150, 75));/* ,
		Gdiplus::RectF(0, 0, bmp.GetWidth(), bmp.GetHeight()),
		Gdiplus::Unit::UnitPixel);*/

	/*Gdiplus::Rect r(150, 425, 150, 75);

	graphics.DrawImage(&bmp,
		r,
		r,
		Gdiplus::Unit::UnitPixel);

	r.Inflate(-1, -1);
	r.Width;
	Gdiplus::SolidBrush brush(Gdiplus::Color::White);
	graphics.FillRectangle(&brush, Gdiplus::Rect(150, 425, 150, 75));

	Gdiplus::Pen pen(Gdiplus::Color::Green, 1.0F);
	graphics.DrawRectangle(&pen, Gdiplus::Rect(150, 425, 150, 75));*/

	

	ReleaseDC(pDC);

	return;

    /*m_curWndCornRadius++;
    CRect rc;
    GetWindowRect(rc);
    CRgn wRgn;
    wRgn.CreateRoundRectRgn(0, 0, rc.Width(), rc.Height(), m_curWndCornRadius, m_curWndCornRadius);
    SetWindowRgn(wRgn, TRUE);
    TRACE("Radius %d\n", m_curWndCornRadius);    
    return;*/

    /*m_TTStatic1.EnableScrollBarCtrl(SB_HORZ, FALSE);
    m_TTStatic2.EnableScrollBarCtrl(SB_VERT, FALSE);*/
	
    TRACE("-----------------------------------------------------------------------------------\n");
    CWnd* pWnd = GetDlgItem(IDC_EDIT8);
    CRect cRect;
    pWnd->GetWindowRect(&cRect);

    ScreenToClient(&cRect);
    ::MoveWindow(pWnd->GetSafeHwnd(), cRect.left, cRect.top, cRect.Width() - 2, cRect.Height() - 2, TRUE);
    return;
	
	/*Invalidate(FALSE);
	
	return;*/

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

		GetDlgItem(IDC_TTGROUPBOX2)->EnableWindow(FALSE);
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
		
		GetDlgItem(IDC_TTGROUPBOX2)->EnableWindow(TRUE);
	}
}


void CFlatTabCtrlDlg::OnBnClickedCheck1()
{
    GetDlgItem(IDC_TT_STATIC1)->SetWindowTextW(_T("NEW STRING"));

    //GetDlgItem(IDC_TT_STATIC6)->SetWindowTextW(_T("NEW STRING"));
    
    if (m_lf.lfWeight < 0)
    {
        CFont* curFont = GetDlgItem(IDC_TT_STATIC6)->GetFont();
        curFont->GetLogFont(&m_lf);
    }
    else
    {
        m_lf.lfWeight += m_lf.lfWeight == 900 ? -900 : 100;
    }    

	CFont m_NewFont;
    m_NewFont.CreateFontIndirect(&m_lf);

    GetDlgItem(IDC_TT_STATIC6)->SetFont(&m_NewFont);
	//Invalidate();

	//GetDlgItem(IDC_TT_STATIC6)->SetWindowTextW(CString(m_lf.lfWeight.ToString()));    
}


void CFlatTabCtrlDlg::OnTabChanging(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    m_vPages[m_FlatTabCtrl.GetCurSel()]->ShowWindow(SW_HIDE);
    *pResult = 0;
}


void CFlatTabCtrlDlg::OnTabChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    RECT rc;
    m_FlatTabCtrl.GetItemRect(0, &rc);
    int nIndex = m_FlatTabCtrl.GetCurSel();

    m_vPages[nIndex]->SetWindowPos(NULL, rc.left + 1, rc.bottom + 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
    m_vPages[nIndex]->SetFocus();
}

void CFlatTabCtrlDlg::OnSize(UINT nType, int cx, int cy)
{
    __super::OnSize(nType, cx, cy);

    m_dlgAnchor.OnSize(FALSE);
}


HBRUSH CFlatTabCtrlDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	/*switch (nCtlColor)
	{

	case CTLCOLOR_EDIT:

		if (!pWnd->IsWindowEnabled() || (pWnd->GetStyle() & ES_READONLY))
		{
			LOGBRUSH lb;
			m_DisabledBkgndBrush.GetLogBrush(&lb);
			pDC->SetBkColor(lb.lbColor);
			hbr = m_DisabledBkgndBrush;
		}
		else
			hbr = m_NormalBkgndBrush;
		break;

	case CTLCOLOR_STATIC:

		if ((!pWnd->IsWindowEnabled() || pWnd->GetStyle() & ES_READONLY)
			&& dynamic_cast<CEdit*>(pWnd))
		{
			hbr = m_DisabledBkgndBrush;
		}
		else
		{
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_NormalBkgndBrush;
		}

		break;

	default:
		if (pWnd->IsWindowEnabled())
			hbr = m_NormalBkgndBrush;
		else
			hbr = m_DisabledBkgndBrush;
		break;
	}*/

	return hbr;
}


void CFlatTabCtrlDlg::OnSetfocusEdit4()
{
	//GetDlgItem(IDC_EDIT4)->SendMessage(WM_KEYDOWN, VK_DOWN, 0);
}


void CFlatTabCtrlDlg::OnKillfocusEdit4()
{
	/*if (pTestDlg != NULL && pTestDlg->IsWindowVisible() && pTestDlg != GetForegroundWindow())
		pTestDlg->ShowWindow(SW_HIDE);*/
}


void CFlatTabCtrlDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);

	if (pTestDlg)
	{
		SetToolTipWindowPosition(GetDlgItem(IDC_EDIT4));
		/*CRect cEditRect, cDlgRect;
		GetDlgItem(IDC_EDIT4)->GetWindowRect(&cEditRect);

		pTestDlg->GetWindowRect(&cDlgRect);

		CRect cNewRect(cEditRect.left, cEditRect.bottom, cEditRect.left + cDlgRect.Width(), cEditRect.bottom + cDlgRect.Height());

		pTestDlg->MoveWindow(cNewRect);*/
	}
	// TODO: Add your message handler code here
}

void CFlatTabCtrlDlg::AddTips()
{
	if (pTestDlg == NULL || !::IsWindow(pTestDlg->GetSafeHwnd()))
		return;

	pTestDlg->ClearTips();

	for (int i = 0; i < 10; i++)
	{
		CString string;
		string.Format(_T("Here is a directory name №%d"), i);

		pTestDlg->AddTip(i, string);
	}
}

void CFlatTabCtrlDlg::SetToolTipWindowPosition(CWnd* pDockWnd)
{
	if (pTestDlg == NULL || !pTestDlg->IsWindowVisible())
		return;

	CRect cDockWndRect, cDlgRect;
	pDockWnd->GetWindowRect(&cDockWndRect);

	pTestDlg->GetWindowRect(&cDlgRect);

	CRect cNewRect(cDockWndRect.left, cDockWndRect.bottom, cDockWndRect.left + cDlgRect.Width(), cDockWndRect.bottom + cDlgRect.Height());

	pTestDlg->SetWindowPos(this, cNewRect.left, cNewRect.top, cNewRect.Width(), cNewRect.Height(),
		/*SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER |*/ SWP_NOACTIVATE);
	//pTestDlg->MoveWindow(cNewRect);
}

void CFlatTabCtrlDlg::OnEnChangeEdit4()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	

	// TODO:  Add your control notification handler code here
	if (pTestDlg == NULL)
	{
		pTestDlg = new CTestDialog(this);
		pTestDlg->Create(IDD_DIALOG1, this);
	}

	if (!pTestDlg->IsWindowVisible())
	{
		pTestDlg->ShowWindow(SW_SHOW);
		AddTips();
		SetToolTipWindowPosition(GetDlgItem(IDC_EDIT4));
	}
}


void CFlatTabCtrlDlg::OnDestroy()
{
	/*if (m_pac)
		m_pac->Unbind();*/

	CDialogEx::OnDestroy();

	//CoUninitialize();
}


BOOL CFlatTabCtrlDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect clRect;
	GetClientRect(clRect);

	pDC->FillSolidRect(clRect, RGB(255, 255, 255));

	CRect fRect(clRect.left, clRect.bottom - 100, clRect.left + 100, clRect.bottom);

	COLORREF cl = RGB(128, 255, 0);
	pDC->FillSolidRect(fRect, cl);
	
	COLORREF contrColor = ColorsCommon::SfxContrastingColor(cl, 50);

	fRect.OffsetRect(100, 0);
	pDC->FillSolidRect(fRect, contrColor);

	return TRUE;
}
