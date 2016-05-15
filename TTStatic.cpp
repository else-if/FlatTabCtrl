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
	SetDrawingProperties(1, 5, false, false);
	
	m_ControlState = Normal;    
    m_borderColor = 0;

	m_bUseBmp = false;
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

void CTTStatic::SetDrawingProperties(int borderPenWidth, int cornerRadius, bool drawBorders, bool drawBackground)
{
	m_borderPenWidth = borderPenWidth;
	m_iCornerRadius = cornerRadius;
	m_bDrawBorders = drawBorders;
    m_bDrawBackground = drawBackground;
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

void CTTStatic::DrawBackground(bool drawBackground)
{
    m_bDrawBackground = drawBackground;
}

void CTTStatic::SetColorProperties(COLORREF backgroundColor, COLORREF textColor, COLORREF borderColor)
{
	SetBackgroundColor(backgroundColor);
	SetTextColor(textColor);
	SetBorderColor(borderColor);
}

void CTTStatic::UpdateTextFont()
{
	LOGFONT m_lf;
	CFont* curFont = GetFont();
	curFont->GetLogFont(&m_lf);

	m_TextFont.Detach();
	m_TextFont.CreateFontIndirect(&m_lf);
}

BEGIN_MESSAGE_MAP(CTTStatic, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_ENABLE()
    ON_WM_CTLCOLOR()    
    ON_MESSAGE(WM_SETTEXT, &CTTStatic::OnSetText)
    ON_MESSAGE(WM_SETFONT, &CTTStatic::OnSetFont)
END_MESSAGE_MAP()

BOOL CTTStatic::OnEraseBkgnd(CDC* pDC)
{
	if (!m_bUseBmp){
		CRect Rect;
		GetWindowRect(&Rect);
		CWnd *pParent = GetParent();
		pParent->ScreenToClient(&Rect);
		CDC *pParentDC = pParent->GetDC();
		
		m_dc.DeleteDC();
		// store into m_dc
		CBitmap bmp;
		m_dc.CreateCompatibleDC(pParentDC);
		bmp.CreateCompatibleBitmap(pParentDC, Rect.Width(), Rect.Height());
		m_dc.SelectObject(&bmp);
		m_dc.BitBlt(0, 0, Rect.Width(), Rect.Height(), pParentDC, Rect.left, Rect.top, SRCCOPY);
		bmp.DeleteObject();

		m_bUseBmp = true;

		pParent->ReleaseDC(pParentDC);
	}
	
    return TRUE;
}

void CTTStatic::OnPaint()
{
	CRect cRect;
    GetClientRect(&cRect);
    CPaintDC dc(this);

    Graphics graphics(dc.GetSafeHdc());
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

    UpdateControlState();

	if (m_bUseBmp)
	{
		dc.BitBlt(cRect.left, cRect.top, cRect.Width(), cRect.Height(),
			&m_dc, 0, 0, SRCCOPY);
	}

    dc.SetBkMode(TRANSPARENT);

    //Background
    if (m_bDrawBackground)
        FillRectRegion(cRect, dc, m_backgroundColor, m_iCornerRadius);

    // Borders
    if (m_bDrawBorders)
    {
        COLORREF borderColor = m_ColorMap.GetColor(m_ControlState, Border);
        if (m_borderColor && m_ControlState != Disable)
            borderColor = m_borderColor;

        Gdiplus::Rect BorderRect(cRect.left, cRect.top, cRect.right, cRect.bottom);
        DrawRectArea(BorderRect, graphics, borderColor, m_iCornerRadius, m_borderPenWidth);
    }

    COLORREF textColor = m_textColor;
    if (m_ControlState == Disable)
        textColor = GetSysColor(COLOR_GRAYTEXT);

    dc.SetTextColor(textColor);

    CString strText = _T("");
    GetWindowText(strText);

    dc.SelectObject(m_TextFont);

    DWORD ExStyle = GetExStyle();
    DWORD Style = GetStyle();

    UINT nFormat;

    if (Style & SS_CENTERIMAGE)
        nFormat = DT_VCENTER | DT_SINGLELINE;
    else
        nFormat = DT_WORDBREAK;

    Style &= SS_TYPEMASK;

    if (Style == SS_LEFT)
        nFormat |= DT_LEFT;
    else if (Style == SS_RIGHT || ExStyle & WS_EX_RIGHT)
        nFormat |= DT_RIGHT;
    else if ((Style == SS_CENTER))
        nFormat |= DT_CENTER;

    int textOffset = 0;
    if (m_bDrawBorders)
        textOffset += m_borderPenWidth + (m_borderPenWidth > 1 ? 0 : 1);

    cRect.DeflateRect(textOffset, textOffset, textOffset, textOffset);
    dc.DrawText(strText, cRect, nFormat);
}

void CTTStatic::OnEnable(BOOL bEnable)
{
	Invalidate();
}

HBRUSH CTTStatic::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    pDC->SetBkMode(TRANSPARENT);
    return (HBRUSH)GetStockObject(NULL_BRUSH);
}

void CTTStatic::PreSubclassWindow()
{
    HWND hWnd = GetSafeHwnd();

    LONG lStyle = GetWindowLong(hWnd, GWL_STYLE);
    LONG lExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

    LONG lStyleBorderMask = WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU;
    LONG lExStyleBorderMask = WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE;

    // Remove default border style, if exist
    if ((lStyle & lStyleBorderMask) || (lExStyle & lExStyleBorderMask))
    {
        DrawBorders(true);
        lStyle &= ~lStyleBorderMask;
        SetWindowLong(hWnd, GWL_STYLE, lStyle);

        lExStyle &= ~lExStyleBorderMask;
        SetWindowLong(hWnd, GWL_EXSTYLE, lExStyle);

        SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
    }

	UpdateTextFont();

    CStatic::PreSubclassWindow();
}

afx_msg LRESULT CTTStatic::OnSetText(WPARAM wParam, LPARAM lParam)
{
    LRESULT Result = Default();
    CRect Rect;
    GetWindowRect(&Rect);
    GetParent()->ScreenToClient(&Rect);
    GetParent()->InvalidateRect(&Rect);
    GetParent()->UpdateWindow();
    return Result;
}

afx_msg LRESULT CTTStatic::OnSetFont(WPARAM wParam, LPARAM lParam)
{
    LRESULT Result = Default();
	
	UpdateTextFont();

    CRect Rect;
    GetWindowRect(&Rect);
    GetParent()->ScreenToClient(&Rect);
    GetParent()->InvalidateRect(&Rect);
    GetParent()->UpdateWindow();
	return Result;
}
