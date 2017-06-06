#include "stdafx.h"
#include "TTListCtrl.h"
#include "CommonDrawing.h"

using namespace Gdiplus;

CTTListCtrl::CTTListCtrl() :
m_ClientRect(0, 0, 0, 0),
m_OffsetRect(0, 0, 0, 0),
m_oldParentRect(0, 0, 0, 0)
{
	m_bNcSizeIsCalculated = false;
	m_bUseBitmap = false;
	m_bPainted = false;

	SetDrawingProperties(1, 5);
	m_ControlState = Normal;

	m_bDrawBorders = false;
	m_bSized = false;
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

void CTTListCtrl::PreSubclassWindow()
{
	// rounded borders should be drawn only if original control has style with border
	// but to draw new borders - original style should be removed

	HWND hWnd = GetSafeHwnd();

	LONG lStyle = GetWindowLong(hWnd, GWL_STYLE);
	LONG lExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

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

	CListCtrl::PreSubclassWindow();
}

void CTTListCtrl::NcCalcSize(CWnd* pWnd, NCCALCSIZE_PARAMS* lpncsp, CRect* pOffsetRect, int borderWidth)
{
	if (pWnd == NULL)
		return;

	if (lpncsp == NULL)
		return;

	// default offset for client area
	pOffsetRect->SetRect(borderWidth + 1, borderWidth + 1, borderWidth + 1, borderWidth + 1);

	DWORD dwStyle = pWnd->GetStyle();
	DWORD dwExStyle = pWnd->GetExStyle();
	BOOL  bLeftScroll = dwExStyle & WS_EX_LEFTSCROLLBAR;

	// change offset is scrollbars are visible
	if (dwStyle & WS_VSCROLL)
	{
		if (bLeftScroll)
			pOffsetRect->left--;
		else
			pOffsetRect->right--;
	}

	if (dwStyle & WS_HSCROLL)
		pOffsetRect->bottom--;

	// change client area
	lpncsp->rgrc[0].top += pOffsetRect->top;
	lpncsp->rgrc[0].bottom -= pOffsetRect->bottom;

	lpncsp->rgrc[0].left += pOffsetRect->left;
	lpncsp->rgrc[0].right -= pOffsetRect->right;
}

void CTTListCtrl::PaintNonClientArea(CWnd* pWnd, CDC* pDC, CRect* pOffsetRect, ControlState controlState, CDC* pMemDC, ControlsColorMap* pColorMap,
	int cornerRadius, int borderWidth)
{
	if (pWnd == NULL)
		return;

	if (pDC == NULL)
		return;

	if (pOffsetRect == NULL)
		pOffsetRect->SetRect(0, 0, 0, 0);

	bool bDeleteColorMap = false;
	if (pColorMap == NULL)
	{
		pColorMap = new ControlsColorMap();
		bDeleteColorMap = true;
	}

	CRect clientRect, windowRect;

	pWnd->GetClientRect(clientRect);
	pWnd->ClientToScreen(clientRect);

	pWnd->GetWindowRect(windowRect);

	clientRect.OffsetRect(-windowRect.left, -windowRect.top);
	windowRect.OffsetRect(-windowRect.left, -windowRect.top);

	// Get only non-client region
	CRgn nonClientRgn;
	CreateRectRgnInDevicePoints(pDC, &nonClientRgn, windowRect);

	pDC->SelectClipRgn(&nonClientRgn);

	int nWid = ::GetSystemMetrics(SM_CXVSCROLL);

	DWORD dwStyle = pWnd->GetStyle();
	bool bHScroll = dwStyle & WS_HSCROLL;
	bool bVScroll = dwStyle & WS_VSCROLL;

	if (bHScroll)
	{
		// Exclude horizontal scrollbar region

		CRect cHScrRect(windowRect.left + pOffsetRect->left,
			clientRect.bottom,
			windowRect.right - pOffsetRect->right,
			windowRect.bottom - pOffsetRect->bottom);

		if (bVScroll)
		{
			if (pWnd->GetExStyle() & WS_EX_LEFTSCROLLBAR)
				cHScrRect.left = nWid;
			else
				cHScrRect.right -= nWid;
		}

		clientRect.bottom++;

		CRgn cHScrRgn;
		CreateRectRgnInDevicePoints(pDC, &cHScrRgn, cHScrRect, cornerRadius);
		pDC->SelectClipRgn(&cHScrRgn, RGN_XOR);
	}

	if (bVScroll)
	{
		// Exclude vertical scrollbar region
		CRect cVScrRect;

		if (pWnd->GetExStyle() & WS_EX_LEFTSCROLLBAR)
		{
			cVScrRect.SetRect(windowRect.left + pOffsetRect->left,
				windowRect.top + pOffsetRect->top,
				clientRect.left,
				windowRect.bottom - pOffsetRect->bottom);

			clientRect.left--;
		}
		else
		{
			cVScrRect.SetRect(clientRect.right,
				windowRect.top + pOffsetRect->top,
				windowRect.right - pOffsetRect->right,
				windowRect.bottom - pOffsetRect->bottom);

			clientRect.right++;
		}

		if (bHScroll)
			cVScrRect.bottom -= nWid;

		CRgn cVScrRgn;
		CreateRectRgnInDevicePoints(pDC, &cVScrRgn, cVScrRect, cornerRadius);
		pDC->SelectClipRgn(&cVScrRgn, RGN_XOR);
	}

	// Exclude client area
	pDC->ExcludeClipRect(&clientRect);

	if (pMemDC != NULL)
	{
		// if it already drawn - just copy existing bitmap

		pDC->BitBlt(windowRect.left,
			windowRect.top,
			windowRect.Width(),
			windowRect.Height(),
			pMemDC, 0, 0, SRCCOPY);
	}
	else
	{
		// background
		pDC->FillSolidRect(&windowRect, GetSysColor(COLOR_3DFACE));
		DrawBorderRect(pDC, &windowRect, controlState, pColorMap, cornerRadius, borderWidth);
	}

	if (bDeleteColorMap && pColorMap != NULL)
	{
		delete pColorMap;
		pColorMap = NULL;
	}
}

void CTTListCtrl::DrawBorderRect(CDC* pDC, CRect* pBorderRect, ControlState controlState, ControlsColorMap* pColorMap,
	int cornerRadius, int borderWidth)
{
	if (pDC == NULL)
		return;

	if (pBorderRect == NULL)
		return;

	bool bDeleteColorMap = false;
	if (pColorMap == NULL)
	{
		pColorMap = new ControlsColorMap();
		bDeleteColorMap = true;
	}

	Graphics graphics(pDC->GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

	// border
	Gdiplus::Rect borderRect(pBorderRect->left, pBorderRect->top, pBorderRect->Width(), pBorderRect->Height());

	DrawRectArea(borderRect, graphics, pColorMap->GetColor(controlState, Border),
		cornerRadius, borderWidth);

	if (bDeleteColorMap && pColorMap != NULL)
	{
		delete pColorMap;
		pColorMap = NULL;
	}
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

	if (!m_bDrawBorders)
		return;

	NcCalcSize(this, lpncsp, &m_OffsetRect, m_borderPenWidth);

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
	UpdateControlState();

	PaintNonClientArea(this, &dc, &m_OffsetRect, m_ControlState, m_bUseBitmap ? &m_dc : NULL,
		&m_ColorMap, m_CornerRadius, m_borderPenWidth);

	// store into m_dc
	CRect wndRect;
	GetWindowRect(&wndRect);
	wndRect.OffsetRect(-wndRect.left, -wndRect.top);

	int x = wndRect.left;
	int y = wndRect.top;
	int width = wndRect.Width();
	int height = wndRect.Height();

	m_dc.DeleteDC();

	CBitmap bmp;
	m_dc.CreateCompatibleDC(&dc);
	bmp.CreateCompatibleBitmap(&dc, width, height);
	m_dc.SelectObject(&bmp);
	m_dc.BitBlt(0, 0, width, height, &dc, x, y, SRCCOPY);
	bmp.DeleteObject();

	m_bUseBitmap = true;
	m_bPainted = true;
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

	// save window rect where control was drawn
	CWnd *pWnd = GetParent();
	if (pWnd != NULL)
	{
		GetWindowRect(m_oldParentRect);
		::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&m_oldParentRect, 2);
	}
}

void CTTListCtrl::OnMove(int x, int y)
{
	CListCtrl::OnMove(x, y);

	if (!::IsWindow(GetSafeHwnd()))
		return;

	m_bUseBitmap = false;

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

void CTTListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	if (!::IsWindow(GetSafeHwnd()))
		return;

	m_bUseBitmap = false;

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
