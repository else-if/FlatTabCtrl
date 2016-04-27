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

void CTTEdit2::SetPosition(int x, int y)
{
	m_ClientRect.X = x;
	m_ClientRect.Y = y;
}

void CTTEdit2::UpdateControlState()
{
	ControlState oldCtrlStrate = m_ControlState;
	if (!IsWindowEnabled())
		m_ControlState = Disable;
	else if (m_bHover || GetFocus() == this)
		m_ControlState = Mouseover;
	else
		m_ControlState = Normal;

	m_bUseBitmap = (m_bUseBitmap && oldCtrlStrate == m_ControlState);	
}

void CTTEdit2::Paint(CDC* pDC)
{
	if (m_ClientRect.IsEmptyArea())
		return;

	UpdateControlState();

	if (m_bUseBitmap)
	{
		pDC->BitBlt(m_ClientRect.X,
			m_ClientRect.Y,
			m_ClientRect.Width,
			m_ClientRect.Height,
			&m_dc, 0, 0, SRCCOPY);
		
		return;
	}
	
	Gdiplus::Graphics graphics(pDC->m_hDC);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

	// Borders
	Gdiplus::Rect BorderRect(m_ClientRect.X, m_ClientRect.Y, m_ClientRect.X + m_ClientRect.Width, m_ClientRect.Y + m_ClientRect.Height);

	// Clear background
	COLORREF backgroundColor = m_ColorMap.GetColor(m_ControlState, BackgroundTopGradientFinish);;
	Color cl(0, 0, 0);
	cl.SetFromCOLORREF(backgroundColor);
	Gdiplus::SolidBrush brush(cl);
	graphics.FillRectangle(&brush, BorderRect);

	DrawRectArea(BorderRect, graphics, m_ColorMap.GetColor(m_ControlState, Border),
		m_CornerRadius, m_borderPenWidth);	

	m_dc.DeleteDC();

	int x = m_ClientRect.X;
	int y = m_ClientRect.Y;
	int width = m_ClientRect.Width;
	int height = m_ClientRect.Height;

	// store into m_dc
	CBitmap bmp;
	m_dc.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, width, height);
	m_dc.SelectObject(&bmp);
	m_dc.BitBlt(0, 0, width, height, pDC, x, y, SRCCOPY);
	bmp.DeleteObject();

	m_bUseBitmap = true;
}

BEGIN_MESSAGE_MAP(CTTEdit2, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()	
	ON_WM_SETCURSOR()
	ON_WM_MOUSELEAVE()
	ON_CONTROL_REFLECT(EN_UPDATE, &CTTEdit2::OnEnUpdate)
END_MESSAGE_MAP()

HBRUSH CTTEdit2::CtlColor(CDC* pDC, UINT nCtlColor)
{
	return NULL;
}

BOOL CTTEdit2::OnEraseBkgnd(CDC* pDC)
{
	if (m_ClientRect.IsEmptyArea())
		return TRUE;

	SetPosition(-m_borderPenWidth, m_OffsetY);
	Paint(pDC);
	
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

	CRect rc;
	GetWindowRect(rc);
	rc.OffsetRect(-rc.left, -rc.top);
	m_ClientRect.X = rc.left;
	m_ClientRect.Y = rc.top;
	m_ClientRect.Width = rc.Width();
	m_ClientRect.Height = rc.Height();

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
	SetPosition(0, 0);
	Paint(&dc);		
}

BOOL CTTEdit2::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	m_bHover = nHitTest == HTCLIENT;
	return CEdit::OnSetCursor(pWnd, nHitTest, message);
}

void CTTEdit2::OnMouseLeave()
{
	m_bHover = false;
	CEdit::OnMouseLeave();
}

void CTTEdit2::OnEnUpdate()
{
	Invalidate();
}
