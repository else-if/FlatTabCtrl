#pragma once
#include <vcclr.h>
#include <msclr\event.h>
#include "afxcmn.h"
#include "afxwin.h"

using namespace System;
using namespace System::ComponentModel;

// CBGWorkerDlg dialog

class CBGWorkerDlg : public CDialog
{
	DECLARE_DYNAMIC(CBGWorkerDlg)

public:
	CBGWorkerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBGWorkerDlg();

// Dialog Data
	enum { IDD = IDD_BGWORKER_DIALOG };

	BEGIN_DELEGATE_MAP(CBGWorkerDlg)
		EVENT_DELEGATE_ENTRY(bgWorker_DoWork, Object^, DoWorkEventArgs^)
		EVENT_DELEGATE_ENTRY(bgWorker_RunWorkerCompleted, Object^, RunWorkerCompletedEventArgs^)
		EVENT_DELEGATE_ENTRY(bgWorker_ProgressChanged, Object^, ProgressChangedEventArgs^)
	END_DELEGATE_MAP()

	gcroot<BackgroundWorker^> m_bgWorker;

protected:

	
	gcroot<System::Windows::Forms::WindowsFormsSynchronizationContext^> m_wfsc;

	void InitializeBackgroundWorker();

	void bgWorker_DoWork(Object^ sender, DoWorkEventArgs^ e);

	void bgWorker_RunWorkerCompleted(Object^ sender, RunWorkerCompletedEventArgs^ e);

	void bgWorker_ProgressChanged(Object^ sender, ProgressChangedEventArgs^ e);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	
	CProgressCtrl m_progressBar;
	CStatic m_infoStatic;
};
