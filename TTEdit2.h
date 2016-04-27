#pragma once

#include "ControlsColorMap.h"

// CTTEdit

class CTTEdit2 : public CEdit
{

public:
	CTTEdit2();
	virtual ~CTTEdit2();

	void SetDrawingProperties(int borderPenWidth, int cornerRadius);

protected:

	Gdiplus::Rect m_ClientRect;

	ControlState m_ControlState;
	ControlsColorMap m_ColorMap;

	int m_borderPenWidth;
	int m_CornerRadius;
	
	bool m_bHover;

	CDC m_dc;
	bool m_bStateChanged;
	bool m_bUseBitmap;	
	bool painted;

	void UpdateControlState();
	void Paint(CDC* pDC);

	int m_OffsetY;
	UINT uiCX, uiCY;

	void SetPosition(int x, int y);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	afx_msg void OnNcPaint();

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseLeave();
	afx_msg void OnEnUpdate();
	virtual void PreSubclassWindow();
	afx_msg void OnPaint();
};


