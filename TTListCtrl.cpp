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
    
    m_bHScroll = lStyle & WS_HSCROLL;
    m_bVScroll = lStyle & WS_VSCROLL;

    lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU | WS_BORDER);
    SetWindowLong(hWnd, GWL_STYLE, lStyle);

    LONG lExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
    lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
    SetWindowLong(hWnd, GWL_EXSTYLE, lExStyle);

    SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

    CListCtrl::PreSubclassWindow();
}

void CTTListCtrl::Paint(CDC* pDC)
{
    UpdateControlState();

    TRACE("PAINT: border [%d %d] [%d %d]\n", m_BorderRect.X, m_BorderRect.X, m_BorderRect.Width, m_BorderRect.Height);

    int nSave = pDC->SaveDC();

    CRgn nonClientRgn;
    CRect nonClientRect(m_BorderRect.X, m_BorderRect.Y, m_BorderRect.GetRight(), m_BorderRect.GetBottom());
    CreateRectRgnInDevicePoints(pDC, &nonClientRgn, nonClientRect);

    pDC->SelectClipRgn(&nonClientRgn);

    int nWid = ::GetSystemMetrics(SM_CXVSCROLL);

    if (m_bHScroll)
    {
        CRect cHScrRect(m_BorderRect.X + m_OffsetRect.left,
            m_BorderRect.Y + m_ClientRect.Height,
            m_BorderRect.GetRight() - m_OffsetRect.right,
            m_BorderRect.GetBottom() - m_OffsetRect.bottom);

        if (m_bVScroll)
        {
            if (GetExStyle() & WS_EX_LEFTSCROLLBAR)
                cHScrRect.left += nWid;
            else
                cHScrRect.right -= nWid;
        }

        CRgn cHScrRgn;
        CreateRectRgnInDevicePoints(pDC, &cHScrRgn, cHScrRect, m_CornerRadius);
        pDC->SelectClipRgn(&cHScrRgn, RGN_XOR);
    }

    if (m_bVScroll)
    {
        CRect cVScrRect(m_BorderRect.X + m_ClientRect.Width,
            m_BorderRect.Y + m_OffsetRect.top,
            m_BorderRect.GetRight() - m_OffsetRect.right,
            m_BorderRect.GetBottom() - m_OffsetRect.bottom);

        if (m_bHScroll)
            cVScrRect.bottom -= nWid;

        CRgn cVScrRgn;
        CreateRectRgnInDevicePoints(pDC, &cVScrRgn, cVScrRect, m_CornerRadius);
        pDC->SelectClipRgn(&cVScrRgn, RGN_XOR);
    }

    CRect clientRect(m_ClientRect.X + m_OffsetRect.left,
        m_ClientRect.Y + m_OffsetRect.top,
        m_ClientRect.GetRight() - m_OffsetRect.right,
        m_ClientRect.GetBottom() - m_OffsetRect.bottom);

    CRgn clientRgn;
    CreateRectRgnInDevicePoints(pDC, &clientRgn, clientRect);
    pDC->SelectClipRgn(&clientRgn, RGN_XOR);

    if (m_bUseBitmap)
    {
        pDC->BitBlt(m_BorderRect.X,
            m_BorderRect.Y,
            m_BorderRect.Width,
            m_BorderRect.Height,
            &m_dc, 0, 0, SRCCOPY);

        pDC->RestoreDC(nSave);

        return;
    }

    pDC->FillSolidRect(&nonClientRect, GetSysColor(COLOR_3DFACE));

    Graphics graphics(pDC->GetSafeHdc());
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
    
    // Clear background
    COLORREF backgroundColor = m_ColorMap.GetColor(m_ControlState, BackgroundTopGradientFinish);
    Color cl(0, 0, 0);
    cl.SetFromCOLORREF(backgroundColor);
    Gdiplus::SolidBrush brush(cl);
    graphics.FillRectangle(&brush, m_ClientRect);

    DrawRectArea(m_BorderRect, graphics, m_ColorMap.GetColor(m_ControlState, Border),
        m_CornerRadius, m_borderPenWidth);

    pDC->RestoreDC(nSave);

    int x = m_BorderRect.X;
    int y = m_BorderRect.Y;
    int width = m_BorderRect.Width;
    int height = m_BorderRect.Height;

    m_dc.DeleteDC();

    // store into m_dc
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
END_MESSAGE_MAP()

void CTTListCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
    CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);

    CRect cBorder;
    GetWindowRect(cBorder);
    cBorder.OffsetRect(-cBorder.left, -cBorder.top);

    m_BorderRect.X = cBorder.left;
    m_BorderRect.Y = cBorder.top;
    m_BorderRect.Width = cBorder.Width();
    m_BorderRect.Height = cBorder.Height();

    CRect cClient(lpncsp->rgrc[0]);
    cClient.OffsetRect(-lpncsp->rgrc[1].left, -lpncsp->rgrc[1].top);

    m_ClientRect.X = cClient.left;
    m_ClientRect.Y = cClient.top;
    m_ClientRect.Width = cClient.Width();
    m_ClientRect.Height = cClient.Height();

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
    m_bNcSizeIsCalculated = true;

    TRACE("NcCalc: border [%d %d] [%d %d]\n", m_BorderRect.X, m_BorderRect.X, m_BorderRect.Width, m_BorderRect.Height);

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

    TRACE(CString("NcPaint\n"));

    Default();

    SetPosition(0, 0);

    CWindowDC dc(this);
    //CRect nonClientRect(m_BorderRect.X, m_BorderRect.Y, m_BorderRect.GetRight(), m_BorderRect.GetBottom());
    //CMemDC memDC(dc, &nonClientRect);
    
    /*CRgn nonClientRgn;
    CRect nonClientRect(m_BorderRect.X, m_BorderRect.Y, m_BorderRect.GetRight(), m_BorderRect.GetBottom());
    CreateRectRgnInDevicePoints(&dc, &nonClientRgn, nonClientRect);

    dc.SelectClipRgn(&nonClientRgn);

    int nWid = ::GetSystemMetrics(SM_CXVSCROLL);

    if (m_bHScroll)
    {
        CRect cHScrRect(m_BorderRect.X + m_OffsetRect.left,
            m_BorderRect.Y + m_ClientRect.Height,
            m_BorderRect.GetRight() - m_OffsetRect.right,
            m_BorderRect.GetBottom() - m_OffsetRect.bottom);

        if (m_bVScroll)
        {
            if (GetExStyle() & WS_EX_LEFTSCROLLBAR)
                cHScrRect.left += nWid;
            else
                cHScrRect.right -= nWid;
        }

        CRgn cHScrRgn;
        CreateRectRgnInDevicePoints(&dc, &cHScrRgn, cHScrRect, m_CornerRadius);
        dc.SelectClipRgn(&cHScrRgn, RGN_XOR);
    }

    if (m_bVScroll)
    {
        CRect cVScrRect(m_BorderRect.X + m_ClientRect.Width,
            m_BorderRect.Y + m_OffsetRect.top,
            m_BorderRect.GetRight() - m_OffsetRect.right,
            m_BorderRect.GetBottom() - m_OffsetRect.bottom);

        if (m_bHScroll)
            cVScrRect.bottom -= nWid;

        CRgn cVScrRgn;
        CreateRectRgnInDevicePoints(&dc, &cVScrRgn, cVScrRect, m_CornerRadius);
        dc.SelectClipRgn(&cVScrRgn, RGN_XOR);
    }

    CRect clientRect(m_ClientRect.X + m_OffsetRect.left,
        m_ClientRect.Y + m_OffsetRect.top,
        m_ClientRect.GetRight() - m_OffsetRect.right,
        m_ClientRect.GetBottom() - m_OffsetRect.bottom);

    CRgn clientRgn;
    CreateRectRgnInDevicePoints(&dc, &clientRgn, clientRect);
    dc.SelectClipRgn(&clientRgn, RGN_XOR);

    dc.FillSolidRect(&nonClientRect, GetSysColor(COLOR_3DFACE));*/

    Paint(&dc);
    /*DrawRectArea(m_BorderRect, graphics, m_ColorMap.GetColor(m_ControlState, Border),
        m_CornerRadius, m_borderPenWidth);*/

    m_bUseBitmap = true;
}

BOOL CTTListCtrl::OnEraseBkgnd(CDC* pDC)
{
    /*if (m_BorderRect.IsEmptyArea())
        return TRUE;
    TRACE(CString("Erase\n"));
    SetPosition(-m_OffsetRect.left - m_ClientRect.GetLeft(), -m_OffsetRect.top);
    Paint(pDC);
    SetPosition(0, 0);*/

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
    {
        pHeader->GetClientRect(&headerRect);
        if (headerRect.Width() > 0 && headerRect.Height() > 0)
        {
            //CPaintDC headerDC(pHeader);
            //CMemDC hearedMemDC(headerDC, &headerRect);
            //pHeader->SendMessage(WM_PAINT, (WPARAM)hearedMemDC.GetDC().m_hDC, (LPARAM)0);
        }
    }
    
    CRect clientRect(m_ClientRect.X/* + m_OffsetRect.left*/,
        m_ClientRect.Y/* + m_OffsetRect.top*/ + headerRect.Height(),
        m_ClientRect.GetRight() - m_OffsetRect.right,
        m_ClientRect.GetBottom() - m_OffsetRect.bottom);

    if (clientRect.Width() <= 0 || clientRect.Height() <= 0)
        return;

    TRACE("OnPaint\n");

    CPaintDC dc(this);
    CMemDC memDC(dc, &clientRect);

    UpdateControlState();
    COLORREF backgroundColor = m_ColorMap.GetColor(m_ControlState, BackgroundTopGradientFinish);
    memDC.GetDC().FillSolidRect(&clientRect, backgroundColor);

    DefWindowProc(WM_PAINT, (WPARAM)memDC.GetDC().m_hDC, (LPARAM)0);

    ReleaseDC(&memDC.GetDC());

}

void CTTListCtrl::OnMove(int x, int y)
{
    CListCtrl::OnMove(x, y);

    CString cStr;
    cStr.Format(_T("OnMove: cx,cy:[%d %d]\n"), x, y);
    TRACE(cStr);

    if (m_bPainted)
    {
        GetParent()->InvalidateRect(&m_oldWndRect);
        
        m_bUseBitmap = false;
        this->SendMessage(WM_NCPAINT);        
    }

    m_oldWndRect.left = x - m_OffsetRect.left;
    m_oldWndRect.top = y - m_OffsetRect.top;
    m_oldWndRect.right = m_oldWndRect.left + m_BorderRect.Width;
    m_oldWndRect.bottom = m_oldWndRect.top + m_BorderRect.Height;
}

void CTTListCtrl::OnSize(UINT nType, int cx, int cy)
{
    TRACE("OnSize: cx,cy:[%d %d] ", (oldX-cx), (oldY-cy));
    
    DWORD lStyle = GetStyle();

    bool ScrollBarsVisChanged = (m_bHScroll != (bool)(lStyle & WS_HSCROLL)) || (m_bVScroll != (bool)(lStyle & WS_VSCROLL));

    if (m_Sized && (cx != oldX || cy != oldY) &&
        !ScrollBarsVisChanged)
    {
        TRACE("Applied\n");

        int iOffWidth = (cx - oldX);
        int iOffHeight = (cy - oldY);

        m_BorderRect.Width += iOffWidth;
        m_BorderRect.Height += iOffHeight;
        
        m_bUseBitmap = false;
        if (m_bPainted)
        {            
            this->SendMessage(WM_NCPAINT);
            
            if (oldX != 0 || oldY != 0)
            {
                CRect oldClientRect(m_ClientRect.X, m_ClientRect.Y, m_ClientRect.GetRight() - iOffWidth, m_ClientRect.GetBottom() - iOffHeight);
                CRect curClientRect(m_ClientRect.X, m_ClientRect.Y, m_ClientRect.GetRight(), m_ClientRect.GetBottom());

                CRgn oldRgn, curRgn;
                oldRgn.CreateRectRgnIndirect(&oldClientRect);
                curRgn.CreateRectRgnIndirect(&curClientRect);

                CRgn invalidRgn;
                ::CombineRgn(invalidRgn, oldRgn, curRgn, RGN_XOR);
                InvalidateRgn(&invalidRgn);
            }
        }
    }
    else
    {
        TRACE("Skipped\n");
    }
    
    oldX = cx;
    oldY = cy;

    CListCtrl::OnSize(nType, cx, cy);

    m_bHScroll = (lStyle & WS_HSCROLL);
    m_bVScroll = (lStyle & WS_VSCROLL);

    /*CRect cBorder;
    GetWindowRect(cBorder);
    cBorder.OffsetRect(-cBorder.left, -cBorder.top);

    m_BorderRect.X = cBorder.left;
    m_BorderRect.Y = cBorder.top;
    m_BorderRect.Width = cBorder.Width();
    m_BorderRect.Height = cBorder.Height();*/

    CRgn oldRgn;
    CRect cRect(m_oldWndRect);
    oldRgn.CreateRectRgnIndirect(&cRect);

    m_oldWndRect.right = m_oldWndRect.left + m_BorderRect.Width;
    m_oldWndRect.bottom = m_oldWndRect.top + m_BorderRect.Height;

    cRect.CopyRect(&m_oldWndRect);
    CRgn invalidRgn;
    invalidRgn.CreateRectRgnIndirect(&cRect);
    invalidRgn.CombineRgn(&invalidRgn, &oldRgn, RGN_XOR);

    GetParent()->InvalidateRgn(&invalidRgn);

	/*CDC* parDC=GetParent()->GetDC();
	parDC->FillSolidRect(m_oldWndRect, RGB(255, 0, 0));*/

    m_Sized = true;
}
