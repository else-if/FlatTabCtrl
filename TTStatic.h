﻿#pragma once

#include "ControlsColorMap.h"

class CTTStatic : public CStatic
{
	DECLARE_DYNAMIC(CTTStatic)

	public:
		CTTStatic();
		virtual ~CTTStatic();

		COLORREF GetBackgroundColor();
		void SetBackgroundColor(COLORREF backgroundColor);

		COLORREF GetTextColor();
		void SetTextColor(COLORREF textColor);

		COLORREF GetBorderColor();
		void SetBorderColor(COLORREF textColor);

		void DrawBorders(bool drawBorder);

		void SetDrawingProperties(int borderPenWidth, int cornerRadius, bool drawBorder = false);

        void SetColorProperties(COLORREF backgroundColor, COLORREF textColor, COLORREF borderColor);

	protected:
        CBitmap m_Bmp;

		ControlState m_ControlState;
		ControlsColorMap m_ColorMap;

		COLORREF m_backgroundColor;
		COLORREF m_textColor;
		COLORREF m_borderColor;
		
		bool m_bDrawBorders;
		int m_borderPenWidth;
		int m_iCornerRadius;
		
		void UpdateControlState();

		DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
