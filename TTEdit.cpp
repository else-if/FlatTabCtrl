#include "stdafx.h"
#include "TTEdit.h"
#include "CommonDrawing.h"
#include "ControlsColorMap.h"

//IMPLEMENT_DYNAMIC(CTTEdit, CEdit);

CTTEdit::CTTEdit()
    :m_ClientRect(0, 0, 0, 0)
{
    m_OffsetY = -1;
    SetDrawingProperties(1, 5);

    m_ControlState = Normal;

    m_bUseBitmap = false;
    m_bHover = false;

    m_ColorMap.SetColor(Mouseover, BackgroundTopGradientFinish, RGB(255, 255, 255));
}

CTTEdit::~CTTEdit()
{
}

void CTTEdit::SetDrawingProperties(int borderPenWidth, int cornerRadius)
{
    m_borderPenWidth = borderPenWidth;
    m_CornerRadius = cornerRadius;
}

void CTTEdit::SetPosition(int x, int y)
{
    m_ClientRect.X = x;
    m_ClientRect.Y = y;
}

void CTTEdit::UpdateControlState()
{
    ControlState oldCtrlStrate = m_ControlState;
    if (!IsWindowEnabled())
        m_ControlState = Disable;
    else if (m_bHover || (GetFocus() == this))
        m_ControlState = Mouseover;
    else
        m_ControlState = Normal;

    m_bStateChanged = oldCtrlStrate != m_ControlState;
    m_bUseBitmap = (m_bUseBitmap && oldCtrlStrate == m_ControlState);
}

void CTTEdit::Paint(CDC* pDC)
{
    if (m_ClientRect.IsEmptyArea())
        return;

    UpdateControlState();

    if (m_bUseBitmap && !m_bStateChanged)
        return;

    /*CString str;
    DWORD curSel = GetSel();
    GetWindowText(str);*/

    if (m_bUseBitmap)
    {
        Trace(_T("paint bmp"));

        pDC->BitBlt(m_ClientRect.X,
            m_ClientRect.Y,
            m_ClientRect.Width,
            m_ClientRect.Height,
            &m_dc, 0, 0, SRCCOPY);

        /*SetWindowText(str);
        SetSel(curSel);*/

        return;
    }

    Trace(_T("paint dc"));

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

    /*SetWindowText(str);
    SetSel(curSel);*/

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

BEGIN_MESSAGE_MAP(CTTEdit, CEdit)
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_ERASEBKGND()
    ON_WM_NCCALCSIZE()
    ON_WM_NCPAINT()
    ON_WM_SETCURSOR()
    ON_WM_MOUSELEAVE()
    ON_CONTROL_REFLECT(EN_UPDATE, &CTTEdit::OnEnUpdate)
    ON_WM_PAINT()
    ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

HBRUSH CTTEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
    return NULL;
}

BOOL CTTEdit::OnEraseBkgnd(CDC* pDC)
{
    if (m_ClientRect.IsEmptyArea())
        return TRUE;

    Trace(_T("Erase"));

    SetPosition(-m_borderPenWidth, m_OffsetY);
    Paint(pDC);

    return TRUE;
}

void CTTEdit::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
    m_bUseBitmap = false;

    if (m_OffsetY != -1)
    {
        Trace(_T("ReCalc"));
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

    CRect rc;
    GetWindowRect(rc);
    rc.OffsetRect(-rc.left, -rc.top);
    m_ClientRect.X = rc.left;
    m_ClientRect.Y = rc.top;
    m_ClientRect.Width = rc.Width();
    m_ClientRect.Height = rc.Height();

    //calculate NC area to center text.

    GetClientRect(rectClient);
    if (rectClient.IsRectEmpty())
    {
        GetWindowRect(rectClient);
        rectClient.OffsetRect(-rectClient.left, -rectClient.top);
    }
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

    CString cStr;
    cStr.Format(_T("Calc: [%d %d] [%d %d]"), lpncsp->rgrc[0].left, lpncsp->rgrc[0].top, lpncsp->rgrc[0].right, lpncsp->rgrc[0].bottom);
    Trace(cStr);
    //SetRedraw(TRUE);
    Invalidate();
}

BOOL CTTEdit::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
    if (m_OffsetY == -1)
    {
        Trace(_T("ChildNotify"));
        SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
    }

    return CEdit::OnChildNotify(message, wParam, lParam, pLResult);
}

void CTTEdit::OnNcPaint()
{
    if (m_OffsetY == -1)
    {
        Trace(_T("Skipped NcPaint"));
        return;
    }

    Trace(_T("NcPaint"));

    Default();

    CWindowDC dc(this);
    SetPosition(0, 0);
    Paint(&dc);
}

BOOL CTTEdit::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    //Trace(_T("SetCursor"));
    m_bHover = (nHitTest == HTCLIENT);
    return CEdit::OnSetCursor(pWnd, nHitTest, message);
}

void CTTEdit::OnMouseLeave()
{
    Trace(_T("OnMouseLeave"));
    m_bHover = false;
    CEdit::OnMouseLeave();
}

void CTTEdit::OnEnUpdate()
{
    Trace(_T("Update"));
    Invalidate();
}

void CTTEdit::PreSubclassWindow()
{
    Trace(_T("PreSubclass"));

    HWND hWnd = GetSafeHwnd();

    LONG lStyle = GetWindowLong(hWnd, GWL_STYLE);
    lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
    SetWindowLong(hWnd, GWL_STYLE, lStyle);

    LONG lExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
    lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
    SetWindowLong(hWnd, GWL_EXSTYLE, lExStyle);

    SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

    CEdit::PreSubclassWindow();
}

void CTTEdit::OnPaint()
{

    if (m_OffsetY == -1)
    {
        Trace(_T("Skipped OnPaint"));

        // still no NC_CalcSize

        CRect cRect;
        GetClientRect(&cRect);
        CPaintDC dc(this);
        DrawThemeParentBackground(GetSafeHwnd(), dc.m_hDC, cRect);

        // 
        SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

        return;
    }
    /*else if (!m_dc)
    {
    OnNcPaint();
    }*/
    /*if (!m_dc)
    {
    Trace(_T("OnPaint !m_dc"));
    //OnNcPaint();
    //SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);

    if (m_ClientRect.IsEmptyArea())
    {
    CRect rc;
    GetWindowRect(rc);
    rc.OffsetRect(-rc.left, -rc.top);
    m_ClientRect.X = rc.left;
    m_ClientRect.Y = rc.top;
    m_ClientRect.Width = rc.Width();
    m_ClientRect.Height = rc.Height();
    }
    OnNcPaint();

    //return;
    }*/

    Trace(_T("OnPaint"));
    CEdit::OnPaint();
}

void CTTEdit::Trace(CString cMsg)
{
    CString cStr;
    cStr.Format(_T(" id: %d\n"), GetDlgCtrlID());
    TRACE(cMsg + cStr);
}

void CTTEdit::OnKillFocus(CWnd* pNewWnd)
{
    CEdit::OnKillFocus(pNewWnd);
    Invalidate();
}
