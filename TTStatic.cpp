#pragma once

#include "stdafx.h"
#include "TTStatic.h"
#include "CommonDrawing.h"

using namespace Gdiplus;

IMPLEMENT_DYNAMIC(CTTStatic, CStatic)

CTTStatic::CTTStatic()
{	
	SetBackgroundColor(GetSysColor(COLOR_3DFACE));
	SetTextColor(GetSysColor(COLOR_CAPTIONTEXT));
	SetDrawingProperties(1, 5, false);
	
	m_ControlState = Normal;	
}

CTTStatic::~CTTStatic()
{	
}

void CTTStatic::UpdateControlState()
{
	if (!IsWindowEnabled())
		m_ControlState = Disable;
	else
		m_ControlState = Normal;
}

COLORREF CTTStatic::GetBackgroundColor()
{
	return m_backgroundColor;
}

void CTTStatic::SetBackgroundColor(COLORREF backgroundColor)
{
	m_backgroundColor = backgroundColor;
}

COLORREF CTTStatic::GetTextColor()
{
	return m_textColor;
}

void CTTStatic::SetTextColor(COLORREF textColor)
{
	m_textColor = textColor;
}

void CTTStatic::SetDrawingProperties(int borderPenWidth, int cornerRadius, bool drawBorders)
{
	m_borderPenWidth = borderPenWidth;
	m_iCornerRadius = cornerRadius;
	m_bDrawBorders = drawBorders;
}

COLORREF CTTStatic::GetBorderColor()
{
	return m_borderColor;
}

void CTTStatic::SetBorderColor(COLORREF borderColor)
{
	m_borderColor = borderColor;
}

void CTTStatic::DrawBorders(bool drawBorders)
{
	m_bDrawBorders = drawBorders;
}

void CTTStatic::SetColorProperties(COLORREF backgroundColor, COLORREF textColor, COLORREF borderColor)
{
	SetBackgroundColor(backgroundColor);
	SetTextColor(textColor);
	SetBorderColor(borderColor);
}

BEGIN_MESSAGE_MAP(CTTStatic, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

BOOL CTTStatic::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CTTStatic::OnPaint()
{
	CRect cRect;
	GetClientRect(&cRect);
	CPaintDC dc(this);
	CMemDC memDC(dc, cRect);

	Graphics graphics(memDC.GetDC().GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	UpdateControlState();

	//Background

	DrawThemeParentBackground(GetSafeHwnd(), memDC.GetDC().GetSafeHdc(), cRect);

	if (m_backgroundColor)
		FillRectRegion(cRect, memDC, m_backgroundColor, m_iCornerRadius);

	// Borders
	if (m_bDrawBorders)
	{
		COLORREF borderColor;
		if (m_ControlState == Disable)
			borderColor = m_ColorMap.GetColor(m_ControlState, Border);
		else if (m_borderColor)
			borderColor = m_borderColor;
		else
			borderColor = m_ColorMap.GetColor(m_ControlState, Border);

		Gdiplus::Rect BorderRect(cRect.left, cRect.top, cRect.right, cRect.bottom);
		DrawRectArea(BorderRect, graphics, borderColor, m_iCornerRadius, m_borderPenWidth);
	}

    CString strText = _T("");
    GetWindowText(strText);

    memDC.GetDC().SetTextColor(m_textColor);

    LOGFONT logFont;
    GetFont()->GetLogFont(&logFont);
    CFont font;
    font.CreateFontIndirectW(&logFont);
    memDC.GetDC().SelectObject(&font);

    DWORD ExStyle = GetExStyle();
    DWORD Style = GetStyle() & SS_TYPEMASK;

    UINT nFormat = DT_WORDBREAK;

    if (Style == SS_LEFT)
        nFormat |= DT_LEFT;
    else if (Style == SS_RIGHT || ExStyle & WS_EX_RIGHT)
        nFormat |= DT_RIGHT;
    else if (Style == SS_CENTER)
        nFormat |= DT_CENTER;

    int textOffset = 0;
    if (m_bDrawBorders)
        textOffset += m_borderPenWidth + (m_borderPenWidth > 1 ? 0 : 1);

    cRect.DeflateRect(textOffset, textOffset, textOffset, textOffset);
    memDC.GetDC().DrawText(strText, cRect, nFormat);
}

void CTTStatic::OnEnable(BOOL bEnable)
{
	Invalidate();
}
