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

protected:

	bool m_bTracking;
	int m_iMouseOverTab;
	COLORREF m_backgroundColor;
	int m_borderWidth;
	int m_cornerRadius;

	ControlsColorMap m_ColorMap;

	void GetTabControlPath(Gdiplus::GraphicsPath *pPath, Gdiplus::Rect tab, Gdiplus::Rect tabView, int dia);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void PreSubclassWindow();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void GetInteriorRect(CRect& rect);
	afx_msg void OnPaint();
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
};


