#pragma once
#include "ControlsColorMap.h"

#define OFS_LEFT 10

class CTTGroupBox : public CButton
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
};
