// BoringStaticCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "FlatTabCtrl.h"
#include "BoringStaticCtrl.h"
#include "common.h"

// BoringStaticCtrl

IMPLEMENT_DYNAMIC(BoringStaticCtrl, CStatic)

BoringStaticCtrl::BoringStaticCtrl()
{
	m_backgroundColor = ::GetSysColor(COLOR_3DFACE);
}

BoringStaticCtrl::~BoringStaticCtrl()
{
}


BEGIN_MESSAGE_MAP(BoringStaticCtrl, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// BoringStaticCtrl message handlers




void BoringStaticCtrl::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	CStatic::PreSubclassWindow();
}


BOOL BoringStaticCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}


void BoringStaticCtrl::OnPaint()
{
	CRect cRect;
	GetClientRect(&cRect);
	CPaintDC dc(this); // device context for painting
	CMemDC memDC(dc, cRect);
	
	Graphics graphics(memDC.GetDC().GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	

	if (GetStyle() & SS_BLACKFRAME)
	{
		memDC.GetDC().FillSolidRect(&cRect, m_backgroundColor);
		CRect rPage;
		GetClientRect(&rPage);
		rPage.DeflateRect(2, 2);

		Pen pen(Color::Black, 2);
		pen.SetLineJoin(LineJoinRound);
		GraphicsPath path;
		GetRoundRectPath(&path, Rect(rPage.left, rPage.top, rPage.Width(), rPage.Height()), 10, 2);
		graphics.DrawPath(&pen, &path);
	}
	else
	{
		//DrawThemeParentBackground(GetSafeHwnd(), memDC.GetDC().GetSafeHdc(), cRect);
		memDC.GetDC().FillSolidRect(&cRect, m_backgroundColor);

		CRect r = cRect;

		//memDC.GetDC().SelectObject(GetFont());
		LOGFONT logFont;
		GetFont()->GetLogFont(&logFont);
		Gdiplus::Font font(memDC.GetDC().GetSafeHdc(), &logFont);

		CRect rPage;
		GetClientRect(&rPage);
		rPage.DeflateRect(1, 1, 1, 1);

		Pen pen(Color(192, 192, 192), 2);
		pen.SetLineJoin(LineJoinRound);
		GraphicsPath path;
		GetRoundRectPath(&path, Rect(rPage.left, rPage.top, rPage.Width(), rPage.Height()), 10, 2);
		graphics.DrawPath(&pen, &path);

		CString staticText;
		GetWindowText(staticText);

		SolidBrush blackBrush(Color(255, 0, 0, 0));
		rPage.DeflateRect(2, 2);
		Gdiplus::RectF layoutRect(rPage.left, rPage.top, rPage.Width(), rPage.Height());

		graphics.DrawString(staticText.GetBuffer(0), staticText.GetLength(), &font,
			layoutRect, Gdiplus::StringFormat::GenericDefault(), &blackBrush);
	}
}
