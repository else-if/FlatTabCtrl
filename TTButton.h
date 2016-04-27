#pragma once
#include "ControlsColorMap.h"

class CTTButton : public CButton
{

public:
	CTTButton();
	virtual ~CTTButton();

	void SetDrawingProperties(int borderPenWidth, int cornerRadius);

protected:
	
	ControlState m_ButtonState;
	ControlsColorMap m_ColorMap;

	COLORREF m_CaptionTextColor;

	int m_BorderPenWidth;
	int m_CornerRadius;
	bool m_bTracking;	
    bool m_bIsDefault;

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
    virtual void PreSubclassWindow();
    afx_msg void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnNcPaint();
    afx_msg void OnUpdateAfxIdPreviewClose(CCmdUI *pCmdUI);
    afx_msg void OnAfxIdPreviewClose();
};