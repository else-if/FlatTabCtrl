#include "stdafx.h"
#include "TTButton.h"
#include "CommonDrawing.h"


using namespace Gdiplus;

CTTButton::CTTButton() :
	m_oldParentRect(0, 0, 0, 0)
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

void CTTButton::DrawButtonControl(CDC* pDC, CRect* pRect, CRect* pClipRect, int nStyle, ControlState buttonState,
    CString buttonText, CFont* pFont, int cornerRadius, int borderWidth, ControlsColorMap* pColorMap, COLORREF captionTextColor,
    bool bIsDefault)
{
    if (pDC == NULL)
        return;

    if (pRect == NULL || pClipRect == NULL)
        return;

    switch (nStyle)
    {
    case BS_PUSHBUTTON:
    case BS_DEFPUSHBUTTON:
        DrawPushButton(pDC, pRect, pClipRect, buttonState, buttonText, pFont, cornerRadius,
            borderWidth, pColorMap, captionTextColor, bIsDefault);
        break;
    case BS_CHECKBOX:
        // no support yet...
        break;
    case BS_AUTOCHECKBOX:
        DrawCheckBox(pDC, pRect, pClipRect, buttonState, buttonText, pFont, cornerRadius,
            borderWidth, pColorMap, captionTextColor);
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

void CTTButton::DrawPushButton(CDC* pDC, CRect* pRect, CRect* pClipRect, ControlState buttonState, CString buttonText,
    CFont* pFont, int cornerRadius, int borderWidth, ControlsColorMap* pColorMap, COLORREF captionTextColor, bool bIsDefault)
{
    if (pDC == NULL)
        return;

    if (pRect == NULL || pClipRect == NULL)
        return;

    int nSave = pDC->SaveDC();

    Graphics graphics(pDC->GetSafeHdc());
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    graphics.SetTextRenderingHint(TextRenderingHintSingleBitPerPixel);

    bool bDeleteColorMap = false;
    if (pColorMap == NULL)
    {
        pColorMap = new ControlsColorMap();
        bDeleteColorMap = true;
    }

    CRgn clipRgn;
    CreateRectRgnInDevicePoints(pDC, &clipRgn, *pClipRect);

    pDC->SelectClipRgn(&clipRgn);

    CRect cRect(pRect);
    cRect.DeflateRect(1, 1);

    Gdiplus::Rect BorderRect(cRect.left, cRect.top, cRect.Width(), cRect.Height());

    Gdiplus::Rect LightRect(cRect.left, cRect.top, cRect.Width(), cRect.Height());
    LightRect.Inflate(-1, -1);

    cRect.DeflateRect(1, 1, 1, 1);

    int lightPenWidth = (buttonState == Press || buttonState == Focus) ? 2 : 1;

    // Background
    Draw4ColorsGradientRect(cRect, *pDC,
        pColorMap->GetColor(buttonState, BackgroundTopGradientStart),
        pColorMap->GetColor(buttonState, BackgroundTopGradientFinish),
        pColorMap->GetColor(buttonState, BackgroundBottomGradientStart),
        pColorMap->GetColor(buttonState, BackgroundBottomGradientFinish),
        cornerRadius);

    // Highlight area
    DrawRectArea(LightRect, graphics, pColorMap->GetColor(buttonState, BorderLight),
        cornerRadius, lightPenWidth);

    // Border
    COLORREF borderColor = pColorMap->GetColor(buttonState, Border);
    if (bIsDefault && (buttonState == Normal || buttonState == Focus))
        borderColor = pColorMap->GetColor(Mouseover, Border);

    DrawRectArea(BorderRect, graphics, borderColor, cornerRadius, borderWidth);

    pDC->RestoreDC(nSave);

    // Button text
    COLORREF textColor = buttonState == Disable ? GetSysColor(COLOR_GRAYTEXT) : captionTextColor;
    DrawCaptionText(pDC, &cRect, pClipRect, buttonText, -1, pFont, textColor);

    if (bDeleteColorMap && pColorMap != NULL)
    {
        delete pColorMap;
        pColorMap = NULL;
    }
}

void CTTButton::DrawCheckBox(CDC* pDC, CRect* pRect, CRect* pClipRect, ControlState buttonState, CString buttonText,
    CFont* pFont, int cornerRadius, int borderWidth, ControlsColorMap* pColorMap, COLORREF captionTextColor)
{
    if (pDC == NULL)
        return;

    if (pRect == NULL || pClipRect == NULL)
        return;

    int nSave = pDC->SaveDC();

    Graphics graphics(pDC->GetSafeHdc());
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    graphics.SetTextRenderingHint(TextRenderingHintSingleBitPerPixel);

    bool bDeleteColorMap = false;
    if (pColorMap == NULL)
    {
        pColorMap = new ControlsColorMap();
        bDeleteColorMap = true;
    }

    CRgn clipRgn;
    CreateRectRgnInDevicePoints(pDC, &clipRgn, *pClipRect);

    pDC->SelectClipRgn(&clipRgn);

    CRect cRect(pRect);
    cRect.DeflateRect(1, 1);

    Gdiplus::Rect BorderRect(cRect.left, cRect.top, cRect.Width(), cRect.Height());

    Gdiplus::Rect LightRect(cRect.left, cRect.top, cRect.Width(), cRect.Height());
    LightRect.Inflate(-1, -1);

    cRect.DeflateRect(1, 1, 1, 1);

    int lightPenWidth = (buttonState == Press || buttonState == Focus) ? 2 : 1;

    // Background
    Draw4ColorsGradientRect(cRect, *pDC,
        pColorMap->GetColor(buttonState, BackgroundTopGradientStart),
        pColorMap->GetColor(buttonState, BackgroundTopGradientFinish),
        pColorMap->GetColor(buttonState, BackgroundBottomGradientStart),
        pColorMap->GetColor(buttonState, BackgroundBottomGradientFinish),
        cornerRadius);

    // Highlight area
    DrawRectArea(LightRect, graphics, pColorMap->GetColor(buttonState, BorderLight),
        cornerRadius, lightPenWidth);

    // Border
    COLORREF borderColor = pColorMap->GetColor(buttonState, Border);

    DrawRectArea(BorderRect, graphics, borderColor, cornerRadius, borderWidth);

    pDC->RestoreDC(nSave);

    // Button text
    COLORREF textColor = buttonState == Disable ? GetSysColor(COLOR_GRAYTEXT) : captionTextColor;
    DrawCaptionText(pDC, &cRect, pClipRect, buttonText, -1, pFont, textColor);

    if (bDeleteColorMap && pColorMap != NULL)
    {
        delete pColorMap;
        pColorMap = NULL;
    }

}

void CTTButton::DrawCaptionText(CDC* pDC, CRect* pRect, CRect* pClipRect, CString buttonText, int nFormat,
    CFont* pFont, COLORREF textColor)
{
    if (pDC == NULL)
        return;

    if (pRect == NULL || pClipRect == NULL)
        return;

    if (buttonText.IsEmpty())
        return;

    int nSave = pDC->SaveDC();

    CRgn clipRgn;
    CreateRectRgnInDevicePoints(pDC, &clipRgn, *pClipRect);

    pDC->SelectClipRgn(&clipRgn);

    if (pFont != NULL)
        pDC->SelectObject(pFont);

    CRect captionRect(pRect);
    captionRect.DeflateRect(1, 1);

    if (nFormat == -1)
        nFormat = DT_WORDBREAK | DT_CENTER | DT_EDITCONTROL;

    pDC->DrawText(buttonText, buttonText.GetLength(), captionRect, nFormat | DT_CALCRECT);

    int iHeight = captionRect.Height();
    int iWidth = captionRect.Width();

    captionRect.left = pRect->left + (pRect->Width() - captionRect.Width()) / 2;
    captionRect.right = captionRect.left + iWidth;
    captionRect.top = pRect->top + (pRect->Height() - captionRect.Height()) / 2;
    captionRect.bottom = captionRect.top + iHeight;

    CRgn rgn;
    CreateRectRgnInDevicePoints(pDC, &rgn, *pRect);
    rgn.CombineRgn(&rgn, &clipRgn, RGN_AND);

    pDC->SelectClipRgn(&rgn);

    CFont* font = pDC->GetCurrentFont();

    nFormat &= ~DT_CALCRECT;

    DrawText(captionRect, *pDC, *font, textColor, buttonText, nFormat);

    pDC->RestoreDC(nSave);

}

BEGIN_MESSAGE_MAP(CTTButton, CTTBaseButton)
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSELEAVE()
    ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CTTButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC xdc;
    xdc.Attach(lpDrawItemStruct->hDC);

    CRect cRect(lpDrawItemStruct->rcItem);

    if (cRect.Width() <= 0 || cRect.Height() <= 0)
        return;

    CMemDC memDC(xdc, cRect);

    CString buttonText;
    GetWindowText(buttonText);

    UINT btnStyle = GetControlType();

    UpdateButtonState(lpDrawItemStruct->itemState);

    LOGFONT logFont;
    GetFont()->GetLogFont(&logFont);
    CFont font;
    font.CreateFontIndirectW(&logFont);

    DrawThemeParentBackground(GetSafeHwnd(), memDC.GetDC().GetSafeHdc(), cRect);

    DrawButtonControl(&memDC.GetDC(), &cRect, &cRect, btnStyle, m_ButtonState, buttonText, &font,
        m_CornerRadius, m_BorderPenWidth, &m_ColorMap, m_CaptionTextColor, IsDefault());

	CWnd *pWnd = GetParent();
	if (pWnd != NULL)
	{
		GetWindowRect(m_oldParentRect);
		::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&m_oldParentRect, 2);
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


void CTTButton::OnMove(int x, int y)
{
	CTTBaseButton::OnMove(x, y);
	
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


void CTTButton::OnSize(UINT nType, int cx, int cy)
{
	CTTBaseButton::OnSize(nType, cx, cy);

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
