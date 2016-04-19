// TTEdit.cpp : implementation file
//

#include "stdafx.h"
#include "FlatTabCtrl.h"
#include "TTEdit.h"


// CTTEdit

IMPLEMENT_DYNAMIC(CTTEdit, CEdit)

CTTEdit::CTTEdit()
:m_EditRect(0, 0, 0, 0)
, m_rectNCBottom(0, 0, 0, 0)
, m_rectNCTop(0, 0, 0, 0)
{
	m_HollowBrush.CreateStockObject(HOLLOW_BRUSH);
	m_TextColor = RGB(0, 0, 0);	
	m_bHover = false;
}

CTTEdit::~CTTEdit()
{
}


BEGIN_MESSAGE_MAP(CTTEdit, CEdit)
	//ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()



// CTTEdit message handlers


BOOL CTTEdit::OnEraseBkgnd(CDC* pDC)
{
//  	CRect rc;
//  	GetClientRect(rc);
//  	
//  	CMemDC dc(*pDC, rc);
// 
// 	dc.GetDC().FillSolidRect(&rc, RGB(255, 255, 255));
 
// 	using namespace Gdiplus;
// 	dc.GetDC().ExcludeClipRect(rc);
// 	Graphics graphics(dc.GetDC().GetSafeHdc());
// 	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
// 	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
// 
// 	//DrawThemeParentBackground(GetSafeHwnd(), dc.GetDC().GetSafeHdc(), rc);
// 
// 	//rc.DeflateRect(5, 5);
// 	Gdiplus::Rect r2(rc.left, rc.top, rc.Width(), rc.Height());
// 	Gdiplus::Rect r3(r2);
// 	Gdiplus::Color c1(Gdiplus::Color::Red);
// 
// 	//r2.InflateRect(10, 10);
// 	//DrawThemeParentBackground(GetSafeHwnd(), dc.GetDC().GetSafeHdc(), rc);
// 	r3.Inflate(-5, -5);
// 
// 	graphics.ExcludeClip(r3);
// 	RectangleDropShadow(graphics, Gdiplus::Rect(r3.GetLeft(), r3.GetTop(), r3.Width, r3.Height), c1, 5, 128);
// 
// 	//pDC->SetBkMode(TRANSPARENT);

	// TODO: Add your message handler code here and/or call default
	return FALSE;
	//return CEdit::OnEraseBkgnd(pDC);
}


HBRUSH CTTEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	return NULL;
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_TextColor);
	return (HBRUSH)m_HollowBrush;
}


void CTTEdit::OnNcPaint()
{
	Default();

	CWindowDC dc(this);
	CBrush Brush(GetSysColor(COLOR_WINDOW));

	dc.FillRect(m_rectNCBottom, &Brush);
	dc.FillRect(m_rectNCTop, &Brush);
	
	if (m_bHover && GetFocus() != this)
	{

		CRect rc;
		GetClientRect(&rc);
		using namespace Gdiplus;

		Graphics graphics(dc.GetSafeHdc());
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);
		graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

		//DrawThemeParentBackground(GetSafeHwnd(), dc.GetDC().GetSafeHdc(), rc);


		//rc.InflateRect(5, 5);
		//rc.DeflateRect(5, 5);
		Gdiplus::Rect r2(m_rectNCTop.left, m_rectNCTop.top, m_rectNCTop.Width(), m_rectNCTop.Height() + m_rectNCBottom.Height() + rc.Height());

		Gdiplus::Rect r3(r2);
		Gdiplus::Color c1(Gdiplus::Color::Red);

		//r2.InflateRect(10, 10);
		//DrawThemeParentBackground(GetSafeHwnd(), dc.GetDC().GetSafeHdc(), rc);
		//r3.Inflate(-5, -5);

		//graphics.ExcludeClip(r3);

		RectangleDropShadow(graphics, Gdiplus::Rect(r3.GetLeft(), r3.GetTop(), r3.Width, r3.Height), c1, 2, 128);
	}
}


void CTTEdit::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	CRect rectWnd, rectClient;

	//calculate client area height needed for a font
	CFont *pFont = GetFont();
	CRect rectText;
	rectText.SetRectEmpty();

	CDC *pDC = GetDC();

	CFont *pOld = pDC->SelectObject(pFont);
	pDC->DrawText(_T("Ky"), rectText, DT_CALCRECT | DT_LEFT);
	UINT uiVClientHeight = rectText.Height();

	pDC->SelectObject(pOld);
	ReleaseDC(pDC);

	//calculate NC area to center text.

	GetClientRect(rectClient);
	GetWindowRect(rectWnd);

	ClientToScreen(rectClient);

	UINT uiCenterOffset = (rectClient.Height() - uiVClientHeight) / 2;
	UINT uiCY = (rectWnd.Height() - rectClient.Height()) / 2;
	UINT uiCX = (rectWnd.Width() - rectClient.Width()) / 2;

	rectWnd.OffsetRect(-rectWnd.left, -rectWnd.top);
	m_rectNCTop = rectWnd;

	m_rectNCTop.DeflateRect(uiCX, uiCY, uiCX, uiCenterOffset + uiVClientHeight + uiCY);

	m_rectNCBottom = rectWnd;

	m_rectNCBottom.DeflateRect(uiCX, uiCenterOffset + uiVClientHeight + uiCY, uiCX, uiCY);

	lpncsp->rgrc[0].top += uiCenterOffset;
	lpncsp->rgrc[0].bottom -= uiCenterOffset;

	lpncsp->rgrc[0].left += uiCX;
	lpncsp->rgrc[0].right -= uiCY;
}


void CTTEdit::OnPaint()
{
	CPaintDC paintDC(this); // device context for painting
	CRect rc;
	GetClientRect(&rc);
	
	CFont *pFont = GetFont();

	CMemDC dc(paintDC, rc);	

	CFont *pOld = dc.GetDC().SelectObject(pFont);

	dc.GetDC().FillSolidRect(rc, GetSysColor(COLOR_WINDOW));

	if (m_bHover && GetFocus() != this)
	{
		using namespace Gdiplus;
		
		Graphics graphics(dc.GetDC().GetSafeHdc());
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);
		graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

		//DrawThemeParentBackground(GetSafeHwnd(), dc.GetDC().GetSafeHdc(), rc);

		
		//rc.InflateRect(5, 5);
		//rc.DeflateRect(5, 5);
		Gdiplus::Rect r2(m_rectNCTop.left, m_rectNCTop.top, m_rectNCTop.Width(), m_rectNCTop.Height() + m_rectNCBottom.Height() + rc.Height());

		Gdiplus::Rect r3(r2);
		Gdiplus::Color c1(Gdiplus::Color::Red);

		//r2.InflateRect(10, 10);
		//DrawThemeParentBackground(GetSafeHwnd(), dc.GetDC().GetSafeHdc(), rc);
		//r3.Inflate(-5, -5);

		//graphics.ExcludeClip(r3);
		
		RectangleDropShadow(graphics, Gdiplus::Rect(r3.GetLeft(), r3.GetTop(), r3.Width, r3.Height), c1, 5, 128);
	}

	// Clearing the background
	dc.GetDC().FillSolidRect(rc, GetSysColor(COLOR_WINDOW));

	CString text;
	GetWindowText(text);
	dc.GetDC().DrawText(text, rc, DT_SINGLELINE | DT_INTERNAL | DT_EDITCONTROL);
}


BOOL CTTEdit::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{	
	if (nHitTest == HTCLIENT)
	{
		m_bHover = true;			
	}
	else
	{
		m_bHover = false;
	}

	return CEdit::OnSetCursor(pWnd, nHitTest, message);
}


UINT CTTEdit::OnGetDlgCode()
{
	if (m_rectNCTop.IsRectEmpty())
	{
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
	}

	return CEdit::OnGetDlgCode();
}
