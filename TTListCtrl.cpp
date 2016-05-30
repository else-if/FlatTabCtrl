#include "stdafx.h"
#include "TTListCtrl.h"
#include "CommonDrawing.h"

using namespace Gdiplus;

CTTListCtrl::CTTListCtrl() :
m_BorderRect(0, 0, 0, 0),
m_ClientRect(0, 0, 0, 0),
m_OffsetRect(0, 0, 0, 0),
m_oldWndRect(0, 0, 0, 0)
{
    m_bNcSizeIsCalculated = false;
    m_bUseBitmap = false;
    m_bPainted = false;

    SetDrawingProperties(1, 5);
    m_ControlState = Normal;

    oldX = 0;
    oldY = 0;
    m_bHScroll = false;
    m_bVScroll = false;

    m_bDrawBorders = false;

    m_Sized = false;
}

CTTListCtrl::~CTTListCtrl()
{
}

void CTTListCtrl::UpdateControlState()
{
    ControlState oldCtrlStrate = m_ControlState;

    if (!IsWindowEnabled())
        m_ControlState = Disable;
    else
        m_ControlState = Normal;

    m_bUseBitmap = (m_bUseBitmap && oldCtrlStrate == m_ControlState);
}

void CTTListCtrl::SetDrawingProperties(int borderPenWidth, int cornerRadius)
{
    m_borderPenWidth = borderPenWidth;
    m_CornerRadius = cornerRadius;
}

void CTTListCtrl::SetPosition(int x, int y)
{
    m_BorderRect.X = x;
    m_BorderRect.Y = y;
}

void CTTListCtrl::PreSubclassWindow()
{
    HWND hWnd = GetSafeHwnd();

    LONG lStyle = GetWindowLong(hWnd, GWL_STYLE);
    LONG lExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

    m_bHScroll = lStyle & WS_HSCROLL;
    m_bVScroll = lStyle & WS_VSCROLL;

    LONG lBorderMask = WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU | WS_BORDER;
    LONG lExBorderMask = WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE;

    if ((lStyle & lBorderMask) || (lExStyle & lExBorderMask))
    {
        m_bDrawBorders = true;

        lStyle &= ~(lBorderMask);
        SetWindowLong(hWnd, GWL_STYLE, lStyle);

        lExStyle &= ~(lExBorderMask);
        SetWindowLong(hWnd, GWL_EXSTYLE, lExStyle);

        SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
    }

    GetWindowRect(m_oldWndRect);

    CListCtrl::PreSubclassWindow();
}

void CTTListCtrl::Paint(CDC* pDC)
{
    UpdateControlState();

    int nSave = pDC->SaveDC();

    // Get only non-client region
    CRgn nonClientRgn;
    CRect nonClientRect(m_BorderRect.X, m_BorderRect.Y, m_BorderRect.GetRight(), m_BorderRect.GetBottom());
    CreateRectRgnInDevicePoints(pDC, &nonClientRgn, nonClientRect);

    pDC->SelectClipRgn(&nonClientRgn);

    int nWid = ::GetSystemMetrics(SM_CXVSCROLL);

    if (m_bHScroll)
    {
        // Exclude horizontal scrollbar region

        CRect cHScrRect(m_BorderRect.X + m_OffsetRect.left,
            m_BorderRect.Y + m_ClientRect.Height,
            m_BorderRect.GetRight() - m_OffsetRect.right,
            m_BorderRect.GetBottom() - m_OffsetRect.bottom);

        if (m_bVScroll)
        {
            if (GetExStyle() & WS_EX_LEFTSCROLLBAR)
                cHScrRect.left = nWid;
            else
                cHScrRect.right -= nWid;
        }

        CRgn cHScrRgn;
        CreateRectRgnInDevicePoints(pDC, &cHScrRgn, cHScrRect, m_CornerRadius);
        pDC->SelectClipRgn(&cHScrRgn, RGN_XOR);
    }

    if (m_bVScroll)
    {
        // Exclude vertical scrollbar region
        CRect cVScrRect;

        if (GetExStyle() & WS_EX_LEFTSCROLLBAR)
            cVScrRect.SetRect(m_BorderRect.X + m_OffsetRect.left,//m_ClientRect.Width,
            m_BorderRect.Y + m_OffsetRect.top,
            m_BorderRect.GetRight() - m_ClientRect.Width,//m_OffsetRect.right,
            m_BorderRect.GetBottom() - m_OffsetRect.bottom);
        else
            cVScrRect.SetRect(m_BorderRect.X + m_ClientRect.Width,
            m_BorderRect.Y + m_OffsetRect.top,
            m_BorderRect.GetRight() - m_OffsetRect.right,
            m_BorderRect.GetBottom() - m_OffsetRect.bottom);

        if (m_bHScroll)
            cVScrRect.bottom -= nWid;

        CRgn cVScrRgn;
        CreateRectRgnInDevicePoints(pDC, &cVScrRgn, cVScrRect, m_CornerRadius);
        pDC->SelectClipRgn(&cVScrRgn, RGN_XOR);
    }

    // Exclude client area
    CRect clientRect(m_ClientRect.X + m_OffsetRect.left,
        m_ClientRect.Y + m_OffsetRect.top,
        m_ClientRect.GetRight() - m_OffsetRect.right,
        m_ClientRect.GetBottom() - m_OffsetRect.bottom);

    CRgn clientRgn;
    CreateRectRgnInDevicePoints(pDC, &clientRgn, clientRect);
    pDC->SelectClipRgn(&clientRgn, RGN_XOR);

    if (m_bUseBitmap)
    {
        // if it already drawn - just copy existing bitmap

        pDC->BitBlt(m_BorderRect.X,
            m_BorderRect.Y,
            m_BorderRect.Width,
            m_BorderRect.Height,
            &m_dc, 0, 0, SRCCOPY);

        pDC->RestoreDC(nSave);

        return;
    }

    // background
    pDC->FillSolidRect(&nonClientRect, GetSysColor(COLOR_3DFACE));

    Graphics graphics(pDC->GetSafeHdc());
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

    // client area parts
    COLORREF backgroundColor = m_ColorMap.GetColor(m_ControlState, BackgroundTopGradientFinish);
    Color cl(0, 0, 0);
    cl.SetFromCOLORREF(backgroundColor);
    Gdiplus::SolidBrush brush(cl);
    graphics.FillRectangle(&brush, m_ClientRect);

    // border
    DrawRectArea(m_BorderRect, graphics, m_ColorMap.GetColor(m_ControlState, Border),
        m_CornerRadius, m_borderPenWidth);

    pDC->RestoreDC(nSave);

    // store into m_dc
    int x = m_BorderRect.X;
    int y = m_BorderRect.Y;
    int width = m_BorderRect.Width;
    int height = m_BorderRect.Height;

    m_dc.DeleteDC();

    CBitmap bmp;
    m_dc.CreateCompatibleDC(pDC);
    bmp.CreateCompatibleBitmap(pDC, width, height);
    m_dc.SelectObject(&bmp);
    m_dc.BitBlt(0, 0, width, height, pDC, x, y, SRCCOPY);
    bmp.DeleteObject();

    m_bPainted = true;
}

BEGIN_MESSAGE_MAP(CTTListCtrl, CListCtrl)
    ON_WM_NCCALCSIZE()
    ON_WM_NCPAINT()
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
    ON_WM_PAINT()
    ON_WM_MOVE()
    ON_WM_SIZE()
    ON_WM_ENABLE()
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

void CTTListCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
    CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);

    m_bNcSizeIsCalculated = true;

    CRect cClient(lpncsp->rgrc[0]);
    cClient.OffsetRect(-lpncsp->rgrc[1].left, -lpncsp->rgrc[1].top);

    m_ClientRect.X = cClient.left;
    m_ClientRect.Y = cClient.top;
    m_ClientRect.Width = cClient.Width();
    m_ClientRect.Height = cClient.Height();

    CRect cBorder;
    GetWindowRect(cBorder);
    cBorder.OffsetRect(-cBorder.left, -cBorder.top);

    m_BorderRect.X = cBorder.left;
    m_BorderRect.Y = cBorder.top;
    m_BorderRect.Width = cBorder.Width();
    m_BorderRect.Height = cBorder.Height();

    if (!m_bDrawBorders)
        return;

    m_OffsetRect.CopyRect(CRect(m_borderPenWidth + 1, m_borderPenWidth + 1, m_borderPenWidth + 1, m_borderPenWidth + 1));

    HWND hwnd = GetSafeHwnd();
    DWORD dwStyle = ::GetWindowLong(hwnd, GWL_STYLE);
    DWORD dwExStyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);
    BOOL  bLeftScroll = dwExStyle & WS_EX_LEFTSCROLLBAR;

    if (dwStyle & WS_VSCROLL)
    {
        if (bLeftScroll)
            m_OffsetRect.left--;
        else
            m_OffsetRect.right--;
    }

    if (dwStyle & WS_HSCROLL)
        m_OffsetRect.bottom--;

    lpncsp->rgrc[0].top += m_OffsetRect.top;
    lpncsp->rgrc[0].bottom -= m_OffsetRect.bottom;

    lpncsp->rgrc[0].left += m_OffsetRect.left;
    lpncsp->rgrc[0].right -= m_OffsetRect.right;

    m_bUseBitmap = false;
}

BOOL CTTListCtrl::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
    if (!m_bNcSizeIsCalculated)
        SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

    return CListCtrl::OnChildNotify(message, wParam, lParam, pLResult);
}

void CTTListCtrl::OnNcPaint()
{
    if (!m_bNcSizeIsCalculated)
        return;

    if (!m_bDrawBorders)
    {
        CListCtrl::OnNcPaint();
        return;
    }

    Default();

    CWindowDC dc(this);

    SetPosition(0, 0);
    Paint(&dc);

    m_bUseBitmap = true;
}

BOOL CTTListCtrl::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}

void CTTListCtrl::OnPaint()
{
    if (!m_bNcSizeIsCalculated)
    {
        // force NCCalcSize
        SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
        return;
    }

    CRect headerRect(0, 0, 0, 0);
    CWnd *pHeader = GetHeaderCtrl();

    if (pHeader)
        pHeader->GetClientRect(&headerRect);

    CRect clientRect(m_ClientRect.X,
        m_ClientRect.Y + headerRect.Height(),
        m_ClientRect.GetRight() - m_OffsetRect.right,
        m_ClientRect.GetBottom() - m_OffsetRect.bottom);

    DWORD dwStyle = GetStyle();
    DWORD dwExStyle = GetExStyle();
    BOOL  bLeftScroll = dwExStyle & WS_EX_LEFTSCROLLBAR;

    if ((dwStyle & WS_VSCROLL) && bLeftScroll)
        clientRect.left -= ::GetSystemMetrics(SM_CXVSCROLL);

    if (clientRect.Width() <= 0 || clientRect.Height() <= 0)
        return;

    CPaintDC dc(this);
    CMemDC memDC(dc, &clientRect);

    UpdateControlState();
    COLORREF backgroundColor = m_ColorMap.GetColor(m_ControlState, BackgroundTopGradientFinish);
    memDC.GetDC().FillSolidRect(&clientRect, backgroundColor);

    DefWindowProc(WM_PAINT, (WPARAM)memDC.GetDC().m_hDC, (LPARAM)0);

    ReleaseDC(&memDC.GetDC());

    // when control has no borders non client area should be repainted to draw sroll bars
    if (!m_bDrawBorders)
        this->SendMessage(WM_NCPAINT);
}

void CTTListCtrl::OnMove(int x, int y)
{
    CListCtrl::OnMove(x, y);

    if (m_bPainted)
    {
        if (m_bDrawBorders)
        {
            m_bUseBitmap = false;
            this->SendMessage(WM_NCPAINT);
        }
    }

    CWnd *pWnd = GetParent();
    if (pWnd != NULL)
    {
        CRect oldWindowRect, curWindowRect;

        oldWindowRect.CopyRect(m_oldWndRect);
        GetWindowRect(curWindowRect);

        m_oldWndRect.CopyRect(curWindowRect);

        ::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&oldWindowRect, 2);
        ::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&curWindowRect, 2);

        InvalidateRectRegions(pWnd, oldWindowRect, curWindowRect, RGN_OR);
    }
}

void CTTListCtrl::OnSize(UINT nType, int cx, int cy)
{
    DWORD lStyle = GetStyle();

    bool ScrollBarsVisChanged = (m_bHScroll != (bool)(lStyle & WS_HSCROLL)) || (m_bVScroll != (bool)(lStyle & WS_VSCROLL));

    int iOffWidth = (cx - oldX);
    int iOffHeight = (cy - oldY);

    if (m_Sized && (cx != oldX || cy != oldY) &&
        !ScrollBarsVisChanged)
    {
        m_BorderRect.Width += iOffWidth;
        m_BorderRect.Height += iOffHeight;

        m_bUseBitmap = false;
        if (m_bPainted)
            this->SendMessage(WM_NCPAINT);

        Invalidate();
    }

    CListCtrl::OnSize(nType, cx, cy);

    // invalidate changed parent region
    CWnd *pWnd = GetParent();
    if (pWnd != NULL)
    {
        CRect oldWindowRect, curWindowRect;

        oldWindowRect.CopyRect(m_oldWndRect);
        GetWindowRect(curWindowRect);

        oldWindowRect.CopyRect(curWindowRect);
        oldWindowRect.right -= iOffWidth;
        oldWindowRect.bottom -= iOffHeight;

        if (m_bHScroll != (bool)(lStyle & WS_HSCROLL))
            oldWindowRect.bottom += ::GetSystemMetrics(SM_CXVSCROLL);

        if (m_bVScroll != (bool)(lStyle & WS_VSCROLL))
            oldWindowRect.right += ::GetSystemMetrics(SM_CXVSCROLL);

        m_oldWndRect.CopyRect(curWindowRect);

        ::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&oldWindowRect, 2);
        ::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&curWindowRect, 2);

        InvalidateRectRegions(pWnd, oldWindowRect, curWindowRect, RGN_XOR);
    }

    m_bHScroll = (lStyle & WS_HSCROLL);
    m_bVScroll = (lStyle & WS_VSCROLL);

    oldX = cx;
    oldY = cy;

    m_Sized = true;
}

void CTTListCtrl::OnEnable(BOOL bEnable)
{
    CListCtrl::OnEnable(bEnable);

    if (m_bPainted)
        SendMessage(WM_NCPAINT);
    Invalidate();
}


void CTTListCtrl::OnSetFocus(CWnd* pOldWnd)
{
    CListCtrl::OnSetFocus(pOldWnd);

    if (m_bPainted)
        SendMessage(WM_NCPAINT);
    Invalidate();
}

void CTTListCtrl::OnKillFocus(CWnd* pNewWnd)
{
    CListCtrl::OnKillFocus(pNewWnd);

    if (m_bPainted)
        SendMessage(WM_NCPAINT);
    Invalidate();
}
