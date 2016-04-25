#pragma once

//#include "Style.h"

/////////////////////////////////////////////////////////////////////////////
// CTTEdit window

class CTTEdit : public CEdit
{
	// Construction
public:
	CTTEdit();
	virtual ~CTTEdit();

	//void SetStack(Stack stack);
	//void SetTextClr(COLORREF clr);

	CRect m_rectNCBottom;
	CRect m_rectNCTop;

protected:

	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnUpdate();

	DECLARE_MESSAGE_MAP()

private:

	//Style		m_Style;
	CRect		m_EditRect;
	int			m_OffsetY;
	CRect		m_NCRect;
	CBrush		m_HollowBrush;
	COLORREF	m_TextColor;

	Gdiplus::Rect m_FrameRect;
	Gdiplus::Rect m_LayerRect;

	CDC m_MemDC;
	CDC m_pDC;
	CDC m_LayerDC;
	CRect m_MemDCrect;

			

	bool m_bUseBitmap;

	Gdiplus::Graphics*	m_LayerpGdi;
	CDC*		m_LayerMemDC;

	void SetPosition(int x, int y);
	void Paint(CDC* pDevC, CRect rect);

	void GetpDC(CDC* pDC, CRect rect, BOOL bCopyFirst = FALSE);

	void PaintStack(CDC* pDC, Gdiplus::Graphics* pGraphics);
	void SetBorderClip(Gdiplus::Graphics* pGraphics, Gdiplus::Rect rc, Gdiplus::CombineMode mode = Gdiplus::CombineModeReplace);
	void PaintBorders(Gdiplus::Graphics* pGraphics, Gdiplus::Rect r, Gdiplus::Color ulclr, Gdiplus::Color brclr, int width);
	void DeletepDC(CDC* pDC);

public:
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	afx_msg void OnNcPaint();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
