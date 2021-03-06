﻿#include "stdafx.h"
#include "TTGroupBox.h"
#include "CommonDrawing.h"

CTTGroupBox::CTTGroupBox()
    :m_CaptionRect(0, 0, 0, 0)
{
    SetDrawingProperties(1, 5);

    m_CaptionTextColor = GetSysColor(COLOR_CAPTIONTEXT);

    m_ControlState = Normal;
    m_ColorMap.SetColor(Normal, Border, RGB(168, 167, 174));

    m_bUseBmp = false;
}

CTTGroupBox::~CTTGroupBox()
{
}

void CTTGroupBox::SetDrawingProperties(int borderPenWidth, int cornerRadius)
{
    m_BorderPenWidth = borderPenWidth;
    m_CornerRadius = cornerRadius;
}

void CTTGroupBox::UpdateControlState()
{
    if (!IsWindowEnabled())
        m_ControlState = Disable;
    else
        m_ControlState = Normal;
}

BEGIN_MESSAGE_MAP(CTTGroupBox, CStatic)
    ON_WM_PAINT()
    ON_WM_ENABLE()
    ON_WM_CTLCOLOR()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CTTGroupBox::OnPaint()
{
    CPaintDC dc(this);

    CRect cRect;
    GetWindowRect(&cRect);
    ScreenToClient(&cRect);

    UpdateControlState();

    // exclude modal frame borders
    if (GetExStyle() & WS_EX_DLGMODALFRAME)
    {
        cRect.left += OFS_MODALFRAME;
        cRect.top += OFS_MODALFRAME;
        cRect.right -= OFS_MODALFRAME + 1;
        cRect.bottom -= OFS_MODALFRAME + 1;
    }

    LOGFONT logFont;
    GetFont()->GetLogFont(&logFont);
    CFont font;
    font.CreateFontIndirect(&logFont);

    dc.SelectObject(font);

    CString captionText;
    GetWindowText(captionText);

    CSize captionSize(0, 0);
    int iTopOffset = 0;

    if (!captionText.IsEmpty())
    {
        captionText = _T(" ") + captionText + _T(" ");
        captionSize = dc.GetTextExtent(captionText);

        captionSize.cy += 2;
        iTopOffset = (captionSize.cy / 2);
    }
    else
    {
        CSize testSize = dc.GetTextExtent(_T("Test"));
        iTopOffset = (testSize.cy / 2);
    }

    // Calcilate caption rect position
    DWORD dwStyle = GetStyle();
    CPoint ptStart, ptEnd;

    ptStart.y = cRect.top;// + iTopOffset;
    ptEnd.y = cRect.top + captionSize.cy;

    int sizeDiff = max((cRect.Width() - captionSize.cx) / 2, 0); // to prevent diff below zero
    int Offset = min(sizeDiff, OFS_LEFT); // offset may be lower than predefined default OFS_LEFT

    if ((dwStyle & BS_CENTER) == BS_RIGHT) // right aligned
    {
        ptEnd.x = cRect.right - Offset;
        ptStart.x = ptEnd.x - captionSize.cx;
    }
    else if ((!(dwStyle & BS_CENTER)) || ((dwStyle & BS_CENTER) == BS_LEFT)) // left aligned	/ default
    {
        ptStart.x = cRect.left + Offset;
        ptEnd.x = ptStart.x + captionSize.cx;
    }
    else if ((dwStyle & BS_CENTER) == BS_CENTER) // text centered
    {
        ptStart.x = (cRect.Width() - captionSize.cx) / 2;
        ptEnd.x = ptStart.x + captionSize.cx;
    }

    m_CaptionRect.CopyRect(CRect(ptStart, ptEnd));
    CRgn captionRgn;
    captionRgn.CreateRectRgnIndirect(&m_CaptionRect);

    CRect bordRect(cRect.left, cRect.top + iTopOffset, cRect.right + 1, cRect.bottom + 1);
    CRgn borderRgn;
    borderRgn.CreateRectRgnIndirect(&bordRect);

    borderRgn.CombineRgn(&borderRgn, &captionRgn, RGN_DIFF);

    dc.SelectClipRgn(&borderRgn);

    //Borders
    CPen pen(PS_SOLID, m_BorderPenWidth, m_ColorMap.GetColor(m_ControlState, Border));
    CBrush* pOldBrush;
    CPen* pOldPen;
    pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
    pOldPen = dc.SelectObject(&pen);

    dc.RoundRect(&bordRect, CPoint(m_CornerRadius, m_CornerRadius));

    dc.SelectObject(pOldBrush);
    dc.SelectObject(pOldPen);

    // Caption

    dc.SelectClipRgn(&captionRgn);

    dc.SetBkMode(TRANSPARENT);
    (CBrush*)dc.SelectStockObject(NULL_BRUSH);

    COLORREF textColor = m_ControlState == Disable ? GetSysColor(COLOR_GRAYTEXT) : m_CaptionTextColor;
    dc.SetTextColor(textColor);

    dc.DrawText(captionText, m_CaptionRect, DT_VCENTER | DT_CENTER | DT_SINGLELINE | DT_NOCLIP);
}

void CTTGroupBox::OnEnable(BOOL bEnable)
{
    Invalidate();
}

void CTTGroupBox::PreSubclassWindow()
{
    ModifyStyle(0, BS_GROUPBOX);

    CStatic::PreSubclassWindow();
}

HBRUSH CTTGroupBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    pDC->SetBkMode(TRANSPARENT);
    pDC->SetTextColor(m_CaptionTextColor);

    HBRUSH hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
    return hbr;
}

BOOL CTTGroupBox::OnEraseBkgnd(CDC* pDC)
{
    if (m_bUseBmp)
    {
        if (!m_CaptionRect.IsRectEmpty())
        {
            int iTopOffset = GetExStyle() & WS_EX_DLGMODALFRAME ? OFS_MODALFRAME : 0;

            pDC->BitBlt(m_CaptionRect.left, m_CaptionRect.top, m_CaptionRect.Width(), m_CaptionRect.Height(),
                &m_dc, m_CaptionRect.left, m_CaptionRect.top + iTopOffset, SRCCOPY);
        }
    }
    else
    {
        CRect Rect;
        GetWindowRect(&Rect);
        CWnd *pParent = GetParent();
        pParent->ScreenToClient(&Rect);
        CDC *pParentDC = pParent->GetDC();

        m_dc.DeleteDC();
        // store into m_dc
        CBitmap bmp;
        m_dc.CreateCompatibleDC(pParentDC);
        bmp.CreateCompatibleBitmap(pParentDC, Rect.Width(), Rect.Height());
        m_dc.SelectObject(&bmp);
        m_dc.BitBlt(0, 0, Rect.Width(), Rect.Height(), pParentDC, Rect.left, Rect.top, SRCCOPY);
        bmp.DeleteObject();

        m_bUseBmp = true;

        pParent->ReleaseDC(pParentDC);
    }

    return TRUE;
}
