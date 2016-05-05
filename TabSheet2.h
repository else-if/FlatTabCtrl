#pragma once
#include "afxwin.h"
#include "TTButton.h"

// CTabSheet2 dialog

class CTabSheet2 : public CDialog
{
	DECLARE_DYNAMIC(CTabSheet2)

public:
	CTabSheet2(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTabSheet2();

// Dialog Data
	enum { IDD = IDD_TABSHEET2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    CTTButton m_OkBtn;
    CTTButton m_CancelBtn;
};
