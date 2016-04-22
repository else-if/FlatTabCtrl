#pragma once

#include "stdafx.h"
#include "TTEdit.h"
#include "common.h"

using namespace Gdiplus;

IMPLEMENT_DYNAMIC(CTTEdit, CEdit)

CTTEdit::CTTEdit()
	: m_rectNCBottom(0, 0, 0, 0)
	, m_rectNCTop(0, 0, 0, 0)
{
	m_bTracking = false;
	m_ControlState = Normal;

	SetDrawingProperties(2, 10);

	m_ColorMap.SetDefaultColors();
}

CTTEdit::~CTTEdit()
{
}

BEGIN_MESSAGE_MAP(CTTEdit, CEdit)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_GETDLGCODE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

void CTTEdit::SetDrawingProperties(float borderPenWidth, int cornerRadius)
{
	m_fBorderPenWidth = borderPenWidth;
	m_CornerRadius = cornerRadius;
}

void CTTEdit::UpdateControlState()
{	
}

BOOL CTTEdit::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void CTTEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = GetSafeHwnd();
		m_bTracking = ::_TrackMouseEvent(&tme) ? true : false;
	}

	CEdit::OnMouseMove(nFlags, point);
}

void CTTEdit::OnMouseLeave()
{
	m_bTracking = false;
	Invalidate();

	CEdit::OnMouseLeave();
}

void CTTEdit::OnNcPaint()
{
	Default();

	CWindowDC dc(this);
	CBrush Brush(GetSysColor(COLOR_WINDOW));

	dc.FillRect(m_rectNCBottom, &Brush);
	dc.FillRect(m_rectNCTop, &Brush);
}

void CTTEdit::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	CRect rectWnd, rectClient;

	//calculate client area height needed for a font
	CFont *pFont = GetFont();
	CRect rectText;
	rectText.SetRectEmpty();

	CDC *pDC = GetDC();

	CFont *pOld = pDC->SelectObject(pFont);
	pDC->DrawText(_T("Ky"), rectText, DT_CALCRECT | DT_LEFT);
	UINT uiVClientHeight = rectText.Height();

	pDC->SelectObject(pOld);
	ReleaseDC(pDC);

	//calculate NC area to center text.

	GetClientRect(rectClient);
	GetWindowRect(rectWnd);

	ClientToScreen(rectClient);

	UINT uiCenterOffset = (rectClient.Height() - uiVClientHeight) / 2;
	UINT uiCY = (rectWnd.Height() - rectClient.Height()) / 2;
	UINT uiCX = (rectWnd.Width() - rectClient.Width()) / 2;

	rectWnd.OffsetRect(-rectWnd.left, -rectWnd.top);
	m_rectNCTop = rectWnd;

	m_rectNCTop.DeflateRect(uiCX, uiCY, uiCX, uiCenterOffset + uiVClientHeight + uiCY);

	m_rectNCBottom = rectWnd;

	m_rectNCBottom.DeflateRect(uiCX, uiCenterOffset + uiVClientHeight + uiCY, uiCX, uiCY);

	lpncsp->rgrc[0].top += uiCenterOffset;
	lpncsp->rgrc[0].bottom -= uiCenterOffset;

	lpncsp->rgrc[0].left += uiCX;
	lpncsp->rgrc[0].right -= uiCY;

	//CEdit::OnNcCalcSize(bCalcValidRects, lpncsp);
}

UINT CTTEdit::OnGetDlgCode()
{
	if (m_rectNCTop.IsRectEmpty())
	{
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
	}

	return CEdit::OnGetDlgCode();
}



HBRUSH CTTEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CEdit::OnCtlColor(pDC, pWnd, nCtlColor);

	hbr = CreateSolidBrush(RGB(255, 0, 0));
	
	return hbr;
}
