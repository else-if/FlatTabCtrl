// BoringEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "FlatTabCtrl.h"
#include "BoringEditCtrl.h"
#include "CommonDrawing.h"

// BoringEditCtrl

IMPLEMENT_DYNAMIC(BoringEditCtrl, CEdit)

BoringEditCtrl::BoringEditCtrl()
	: m_rectNCBottom(0, 0, 0, 0)
	, m_rectNCTop(0, 0, 0, 0)
{
	m_backgroundColor = ::GetSysColor(COLOR_3DFACE);
}

BoringEditCtrl::~BoringEditCtrl()
{
}


BEGIN_MESSAGE_MAP(BoringEditCtrl, CEdit)
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_GETDLGCODE()
	//ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BoringEditCtrl message handlers

void BoringEditCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp)
{
	CRect rectWnd, rectClient;

	//calculate client area height needed for a font
	CFont *pFont = GetFont();
	CRect rectText;
	rectText.SetRectEmpty();

	CDC *pDC = GetDC();

	CFont *pOld = pDC->SelectObject(pFont);
	pDC->DrawText(L"Ky", rectText, DT_CALCRECT | DT_LEFT);
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

	lpncsp->rgrc[0].left += uiCX + 2;
	lpncsp->rgrc[0].right -= uiCY - 2;

}

void BoringEditCtrl::OnNcPaint()
{
	Default();

	CRect cRect;
	GetClientRect(&cRect);
	CWindowDC dc(this);
	CMemDC memDC(dc, cRect);

	Graphics graphics(memDC.GetDC().GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	CRect rPage;
	GetClientRect(&rPage);
	rPage.DeflateRect(1, 1, 1, 1);
	
	Pen pen(Color(192, 192, 192), 2);
	pen.SetLineJoin(LineJoinRound);
	GraphicsPath path;
	GetRoundRectPath(&path, Rect(rPage.left, rPage.top, rPage.Width(), rPage.Height()), 10, 2);
	graphics.DrawPath(&pen, &path);
	

	

	
	/* Brush(GetSysColor(COLOR_WINDOW));

	dc.FillRect(m_rectNCBottom, &Brush);
	dc.FillRect(m_rectNCTop, &Brush);*/
}

UINT BoringEditCtrl::OnGetDlgCode()
{
	if (m_rectNCTop.IsRectEmpty())
	{
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
	}

	return CEdit::OnGetDlgCode();
}


void BoringEditCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CEdit::OnPaint() for painting messages
	Default();
}
