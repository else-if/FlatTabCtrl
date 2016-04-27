#pragma once

#include "stdafx.h"
#include "TTButton.h"
#include "CommonDrawing.h"


using namespace Gdiplus;

CTTButton::CTTButton()
{
	m_bTracking = false;
	
	SetDrawingProperties(1, 5);

	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
	m_TextFont.CreateFontIndirect(&ncm.lfStatusFont);
	
	UpdateButtonState(0);

	m_ColorMap.SetDefaultColors();
	m_CaptionTextColor = GetSysColor(COLOR_BTNTEXT);
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
    else if ((state & ODS_SELECTED) || (state & ODS_CHECKED))
		m_ButtonState = Press;
	else if (m_bTracking)
		m_ButtonState = Mouseover;
    else if ((state & ODS_FOCUS) || 
        (m_bIsDefault && m_bIsDefault))
		m_ButtonState = Focus;
    else
		m_ButtonState = Normal;
}

BEGIN_MESSAGE_MAP(CTTButton, CButton)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()	
	ON_WM_UPDATEUISTATE()
    ON_WM_TIMER()
    ON_WM_NCPAINT()
    ON_UPDATE_COMMAND_UI(AFX_ID_PREVIEW_CLOSE, &CTTButton::OnUpdateAfxIdPreviewClose)
    ON_COMMAND(AFX_ID_PREVIEW_CLOSE, &CTTButton::OnAfxIdPreviewClose)
END_MESSAGE_MAP()

void CTTButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC xdc;
	xdc.Attach(lpDrawItemStruct->hDC);

	CRect cRect;
	GetClientRect(&cRect);

	CMemDC memDC(xdc, cRect);

	Graphics graphics(memDC.GetDC().GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	DrawThemeParentBackground(GetSafeHwnd(), memDC.GetDC().GetSafeHdc(), cRect);
		
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
	DrawRectArea(BorderRect, graphics, m_ColorMap.GetColor(m_ButtonState, Border),
		m_CornerRadius, m_BorderPenWidth);

	// Button text
	CString buttonText;
	GetWindowText(buttonText);
	COLORREF textColor = m_ButtonState == Disable ? GetSysColor(COLOR_GRAYTEXT) : m_CaptionTextColor;

	DrawText(cRect, memDC, m_TextFont, textColor, buttonText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

BOOL CTTButton::OnEraseBkgnd(CDC* pDC)
{
    TRACE(_T("btn ERase\n"));
	return FALSE;
}

BOOL CTTButton::PreTranslateMessage(MSG* pMsg)
{
	// treat double-click like single click
	if (pMsg->message == WM_LBUTTONDBLCLK) {
		pMsg->message = WM_LBUTTONDOWN;
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
	Invalidate(false);

	CButton::OnMouseLeave();
}

void CTTButton::PreSubclassWindow()
{
    DWORD style = GetStyle();
    m_bIsDefault = ((style & BS_TYPEMASK) == BS_DEFPUSHBUTTON);
    if (m_bIsDefault)
        SetTimer(1, 2000, NULL);

    ModifyStyle(0, BS_OWNERDRAW, SWP_FRAMECHANGED);

    CButton::PreSubclassWindow();
}

void CTTButton::OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/)
{
    // This feature requires Windows 2000 or greater.
    // The symbols _WIN32_WINNT and WINVER must be >= 0x0500.
    // TODO: Add your message handler code here
    TRACE(_T("upd UI state\n"));
}


void CTTButton::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    if (m_bIsDefault)
    {
        TRACE(_T("on Timer\n"));
        CWnd* focused = GetFocus();
    }

    CButton::OnTimer(nIDEvent);
}


void CTTButton::OnNcPaint()
{
    // TODO: Add your message handler code here
    // Do not call CButton::OnNcPaint() for painting messages
    TRACE(_T("nc paint\n"));
}


void CTTButton::OnUpdateAfxIdPreviewClose(CCmdUI *pCmdUI)
{
    // TODO: Add your command update UI handler code here
    TRACE(_T("Update afx Id preview\n"));
}


void CTTButton::OnAfxIdPreviewClose()
{
    // TODO: Add your command handler code here
    TRACE(_T("Command\n"));
}
