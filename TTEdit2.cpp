#include "stdafx.h"
#include "TTEdit2.h"
#include "CommonDrawing.h"
#include "ControlsColorMap.h"

CTTEdit2::CTTEdit2()
    :m_ClientRect(0, 0, 0, 0),
    m_oldWndRect(0, 0, 0, 0)
{
    m_OffsetY = 0;
    uiCX = 0;
    uiCY = 0;

    SetDrawingProperties(1, 5);

    m_ControlState = Normal;

    m_bUseBitmap = false;
    m_bHover = false;
    m_bFocused = false;

    m_ColorMap.SetColor(Mouseover, BackgroundTopGradientFinish, RGB(255, 255, 255));

    m_bUseBaseMessageHandlers = false;
    m_bNcSizeIsCalculated = false;
    m_bPainted = false;

    m_bHScroll = false;
    m_bVScroll = false;

    oldCX = 0;
    oldCY = 0;
    oldX = 0;
    oldY = 0;
    m_bSized = false;
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
    m_BorderRect.X = x;
    m_BorderRect.Y = y;
    /*m_ClientRect.X = x;
    m_ClientRect.Y = y;*/
}

void CTTEdit2::UpdateControlState()
{
    ControlState oldCtrlStrate = m_ControlState;

    if (!IsWindowEnabled())
        m_ControlState = Disable;
    else if (GetStyle() & ES_READONLY)
        m_ControlState = ReadOnly;
    else if (m_bHover || m_bFocused)
        m_ControlState = Mouseover;
    else
        m_ControlState = Normal;

    m_bUseBitmap = (m_bUseBitmap && oldCtrlStrate == m_ControlState);
}

void CTTEdit2::Paint(CDC* pDC)
{
    TRACE("Paint Border [%d %d][%d %d]; Offset: [%d %d][%d %d]\n", m_BorderRect.X, m_BorderRect.Y, m_BorderRect.GetRight(), m_BorderRect.GetBottom(),
        m_OffsetRect.left, m_OffsetRect.top, m_OffsetRect.right, m_OffsetRect.bottom);

    UpdateControlState();

    int nSave = pDC->SaveDC();

    // Get only non-client region
    CRgn nonClientRgn;
    CRect nonClientRect(m_BorderRect.X, m_BorderRect.Y, m_BorderRect.GetRight(), m_BorderRect.GetBottom());
    CreateRectRgnInDevicePoints(pDC, &nonClientRgn, nonClientRect);

    pDC->SelectClipRgn(&nonClientRgn);

    // Exclude client area
    CRect clientRect;
    if (GetStyle() & ES_MULTILINE)
    {
        GetRect(clientRect);
        clientRect.left = m_ClientRect.X;
        clientRect.top = m_ClientRect.Y;
    }
    else
    {
        clientRect.SetRect(m_ClientRect.X, m_ClientRect.Y,
            m_ClientRect.GetRight(), m_ClientRect.GetBottom());
    }

    CRgn clientRgn;
    CreateRectRgnInDevicePoints(pDC, &clientRgn, clientRect);
    pDC->SelectClipRgn(&clientRgn, RGN_XOR);

    int nWid = ::GetSystemMetrics(SM_CXVSCROLL);

    if (m_bHScroll)
    {
        // Exclude horizontal scrollbar region

        CRect cHScrRect(m_BorderRect.X + m_OffsetRect.left,
            m_BorderRect.GetBottom() - m_OffsetRect.bottom - nWid,
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
            cVScrRect.SetRect(m_BorderRect.X + m_OffsetRect.left,
            m_BorderRect.Y + m_OffsetRect.top,
            m_BorderRect.X + m_OffsetRect.right + nWid,
            m_BorderRect.GetBottom() - m_OffsetRect.bottom);
        else
            cVScrRect.SetRect(m_BorderRect.GetRight() - m_OffsetRect.right - nWid,
            m_BorderRect.Y + m_OffsetRect.top,
            m_BorderRect.GetRight() - m_OffsetRect.right,
            m_BorderRect.GetBottom() - m_OffsetRect.bottom);

        if (m_bHScroll)
            cVScrRect.bottom -= nWid;

        CRgn cVScrRgn;
        CreateRectRgnInDevicePoints(pDC, &cVScrRgn, cVScrRect, m_CornerRadius);
        pDC->SelectClipRgn(&cVScrRgn, RGN_XOR);
    }

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

    CRect cBorderRect(m_BorderRect.X, m_BorderRect.Y, m_BorderRect.GetRight(), m_BorderRect.GetBottom());
    CRect cClientRect(m_BorderRect.X + m_borderPenWidth,
        m_BorderRect.Y + m_borderPenWidth,
        m_BorderRect.GetRight() - m_borderPenWidth,
        m_BorderRect.GetBottom() - m_borderPenWidth);
    
    if (m_bHScroll)
        cClientRect.bottom -= nWid - 1;

    if (m_bVScroll)
        if (GetExStyle() & WS_EX_LEFTSCROLLBAR)
            cClientRect.left += nWid - 1;
        else
            cClientRect.right -= nWid - 1;

    //pDC->FillSolidRect(cBorderRect, RGB(255, 0, 0));
    //pDC->FillSolidRect(cClientRect, RGB(0, 255, 0));
    DrawEditControlFrame(pDC, &cBorderRect, &cClientRect, NULL, m_ControlState, m_CornerRadius,
        m_borderPenWidth, &m_ColorMap);

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

    /*if (m_ClientRect.IsEmptyArea())
        return;

    UpdateControlState();

    if (m_bUseBitmap)
    {
        pDC->BitBlt(m_ClientRect.X,
            m_ClientRect.Y,
            m_ClientRect.Width,
            m_ClientRect.Height,
            &m_dc, 0, 0, SRCCOPY);

        return;
    }
    else
    {
        CRect cClientRect(m_ClientRect.X, m_ClientRect.Y, m_ClientRect.X + m_ClientRect.Width, m_ClientRect.Y + m_ClientRect.Height);

        DrawEditControlFrame(pDC, &cClientRect, &cClientRect, m_ControlState, m_CornerRadius,
            m_borderPenWidth, &m_ColorMap);

        int x = m_ClientRect.X;
        int y = m_ClientRect.Y;
        int width = m_ClientRect.Width;
        int height = m_ClientRect.Height;

        m_dc.DeleteDC();

        // store into m_dc
        CBitmap bmp;
        m_dc.CreateCompatibleDC(pDC);
        bmp.CreateCompatibleBitmap(pDC, width, height);
        m_dc.SelectObject(&bmp);
        m_dc.BitBlt(0, 0, width, height, pDC, x, y, SRCCOPY);
        bmp.DeleteObject();

        m_bPainted = true;
    }*/
}

void CTTEdit2::DrawEditControlFrame(CDC* pDC, CRect* pBorderRect, CRect* pClientRect, CRect* pClipRect, ControlState controlState,
    int cornerRadius, int borderWidth, ControlsColorMap* pColorMap)
{
    if (pDC == NULL)
        return;

    if (pBorderRect == NULL)
        return;

    int nSave = pDC->SaveDC();

    Graphics graphics(pDC->GetSafeHdc());
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

    bool bDeleteColorMap = false;
    if (pColorMap == NULL)
    {
        pColorMap = new ControlsColorMap();
        bDeleteColorMap = true;
    }

    if (pClipRect != NULL)
    {
        CRgn clipRgn;
        CreateRectRgnInDevicePoints(pDC, &clipRgn, *pClipRect);

        pDC->SelectClipRgn(&clipRgn);
    }

    // background
    pDC->FillSolidRect(pBorderRect, GetSysColor(COLOR_3DFACE));

    Gdiplus::Rect BorderRect(pBorderRect->left, pBorderRect->top, pBorderRect->Width(), pBorderRect->Height());
    Gdiplus::Rect ClientRect(pClientRect->left, pClientRect->top, pClientRect->Width(), pClientRect->Height());

    // Clear background
    COLORREF backgroundColor = pColorMap->GetColor(controlState, BackgroundTopGradientFinish);;
    Color cl(0, 0, 0);
    cl.SetFromCOLORREF(backgroundColor);
    Gdiplus::SolidBrush brush(cl);
    graphics.FillRectangle(&brush, ClientRect);

    DrawRectArea(BorderRect, graphics, pColorMap->GetColor(controlState, Border),
        cornerRadius, borderWidth);

    pDC->RestoreDC(nSave);

    if (bDeleteColorMap && pColorMap != NULL)
    {
        delete pColorMap;
        pColorMap = NULL;
    }

}

void CTTEdit2::UpdatePaintRects()
{
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

    if (!(GetStyle() & ES_MULTILINE))  // multiline edit box shouldn't change NC area
    {
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

        //calculate NC area to center text.

        if (!m_BorderRect.IsEmptyArea())
        {
            m_OffsetRect.top = max(m_BorderRect.Height - uiVClientHeight, m_OffsetRect.top) / 2;
            m_OffsetRect.bottom = m_BorderRect.Height - uiVClientHeight - m_OffsetRect.top - 1;            
        }
    }
}

BEGIN_MESSAGE_MAP(CTTEdit2, CEdit)
    ON_WM_ERASEBKGND()
    ON_WM_NCCALCSIZE()
    ON_WM_NCPAINT()
    ON_WM_SETCURSOR()
    ON_WM_MOUSELEAVE()
    ON_WM_PAINT()
    ON_WM_KILLFOCUS()
    ON_WM_SETFOCUS()
    ON_WM_CTLCOLOR()
    ON_WM_SIZE()
    ON_WM_MOVE()
END_MESSAGE_MAP()

BOOL CTTEdit2::OnEraseBkgnd(CDC* pDC)
{
    if (m_bUseBaseMessageHandlers)
        return CEdit::OnEraseBkgnd(pDC);
    
    /*SetPosition(-m_OffsetRect.left, -m_OffsetRect.top);
    Paint(pDC);*/

    return TRUE;
}

void CTTEdit2::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
    //CEdit::OnNcCalcSize(bCalcValidRects, lpncsp);

    m_bNcSizeIsCalculated = true;

    if (m_bUseBaseMessageHandlers)
        return;

    UpdatePaintRects();

    TRACE("NcCalcSize [%d %d] [%d %d]\n", m_BorderRect.X, m_BorderRect.Y, m_BorderRect.Width, m_BorderRect.Height);

    lpncsp->rgrc[0].top += m_OffsetRect.top;
    lpncsp->rgrc[0].bottom -= m_OffsetRect.bottom;

    lpncsp->rgrc[0].left += m_OffsetRect.left;
    lpncsp->rgrc[0].right -= m_OffsetRect.right;

    CRect cClient(lpncsp->rgrc[0]);
    cClient.OffsetRect(-lpncsp->rgrc[1].left, -lpncsp->rgrc[1].top);

    m_ClientRect.X = cClient.left;
    m_ClientRect.Y = cClient.top;
    m_ClientRect.Width = cClient.Width();
    m_ClientRect.Height = cClient.Height();

    m_bUseBitmap = false;

    /*if (m_bUseBaseMessageHandlers)
    {
        CEdit::OnNcCalcSize(bCalcValidRects, lpncsp);
        return;
    }

    m_bUseBitmap = false;

    CRect rc;
    GetWindowRect(rc);
    rc.OffsetRect(-rc.left, -rc.top);
    m_ClientRect.X = rc.left;
    m_ClientRect.Y = rc.top;
    m_ClientRect.Width = rc.Width();
    m_ClientRect.Height = rc.Height();

    UINT uiCenterOffset = 0;
    uiCY = 0;
    uiCX = 0;

    CRect rectWnd, rectClient;

    GetWindowRect(rectWnd);
    GetClientRect(rectClient);

    ClientToScreen(rectClient);

    rectClient.top += m_OffsetY;
    rectClient.bottom -= m_OffsetY;
    rectClient.left -= uiCX + m_borderPenWidth;
    rectClient.right += uiCY + m_borderPenWidth;

    if (!(GetStyle() & ES_MULTILINE))  // multiline edit box shouldn't change NC area
    {
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

        //calculate NC area to center text.

        if (!rectClient.IsRectEmpty())
        {
            uiCenterOffset = max(rectClient.Height() - uiVClientHeight, 0) / 2;
            uiCY = max(rectWnd.Height() - rectClient.Height(), 0) / 2;
            uiCX = max(rectWnd.Width() - rectClient.Width(), 0) / 2;
        }
    }
    else
    {
        uiCenterOffset = m_borderPenWidth;
    }

    m_OffsetY = -(int)uiCenterOffset;

    lpncsp->rgrc[0].top += uiCenterOffset;
    lpncsp->rgrc[0].bottom -= uiCenterOffset;

    lpncsp->rgrc[0].left += uiCX + m_borderPenWidth;
    lpncsp->rgrc[0].right -= uiCY + m_borderPenWidth;

    m_bNcSizeIsCalculated = true;*/
}

BOOL CTTEdit2::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
    if (!m_bNcSizeIsCalculated)
        SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
    
    return CEdit::OnChildNotify(message, wParam, lParam, pLResult);
}

void CTTEdit2::OnNcPaint()
{
    if (m_bUseBaseMessageHandlers)
    {
        CEdit::OnNcPaint();
        return;
    }

    Default();

    CWindowDC dc(this);

    SetPosition(0, 0);
    Paint(&dc);

    m_bUseBitmap = true;
}

BOOL CTTEdit2::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    m_bHover = (nHitTest == HTCLIENT);
    return CEdit::OnSetCursor(pWnd, nHitTest, message);
}

void CTTEdit2::OnMouseLeave()
{
    m_bHover = false;
    CEdit::OnMouseLeave();
}

void CTTEdit2::PreSubclassWindow()
{
    CRect rc;
    GetClientRect(rc);
    if (rc.IsRectEmpty())
    {
        // In case when window's client rect is empty (for example - edit box inside GridList control)
        // params needed for TTEdit can't be calculated correctly
        // so for now CEdit message handlers would be used
        m_bUseBaseMessageHandlers = true;
    }
    else
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
            lStyle &= ~(lBorderMask);
            SetWindowLong(hWnd, GWL_STYLE, lStyle);

            lExStyle &= ~(lExBorderMask);
            SetWindowLong(hWnd, GWL_EXSTYLE, lExStyle);

            SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
        }

        /*HWND hWnd = GetSafeHwnd();

        LONG lStyle = GetWindowLong(hWnd, GWL_STYLE);
        lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
        SetWindowLong(hWnd, GWL_STYLE, lStyle);

        LONG lExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
        lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
        SetWindowLong(hWnd, GWL_EXSTYLE, lExStyle);

        SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);*/
    }

    SetMargins(m_borderPenWidth, m_borderPenWidth);

    CEdit::PreSubclassWindow();
}

void CTTEdit2::OnPaint()
{
    if (m_bUseBaseMessageHandlers)
    {
        CEdit::OnPaint();
        return;
    }

    if (!m_bNcSizeIsCalculated)
    {
        // force NCCalcSize
        SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
        return;
    }

    CEdit::OnPaint(); 

    // save window rect where control was drawn
    GetWindowRect(m_oldWndRect);

    TRACE("OnPaint new wndRect: [%d %d] [%d %d]\n", m_oldWndRect.left, m_oldWndRect.top, m_oldWndRect.Width(), m_oldWndRect.Height());

    return;

    CRect clientRect(m_ClientRect.X - m_OffsetRect.left,
        m_ClientRect.Y - m_OffsetRect.top,
        m_ClientRect.GetRight() - 2 * m_OffsetRect.right,
        m_ClientRect.GetBottom() - m_OffsetRect.bottom);

    //CRect clientRect(-100, -100, 100, 100);

    DWORD dwStyle = GetStyle();
    DWORD dwExStyle = GetExStyle();
    BOOL  bLeftScroll = dwExStyle & WS_EX_LEFTSCROLLBAR;

    if ((dwStyle & WS_VSCROLL) && bLeftScroll)
        clientRect.left -= ::GetSystemMetrics(SM_CXVSCROLL);

    if (clientRect.Width() <= 0 || clientRect.Height() <= 0)
        return;

    CPaintDC dc(this);
    CMemDC memDC(dc, &clientRect);

    TRACE("OnPaint [%d %d] [%d %d]\n", clientRect.left, clientRect.top, clientRect.right, clientRect.bottom);

    UpdateControlState();
    COLORREF backgroundColor = m_ColorMap.GetColor(m_ControlState, BackgroundTopGradientFinish);
    memDC.GetDC().FillSolidRect(&clientRect, backgroundColor);
    //memDC.GetDC().FillSolidRect(&clientRect, RGB(255,0,0));

    DefWindowProc(WM_PAINT, (WPARAM)memDC.GetDC().m_hDC, (LPARAM)0);

    ReleaseDC(&memDC.GetDC());

    // save window rect where control was drawn
    GetWindowRect(m_oldWndRect);

    /*if (m_bUseBaseMessageHandlers)
    {
        CEdit::OnPaint();
        return;
    }

    if (!m_bNcSizeIsCalculated)
    {
        // still no NcCalcSize
        CRect cRect;
        GetClientRect(&cRect);
        CPaintDC dc(this);
        DrawThemeParentBackground(GetSafeHwnd(), dc.m_hDC, cRect);

        // force NCCalcSize
        SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

        return;
    }

    SetPosition(-m_borderPenWidth, m_OffsetY);

    int offset = 2;
    CRect clientRect(m_ClientRect.X + 1,
        m_ClientRect.Y + offset,
        m_ClientRect.GetRight() - 1,
        m_ClientRect.GetBottom() - offset);

    if (clientRect.Width() <= 0 || clientRect.Height() <= 0)
        return;

    //TRACE("OnPaint\n");

    CPaintDC dc(this);
    CMemDC memDC(dc, &clientRect);

    UpdateControlState();
    COLORREF backgroundColor = m_ColorMap.GetColor(m_ControlState, BackgroundTopGradientFinish);
    memDC.GetDC().FillSolidRect(&clientRect, backgroundColor);

    DefWindowProc(WM_PAINT, (WPARAM)memDC.GetDC().m_hDC, (LPARAM)0);

    ReleaseDC(&memDC.GetDC());

    //CEdit::OnPaint();*/
}

void CTTEdit2::OnKillFocus(CWnd* pNewWnd)
{
    m_bFocused = false;
    CEdit::OnKillFocus(pNewWnd);
}

void CTTEdit2::OnSetFocus(CWnd* pOldWnd)
{
    m_bFocused = true;
    CEdit::OnSetFocus(pOldWnd);
}

HBRUSH CTTEdit2::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CEdit::OnCtlColor(pDC, pWnd, nCtlColor);
    return hbr;

    /*if (m_bUseBaseMessageHandlers)
    {
        HBRUSH hbr = CEdit::OnCtlColor(pDC, pWnd, nCtlColor);
        return hbr;
    }
    else
    {
        return NULL;
    }*/
}

void CTTEdit2::OnSize(UINT nType, int cx, int cy)
{
    CEdit::OnSize(nType, cx, cy);

    if (!::IsWindow(GetSafeHwnd()))
        return;

    // NcSize should be recalculated
    m_bNcSizeIsCalculated = false;
    SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

    DWORD lStyle = GetStyle();

    bool ScrollBarsVisChanged = (m_bHScroll != (bool)(lStyle & WS_HSCROLL)) || (m_bVScroll != (bool)(lStyle & WS_VSCROLL));

    int iOffWidth = (cx - oldCX);
    int iOffHeight = (cy - oldCY);

    // Invalidate control's region

    if (m_bSized && (cx != oldCX || cy != oldCY) &&
        !ScrollBarsVisChanged)
    {
        //m_BorderRect.Width += iOffWidth;
        /*if (iOffHeight < 0)
            m_BorderRect.Height += iOffHeight;*/

        /*m_OffsetRect.top = max(m_BorderRect.Height - cy, m_borderPenWidth + 1) / 2;
        m_OffsetRect.bottom = m_BorderRect.Height - cy - m_OffsetRect.top - 1;*/

        TRACE("Size applied cx cy [%d %d] - [%d %d][%d %d]\n", iOffWidth, iOffHeight, m_BorderRect.X, m_BorderRect.Y, m_BorderRect.GetRight(), m_BorderRect.GetBottom());

        m_bUseBitmap = false;
        /*if (m_bPainted)
            this->SendMessage(WM_NCPAINT);*/

        Invalidate();
    }
    else
    {
        TRACE("Size skipped cx cy [%d %d]\n", iOffWidth, iOffHeight);
    }

    // Invalidate changed parent region

    CWnd *pWnd = GetParent();
    if (/*m_bPainted &&*/ pWnd != NULL)
    {
        CRect oldWindowRect, curWindowRect;

        /*oldWindowRect.CopyRect(m_oldWndRect);
        GetWindowRect(curWindowRect);*/

        GetWindowRect(curWindowRect);
        oldWindowRect.CopyRect(curWindowRect);

        //oldWindowRect.CopyRect(curWindowRect);

        oldWindowRect.top -= iOffHeight;
        oldWindowRect.left -= iOffWidth;
        oldWindowRect.right -= iOffWidth;
        oldWindowRect.bottom -= iOffHeight;

        /*if (m_bHScroll != (bool)(lStyle & WS_HSCROLL))
            oldWindowRect.bottom += ::GetSystemMetrics(SM_CXVSCROLL);

        if (m_bVScroll != (bool)(lStyle & WS_VSCROLL))
            oldWindowRect.right += ::GetSystemMetrics(SM_CXVSCROLL);*/

        //m_oldWndRect.CopyRect(curWindowRect);

        ::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&oldWindowRect, 2);
        ::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&curWindowRect, 2);

        InvalidateRectRegions(pWnd, oldWindowRect, curWindowRect, RGN_XOR);
    }

    m_bHScroll = (lStyle & WS_HSCROLL);
    m_bVScroll = (lStyle & WS_VSCROLL);

    oldCX = cx;
    oldCY = cy;

    m_bSized = true;
}

void CTTEdit2::OnMove(int x, int y)
{
    CEdit::OnMove(x, y);

    if (m_bUseBaseMessageHandlers)
        return;

    if (!::IsWindow(GetSafeHwnd()))
        return;

    // NcSize should be recalculated
    m_bNcSizeIsCalculated = false;
    SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

    int iOffWidth = (x - oldX);
    int iOffHeight = (y - oldY);

    TRACE("OnMove x y [%d %d] - [%d %d][%d %d]\n", iOffWidth, iOffHeight, m_BorderRect.X, m_BorderRect.Y, m_BorderRect.GetRight(), m_BorderRect.GetBottom());

    CWnd *pWnd = GetParent();
    if (m_bPainted && pWnd != NULL)
    {
        // Invalidate changed parent regions

        CRect oldWindowRect, curWindowRect;

        GetWindowRect(curWindowRect);
        oldWindowRect.CopyRect(curWindowRect);
        

        oldWindowRect.top -= iOffHeight;
        oldWindowRect.left -= iOffWidth;
        oldWindowRect.right -= iOffWidth;
        oldWindowRect.bottom -= iOffHeight;

        //m_oldWndRect.CopyRect(curWindowRect);

        ::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&oldWindowRect, 2);
        ::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&curWindowRect, 2);

        InvalidateRectRegions(pWnd, oldWindowRect, curWindowRect, RGN_XOR);
    }

    // invalidate new control's region

    /*if (m_bPainted)
        this->SendMessage(WM_NCPAINT);*/

    Invalidate();

    oldX = x;
    oldY = y;

    /*if (!m_bUseBaseMessageHandlers && m_bPainted)
    {
        GetParent()->InvalidateRect(&m_oldWndRect);

        m_bUseBitmap = false;
        OnNcPaint();
        Invalidate();
    }

    

    m_oldWndRect.left = x - m_borderPenWidth;
    m_oldWndRect.top = y + m_OffsetY;
    m_oldWndRect.right = m_oldWndRect.left + m_ClientRect.Width;
    m_oldWndRect.bottom = m_oldWndRect.top + m_ClientRect.Height;*/
}