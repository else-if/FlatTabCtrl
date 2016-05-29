#include "stdafx.h"
#include "TTEdit.h"
#include "CommonDrawing.h"
#include "ControlsColorMap.h"

CTTEdit::CTTEdit()
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
    else if (GetStyle() & ES_READONLY)
        m_ControlState = ReadOnly;
    else if (m_bHover || m_bFocused)
        m_ControlState = Mouseover;
    else
        m_ControlState = Normal;

    m_bUseBitmap = (m_bUseBitmap && oldCtrlStrate == m_ControlState);
}

void CTTEdit::Paint(CDC* pDC)
{
    if (m_ClientRect.IsEmptyArea())
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
    }
}

void CTTEdit::DrawEditControlFrame(CDC* pDC, CRect* pRect, CRect* pClipRect, ControlState controlState,
    int cornerRadius, int borderWidth, ControlsColorMap* pColorMap)
{
    if (pDC == NULL)
        return;

    if (pRect == NULL || pClipRect == NULL)
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

    CRgn clipRgn;
    CreateRectRgnInDevicePoints(pDC, &clipRgn, *pClipRect);

    pDC->SelectClipRgn(&clipRgn);

    Gdiplus::Rect BorderRect(pRect->left, pRect->top, pRect->Width(), pRect->Height());

    // Clear background
    COLORREF backgroundColor = pColorMap->GetColor(controlState, BackgroundTopGradientFinish);;
    Color cl(0, 0, 0);
    cl.SetFromCOLORREF(backgroundColor);
    Gdiplus::SolidBrush brush(cl);
    graphics.FillRectangle(&brush, BorderRect);

    DrawRectArea(BorderRect, graphics, pColorMap->GetColor(controlState, Border),
        cornerRadius, borderWidth);

    pDC->RestoreDC(nSave);

    if (bDeleteColorMap && pColorMap != NULL)
    {
        delete pColorMap;
        pColorMap = NULL;
    }

}

BEGIN_MESSAGE_MAP(CTTEdit, CEdit)
    ON_WM_ERASEBKGND()
    ON_WM_NCCALCSIZE()
    ON_WM_NCPAINT()
    ON_WM_SETCURSOR()
    ON_WM_MOUSELEAVE()
    ON_CONTROL_REFLECT(EN_UPDATE, &CTTEdit::OnEnUpdate)
    ON_WM_PAINT()
    ON_WM_KILLFOCUS()
    ON_WM_SETFOCUS()
    ON_WM_CTLCOLOR()
    ON_WM_SIZE()
    ON_WM_MOVE()
END_MESSAGE_MAP()


BOOL CTTEdit::OnEraseBkgnd(CDC* pDC)
{
    if (m_bUseBaseMessageHandlers)
        return CEdit::OnEraseBkgnd(pDC);

    if (m_ClientRect.IsEmptyArea())
        return TRUE;

    SetPosition(-m_borderPenWidth, m_OffsetY);
    Paint(pDC);

    return TRUE;
}

void CTTEdit::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
    if (m_bUseBaseMessageHandlers)
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

    m_bNcSizeIsCalculated = true;
}

BOOL CTTEdit::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
    if (m_bUseBaseMessageHandlers)
        return CEdit::OnChildNotify(message, wParam, lParam, pLResult);

    if (!m_bNcSizeIsCalculated)
    {
        SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
    }

    return CEdit::OnChildNotify(message, wParam, lParam, pLResult);
}

void CTTEdit::OnNcPaint()
{
    if (m_bUseBaseMessageHandlers)
    {
        CEdit::OnNcPaint();
        return;
    }

    if (m_ClientRect.IsEmptyArea())
        return;

    if (!m_bNcSizeIsCalculated)
        return;

    Default();

    CWindowDC dc(this);
    SetPosition(0, 0);
    Paint(&dc);

    m_bUseBitmap = true;

    Invalidate();
}

BOOL CTTEdit::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    m_bHover = (nHitTest == HTCLIENT);
    return CEdit::OnSetCursor(pWnd, nHitTest, message);
}

void CTTEdit::OnMouseLeave()
{
    m_bHover = false;
    CEdit::OnMouseLeave();
}

void CTTEdit::OnEnUpdate()
{
    if (m_bUseBaseMessageHandlers)
    {
        return;
    }

    Invalidate();
}

void CTTEdit::PreSubclassWindow()
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
        lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
        SetWindowLong(hWnd, GWL_STYLE, lStyle);

        LONG lExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
        lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
        SetWindowLong(hWnd, GWL_EXSTYLE, lExStyle);

        SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
    }

    CEdit::PreSubclassWindow();
}

void CTTEdit::OnPaint()
{
    if (m_bUseBaseMessageHandlers)
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

	CRect clientRect(m_ClientRect.X,
		m_ClientRect.Y,
		m_ClientRect.GetRight(),
		m_ClientRect.GetBottom());

	if (clientRect.Width() <= 0 || clientRect.Height() <= 0)
		return;

	TRACE("OnPaint\n");

	CPaintDC dc(this);
	CMemDC memDC(dc, &clientRect);

	/*UpdateControlState();
	COLORREF backgroundColor = m_ColorMap.GetColor(m_ControlState, BackgroundTopGradientFinish);
	memDC.GetDC().FillSolidRect(&clientRect, backgroundColor);*/

	DefWindowProc(WM_PAINT, (WPARAM)memDC.GetDC().m_hDC, (LPARAM)0);

	ReleaseDC(&memDC.GetDC());

    //CEdit::OnPaint();
}

void CTTEdit::OnKillFocus(CWnd* pNewWnd)
{
    m_bFocused = false;
    CEdit::OnKillFocus(pNewWnd);
}

void CTTEdit::OnSetFocus(CWnd* pOldWnd)
{
    m_bFocused = true;
    CEdit::OnSetFocus(pOldWnd);
}

HBRUSH CTTEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    if (m_bUseBaseMessageHandlers)
    {
        HBRUSH hbr = CEdit::OnCtlColor(pDC, pWnd, nCtlColor);
        return hbr;
    }
    else
    {
        return NULL;
    }
}

void CTTEdit::OnSize(UINT nType, int cx, int cy)
{
    if (m_bUseBaseMessageHandlers)
    {
        CEdit::OnSize(nType, cx, cy);
        return;
    }

    m_ClientRect.Width = cx + 2 * m_borderPenWidth;
	
    if (!m_bUseBaseMessageHandlers && m_bPainted)
    {
        GetParent()->InvalidateRect(&m_oldWndRect);
        
        m_bUseBitmap = false;

        OnNcPaint();
    }

    m_oldWndRect.right = m_oldWndRect.left + m_ClientRect.Width;
    m_oldWndRect.bottom = m_oldWndRect.top + m_ClientRect.Height;

    CEdit::OnSize(nType, cx, cy);
}

void CTTEdit::OnMove(int x, int y)
{
    CEdit::OnMove(x, y);

    if (m_bUseBaseMessageHandlers)
        return;

    if (!m_bUseBaseMessageHandlers && m_bPainted)
    {
        GetParent()->InvalidateRect(&m_oldWndRect);
        
        m_bUseBitmap = false;
        OnNcPaint();
        Invalidate();
    }

    m_oldWndRect.left = x - m_borderPenWidth;
    m_oldWndRect.top = y + m_OffsetY;
    m_oldWndRect.right = m_oldWndRect.left + m_ClientRect.Width;
    m_oldWndRect.bottom = m_oldWndRect.top + m_ClientRect.Height;
}
