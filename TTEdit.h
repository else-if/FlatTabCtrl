#pragma once


// CTTEdit

class CTTEdit : public CEdit
{
	DECLARE_DYNAMIC(CTTEdit)

public:
	CTTEdit();
	virtual ~CTTEdit();

protected:
	CBrush m_HollowBrush;
	COLORREF m_TextColor;
	CRect m_rectNCBottom;
	CRect m_rectNCTop;
	CRect m_EditRect;
	bool m_bHover;

	DECLARE_MESSAGE_MAP()
public:	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg UINT OnGetDlgCode();
};


