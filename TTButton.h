#pragma once
#include "ControlsColorMap.h"

class CTTButton : public CButton
{

public:
	CTTButton();
	virtual ~CTTButton();

protected:
	
	ControlState m_ButtonState;
	ControlsColorMap m_ColorMap;

	COLORREF m_CaptionTextColor;

	float m_fBorderPenWidth;
	int m_CornerRadius;
	bool m_bTracking;	

	CFont m_TextFont;

	void UpdateButtonState(UINT state);

public:
	
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);	
	virtual BOOL PreTranslateMessage(MSG* pMsg);	

protected:

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);	
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	
	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();	
};