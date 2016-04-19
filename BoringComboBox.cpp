// BoringComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "FlatTabCtrl.h"
#include "BoringComboBox.h"
#include "common.h"

// CBoringComboBox

IMPLEMENT_DYNAMIC(CBoringComboBox, CComboBoxEx)

CBoringComboBox::CBoringComboBox()
{
	m_bOnButton = false;
	m_bTracking = false;
	m_backgroundColor = ::GetSysColor(COLOR_3DFACE);
}

CBoringComboBox::~CBoringComboBox()
{
}


BEGIN_MESSAGE_MAP(CBoringComboBox, CComboBoxEx)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// CBoringComboBox message handlers




BOOL CBoringComboBox::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return CComboBox::OnEraseBkgnd(pDC);
}


void CBoringComboBox::OnPaint()
{
	CRect cRect;
	GetClientRect(&cRect);
	CPaintDC dc(this); // device context for painting
	CMemDC memDC(dc, cRect);

	Graphics graphics(memDC.GetDC().GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	//DrawThemeParentBackground(GetSafeHwnd(), memDC.GetDC().GetSafeHdc(), cRect);
	

	CRect r = cRect;

	//memDC.GetDC().SelectObject(GetFont());
	LOGFONT logFont;
	GetFont()->GetLogFont(&logFont);
	Gdiplus::Font font(memDC.GetDC().GetSafeHdc(), &logFont);

	CRect rPage;
	GetClientRect(&rPage);
	//rPage.DeflateRect(1, 1, 1, 1);

	Pen pen(Color(192, 192, 192), 2);
	pen.SetLineJoin(LineJoinRound);
	GraphicsPath path;
	GetRoundRectPath(&path, Rect(rPage.left, rPage.top, rPage.Width(), rPage.Height()), 10, 2);

	Gdiplus::Region region(&path);
	graphics.SetClip(&region);

	

	memDC.GetDC().FillSolidRect(&cRect, m_backgroundColor);

	CString staticText;
	GetWindowText(staticText);

	SolidBrush blackBrush(Color(255, 0, 0, 0));
	rPage.DeflateRect(2, 2);
	Gdiplus::RectF layoutRect(rPage.left, rPage.top, rPage.Width(), rPage.Height());

	Gdiplus::SolidBrush b(Gdiplus::Color::White);
	graphics.FillRegion(&b, &region);

	CRect editRect;
	//m_edit.GetWindowRect(&editRect);

	int buttonWidth = GetSystemMetrics(SM_CXVSCROLL);
	CRect winRect;
	GetClientRect(&winRect);
	winRect.right -= 2;
	winRect.left = winRect.right - buttonWidth;
	
	Pen pen2(Color(192, 192, 192), 1);

	if (m_bOnButton || GetDroppedState())
	{
		Gdiplus::SolidBrush b(Gdiplus::Color::LightSlateGray);
		Rect r(winRect.left, winRect.top, winRect.Width(), winRect.Height());
		graphics.FillRectangle(&b, r);
		graphics.DrawLine(&pen2, winRect.left, winRect.top, winRect.left, winRect.bottom);
	}
	
	PointF points[3];
	points[0] = PointF(winRect.left + ((double)winRect.Width() * .2) - 1, (int)((double)winRect.Height() * .4));
	points[2] = PointF(winRect.left + ((double)winRect.Width() * .8) - 1, (int)((double)winRect.Height() * .4));
	points[1] = PointF(winRect.left + ((double)winRect.Width() * .5) - 1, (int)((double)winRect.Height() * .6));

	graphics.DrawLine(&pen2, points[0], points[1]);
	graphics.DrawLine(&pen2, points[1], points[2]);

	graphics.ResetClip();
	graphics.DrawPath(&pen, &path);	
}

void CBoringComboBox::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	
	//m_edit.SubclassWindow(this->ChildWindowFromPoint(tmpPoint)->GetSafeHwnd());


	CComboBox::PreSubclassWindow();
	POINT tmpPoint = { 1, 1 };
	HWND hwnd = ChildWindowFromPoint(tmpPoint)->GetSafeHwnd();
	CEdit* pwnd = (CEdit*)GetDlgItem(1001);
	CRect r;
	pwnd->GetWindowRect(&r);
	r.DeflateRect(0, 2);
	r.top -= 1;
	r.bottom -= 1;
	r.left += 2;
	ScreenToClient(&r);
	pwnd->MoveWindow(r);
	//pwnd->SetMargins(0, 0);
	/*CFont* pFont = pwnd->GetFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	lf.lfHeight -= 20;
	lf.lfItalic = 1;
	CFont cf;
	cf.CreateFontIndirect(&lf);
	pwnd->SetFont(&cf);*/
	//pwnd->EnableWindow(FALSE);
}


void CBoringComboBox::OnMouseLeave()
{
	m_bOnButton = false;
	m_bTracking = false;
	Invalidate(false);

	CComboBoxEx::OnMouseLeave();
}


void CBoringComboBox::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = GetSafeHwnd();
		m_bTracking = ::_TrackMouseEvent(&tme) ? true : false;
	}

	int buttonWidth = GetSystemMetrics(SM_CXVSCROLL);
	CRect winRect;
	GetClientRect(&winRect);
	winRect.right -= 2;
	winRect.left = winRect.right - buttonWidth;

	if (winRect.PtInRect(point))
	{
		m_bOnButton = true;
		Invalidate(false);
	}
	else
	{
		m_bOnButton = false;
		Invalidate(false);
	}
	CComboBoxEx::OnMouseMove(nFlags, point);
}
