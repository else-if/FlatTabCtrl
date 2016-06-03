#pragma once

#include "ControlsColorMap.h"

// CTTEdit

class CTTEdit : public CEdit
{

public:
	CTTEdit();
	virtual ~CTTEdit();

	void SetDrawingProperties(int borderPenWidth, int cornerRadius);

	static void DrawEditControlFrame(CDC* pDC, CRect* pRect, CRect* pClipRect, ControlState controlState = Normal, CRect* pClientRect = NULL,
		int cornerRadius = 5, int borderWidth = 1, ControlsColorMap* pColorMap = NULL);

protected:

	bool m_bUseBaseMessageHandlers;
	bool m_bNcSizeIsCalculated;
	bool m_bPainted;

	Gdiplus::Rect m_ClientRect;
	Gdiplus::Rect m_BorderRect;
	CRect m_OffsetRect;

	CRect m_oldParentRect;

	ControlState m_ControlState;
	ControlsColorMap m_ColorMap;

	int m_borderPenWidth;
	int m_CornerRadius;

	bool m_bHover;
	bool m_bFocused;

	CDC m_dc;
	bool m_bUseBitmap;

	void UpdateControlState();
	void Paint(CDC* pDC);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	afx_msg void OnNcPaint();

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseLeave();

	virtual void PreSubclassWindow();
	afx_msg void OnPaint();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
};