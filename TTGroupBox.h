#pragma once
#include "ControlsColorMap.h"

#define OFS_LEFT 10

class CTTGroupBox : public CStatic
{
public:
	CTTGroupBox();
	virtual ~CTTGroupBox();

	void SetDrawingProperties(int borderPenWidth, int cornerRadius);

protected:

	ControlState m_ControlState;
	ControlsColorMap m_ColorMap;

	COLORREF m_CaptionTextColor;
	
	int m_BorderPenWidth;
	int m_CornerRadius;
	
	void UpdateControlState();
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();	
	afx_msg void OnEnable(BOOL bEnable);
	virtual void PreSubclassWindow();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
