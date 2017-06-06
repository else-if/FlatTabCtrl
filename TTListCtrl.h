#pragma once

#include "ControlsColorMap.h"

class CTTListCtrl : public CListCtrl
{
public:
	CTTListCtrl();
	virtual ~CTTListCtrl();

	void SetDrawingProperties(int borderPenWidth, int cornerRadius);

	static void NcCalcSize(CWnd* pWnd, NCCALCSIZE_PARAMS* lpncsp, CRect* pOffsetRect, int borderWidth);
	static void PaintNonClientArea(CWnd* pWnd, CDC* pDC, CRect* pOffsetRect, ControlState controlState = Normal, CDC* pMemDC = NULL,
		ControlsColorMap* pColorMap = NULL, int cornerRadius = 5, int borderWidth = 1);
	static void DrawBorderRect(CDC* pDC, CRect* pBorderRect, ControlState controlState = Normal, ControlsColorMap* pColorMap = NULL,
		int cornerRadius = 5, int borderWidth = 1);

protected:

	bool m_bNcSizeIsCalculated;
	bool m_bUseBitmap;
	bool m_bPainted;
	bool m_bDrawBorders;
	bool m_bSized;

	ControlState m_ControlState;
	ControlsColorMap m_ColorMap;

	CRect m_OffsetRect;
	CRect m_oldParentRect;
	Gdiplus::Rect m_ClientRect;
	CDC m_dc;

	int m_borderPenWidth;
	int m_CornerRadius;

	void UpdateControlState();

public:

	virtual void PreSubclassWindow();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};

