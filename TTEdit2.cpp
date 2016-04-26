#pragma once

#include "stdafx.h"
#include "TTEdit2.h"
#include "CommonDrawing.h"
#include "ControlsColorMap.h"

IMPLEMENT_DYNAMIC(CTTEdit2, CEdit);

CTTEdit2::CTTEdit2()
	:m_ClientRect(0, 0, 0, 0)
{
	m_OffsetY = -1;
	SetDrawingProperties(1, 5);

	m_ControlState = Normal;

	m_ColorMap.SetDefaultColors();
	m_ColorMap.SetColor(Mouseover, BackgroundTopGradientFinish, RGB(255, 255, 255));
}

CTTEdit2::~CTTEdit2()
{
}

void CTTEdit2::SetDrawingProperties(int borderPenWidth, int cornerRadius)
{
	m_borderPenWidth = borderPenWidth;
	m_CornerRadius = cornerRadius;
}

void CTTEdit2::UpdateControlState()
{
	if (!IsWindowEnabled())
		m_ControlState = Disable;
	else if (m_bHover || GetFocus() == this)
		m_ControlState = Mouseover;
	else
		m_ControlState = Normal;
}

void CTTEdit2::Paint(CDC* pDC)
{
	if (m_ClientRect.IsRectEmpty())
		return;

	UpdateControlState();

	CString str;
	GetWindowText(str);
	DWORD curSel = GetSel();

	//CMemDC memDC(*pDC, m_ClientRect);

	Gdiplus::Graphics graphics(pDC->GetSafeHdc());
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	CRgn frgn;
	frgn.CreateRectRgn(m_ClientRect.left, m_ClientRect.top, m_ClientRect.right, m_ClientRect.bottom);
	Gdiplus::Region grgn(frgn);
	graphics.SetClip(&grgn, CombineModeReplace);

	DrawThemeParentBackground(GetSafeHwnd(), pDC->GetSafeHdc(), m_ClientRect);

	// Borders
	Gdiplus::Rect BorderRect(m_ClientRect.left, m_ClientRect.top, m_ClientRect.right, m_ClientRect.bottom);

	// Clear background
	COLORREF backgroundColor = m_ColorMap.GetColor(m_ControlState, BackgroundTopGradientFinish);;
	Color cl(0, 0, 0);
	cl.SetFromCOLORREF(backgroundColor);
	Gdiplus::SolidBrush brush(cl);
	graphics.FillRectangle(&brush, BorderRect);

	DrawRectArea(BorderRect, graphics, m_ColorMap.GetColor(m_ControlState, Border),
		m_CornerRadius, m_borderPenWidth);

	SetWindowText(str);
	SetSel(curSel);
}

BEGIN_MESSAGE_MAP(CTTEdit2, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_KILLFOCUS()
	ON_WM_SETCURSOR()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

HBRUSH CTTEdit2::CtlColor(CDC* pDC, UINT nCtlColor)
{
	return NULL;
}

BOOL CTTEdit2::OnEraseBkgnd(CDC* pDC)
{
	if (m_ClientRect.IsRectEmpty())
		return TRUE;

	m_ClientRect.DeflateRect(-m_borderPenWidth, m_OffsetY, -m_borderPenWidth, 0);
	Paint(pDC);
	m_ClientRect.DeflateRect(m_borderPenWidth, -m_OffsetY, m_borderPenWidth, 0);

	return TRUE;
}

void CTTEdit2::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	if (m_OffsetY != -1)
	{
		lpncsp->rgrc[0].top += -m_OffsetY;
		lpncsp->rgrc[0].bottom -= -m_OffsetY;

		lpncsp->rgrc[0].left += uiCX + m_borderPenWidth;
		lpncsp->rgrc[0].right -= uiCY + m_borderPenWidth;

		return;
	}

	CRect rectWnd, rectClient;

	//calculate client area height needed for a font
	CFont *pFont = GetFont();
	CRect rectText;
	rectText.SetRectEmpty();

	CDC *pDC = GetDC();

	CFont *pOld = pDC->SelectObject(pFont);
	pDC->DrawText(_T("Eb"), rectText, DT_CALCRECT | DT_LEFT);
	UINT uiVClientHeight = rectText.Height();

	pDC->SelectObject(pOld);
	ReleaseDC(pDC);

	GetWindowRect(m_ClientRect);
	m_ClientRect.OffsetRect(-m_ClientRect.left, -m_ClientRect.top);

	//calculate NC area to center text.

	GetClientRect(rectClient);
	GetWindowRect(rectWnd);

	ClientToScreen(rectClient);

	UINT uiCenterOffset = (rectClient.Height() - uiVClientHeight) / 2;
	UINT uiCY = (rectWnd.Height() - rectClient.Height()) / 2;
	UINT uiCX = (rectWnd.Width() - rectClient.Width()) / 2;

	m_OffsetY = -(int)uiCenterOffset;

	lpncsp->rgrc[0].top += uiCenterOffset;
	lpncsp->rgrc[0].bottom -= uiCenterOffset;

	lpncsp->rgrc[0].left += uiCX + m_borderPenWidth;
	lpncsp->rgrc[0].right -= uiCY + m_borderPenWidth;

}

BOOL CTTEdit2::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
	if (m_OffsetY == -1)
	{
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
	}

	return CEdit::OnChildNotify(message, wParam, lParam, pLResult);
}

void CTTEdit2::OnNcPaint()
{
	Default();

	CWindowDC dc(this);
	Paint(&dc);
}

void CTTEdit2::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	Invalidate();
}

BOOL CTTEdit2::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest == HTCLIENT)
	{
		m_bHover = true;
	}
	else
	{
		m_bHover = false;
	}

	return CEdit::OnSetCursor(pWnd, nHitTest, message);
}

void CTTEdit2::OnMouseLeave()
{
	m_bHover = false;
	Invalidate();

	CEdit::OnMouseLeave();
}
