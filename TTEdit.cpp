#include "stdafx.h"
#include "TTEdit.h"
#include "CommonDrawing.h"
#include "ControlsColorMap.h"

CTTEdit::CTTEdit() :
m_ClientRect(0, 0, 0, 0),
m_BorderRect(0, 0, 0, 0),
m_OffsetRect(0, 0, 0, 0),
m_oldParentRect(0, 0, 0, 0)
{
	m_bUseBaseMessageHandlers = false;
	m_bNcSizeIsCalculated = false;
	m_bUseBitmap = false;
	m_bHover = false;
	m_bFocused = false;
	m_bPainted = false;

	SetDrawingProperties(1, 5);

	m_ControlState = Normal;

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
	UpdateControlState();

	// Get only non-client region
	CRgn nonClientRgn;
	CRect nonClientRect(m_BorderRect.X, m_BorderRect.Y, m_BorderRect.GetRight(), m_BorderRect.GetBottom());
	CreateRectRgnInDevicePoints(pDC, &nonClientRgn, nonClientRect);

	pDC->SelectClipRgn(&nonClientRgn, RGN_AND);

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
	pDC->SelectClipRgn(&clientRgn, RGN_DIFF);

	int nWid = ::GetSystemMetrics(SM_CXVSCROLL);
	DWORD dwStyle = GetStyle();

	if (dwStyle & WS_HSCROLL)
	{
		// Exclude horizontal scrollbar region

		CRect cHScrRect(m_BorderRect.X + m_OffsetRect.left,
			m_BorderRect.GetBottom() - m_OffsetRect.bottom - nWid,
			m_BorderRect.GetRight() - m_OffsetRect.right,
			m_BorderRect.GetBottom() - m_OffsetRect.bottom);

		if (dwStyle & WS_VSCROLL)
		{
			if (GetExStyle() & WS_EX_LEFTSCROLLBAR)
				cHScrRect.left = nWid;
			else
				cHScrRect.right -= nWid;
		}

		CRgn cHScrRgn;
		CreateRectRgnInDevicePoints(pDC, &cHScrRgn, cHScrRect, m_CornerRadius);
		pDC->SelectClipRgn(&cHScrRgn, RGN_DIFF);
	}

	if (dwStyle & WS_VSCROLL)
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

		if (dwStyle & WS_HSCROLL)
			cVScrRect.bottom -= nWid;

		CRgn cVScrRgn;
		CreateRectRgnInDevicePoints(pDC, &cVScrRgn, cVScrRect, m_CornerRadius);
		pDC->SelectClipRgn(&cVScrRgn, RGN_DIFF);
	}

	if (m_bUseBitmap)
	{
		// if it's already drawn - just copy existing bitmap

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

	if (dwStyle & WS_HSCROLL)
		cClientRect.bottom -= nWid - 1;

	if (dwStyle & WS_VSCROLL)
		if (GetExStyle() & WS_EX_LEFTSCROLLBAR)
			cClientRect.left += nWid - 1;
		else
			cClientRect.right -= nWid - 1;

	DrawThemeParentBackground(GetSafeHwnd(), pDC->GetSafeHdc(), cBorderRect);

	DrawEditControlFrame(pDC, &cBorderRect, NULL, m_ControlState, &cClientRect, m_CornerRadius,
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
}

void CTTEdit::DrawEditControlFrame(CDC* pDC, CRect* pBorderRect, CRect* pClipRect, ControlState controlState, CRect* pClientRect,
	int cornerRadius, int borderWidth, ControlsColorMap* pColorMap)
{
	if (pDC == NULL)
		return;

	if (pBorderRect == NULL)
		return;

	CRect clipBox;
	pDC->GetClipBox(&clipBox);

	CDC dc;
	CBitmap bmp;
	dc.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, clipBox.right, clipBox.bottom);
	dc.SelectObject(&bmp);
	dc.BitBlt(pBorderRect->left, pBorderRect->top, pBorderRect->Width(), pBorderRect->Height(),
		pDC, pBorderRect->left, pBorderRect->top, SRCCOPY);
	bmp.DeleteObject();

	Graphics graphics(dc.GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

	bool bDeleteColorMap = false;
	if (pColorMap == NULL)
	{
		pColorMap = new ControlsColorMap();
		pColorMap->SetColor(Mouseover, BackgroundTopGradientFinish, RGB(255, 255, 255));

		bDeleteColorMap = true;
	}

	CRgn clipRgn;
	CreateRectRgnInDevicePoints(&dc, &clipRgn,
		pClipRect == NULL ? *pBorderRect : *pClipRect);
	dc.SelectClipRgn(&clipRgn);

	// non-client background
	CRgn bkgndRgn;
	CreateRectRgnInDevicePoints(&dc, &bkgndRgn, pBorderRect->left, pBorderRect->top,
		pBorderRect->right + 1, pBorderRect->bottom + 1, cornerRadius);
	dc.SelectClipRgn(&bkgndRgn, RGN_AND);
	dc.FillSolidRect(pBorderRect, GetSysColor(COLOR_3DFACE));
	dc.SelectClipRgn(&clipRgn);

	Gdiplus::Rect BorderRect(pBorderRect->left, pBorderRect->top, pBorderRect->Width(), pBorderRect->Height());
	Gdiplus::Rect ClientRect(0, 0, 0, 0);

	if (pClientRect == NULL)
	{
		ClientRect.X = pBorderRect->left + borderWidth;
		ClientRect.Y = pBorderRect->top + borderWidth;
		ClientRect.Width = pBorderRect->Width() - 2 * borderWidth;
		ClientRect.Height = pBorderRect->Height() - 2 * borderWidth;
	}
	else
	{
		ClientRect.X = pClientRect->left;
		ClientRect.Y = pClientRect->top;
		ClientRect.Width = pClientRect->Width();
		ClientRect.Height = pClientRect->Height();
	}

	// Clear client background
	COLORREF backgroundColor = pColorMap->GetColor(controlState, BackgroundTopGradientFinish);;
	Color cl(0, 0, 0);
	cl.SetFromCOLORREF(backgroundColor);
	Gdiplus::SolidBrush brush(cl);
	graphics.FillRectangle(&brush, ClientRect);

	// borders
	DrawRectArea(BorderRect, graphics, pColorMap->GetColor(controlState, Border),
		cornerRadius, borderWidth);

	if (bDeleteColorMap && pColorMap != NULL)
	{
		delete pColorMap;
		pColorMap = NULL;
	}

	pDC->BitBlt(pBorderRect->left, pBorderRect->top, pBorderRect->Width(), pBorderRect->Height(),
		&dc, pBorderRect->left, pBorderRect->top, SRCCOPY);
}

double CTTEdit::GetDpiScaleFactor()
{
	HDC screen = ::GetDC(NULL);
	double m_DpiScaleFactor = GetDeviceCaps(screen, LOGPIXELSX) / 96.0;
	::ReleaseDC(NULL, screen);

	return m_DpiScaleFactor;
}

BEGIN_MESSAGE_MAP(CTTEdit, CEdit)
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

BOOL CTTEdit::OnEraseBkgnd(CDC* pDC)
{
	if (m_bUseBaseMessageHandlers)
		return CEdit::OnEraseBkgnd(pDC);

	return TRUE;
}

void CTTEdit::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	CEdit::OnNcCalcSize(bCalcValidRects, lpncsp);

	m_bNcSizeIsCalculated = true;

	if (m_bUseBaseMessageHandlers)
		return;

	// store border rect
	CRect cBorder;
	GetWindowRect(cBorder);
	cBorder.OffsetRect(-cBorder.left, -cBorder.top);

	m_BorderRect.X = cBorder.left;
	m_BorderRect.Y = cBorder.top;
	m_BorderRect.Width = cBorder.Width();
	m_BorderRect.Height = cBorder.Height();

	// default offset for client area
	m_OffsetRect.CopyRect(CRect(m_borderPenWidth + 1, m_borderPenWidth + 1, m_borderPenWidth + 1, m_borderPenWidth + 1));

	HWND hwnd = GetSafeHwnd();
	DWORD dwStyle = ::GetWindowLong(hwnd, GWL_STYLE);
	DWORD dwExStyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);
	BOOL  bLeftScroll = dwExStyle & WS_EX_LEFTSCROLLBAR;

	// change offset is scrollbars are visible
	if (dwStyle & WS_VSCROLL)
	{
		if (bLeftScroll)
			m_OffsetRect.left--;
		else
			m_OffsetRect.right--;
	}

	if (dwStyle & WS_HSCROLL)
		m_OffsetRect.bottom--;

	m_OffsetRect.left = (int)(GetDpiScaleFactor() * m_OffsetRect.left);
	m_OffsetRect.top = (int)(GetDpiScaleFactor() * m_OffsetRect.top);
	m_OffsetRect.right = (int)(GetDpiScaleFactor() * m_OffsetRect.right);
	m_OffsetRect.bottom = (int)(GetDpiScaleFactor() * m_OffsetRect.bottom);

	if (!(GetStyle() & ES_MULTILINE))  // multiline edit box shouldn't change NC area
	{
		// calculate client area offset
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

	// change client area
	lpncsp->rgrc[0].top += m_OffsetRect.top;
	lpncsp->rgrc[0].bottom -= m_OffsetRect.bottom;

	lpncsp->rgrc[0].left += m_OffsetRect.left;
	lpncsp->rgrc[0].right -= m_OffsetRect.right;

	// store calculated current client area
	CRect cClient(lpncsp->rgrc[0]);
	cClient.OffsetRect(-lpncsp->rgrc[1].left, -lpncsp->rgrc[1].top);

	m_ClientRect.X = cClient.left;
	m_ClientRect.Y = cClient.top;
	m_ClientRect.Width = cClient.Width();
	m_ClientRect.Height = cClient.Height();

	m_bUseBitmap = false;
}

BOOL CTTEdit::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
	if (!m_bNcSizeIsCalculated)
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

	return CEdit::OnChildNotify(message, wParam, lParam, pLResult);
}

void CTTEdit::OnNcPaint()
{
	if (m_bUseBaseMessageHandlers)
	{
		CEdit::OnNcPaint();
		return;
	}

	if (!m_bNcSizeIsCalculated)
		return;

	// this will draw default scroll bars, if any
	Default();

	// draw borders
	CWindowDC dc(this);

	HRGN hRgn;
	const MSG* pMsg = CWnd::GetCurrentMessage();

	// select update region if specified
	if (pMsg->wParam != 0 && pMsg->wParam != 1)
	{
		hRgn = (HRGN)pMsg->wParam;
		CRgn* pRgn = CRgn::FromHandle(hRgn);

		CRect wndRect;
		GetWindowRect(&wndRect);

		CRgn clipRgn;
		clipRgn.CreateRectRgnIndirect(CRect(0, 0, 0, 0));
		clipRgn.CopyRgn(pRgn);

		clipRgn.OffsetRgn(-wndRect.left, -wndRect.top);

		dc.SelectClipRgn(&clipRgn);
	}

	Paint(&dc);

	m_bUseBitmap = true;
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
		// remove default border styles to draw own

		HWND hWnd = GetSafeHwnd();

		LONG lStyle = GetWindowLong(hWnd, GWL_STYLE);
		LONG lExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

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

	// offset text from border edges
	SetMargins(m_borderPenWidth, m_borderPenWidth);

	CEdit::PreSubclassWindow();
}

void CTTEdit::OnPaint()
{
	if (m_bUseBaseMessageHandlers)
	{
		CEdit::OnPaint();
		return;
	}

	// store control rect in parent-client coordinates for invalidation OnMove/OnSize
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

	CEdit::OnPaint();

	if (!m_bPainted)
	{
		this->SendMessage(WM_NCPAINT, 0, 0);
		m_bPainted = true;
	}
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
	HBRUSH hbr = CEdit::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void CTTEdit::OnSize(UINT nType, int cx, int cy)
{
	CEdit::OnSize(nType, cx, cy);

	if (m_bUseBaseMessageHandlers)
		return;

	if (!::IsWindow(GetSafeHwnd()))
		return;

	// NcSize should be recalculated
	bool bIsVisible = IsWindowVisible();
	if (bIsVisible)
		SetRedraw(FALSE);

	SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

	if (bIsVisible)
		SetRedraw(TRUE);

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

void CTTEdit::OnMove(int x, int y)
{
	CEdit::OnMove(x, y);

	if (m_bUseBaseMessageHandlers)
		return;

	if (!::IsWindow(GetSafeHwnd()))
		return;

	// NcSize should be recalculated
	bool bIsVisible = IsWindowVisible();
	if (bIsVisible)
		SetRedraw(FALSE);

	SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

	if (bIsVisible)
		SetRedraw(TRUE);

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