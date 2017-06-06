// BoringButtonCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "FlatTabCtrl.h"
#include "BoringButtonCtrl.h"
#include "CommonDrawing.h"

// BoringButtonCtrl

IMPLEMENT_DYNAMIC(BoringButtonCtrl, CButton)

BoringButtonCtrl::BoringButtonCtrl()
{
	m_backgroundColor = ::GetSysColor(COLOR_3DFACE);
}

BoringButtonCtrl::~BoringButtonCtrl()
{
}


BEGIN_MESSAGE_MAP(BoringButtonCtrl, CButton)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
END_MESSAGE_MAP()



// BoringButtonCtrl message handlers




void BoringButtonCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CButton::OnPaint() for painting messages
	if (GetButtonStyle() == BS_GROUPBOX)
	{
		CRect cRect;
		GetClientRect(&cRect);
		CMemDC memDC(dc, cRect);

		Graphics graphics(memDC.GetDC().GetSafeHdc());
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);
		graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

		CRect rPage;
		GetClientRect(&rPage);
		rPage.DeflateRect(2, 2);

		Pen pen(Color::Black, 2);
		pen.SetLineJoin(LineJoinRound);
		GraphicsPath path;
		GetRoundRectPath(&path, Rect(rPage.left, rPage.top, rPage.Width(), rPage.Height()), 10, 2);

		Gdiplus::Region region(&path);
		graphics.SetClip(&region);
		//graphics.FillRectangle(&SolidBrush(Color::White), Rect(rPage.left, rPage.top, rPage.Width(), rPage.Height()));
		memDC.GetDC().FillSolidRect(&cRect, m_backgroundColor);

		//Gdiplus::SolidBrush b(Gdiplus::Color::BlueViolet);
		//graphics.FillRegion(&b, &region);

		graphics.ResetClip();
		graphics.DrawPath(&pen, &path);
	}
}


BOOL BoringButtonCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	
	return CButton::OnEraseBkgnd(pDC);
}

void BoringButtonCtrl::DrawItem(
	LPDRAWITEMSTRUCT lpDrawItemStruct
	)
{
	int stopp = 2;
}

void BoringButtonCtrl::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	CButton::PreSubclassWindow();
}


afx_msg LRESULT BoringButtonCtrl::OnMsgViewexpand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}


int BoringButtonCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	

	return 0;
}
