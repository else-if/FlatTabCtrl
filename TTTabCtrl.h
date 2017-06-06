#pragma once
#include "ControlsColorMap.h"
// CTTTabCtrl

class CTTTabCtrl : public CTabCtrl
{
	DECLARE_DYNAMIC(CTTTabCtrl)

public:
	CTTTabCtrl();
	virtual ~CTTTabCtrl();

	COLORREF BackgroundColor();
	void BackgroundColor(COLORREF backgroundColor);

	void SetDrawingProperties(int borderPenWidth, int cornerRadius);

	static void DrawTabControlFrame(CWnd* pWnd, CDC* pDC, CRect* pTabRect, CRect* pViewRect, ControlState controlState,
		int cornerRadius, int borderWidth, COLORREF bkgndColor, ControlsColorMap* pColorMap);

	static void DrawTabItem(CDC* pDC, CRect* pTabRect, const CString& tabText, CFont* pFont, COLORREF textColor,
		bool IsSelected, bool IsHotTracked, int cornerRadius, ControlsColorMap* pColorMap);

	static void GetTabControlPath(Gdiplus::GraphicsPath *pPath, Gdiplus::Rect tab, Gdiplus::Rect tabView, int dia);

protected:

	bool m_bTracking;
	int m_iMouseOverTab;
	COLORREF m_backgroundColor;
	int m_borderWidth;
	int m_cornerRadius;

	ControlState m_ControlState;
	ControlsColorMap m_ColorMap;

	void UpdateControlState();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};