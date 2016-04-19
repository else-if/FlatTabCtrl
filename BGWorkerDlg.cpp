// BGWorkerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FlatTabCtrl.h"
#include "BGWorkerDlg.h"
#include "afxdialogex.h"


// CBGWorkerDlg dialog

IMPLEMENT_DYNAMIC(CBGWorkerDlg, CDialog)

CBGWorkerDlg::CBGWorkerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBGWorkerDlg::IDD, pParent)
{
	m_bgWorker = gcnew BackgroundWorker();
}

CBGWorkerDlg::~CBGWorkerDlg()
{
}

void CBGWorkerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_progressBar);
	DDX_Control(pDX, IDC_INFO_STATIC, m_infoStatic);
}

BEGIN_MESSAGE_MAP(CBGWorkerDlg, CDialog)
END_MESSAGE_MAP()

// CBGWorkerDlg message handlers

BOOL CBGWorkerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_wfsc = gcnew System::Windows::Forms::WindowsFormsSynchronizationContext();
	System::Threading::SynchronizationContext::SetSynchronizationContext(m_wfsc);
	InitializeBackgroundWorker();
	m_bgWorker->RunWorkerAsync();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// Set up the BackgroundWorker object by 
// attaching event handlers. 
void CBGWorkerDlg::InitializeBackgroundWorker()
{
	m_bgWorker->WorkerReportsProgress = true;
	//m_bgWorker->DoWork += MAKE_DELEGATE(DoWorkEventHandler, bgWorker_DoWork);
	m_bgWorker->RunWorkerCompleted += MAKE_DELEGATE(RunWorkerCompletedEventHandler, bgWorker_RunWorkerCompleted);
	m_bgWorker->ProgressChanged += MAKE_DELEGATE(ProgressChangedEventHandler, bgWorker_ProgressChanged);
}

void CBGWorkerDlg::bgWorker_DoWork(Object^ sender, DoWorkEventArgs^ e)
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

void CBGWorkerDlg::bgWorker_RunWorkerCompleted(Object^ sender, RunWorkerCompletedEventArgs^ e)
{
	CDialog::OnOK();
}

void CBGWorkerDlg::bgWorker_ProgressChanged(Object^ sender, ProgressChangedEventArgs^ e)
{
	if (e->UserState != nullptr)
	{	
		System::String^ s = (System::String^) e->UserState;
		m_infoStatic.SetWindowText(CString(s));
	}
	m_progressBar.SetPos(e->ProgressPercentage);
}
