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

void CTTButton::DrawPushButton(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC xdc;
    xdc.Attach(lpDrawItemStruct->hDC);

    CRect cRect(lpDrawItemStruct->rcItem);

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
	DrawCaptionText(memDC, cRect);
}

void CTTButton::DrawCheckBox(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    UINT btnStyle = GetButtonStyle();

    // for now support only Pushlike checkboxes
    ASSERT(btnStyle & BS_PUSHLIKE);

    CDC xdc;
    xdc.Attach(lpDrawItemStruct->hDC);
    CRect cRect(lpDrawItemStruct->rcItem);
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
	DrawCaptionText(memDC, cRect);
}

void CTTButton::DrawCaptionText(CMemDC &memDC, CRect clientRect)
{
	CString buttonText;
	GetWindowText(buttonText);
	COLORREF textColor = m_ButtonState == Disable ? GetSysColor(COLOR_GRAYTEXT) : m_CaptionTextColor;

	LOGFONT logFont;
	GetFont()->GetLogFont(&logFont);
	CFont font;
	font.CreateFontIndirectW(&logFont);

	memDC.GetDC().SelectObject(font);

	CRect captionRect;
	captionRect.SetRectEmpty();
	memDC.GetDC().DrawText(buttonText, captionRect, DT_CALCRECT | DT_CENTER);

	int iHeight = captionRect.Height();
	int iWidth = captionRect.Width();

	captionRect.left = clientRect.left + (clientRect.Width() - captionRect.Width()) / 2;
	captionRect.right = captionRect.left + iWidth;
	captionRect.top = clientRect.top + (clientRect.Height() - captionRect.Height()) / 2;
	captionRect.bottom = captionRect.top + iHeight;

	CRgn rgn;
	rgn.CreateRectRgnIndirect(clientRect);
	memDC.GetDC().SelectClipRgn(&rgn);

	DrawText(captionRect, memDC, font, textColor, buttonText, DT_CENTER);
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
    UINT btnStyle = GetControlType();

    switch (btnStyle)
    {
    case BS_PUSHBUTTON:
    case BS_DEFPUSHBUTTON:
        DrawPushButton(lpDrawItemStruct);
        break;
    case BS_CHECKBOX:
    case BS_AUTOCHECKBOX:
        DrawCheckBox(lpDrawItemStruct);
        break;
    case BS_RADIOBUTTON:
    case BS_3STATE:
    case BS_AUTO3STATE:
    case BS_GROUPBOX:
    case BS_USERBUTTON:
    case BS_AUTORADIOBUTTON:
    case BS_PUSHBOX:
        // no support yet...
        break;
    default:
        break;
    }


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

    return CTTBaseButton::PreTranslateMessage(pMsg);
}

void CTTButton::OnLButtonUp(UINT nFlags, CPoint point)
{
    CTTBaseButton::OnLButtonUp(nFlags, point);
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

    CTTBaseButton::OnMouseMove(nFlags, point);
}

void CTTButton::OnMouseLeave()
{
    m_bTracking = false;
    Invalidate();

    CTTBaseButton::OnMouseLeave();
}

void CTTButton::PreSubclassWindow()
{
    CTTBaseButton::PreSubclassWindow();
}

HBRUSH CTTButton::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    pDC->SetBkMode(TRANSPARENT);
    return (HBRUSH)GetStockObject(NULL_BRUSH);
}
