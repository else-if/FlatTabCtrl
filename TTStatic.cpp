﻿#include "stdafx.h"
#include "TTStatic.h"
#include "CommonDrawing.h"

using namespace Gdiplus;

IMPLEMENT_DYNAMIC(CTTStatic, CStatic)

CTTStatic::CTTStatic() :
m_oldParentRect(0, 0, 0, 0)
{
	SetBackgroundColor(GetSysColor(COLOR_3DFACE));
	SetTextColor(GetSysColor(COLOR_CAPTIONTEXT));
	SetDrawingProperties(1, 5, false, false);

	m_ControlState = Normal;
	m_borderColor = 0;

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
	m_TextFont.Detach();

	LOGFONT m_lf;
	CFont* curFont = GetFont();
	if ((HFONT)curFont == NULL)
		curFont = GetParent()->GetFont();

	if ((HFONT)curFont == NULL)
		return;

	curFont->GetLogFont(&m_lf);
	m_TextFont.CreateFontIndirect(&m_lf);

}

BEGIN_MESSAGE_MAP(CTTStatic, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_ENABLE()
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_SETTEXT, &CTTStatic::OnSetText)
	ON_MESSAGE(WM_SETFONT, &CTTStatic::OnSetFont)
	ON_WM_MOVE()
	ON_WM_SIZE()
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

	if (cRect.Width() <= 0 || cRect.Height() <= 0)
		return;

	CMemDC memDC(dc, cRect);

	CDC* pDC = &memDC.GetDC();

	Graphics graphics(pDC->GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	UpdateControlState();

	CRect wndRect;
	GetWindowRect(&wndRect);
	CWnd *pParent = GetParent();
	pParent->ScreenToClient(&wndRect);
	CDC *pParentDC = pParent->GetDC();

	pDC->BitBlt(cRect.left, cRect.top, cRect.Width(), cRect.Height(),
		pParentDC, wndRect.left, wndRect.top, SRCCOPY);
	pParent->ReleaseDC(pParentDC);


	pDC->SetBkMode(TRANSPARENT);

	//Background
	if (m_bDrawBackground)
		FillRectRegion(cRect, *pDC, m_backgroundColor, m_iCornerRadius);

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

	pDC->SetTextColor(textColor);

	CString strText = _T("");
	GetWindowText(strText);

	if ((HFONT)m_TextFont != NULL)
		pDC->SelectObject(m_TextFont);

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
	pDC->DrawText(strText, cRect, nFormat);

	CWnd *pWnd = GetParent();
	if (pWnd != NULL)
	{
		GetWindowRect(m_oldParentRect);
		::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&m_oldParentRect, 2);
	}

}

void CTTStatic::OnEnable(BOOL bEnable)
{
	Invalidate();
}

HBRUSH CTTStatic::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if ((HFONT)m_TextFont == NULL)
		UpdateTextFont();

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


void CTTStatic::OnMove(int x, int y)
{
	CStatic::OnMove(x, y);

	Invalidate();

	CWnd *pWnd = GetParent();
	if (pWnd != NULL)
	{
		CRect oldWindowRect, curWindowRect;

		oldWindowRect.CopyRect(m_oldParentRect);
		GetWindowRect(curWindowRect);

		::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&curWindowRect, 2);

		InvalidateRectRegions(pWnd, oldWindowRect, curWindowRect, RGN_OR);
	}
}


void CTTStatic::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	// invalidate current client region
	Invalidate();

	// invalidate changed parent region
	CWnd *pWnd = GetParent();
	if (pWnd != NULL)
	{
		CRect oldWindowRect, curWindowRect;

		oldWindowRect.CopyRect(m_oldParentRect);
		GetWindowRect(curWindowRect);

		::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&curWindowRect, 2);

		InvalidateRectRegions(pWnd, oldWindowRect, curWindowRect, RGN_XOR);
	}
}
