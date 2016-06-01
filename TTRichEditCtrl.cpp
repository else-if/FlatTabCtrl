#include "stdafx.h"
#include "TTRichEditCtrl.h"
#include "CommonDrawing.h"

using namespace Gdiplus;

CTTRichEditCtrl::CTTRichEditCtrl() :
m_ClientRect(0, 0, 0, 0),
m_BorderRect(0, 0, 0, 0),
m_OffsetRect(0, 0, 0, 0),
m_oldParentRect(0, 0, 0, 0)
{
    m_bNcSizeIsCalculated = false;
    m_bUseBitmap = false;
    m_ControlState = Normal;

    SetDrawingProperties(1, 5);
}

CTTRichEditCtrl::~CTTRichEditCtrl()
{
}

void CTTRichEditCtrl::SetDrawingProperties(int borderPenWidth, int cornerRadius)
{
    m_borderPenWidth = borderPenWidth;
    m_CornerRadius = cornerRadius;
}

void CTTRichEditCtrl::UpdateControlState()
{
    ControlState oldCtrlStrate = m_ControlState;

    if (!IsWindowEnabled())
        m_ControlState = Disable;
    else
        m_ControlState = Normal;

    m_bUseBitmap = (m_bUseBitmap && oldCtrlStrate == m_ControlState);
}

void CTTRichEditCtrl::PreSubclassWindow()
{
    // remove default border styles to draw own

    HWND hWnd = GetSafeHwnd();

    LONG lStyle = GetWindowLong(hWnd, GWL_STYLE);
    LONG lExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

    LONG lBorderMask = WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU | WS_BORDER;
    LONG lExBorderMask = WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE;

    lStyle &= ~(lBorderMask);
    SetWindowLong(hWnd, GWL_STYLE, lStyle);

    lExStyle &= ~(lExBorderMask);
    SetWindowLong(hWnd, GWL_EXSTYLE, lExStyle);

    SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
    
    CRichEditCtrl::PreSubclassWindow();
}

BEGIN_MESSAGE_MAP(CTTRichEditCtrl, CRichEditCtrl)
    ON_WM_ERASEBKGND()
    ON_WM_NCCALCSIZE()
    ON_WM_NCPAINT()
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_WM_MOVE()
END_MESSAGE_MAP()

BOOL CTTRichEditCtrl::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}

void CTTRichEditCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
    CRichEditCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);

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

void CTTRichEditCtrl::OnNcPaint()
{
    if (!m_bNcSizeIsCalculated)
        return;

    CRect wndRect;
    GetWindowRect(wndRect);
    CRect cliRect;
    GetClientRect(cliRect);

    TRACE("wnd-cli [%d %d][%d %d] - [%d %d][%d %d]\n",
        wndRect.left, wndRect.top, wndRect.Width(), wndRect.Height(),
        cliRect.left, cliRect.top, cliRect.Width(), cliRect.Height());

    Default();

    CWindowDC dc(this);
    CDC* pDC = &dc;

    UpdateControlState();

    int nSave = pDC->SaveDC();

    // Get only non-client region
    CRgn nonClientRgn;
    CRect nonClientRect(m_BorderRect.X, m_BorderRect.Y, m_BorderRect.GetRight(), m_BorderRect.GetBottom());
    CreateRectRgnInDevicePoints(pDC, &nonClientRgn, nonClientRect);

    pDC->SelectClipRgn(&nonClientRgn);

    int nWid = ::GetSystemMetrics(SM_CXVSCROLL);
    LONG lStyle = GetStyle();

    if (lStyle & WS_HSCROLL)
    {
        // Exclude horizontal scrollbar region

        CRect cHScrRect(m_BorderRect.X + m_OffsetRect.left,
            m_BorderRect.Y + m_ClientRect.Height,
            m_BorderRect.GetRight() - m_OffsetRect.right,
            m_BorderRect.GetBottom() - m_OffsetRect.bottom);

        if (lStyle & WS_VSCROLL)
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

    if (lStyle & WS_VSCROLL)
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

        if (lStyle & WS_HSCROLL)
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

    m_bUseBitmap = true;
}

void CTTRichEditCtrl::OnPaint()
{
    CWnd *pWnd = GetParent();
    if (pWnd != NULL)
    {
        GetWindowRect(m_oldParentRect);
        ::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&m_oldParentRect, 2);
    }

    if (!m_bNcSizeIsCalculated)
    {
        // Non-client size should be calculated before painting. Force it.
        SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
        return;
    }

    CRichEditCtrl::OnPaint();    
}

void CTTRichEditCtrl::OnSize(UINT nType, int cx, int cy)
{
    CRichEditCtrl::OnSize(nType, cx, cy);

    if (!::IsWindow(GetSafeHwnd()))
        return;

    // NcSize should be recalculated
    //SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
    
    // Invalidate current client region
    Invalidate();

    // Invalidate changed parent region
    CWnd *pWnd = GetParent();
    if (pWnd != NULL)
    {
        CRect oldWindowRect, curWindowRect;

        GetWindowRect(curWindowRect);
        oldWindowRect.CopyRect(m_oldParentRect);

        ::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&curWindowRect, 2);

        InvalidateRectRegions(pWnd, oldWindowRect, curWindowRect, RGN_XOR);
    }
}

void CTTRichEditCtrl::OnMove(int x, int y)
{
    CRichEditCtrl::OnMove(x, y);

    if (!::IsWindow(GetSafeHwnd()))
        return;

    // NcSize should be recalculated
    //SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
    
    // Invalidate current client region
    Invalidate();

    // Invalidate changed parent regions
    CWnd *pWnd = GetParent();
    if (pWnd != NULL)
    {
        CRect oldWindowRect, curWindowRect;

        GetWindowRect(curWindowRect);
        oldWindowRect.CopyRect(m_oldParentRect);

        ::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&curWindowRect, 2);

        InvalidateRectRegions(pWnd, oldWindowRect, curWindowRect, RGN_XOR);
    }
}
