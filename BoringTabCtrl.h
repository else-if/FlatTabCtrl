#pragma once


// CBoringTabCtrl

class CBoringTabCtrl : public CTabCtrl
{
	DECLARE_DYNAMIC(CBoringTabCtrl)

public:
	CBoringTabCtrl();
	virtual ~CBoringTabCtrl();

	COLORREF BackgroundColor();
	void BackgroundColor(COLORREF backgroundColor);

protected:
	bool m_bTracking;
	int m_iMouseOverTab;
	COLORREF m_backgroundColor;
	int m_borderWidth;
	int m_cornerRadius;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void GetInteriorRect(CRect& rect);
	afx_msg void OnPaint();
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


