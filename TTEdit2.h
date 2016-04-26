#pragma once

#include "ControlsColorMap.h"

// CTTEdit

class CTTEdit2 : public CEdit
{
	DECLARE_DYNAMIC(CTTEdit2)

public:
	CTTEdit2();
	virtual ~CTTEdit2();

	void SetDrawingProperties(int borderPenWidth, int cornerRadius);

protected:

	CRect m_ClientRect;

	ControlState m_ControlState;
	ControlsColorMap m_ColorMap;

	int m_borderPenWidth;
	int m_CornerRadius;
	bool m_bHover;

	void UpdateControlState();
	void Paint(CDC* pDC);

	int m_OffsetY;
	int uiCX, uiCY;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	afx_msg void OnNcPaint();

	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseLeave();
};


