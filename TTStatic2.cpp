#include "stdafx.h"
#include "TTStatic2.h"
#include "CommonDrawing.h"


// CTransparentStatic

IMPLEMENT_DYNAMIC(CTTStatic2, CStatic)

CTTStatic2::CTTStatic2()
{
}

CTTStatic2::~CTTStatic2()
{
}

void CTTStatic2::UpdateControlState()
{
    if (!IsWindowEnabled())
        m_ControlState = Disable;
    else
        m_ControlState = Normal;
}

COLORREF CTTStatic2::GetBackgroundColor()
{
    return m_backgroundColor;
}

void CTTStatic2::SetBackgroundColor(COLORREF backgroundColor)
{
    m_backgroundColor = backgroundColor;
}

COLORREF CTTStatic2::GetTextColor()
{
    return m_textColor;
}

void CTTStatic2::SetTextColor(COLORREF textColor)
{
    m_textColor = textColor;
}

void CTTStatic2::SetDrawingProperties(int borderPenWidth, int cornerRadius, bool drawBorders)
{
    m_borderPenWidth = borderPenWidth;
    m_iCornerRadius = cornerRadius;
    m_bDrawBorders = drawBorders;
}

COLORREF CTTStatic2::GetBorderColor()
{
    return m_borderColor;
}

void CTTStatic2::SetBorderColor(COLORREF borderColor)
{
    m_borderColor = borderColor;
}

void CTTStatic2::DrawBorders(bool drawBorders)
{
    m_bDrawBorders = drawBorders;
}

void CTTStatic2::SetColorProperties(COLORREF backgroundColor, COLORREF textColor, COLORREF borderColor)
{
    SetBackgroundColor(backgroundColor);
    SetTextColor(textColor);
    SetBorderColor(borderColor);
}

BEGIN_MESSAGE_MAP(CTTStatic2, CStatic)
    ON_MESSAGE(WM_SETTEXT, OnSetText)
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_PAINT()
END_MESSAGE_MAP()

LRESULT CTTStatic2::OnSetText(WPARAM wParam, LPARAM lParam)
{
    LRESULT Result = Default();
    CRect Rect;
    GetWindowRect(&Rect);
    GetParent()->ScreenToClient(&Rect);
    GetParent()->InvalidateRect(&Rect);
    GetParent()->UpdateWindow();
    return Result;
}

HBRUSH CTTStatic2::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
    pDC->SetBkMode(TRANSPARENT);
    return (HBRUSH)GetStockObject(NULL_BRUSH);
}
 
void CTTStatic2::OnPaint()
{
    CPaintDC dc(this);

    dc.SaveDC();

    dc.SetTextColor(m_textColor);
    //dc.SetBkColor(m_backgroundColor);
    
    dc.SetBkMode(TRANSPARENT);
    //dc.SelectObject(m_pBrush);

    CRect rect;
    GetClientRect(rect);

    LOGFONT logFont;
    GetFont()->GetLogFont(&logFont);
    CFont font;
    font.CreateFontIndirectW(&logFont);
    dc.SelectObject(&font);

    // get static's text
    CString strText = _T("");
    GetWindowText(strText);

    UINT nFormat = 0;
    DWORD dwStyle = GetStyle();

    // set DrawText format from static style settings
    if (dwStyle & SS_CENTER)
        nFormat |= DT_CENTER;
    else if (dwStyle & SS_LEFT)
        nFormat |= DT_LEFT;
    else if (dwStyle & SS_RIGHT)
        nFormat |= DT_RIGHT;

    if (dwStyle & SS_CENTERIMAGE)	// vertical centering ==> single line only
        nFormat |= DT_VCENTER | DT_SINGLELINE;
    else
        nFormat |= DT_WORDBREAK;

    rect.left += m_borderPenWidth;
    rect.top += m_borderPenWidth;
    dc.DrawText(strText, rect, nFormat);

    dc.RestoreDC(-1);

}
