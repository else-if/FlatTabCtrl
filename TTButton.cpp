#include "stdafx.h"
#include "TTButton.h"
#include "FlatTabCtrl.h"
#include "common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Gdiplus;

void GetRoundRectPath(GraphicsPath *pPath, Rect r, int dia);

CTTButton::CTTButton()
{
	m_bCheckBox = false;
	m_bChecked = false;
	m_bTracking = false;

	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
	m_TextFont.CreateFontIndirect(&ncm.lfStatusFont);
	
	m_NormalStateGradientStart = RGB(237, 239, 242);
	m_NormalStateGradientFinish = RGB(255, 255, 255);
	
	m_BorderNormalStateGradientStart = RGB(153, 152, 158);
	m_BorderNormalStateGradientFinish = RGB(193, 192, 200);

	m_SelectStateGradientStart = RGB(0, 73, 162);
	m_SelectStateGradientFinish = RGB(191, 209, 231);

	m_FocusedStateGradientStart = RGB(0, 135, 216);
	m_FocusedStateGradientFinish = RGB(239, 239, 242);

	m_DisabledStateGradientStart = RGB(193, 192, 205);
	m_DisabledStateGradientFinish = RGB(193, 192, 205);

	m_OutlineColor = RGB(0, 73, 162);
	m_CornerRadius = 5;
}

CTTButton::~CTTButton()
{
}

BEGIN_MESSAGE_MAP(CTTButton, CButton)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()	
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CTTButton::SetNormalStateGradients(COLORREF start, COLORREF finish)
{
	m_NormalStateGradientStart = start;
	m_NormalStateGradientFinish = finish;
}

void CTTButton::SetSelectedStateGradients(COLORREF start, COLORREF finish)
{
	m_SelectStateGradientStart = start;
	m_SelectStateGradientFinish = finish;
}

void CTTButton::SetFocusedStateGradients(COLORREF start, COLORREF finish)
{
	m_FocusedStateGradientStart = start;
	m_FocusedStateGradientFinish = finish;
}

void CTTButton::SetDisabledStateGradients(COLORREF start, COLORREF finish)
{
	m_DisabledStateGradientStart = start;
	m_DisabledStateGradientFinish = finish;
}

void CTTButton::SetOutlineColor(COLORREF color)
{
	m_OutlineColor = color;
}

void CTTButton::ActAsCheckBox(bool fActAsCheckBox)
{
	m_bCheckBox = fActAsCheckBox;
}

void CTTButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC xdc;
	xdc.Attach(lpDrawItemStruct->hDC);

	CRect cRect;
	GetClientRect(&cRect);
	
	CMemDC memDC(xdc, cRect);

	Graphics graphics(memDC.GetDC().GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	
	Gdiplus::Rect AreaRect(cRect.left, cRect.top, cRect.Width(), cRect.Height());
	
	DrawThemeParentBackground(GetSafeHwnd(), memDC.GetDC().GetSafeHdc(), cRect);
	
	/*Gdiplus::Color col(Gdiplus::Color::Red);
	RectangleDropShadow(graphics, AreaRect, col, 12, 64);*/
	
	DrawButtonBackground(cRect, memDC, m_NormalStateGradientStart, m_NormalStateGradientFinish, m_CornerRadius);

	cRect.DeflateRect(5, 5, 5, 5);

	

	//DrawButtonBorders
	//

	
	memDC.GetDC().SelectObject(&m_TextFont);
	CString txt;
	GetWindowText(txt);
	memDC.GetDC().DrawText(txt, cRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	
	/*
	GraphicsPath path;
	Gdiplus::Rect AreaRect2(cRect.left, cRect.top, cRect.Width(), cRect.Height());
	GetRoundRectPath(&path, AreaRect2, m_CornerRadius);

	Gdiplus::Pen pen(Gdiplus::Color::Green, 2);
	pen.SetLineJoin(LineJoinRound);
	graphics.DrawPath(&pen, &path);*/

	/*if (dc != NULL)
		CWnd::ReleaseDC(&dc);*/

	/*
	CDC xdc;
	xdc.Attach(lpDrawItemStruct->hDC);
	CRect rc;
	GetClientRect(rc);	
	CMemDC dc(xdc, rc);
	using namespace Gdiplus;
	Graphics graphics(dc.GetDC().GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	Gdiplus::Rect r2(rc.left, rc.top, rc.Width(), rc.Height());
	
	Gdiplus::Color c1(Gdiplus::Color::Red);

	//rc.InflateRect(10, 10);
	DrawThemeParentBackground(GetSafeHwnd(), dc.GetDC().GetSafeHdc(), rc);

	RectangleDropShadow(graphics, Gdiplus::Rect(r2.GetLeft(), r2.GetTop(), r2.Width, r2.Height), c1, 10, 128);

	int test = true;

	if (test)
	{
		rc.DeflateRect(5, 5);


		//DrawThemeParentBackground(GetSafeHwnd(), dc.GetDC().GetSafeHdc(), rc);

		int imode = dc.GetDC().SetBkMode(TRANSPARENT);

		UINT state = lpDrawItemStruct->itemState;
		bool IsDisable = false;

		if (m_bCheckBox) {
			if (m_bChecked || (state & ODS_FOCUS && state & ODS_SELECTED))
			{
			}
			else if (state & ODS_DISABLED)	{
				IsDisable = true;
			}
			else {
				if (m_bChecked)
				{
				}
				else
				{
				}
			}
		}
		else
		{
			if (state & ODS_FOCUS)
			{
				DrawButtonBackground(rc, dc, m_FocusedStateGradientStart, m_FocusedStateGradientFinish, m_CornerRadius);
				if (state & ODS_SELECTED)
				{
					DrawButtonBackground(rc, dc, m_SelectStateGradientStart, m_SelectStateGradientFinish, m_CornerRadius);
				}
			}
			else if (m_bTracking)
			{
				DrawButtonBackground(rc, dc, m_FocusedStateGradientStart, m_FocusedStateGradientFinish, m_CornerRadius);
			}
			else if (state & ODS_DISABLED)	{
				IsDisable = true;
				DrawButtonBackground(rc, dc, m_DisabledStateGradientStart, m_DisabledStateGradientFinish, m_CornerRadius);
			}
			else
			{
				DrawButtonBackground(rc, dc, m_NormalStateGradientStart, m_NormalStateGradientFinish, m_CornerRadius);
			}
		}

		dc.GetDC().SelectObject(&m_TextFont);
		CString txt;
		GetWindowText(txt);
		dc.GetDC().DrawText(txt, rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	Color c = m_OutlineColor;
	Color a = RGB(0, 73, 162);
	Pen pen(Color(0, 73, 162), 2);

	GraphicsPath path;
	Gdiplus::Rect r(rc.left, rc.top, rc.Width(), rc.Height());
	GetRoundRectPath(&path, r, m_CornerRadius);

	pen.SetLineJoin(LineJoinRound);
	graphics.DrawPath(&pen, &path);
	*/

// 	Gdiplus::Rect r2(r);
// 	r2.Inflate(10, 10);
// 	r2.Offset(5, 5);
// 	GraphicsPath path2;
// 	GetRoundRectPath(&path2, r2, m_CornerRadius);
// 
// 	DrawGlow(&xdc, &path2);

}

void CTTButton::DrawButtonBackground(CRect &rc, CMemDC &dc, COLORREF colorStart, COLORREF colorFinish, int cornerRadius)
{
	CDrawingManager drawingManager(dc.GetDC());
	
	CRgn rgnBorder;

	//borders
	rgnBorder.CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, cornerRadius, cornerRadius);
	dc.GetDC().SelectClipRgn(&rgnBorder);
	drawingManager.FillGradient(rc, m_BorderNormalStateGradientStart, m_BorderNormalStateGradientFinish, 1);

	//background	
	CRgn rgnBackground;
	rgnBackground.CreateRoundRectRgn(rc.left + 1, rc.top + 1, rc.right - 1, rc.bottom - 1, cornerRadius, cornerRadius);
	dc.GetDC().SelectClipRgn(&rgnBackground);

	//drawingManager.FillGradient(rc, colorStart, colorFinish, 1);
	COLORREF colorStart1 = RGB(255, 214, 100);
	COLORREF colorFinish1 = RGB(255, 225, 144);
	COLORREF colorStart2 = RGB(254, 251, 205);
	COLORREF colorFinish2 = RGB(255, 214, 100);
	drawingManager.Fill4ColorsGradient(rc, colorStart1, colorFinish1,
		colorStart2, colorFinish2);
	

	COLORREF col = RGB(0, 0, 0);
	Pen pen(col, 1);
	pen.SetLineJoin(LineJoinRound);
	GraphicsPath path;
	GetRoundRectPath(&path, Rect(rc.left, rc.top, rc.Width(), rc.Height()), cornerRadius, 2);
	graphics.DrawPath(&pen, &path);

	/*CPen p;
	p.CreatePen(PS_SOLID, 1, m_OutlineColor);
	CBrush* pOldBrush = (CBrush*)dc.GetDC().SelectStockObject(NULL_BRUSH);
	CPen* pOldPen = dc.GetDC().SelectObject(&p);
	
	dc.GetDC().SelectObject(pOldBrush);
	dc.GetDC().SelectObject(pOldPen);*/
}

BOOL CTTButton::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

BOOL CTTButton::PreTranslateMessage(MSG* pMsg)
{
	// treat double-click like single click
	if (pMsg->message == WM_LBUTTONDBLCLK) {
		pMsg->message = WM_LBUTTONDOWN;
	}
	return CButton::PreTranslateMessage(pMsg);
}

void CTTButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bCheckBox) {
		CRect rectButton;
		GetClientRect(&rectButton);
		if (rectButton.PtInRect(point))
			if (!m_bChecked)
				m_bChecked = !m_bChecked;
	}
	CButton::OnLButtonUp(nFlags, point);
	Invalidate();
}

void CTTButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = GetSafeHwnd();
		m_bTracking = ::_TrackMouseEvent(&tme) ? true : false;
		Invalidate(false);
	}

	CButton::OnMouseMove(nFlags, point);
}

void CTTButton::OnMouseLeave()
{
	m_bTracking = false;
	Invalidate(false);

	CButton::OnMouseLeave();
}
