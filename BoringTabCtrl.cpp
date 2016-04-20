// BoringTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "BoringTabCtrl.h"
#include "common.h"

using namespace Gdiplus;

void GetTabControlPath(GraphicsPath *pPath, Rect tab, Rect tabView, int dia);
void GetTabControlPath2(GraphicsPath *pPath, Rect tab, Rect tabView, int dia);

// CBoringTabCtrl

IMPLEMENT_DYNAMIC(CBoringTabCtrl, CTabCtrl)

CBoringTabCtrl::CBoringTabCtrl()
{
	m_backgroundColor = ::GetSysColor(COLOR_3DFACE);
	m_borderWidth = 2;
	m_cornerRadius = 5;
	m_bTracking = false;
	m_iMouseOverTab = -1;

	m_ColorMap.SetDefaultColors();
}

CBoringTabCtrl::~CBoringTabCtrl()
{
}


COLORREF CBoringTabCtrl::BackgroundColor()
{
	return m_backgroundColor;
}

void CBoringTabCtrl::BackgroundColor(COLORREF backgroundColor)
{
	m_backgroundColor = backgroundColor;
	RedrawWindow();
	UpdateWindow();
}

BEGIN_MESSAGE_MAP(CBoringTabCtrl, CTabCtrl)
	ON_WM_DRAWITEM()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// CBoringTabCtrl message handlers


void CBoringTabCtrl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default

	CTabCtrl::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

int CBoringTabCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= TCS_OWNERDRAWFIXED;

	if (CTabCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}


void CBoringTabCtrl::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	ModifyStyle(0, TCS_OWNERDRAWFIXED);
	
	CTabCtrl::PreSubclassWindow();
}


BOOL CBoringTabCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
//	cs.style |= TCS_OWNERDRAWFIXED;

	return CTabCtrl::PreCreateWindow(cs);
}


void CBoringTabCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	// TODO:  Add your code to draw the specified item
	
}

BOOL CBoringTabCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	Invalidate(FALSE);
	return TRUE;	
}

void CBoringTabCtrl::GetInteriorRect(CRect& rect)
{
	CRect cRect;
	GetClientRect(&cRect);
	AdjustRect(FALSE, &cRect);
	CRect r2;
	VERIFY(GetItemRect(0, &r2));
	cRect.top = r2.bottom;
	cRect.left = r2.left;
	int lengthToPerimeter = (int)ceil((m_cornerRadius)*sin(45.0));
	cRect.DeflateRect(lengthToPerimeter, lengthToPerimeter, lengthToPerimeter - 1, lengthToPerimeter - 1);
	rect = cRect;
}

void CBoringTabCtrl::OnPaint()
{	
	CRect cRect;
	GetClientRect(&cRect);
	CPaintDC dc(this); // device context for painting

	CMemDC memDC(dc, cRect);

	Graphics graphics(memDC.GetDC().GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
		
	memDC.GetDC().FillSolidRect(&cRect, m_backgroundColor);
	
	CRect rPage;
	GetClientRect(&rPage);
	AdjustRect(FALSE, rPage);

	int nTab = GetItemCount();
	int nSel = GetCurSel();

	if (!nTab)
		return;
	
	CRect r1;
	VERIFY(GetItemRect(nSel, &r1));
	CRect r2;
	VERIFY(GetItemRect(0, &r2));
	rPage.top = r1.bottom;
	rPage.left = r2.left;

	Gdiplus::Rect rTab(r1.left, r1.top, r1.Width(), r1.Height());
	Gdiplus::Rect rView(rPage.left, rPage.top, rPage.Width(), rPage.Height());

	GraphicsPath path;
	GetTabControlPath(&path, rTab, rView, 2*m_cornerRadius);

	Gdiplus::Region region(&path);
	graphics.SetClip(&region);

	LOGFONT logFont;
	GetFont()->GetLogFont(&logFont);
	CFont font;	

	if (GetFocus() == this)
		logFont.lfUnderline = 1;		
	
	font.CreateFontIndirect(&logFont);
	memDC.GetDC().SelectObject(&font);
	
	TCHAR buffer[256] = { 0 };
	memDC.GetDC().SetBkMode(TRANSPARENT);
	COLORREF textColor = GetSysColor(COLOR_CAPTIONTEXT);
	
	while (nTab--)
	{
		
		if (nTab != nSel)
		{
			RECT TabRect;			
			GetItemRect(nTab, &TabRect);
			
			if (m_iMouseOverTab == nTab)
			{
				RECT HighlightedRect;
				GetItemRect(m_iMouseOverTab, &HighlightedRect);

				CRect rc = (CRect)HighlightedRect;
				rc.DeflateRect(1, 1);

				Draw4ColorsGradientRect(rc, memDC,					
					m_ColorMap.GetColor(Mouseover, BackgroundBottomGradientFinish), 
					m_ColorMap.GetColor(Mouseover, BackgroundBottomGradientStart),
					m_ColorMap.GetColor(Mouseover, BackgroundTopGradientFinish),
					m_ColorMap.GetColor(Mouseover, BackgroundTopGradientStart),					
					m_cornerRadius);
			}

			TCITEM tcItem;
			tcItem.pszText = buffer;
			tcItem.cchTextMax = 256;
			tcItem.mask = TCIF_TEXT;
			GetItem(nTab, &tcItem);			

			DrawText((CRect)TabRect, memDC, font, textColor, CString(tcItem.pszText), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			
		}		
	}
	
	
	// now selected tab
	RECT TabRect;
	GetItemRect(nSel, &TabRect);
	
	Gdiplus::SolidBrush b(Gdiplus::Color::White);
	TabRect.bottom = rPage.top;
	graphics.FillRegion(&b, &region);

	Pen pen(Color(192, 192, 192), m_borderWidth);
	pen.SetLineJoin(LineJoinRound);
	pen.SetAlignment(PenAlignmentCenter);
	
	TCITEM tcItem;
	tcItem.pszText = buffer;
	tcItem.cchTextMax = 256;
	tcItem.mask = TCIF_TEXT;
	GetItem(nSel, &tcItem);
	
	DrawText((CRect)TabRect, memDC, font, textColor, CString(tcItem.pszText), DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	graphics.ResetClip();
	graphics.DrawPath(&pen, &path);
}

void GetTabControlPath(GraphicsPath *pPath, Rect tab, Rect tabView, int dia)
{
	// diameter can't exceed width or height
	if (dia > tab.Width)	dia = tab.Width;
	if (dia > tab.Height)	dia = tab.Height;

	Rect Corner(tab.X, tab.Y, dia, dia);

	pPath->Reset();

	pPath->AddArc(Corner, 180, 90);

	Corner.X += (tab.Width - dia);
	pPath->AddArc(Corner, 270, 90);

	Corner.X += dia;
	Corner.Y += (tab.Height - dia);
	pPath->AddArc(Corner, 180, -90);

	Corner.Y = tabView.Y;
	Corner.X = (tabView.X + tabView.Width - dia);
	pPath->AddArc(Corner, -90, 90);

	Corner.Y = (tabView.Y + tabView.Height - dia);
	pPath->AddArc(Corner, 0, 90);

	Corner.X -= (tabView.Width - dia);
	pPath->AddArc(Corner, 90, 90);

	if (tab.X == tabView.X)
	{
		Corner.Y = (tab.Y - dia);
		pPath->AddLine(tabView.X, tabView.Y - dia, tabView.X, tab.Y + dia);
	}
	else
	{
		Corner.Y = tabView.Y;
		pPath->AddArc(Corner, 180, 90);

		Corner.Y -= dia;
		Corner.X = tab.X - dia;
		pPath->AddArc(Corner, 90, -90);
	}
	
	pPath->CloseFigure();
}

void GetTabControlPath2(GraphicsPath *pPath, Rect tab, Rect tabView, int dia)
{
	// diameter can't exceed width or height
	if (dia > tab.Width)	dia = tab.Width;
	if (dia > tab.Height)	dia = tab.Height;

	Rect Corner(tab.X, tab.Y, dia, dia);

	pPath->Reset();

	pPath->AddLine(Corner.X, Corner.Y, Corner.X + tab.Width, Corner.Y);
	Corner.X += tab.Width;
	pPath->AddLine(Corner.X, Corner.Y, Corner.X, tabView.Y);
	Corner.Y = tabView.Y;
	pPath->AddLine(Corner.X, Corner.Y, tabView.X + tabView.Width, Corner.Y);
	Corner.X = tabView.X + tabView.Width;
	pPath->AddLine(Corner.X, Corner.Y, Corner.X, tabView.Y + tabView.Height);
	Corner.Y = tabView.Y + tabView.Height;
	pPath->AddLine(tabView.X, Corner.Y, Corner.X, Corner.Y);
	Corner.X = tabView.X;

	if (tab.X == tabView.X)
	{
		pPath->AddLine(Corner.X, Corner.Y, Corner.X, tab.Y);
	}
	else
	{
		pPath->AddLine(tabView.X, tabView.Y + tabView.Height, tabView.X, tabView.Y);
		pPath->AddLine(tabView.X, tabView.Y, tab.X, tabView.Y);
		pPath->AddLine(tab.X, tabView.Y, tab.X, tab.Y);
	}

	pPath->CloseFigure();
}

void CBoringTabCtrl::OnMouseLeave()
{
	m_bTracking = false;
	m_iMouseOverTab = -1;
	Invalidate(false);

	CTabCtrl::OnMouseLeave();
}


void CBoringTabCtrl::OnMouseMove(UINT nFlags, CPoint point)
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
		m_iMouseOverTab = hit;
		Invalidate(false);
	}

	CTabCtrl::OnMouseMove(nFlags, point);
}
