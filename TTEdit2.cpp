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

void CTTEdit2::SetPosition(int x, int y)
{
	int xoff = x - m_ClientRect.X;
	int yoff = y - m_ClientRect.Y;

	// set the frame origin
	m_ClientRect.X = x; m_ClientRect.Y = y;
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
	if (m_ClientRect.IsEmptyArea())
		return;

	UpdateControlState();

	// Borders
	Gdiplus::Rect BorderRect(m_ClientRect.X, m_ClientRect.Y, m_ClientRect.X + m_ClientRect.Width, m_ClientRect.Y + m_ClientRect.Height);

	//TRACE("Erace\n");
	if (m_bUseBitmap)
	{/*
		CBitmap*	m_pOld;
		CBitmap		m_bitmap;
		
		// create a compatible bitmap
		m_bitmap.CreateCompatibleBitmap(pDC, m_ClientRect.Width, m_ClientRect.Height);

		// select bitmap into dc
		m_pOld = pDC->SelectObject(&m_bitmap);

		// set window origin
		//pDC->SetWindowOrg(m_ClientRect.X, m_ClientRect.Y);

		// needed for transparency support
		pDC->BitBlt(m_ClientRect.X, m_ClientRect.Y, m_ClientRect.Width, m_ClientRect.Height,
			pDC, m_ClientRect.X, m_ClientRect.Y, SRCCOPY);
		*/
		pDC->BitBlt(m_ClientRect.X,
			m_ClientRect.Y,
			m_ClientRect.Width,
			m_ClientRect.Height,
			&m_dc, 0, 0, SRCCOPY);
		/*
		pDC->BitBlt(m_ClientRect.X, m_ClientRect.Y, m_ClientRect.Width, m_ClientRect.Height,
			pDC, m_ClientRect.X, m_ClientRect.Y, SRCCOPY);

		// Swap back the original bitmap.
		pDC->SelectObject(m_pOld);*/
		return;
	}
	
	Gdiplus::Graphics graphics(pDC->m_hDC);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

	CRgn frgn;
	frgn.CreateRectRgn(m_ClientRect.X, m_ClientRect.Y, m_ClientRect.X + m_ClientRect.Width, m_ClientRect.Y + m_ClientRect.Height);
	Gdiplus::Region grgn(frgn);
	graphics.SetClip(&grgn, CombineModeReplace);

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
	ON_WM_KILLFOCUS()
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

	TRACE("Erace\n");

	//m_ClientRect.DeflateRect(-m_borderPenWidth, m_OffsetY, -m_borderPenWidth, 0);
	SetPosition(-m_borderPenWidth, m_OffsetY);
	Paint(pDC);
	//m_ClientRect.DeflateRect(m_borderPenWidth, -m_OffsetY, m_borderPenWidth, 0);

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

		//m_bUseBitmap = false;
		Invalidate();
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

	CRect rc; GetWindowRect(rc); rc.OffsetRect(-rc.left, -rc.top);
	m_ClientRect.X = rc.left;
	m_ClientRect.Y = rc.top;
	m_ClientRect.Width = rc.Width();
	m_ClientRect.Height = rc.Height();
	/*GetWindowRect(m_ClientRect);
	m_ClientRect.OffsetRect(-m_ClientRect.left, -m_ClientRect.top);*/

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
	TRACE("NC Paint\n");
	
	if (m_bUseBitmap)
		return;

	/*CString str;
	GetWindowText(str);
	DWORD curSel = GetSel();*/
	
	Default();
	
	//m_bUseBitmap = false;

	CWindowDC dc(this);
	SetPosition(0, 0);
	Paint(&dc);	
	
	/*SetWindowText(str);
	SetSel(curSel);*/
}

void CTTEdit2::OnKillFocus(CWnd* pNewWnd)
{
	TRACE("Kill focus\n");
	CEdit::OnKillFocus(pNewWnd);
	//Invalidate();
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
	TRACE("Mouse leave\n");
	m_bHover = false;
	//Invalidate();

	CEdit::OnMouseLeave();
}

void CTTEdit2::OnEnUpdate()
{
	TRACE(_T("Update\n"));
	Invalidate();
}
