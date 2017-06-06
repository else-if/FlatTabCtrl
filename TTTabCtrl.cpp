#include "stdafx.h"
#include "TTTabCtrl.h"
#include "CommonDrawing.h"

using namespace Gdiplus;

// CTTTabCtrl

IMPLEMENT_DYNAMIC(CTTTabCtrl, CTabCtrl)

CTTTabCtrl::CTTTabCtrl()
{
	m_backgroundColor = RGB(255, 255, 255);
	m_bTracking = false;
	m_iMouseOverTab = -1;
	m_ControlState = Normal;

	SetDrawingProperties(1, 5);

	m_ColorMap.SetDefaultColors();
}

CTTTabCtrl::~CTTTabCtrl()
{
}

void CTTTabCtrl::SetDrawingProperties(int borderPenWidth, int cornerRadius)
{
	m_borderWidth = borderPenWidth;
	m_cornerRadius = cornerRadius;
}

void CTTTabCtrl::UpdateControlState()
{
	if (!IsWindowEnabled())
		m_ControlState = Disable;
	else
		m_ControlState = Normal;
}

COLORREF CTTTabCtrl::BackgroundColor()
{
	return m_backgroundColor;
}

void CTTTabCtrl::BackgroundColor(COLORREF backgroundColor)
{
	m_backgroundColor = backgroundColor;
}

void CTTTabCtrl::GetTabControlPath(GraphicsPath *pPath, Rect tab, Rect tabView, int dia)
{
	// diameter can't exceed width or height
	if (dia > tab.Width)	dia = tab.Width;
	if (dia > tab.Height)	dia = tab.Height;

	Rect Corner(tab.X, tab.Y, dia, dia);

	pPath->Reset();

	// selected tab TopLeft corner
	if (abs(tab.X - tabView.X) < 2 * dia)
		Corner.X -= tab.X;
	pPath->AddArc(Corner, 180, 90);

	if (abs(tab.X - tabView.X) < 2 * dia)
		Corner.X += tab.X;

	if (abs(tab.GetRight() - tabView.GetRight()) < 2 * dia)
	{
		// selected tab TopRight corner
		Corner.X = tabView.GetRight() - dia;
		pPath->AddArc(Corner, 270, 90);

		// if size between right coordinates of tab view and selected tab lower than diameter
		// - continue path with line
		pPath->AddLine(tabView.GetRight(), tabView.Y + dia, tabView.GetRight(), tabView.GetBottom() - dia);
	}
	else
	{
		// selected tab TopRight corner
		Corner.X += (tab.Width - dia);
		pPath->AddArc(Corner, 270, 90);

		// selected tab BottomRight corner
		Corner.X += dia;
		Corner.Y += (tab.Height - dia);
		pPath->AddArc(Corner, 180, -90);

		// tab view TopRight corner
		Corner.Y = tabView.Y;
		Corner.X = (tabView.X + tabView.Width - dia);
		pPath->AddArc(Corner, -90, 90);
	}

	// tab view BottomRight corner
	Corner.Y = (tabView.Y + tabView.Height - dia);
	pPath->AddArc(Corner, 0, 90);

	// tab view BottomLeft corner
	Corner.X -= (tabView.Width - dia);
	pPath->AddArc(Corner, 90, 90);

	if (abs(tab.X - tabView.X) < 2 * dia)
	{
		// if size between left coordinates of tab view and selected tab lower than diameter
		// - continue path with line
		Corner.Y = (tab.Y - dia);
		pPath->AddLine(tabView.X, tabView.Y - dia, tabView.X, tab.Y + dia);
	}
	else
	{
		// tab view TopLeft corner
		Corner.Y = tabView.Y;
		pPath->AddArc(Corner, 180, 90);

		// selected tab bottom right corner
		Corner.Y -= dia;
		Corner.X = tab.X - dia;
		pPath->AddArc(Corner, 90, -90);
	}

	pPath->CloseFigure();
}

void CTTTabCtrl::DrawTabControlFrame(CWnd* pWnd, CDC* pDC, CRect* pTabRect, CRect* pViewRect, ControlState controlState,
	int cornerRadius, int borderWidth, COLORREF bkgndColor, ControlsColorMap* pColorMap)
{
	if (pDC == NULL)
		return;

	if (pTabRect == NULL || pViewRect == NULL)
		return;

	bool bDeleteColorMap = false;
	if (pColorMap == NULL)
	{
		pColorMap = new ControlsColorMap();
		bDeleteColorMap = true;
	}

	CRect clientRect;
	pWnd->GetClientRect(&clientRect);

	pDC->FillSolidRect(&clientRect, GetSysColor(COLOR_3DFACE));
	DrawThemeParentBackground(pWnd->GetSafeHwnd(), pDC->GetSafeHdc(), &clientRect);

	Graphics graphics(pDC->GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	Gdiplus::Rect rTab(pTabRect->left, pTabRect->top, pTabRect->Width(), pTabRect->Height());
	Gdiplus::Rect rView(pViewRect->left, pViewRect->top, pViewRect->Width(), pViewRect->Height());

	GraphicsPath path;
	GetTabControlPath(&path, rTab, rView, cornerRadius);

	Gdiplus::Region region(&path);

	Gdiplus::Color cl(0, 0, 0);
	cl.SetFromCOLORREF(bkgndColor);
	Gdiplus::SolidBrush brush(cl);
	graphics.FillRegion(&brush, &region);

	Color bordColor(0, 0, 0);
	bordColor.SetFromCOLORREF(pColorMap->GetColor(controlState, Border));
	Gdiplus::Pen pen(bordColor, borderWidth);
	pen.SetLineJoin(LineJoinRound);
	pen.SetAlignment(PenAlignmentCenter);

	graphics.DrawPath(&pen, &path);

	if (bDeleteColorMap && pColorMap != NULL)
	{
		delete pColorMap;
		pColorMap = NULL;
	}
}

void CTTTabCtrl::DrawTabItem(CDC* pDC, CRect* pTabRect, const CString& tabText, CFont* pFont, COLORREF textColor,
	bool IsSelected, bool IsHotTracked, int cornerRadius, ControlsColorMap* pColorMap)
{
	if (pDC == NULL)
		return;

	if (pTabRect == NULL)
		return;

	bool bDeleteColorMap = false;
	if (pColorMap == NULL)
	{
		pColorMap = new ControlsColorMap();
		bDeleteColorMap = true;
	}

	if (IsHotTracked && !IsSelected)
	{
		CRect highLightRect(pTabRect);
		highLightRect.DeflateRect(1, 0);

		Draw4ColorsGradientRect(highLightRect, *pDC,
			pColorMap->GetColor(Mouseover, BackgroundBottomGradientFinish),
			pColorMap->GetColor(Mouseover, BackgroundBottomGradientStart),
			pColorMap->GetColor(Mouseover, BackgroundTopGradientFinish),
			pColorMap->GetColor(Mouseover, BackgroundTopGradientStart),
			cornerRadius);
	}

	DrawText(*pTabRect, *pDC, *pFont, textColor, tabText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	if (bDeleteColorMap && pColorMap != NULL)
	{
		delete pColorMap;
		pColorMap = NULL;
	}
}

BEGIN_MESSAGE_MAP(CTTTabCtrl, CTabCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

BOOL CTTTabCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CTTTabCtrl::OnPaint()
{
	CRect cRect;
	GetClientRect(&cRect);

	UpdateControlState();

	CPaintDC dc(this);
	CMemDC memDC(dc, cRect);

	int nSel = GetCurSel();

	CRect viewRect, tabRect;
	GetItemRect(nSel, &tabRect);

	viewRect.top = tabRect.bottom;
	viewRect.left = cRect.left;
	viewRect.right = cRect.right - 1;
	viewRect.bottom = cRect.bottom - 1;

	DrawTabControlFrame(this, &memDC.GetDC(), &tabRect, &viewRect, m_ControlState, m_cornerRadius,
		m_borderWidth, m_backgroundColor, &m_ColorMap);

	LOGFONT logFont;
	GetFont()->GetLogFont(&logFont);

	if (GetFocus() == this)
		logFont.lfUnderline = 1;

	CFont font;
	font.CreateFontIndirect(&logFont);

	TCHAR buffer[256] = { 0 };
	COLORREF textColor = GetSysColor(COLOR_CAPTIONTEXT);

	for (int i = 0; i < GetItemCount(); i++)
	{
		GetItemRect(i, &tabRect);

		TCITEM tcItem;
		tcItem.pszText = buffer;
		tcItem.cchTextMax = 256;
		tcItem.mask = TCIF_TEXT;
		GetItem(i, &tcItem);

		bool bIsSelected = i == nSel;
		bool bIsHotTracked = i == m_iMouseOverTab;

		DrawTabItem(&memDC.GetDC(), &tabRect, CString(tcItem.pszText), &font, textColor,
			bIsSelected, bIsHotTracked, m_cornerRadius, &m_ColorMap);
	}
}

void CTTTabCtrl::OnMouseLeave()
{
	m_bTracking = false;

	if (m_iMouseOverTab >= 0)
	{
		if (m_iMouseOverTab != GetCurSel())
		{

			//If we are here,then mouse leaved the raised item, so
			//we have to redraw it as normal (unraised).

			CRect rectItem;
			GetItemRect(m_iMouseOverTab, &rectItem);

			m_iMouseOverTab = -1;

			InvalidateRect(&rectItem);
			UpdateWindow();
		}
		m_iMouseOverTab = -1;
	}

	CTabCtrl::OnMouseLeave();
}

void CTTTabCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = GetSafeHwnd();
		m_bTracking = ::_TrackMouseEvent(&tme) ? true : false;
	}

	TCHITTESTINFO info;
	info.pt = point;
	info.flags = TCHT_ONITEM;
	int hit = HitTest(&info);

	if (m_iMouseOverTab != hit)
	{
		CRect rectItem(0, 0, 0, 0);

		//	Mouse is over new inactive item.
		if (m_iMouseOverTab >= 0 && m_iMouseOverTab != GetCurSel())
		{
			//	Redraw the item, over which mouse was on the previous step.
			//	We redraw it as normal (unraised).

			GetItemRect(m_iMouseOverTab, &rectItem);

			m_iMouseOverTab = hit;

			InvalidateRect(&rectItem);
			UpdateWindow();
		}

		m_iMouseOverTab = hit;

		if (hit != GetCurSel())
		{
			//Redraw the item, over which mouse is now.
			//We redraw it as raised.
			GetItemRect(hit, &rectItem);

			InvalidateRect(&rectItem);
			UpdateWindow();
		}
	}

	CTabCtrl::OnMouseMove(nFlags, point);
}

void CTTTabCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CTabCtrl::OnSetFocus(pOldWnd);

	int nTab = GetItemCount();
	if (nTab > 0)
	{
		CRect cClientRect, cTabRect;
		GetItemRect(0, &cTabRect);
		GetClientRect(&cClientRect);

		int iTabHeight = cTabRect.Height() * GetRowCount();

		cTabRect.top = cClientRect.top;
		cTabRect.bottom = cTabRect.top + iTabHeight;
		cTabRect.left = cClientRect.left;
		cTabRect.right = cClientRect.right;

		InvalidateRect(&cTabRect);
	}
}

void CTTTabCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CTabCtrl::OnKillFocus(pNewWnd);

	int nTab = GetItemCount();
	if (nTab > 0)
	{
		CRect cClientRect, cTabRect;
		GetItemRect(0, &cTabRect);
		GetClientRect(&cClientRect);

		int iTabHeight = cTabRect.Height() * GetRowCount();

		cTabRect.top = cClientRect.top;
		cTabRect.bottom = cTabRect.top + iTabHeight;
		cTabRect.left = cClientRect.left;
		cTabRect.right = cClientRect.right;

		InvalidateRect(&cTabRect);
	}
}
