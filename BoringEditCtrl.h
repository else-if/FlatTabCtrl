#pragma once

// BoringEditCtrl

class BoringEditCtrl : public CEdit
{
	DECLARE_DYNAMIC(BoringEditCtrl)

	// Construction
public:
	BoringEditCtrl();

protected:
	COLORREF m_backgroundColor;
	CRect m_rectNCBottom;
	CRect m_rectNCTop;

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditVC)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~BoringEditCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditVC)
	// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};
