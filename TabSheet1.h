#pragma once


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
};
