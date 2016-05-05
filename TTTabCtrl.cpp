#include "stdafx.h"
#include "TTTabCtrl.h"
#include "CommonDrawing.h"

using namespace Gdiplus;

// CTTTabCtrl

IMPLEMENT_DYNAMIC(CTTTabCtrl, CTabCtrl)

CTTTabCtrl::CTTTabCtrl()
{
	m_backgroundColor = ::GetSysColor(COLOR_3DFACE);
	m_bTracking = false;
	m_iMouseOverTab = -1;

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

COLORREF CTTTabCtrl::BackgroundColor()
{
	return m_backgroundColor;
}

void CTTTabCtrl::BackgroundColor(COLORREF backgroundColor)
{
	m_backgroundColor = backgroundColor;
	RedrawWindow();
	UpdateWindow();
}

void CTTTabCtrl::GetTabControlPath(GraphicsPath *pPath, Rect tab, Rect tabView, int dia)
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
    else if (abs(tab.X - tabView.X) < 2 * dia)
    {
        int diff = abs(tab.X - tabView.X);
        Corner.Width = diff / 2;

        Corner.Y = tabView.Y;
        pPath->AddArc(Corner, 180, 90);

        Corner.Width = diff - Corner.Width;

        Corner.Y -= dia;
        Corner.X = tab.X - Corner.Width;
        pPath->AddArc(Corner, 90, -90);
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

void CTTTabCtrl::GetInteriorRect(CRect& rect)
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

BEGIN_MESSAGE_MAP(CTTTabCtrl, CTabCtrl)
	ON_WM_DRAWITEM()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CTTTabCtrl message handlers

void CTTTabCtrl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CTabCtrl::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

int CTTTabCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= TCS_OWNERDRAWFIXED;

	if (CTabCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CTTTabCtrl::PreSubclassWindow()
{
	ModifyStyle(0, TCS_OWNERDRAWFIXED);
	
	CTabCtrl::PreSubclassWindow();
}

BOOL CTTTabCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;	
}

void CTTTabCtrl::OnPaint()
{
    CRect cRect;
    GetClientRect(&cRect);
    CPaintDC dc(this); // device context for painting

    CMemDC memDC(dc, cRect);

    Graphics graphics(memDC.GetDC().GetSafeHdc());
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

    memDC.GetDC().FillSolidRect(&cRect, m_backgroundColor);

    int nTab = GetItemCount();
    int nSel = GetCurSel();

    if (!nTab)
        return;

    CRect rPage;
    CRect r1;
    VERIFY(GetItemRect(nSel, &r1));
    CRect r2;
    VERIFY(GetItemRect(0, &r2));
    rPage.top = r1.bottom;
    rPage.left = cRect.left;
    rPage.right = cRect.right - 1;
    rPage.bottom = cRect.bottom - 1;

    Gdiplus::Rect rTab(r1.left, r1.top, r1.Width(), r1.Height());
    Gdiplus::Rect rView(rPage.left, rPage.top, rPage.Width(), rPage.Height());

    GraphicsPath path;
    GetTabControlPath(&path, rTab, rView, m_cornerRadius);

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

    Color bordColor(0, 0, 0);
    bordColor.SetFromCOLORREF(m_ColorMap.GetColor(Normal, Border));
    Gdiplus::Pen pen(bordColor, (REAL)m_borderWidth);
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
            GetClientRect(&rectItem);
            ValidateRect(&rectItem);
            GetItemRect(hit, &rectItem);
            
            InvalidateRect(&rectItem);
            UpdateWindow();
        }        
	}   

	CTabCtrl::OnMouseMove(nFlags, point);
}
