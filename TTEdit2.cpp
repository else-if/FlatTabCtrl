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
    UpdateControlState();

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

        return;
    }

	// draw on current region

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

    DrawEditControlFrame(pDC, &cBorderRect, &cClientRect, NULL, m_ControlState, m_CornerRadius,
        m_borderPenWidth, &m_ColorMap);

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
    
   return TRUE;
}

void CTTEdit2::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
    CEdit::OnNcCalcSize(bCalcValidRects, lpncsp);

    m_bNcSizeIsCalculated = true;

    if (m_bUseBaseMessageHandlers)
        return;

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

	if (!m_bNcSizeIsCalculated)
		return;

    Default();

    CWindowDC dc(this);	
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

	// store parent rect for invalidation OnMove/OnSize
	CWnd *pWnd = GetParent();
	if (pWnd != NULL)
	{
		GetWindowRect(m_oldWndRect);
		::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&m_oldWndRect, 2);
	}

	if (!m_bNcSizeIsCalculated)
    {
        // force NCCalcSize
        SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
        return;
    }

	CEdit::OnPaint();     
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
}

void CTTEdit2::OnSize(UINT nType, int cx, int cy)
{
    CEdit::OnSize(nType, cx, cy);

	if (m_bUseBaseMessageHandlers)
		return;

    if (!::IsWindow(GetSafeHwnd()))
        return;

    // NcSize should be recalculated
    SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

    Invalidate();
    
    // Invalidate changed parent region

    CWnd *pWnd = GetParent();
    if (pWnd != NULL)
    {
        CRect oldWindowRect, curWindowRect;

        GetWindowRect(curWindowRect);
        oldWindowRect.CopyRect(m_oldWndRect);

		::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&curWindowRect, 2);

        InvalidateRectRegions(pWnd, oldWindowRect, curWindowRect, RGN_XOR);
    }    
}

void CTTEdit2::OnMove(int x, int y)
{
    CEdit::OnMove(x, y);

    if (m_bUseBaseMessageHandlers)
        return;

    if (!::IsWindow(GetSafeHwnd()))
        return;

    // NcSize should be recalculated
    SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

	Invalidate();

	// Invalidate changed parent regions
    CWnd *pWnd = GetParent();
    if (m_bPainted && pWnd != NULL)
    {
		CRect oldWindowRect, curWindowRect;

        GetWindowRect(curWindowRect);
        oldWindowRect.CopyRect(m_oldWndRect);

		::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&curWindowRect, 2);

        InvalidateRectRegions(pWnd, oldWindowRect, curWindowRect, RGN_XOR);
    }    
}