#pragma once

#include "ControlsColorMap.h"

// CTTEdit

class CTTEdit : public CEdit
{
	DECLARE_DYNAMIC(CTTEdit)

public:
	CTTEdit();
	virtual ~CTTEdit();

	void SetDrawingProperties(float borderPenWidth, int cornerRadius);

protected:
	CRect m_rectNCBottom;
	CRect m_rectNCTop;

	ControlState m_ControlState;
	ControlsColorMap m_ColorMap;
	
	float m_fBorderPenWidth;
	int m_CornerRadius;
	bool m_bTracking;	

	void UpdateControlState();

	DECLARE_MESSAGE_MAP()
public:	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg UINT OnGetDlgCode();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};


