#pragma once
#include "afxwin.h"
#include "TTButton.h"


// CTabSheet1 dialog

class CTabSheet1 : public CDialog
{
	DECLARE_DYNAMIC(CTabSheet1)

public:
	CTabSheet1(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTabSheet1();

// Dialog Data
	enum { IDD = IDD_TABSHEET1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    CTTButton m_CancelBtn;
    CTTButton m_OkBtn;
    CTTButton m_btn1;
    CTTButton m_btn2;
    CTTButton m_btn3;
    CTTButton m_btn4;
    CTTButton m_btn5;
    CTTButton m_btn6;
    CTTButton m_btn7;
    CTTButton m_btn8;
};
