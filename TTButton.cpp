#pragma once

#include "stdafx.h"
#include "TTButton.h"
#include "CommonDrawing.h"


using namespace Gdiplus;

CTTButton::CTTButton()
{
	m_bTracking = false;
	
	SetDrawingProperties(1, 5);

	UpdateButtonState(0);

	m_ColorMap.SetDefaultColors();
	m_CaptionTextColor = GetSysColor(COLOR_BTNTEXT);

    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
    m_TextFont.CreateFontIndirect(&ncm.lfStatusFont);
}

CTTButton::~CTTButton()
{
}

void CTTButton::SetDrawingProperties(int borderPenWidth, int cornerRadius)
{
	m_BorderPenWidth = borderPenWidth;
	m_CornerRadius = cornerRadius;
}

void CTTButton::UpdateButtonState(UINT state)
{
    CWnd* focused = GetFocus();

    if (state & ODS_DISABLED)
		m_ButtonState = Disable;
    else if ((state & ODS_SELECTED) || 
        (IsCheckBox() && GetCheck()))
		m_ButtonState = Press;
	else if (m_bTracking)
		m_ButtonState = Mouseover;
    else if (state & ODS_FOCUS)
		m_ButtonState = Focus;
    else
		m_ButtonState = Normal;
}

BEGIN_MESSAGE_MAP(CTTButton, CTTBaseButton)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()    
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

void CTTButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC xdc;
	xdc.Attach(lpDrawItemStruct->hDC);

    CRect cRect(lpDrawItemStruct->rcItem);
	//GetClientRect(&cRect);

    CMemDC memDC(xdc, cRect);

	Graphics graphics(memDC.GetDC().GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    graphics.SetTextRenderingHint(TextRenderingHintSingleBitPerPixel);

	DrawThemeParentBackground(GetSafeHwnd(), memDC.GetDC().GetSafeHdc(), cRect);
    cRect.DeflateRect(1, 1);

	UpdateButtonState(lpDrawItemStruct->itemState);	
	
	Gdiplus::Rect BorderRect(cRect.left, cRect.top, cRect.Width(), cRect.Height());

	Gdiplus::Rect LightRect(cRect.left, cRect.top, cRect.Width(), cRect.Height());
	LightRect.Inflate(-1, -1);

	cRect.DeflateRect(1, 1, 1, 1);
	
	int lightPenWidth = (m_ButtonState == Press || m_ButtonState == Focus) ? 2 : 1;

	// Background
	Draw4ColorsGradientRect(cRect, memDC, 
		m_ColorMap.GetColor(m_ButtonState, BackgroundTopGradientStart), 
		m_ColorMap.GetColor(m_ButtonState, BackgroundTopGradientFinish), 
		m_ColorMap.GetColor(m_ButtonState, BackgroundBottomGradientStart), 
		m_ColorMap.GetColor(m_ButtonState, BackgroundBottomGradientFinish),
		m_CornerRadius);

	// Highlight area
	DrawRectArea(LightRect, graphics, m_ColorMap.GetColor(m_ButtonState, BorderLight),
		m_CornerRadius, lightPenWidth);
	
	// Border
    COLORREF borderColor = m_ColorMap.GetColor(m_ButtonState, Border);
    if (IsDefault() && (m_ButtonState == Normal || m_ButtonState == Focus))
        borderColor = m_ColorMap.GetColor(Mouseover, Border);

    DrawRectArea(BorderRect, graphics, borderColor, m_CornerRadius, m_BorderPenWidth);

	// Button text
	CString buttonText;
	GetWindowText(buttonText);
	COLORREF textColor = m_ButtonState == Disable ? GetSysColor(COLOR_GRAYTEXT) : m_CaptionTextColor;

    DrawText(cRect, memDC, m_TextFont, textColor, buttonText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

BOOL CTTButton::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}

BOOL CTTButton::PreTranslateMessage(MSG* pMsg)
{
	// treat double-click like single click
	if (pMsg->message == WM_LBUTTONDBLCLK) {
		pMsg->message = WM_LBUTTONDOWN;
	}
    
    
    if (IsCheckBox() && pMsg->message == WM_COMMAND)
    {
        WPARAM wmId = LOWORD(pMsg->wParam);
        LPARAM wmEvent = HIWORD(pMsg->wParam);
            
        if (wmEvent == BN_CLICKED)
        {
            m_bChecked = !m_bChecked;
        }
    }

    return CButton::PreTranslateMessage(pMsg);	
}

void CTTButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	CButton::OnLButtonUp(nFlags, point);
    Invalidate();
}

void CTTButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = GetSafeHwnd();
		m_bTracking = ::_TrackMouseEvent(&tme) ? true : false;
		Invalidate(false);
	}

	CButton::OnMouseMove(nFlags, point);
}

void CTTButton::OnMouseLeave()
{
    m_bTracking = false;
	Invalidate();

	CButton::OnMouseLeave();
}

void CTTButton::PreSubclassWindow()
{
    CTTBaseButton::PreSubclassWindow();
}

HBRUSH CTTButton::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    pDC->SetBkMode(TRANSPARENT);
    return (HBRUSH)GetStockObject(NULL_BRUSH);
    /*HBRUSH hbr = CButton::OnCtlColor(pDC, pWnd, nCtlColor);
    return hbr;*/
}
