#pragma once
#include "afxwin.h"
#include "IView.h"

class CViewHeader : public CStatic
{
public:
	CViewHeader(IView* view);
	virtual ~CViewHeader();

	IView* m_pView;

	void SetCollapsed(bool collapsed);

protected:
	bool m_bCollapsed;
	int m_nButtonSize;
	int m_nBtnItemOffset;

	CRect m_ExColBtnRect;
	CRect m_CloseBtnRect;

	CPoint m_CurMousePt;

	void InvalidateBtnRect(CRect &btnRect);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
};

