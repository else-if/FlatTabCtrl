#pragma once
#include "TTListCtrl.h"


// CTabSheet3 dialog

class CTabSheet3 : public CDialog
{
	DECLARE_DYNAMIC(CTabSheet3)

public:
	CTabSheet3(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTabSheet3();

// Dialog Data
	enum { IDD = IDD_TABSHEET3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    CTTListCtrl listCtrl;
    virtual BOOL OnInitDialog();
    CListCtrl/*CTTListCtrl*/ listctrl2;
};
