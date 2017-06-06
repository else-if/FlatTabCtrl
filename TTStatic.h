#pragma once

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
	void DrawBackground(bool drawBackground);

	void SetDrawingProperties(int borderPenWidth, int cornerRadius, bool drawBorder = false, bool drawBackground = false);

	void SetColorProperties(COLORREF backgroundColor, COLORREF textColor, COLORREF borderColor);

protected:

	ControlState m_ControlState;
	ControlsColorMap m_ColorMap;

	COLORREF m_backgroundColor;
	COLORREF m_textColor;
	COLORREF m_borderColor;

	CFont m_TextFont;

	CRect m_oldParentRect;

	bool m_bDrawBorders;
	bool m_bDrawBackground;
	int m_borderPenWidth;
	int m_iCornerRadius;

	void UpdateControlState();
	void UpdateTextFont();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
protected:
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	virtual void PreSubclassWindow();
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
