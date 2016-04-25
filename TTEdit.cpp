#include "stdafx.h"
#include "TTEdit.h"

//#include "colors.h"

using namespace Gdiplus;

/////////////////////////////////////////////////////////////////////////////
// CTTEdit

CTTEdit::CTTEdit()
	:m_EditRect(0, 0, 0, 0),
	m_LayerRect(0, 0, 0, 0)
{
	m_OffsetY = -1;
	m_HollowBrush.CreateStockObject(HOLLOW_BRUSH);
	m_TextColor = RGB(0, 0, 0);
}

CTTEdit::~CTTEdit()
{
}

void CTTEdit::SetPosition(int x, int y)
{
	int xoff = x - m_FrameRect.X;
	int yoff = y - m_FrameRect.Y;

	// set the frame origin
	m_FrameRect.X = x; m_FrameRect.Y = y;

	// iterate all the layers
	m_LayerRect.X += xoff;
	m_LayerRect.Y += yoff;

}

void CTTEdit::Paint(CDC* pDevC, CRect rect)
{
	//GetpDC(pDevC, rect, TRUE);

	CDC* pDC = new CDC();
	pDC->CreateCompatibleDC(pDevC);

	CBitmap*	m_pOld;
	CBitmap		m_bitmap;

	// create a compatible bitmap
	m_bitmap.CreateCompatibleBitmap(pDevC, rect.Width(), rect.Height());

	// select bitmap into dc
	m_pOld = pDC->SelectObject(&m_bitmap);

	// set window origin
	pDC->SetWindowOrg(rect.left, rect.top);

	// needed for transparency support
	pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),
		pDevC, rect.left, rect.top, SRCCOPY);
	
	// = &m_MemDC;

	Graphics gdi(pDC->m_hDC);

	// default smoothing mode
	gdi.SetSmoothingMode(SmoothingModeAntiAlias);
	gdi.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

	// paint all stacks
	if (m_bUseBitmap)
	{
		// copy the bitmap to the DC
		pDC->BitBlt(m_FrameRect.X,
			m_FrameRect.Y,
			m_FrameRect.Width,
			m_FrameRect.Height,
			&m_LayerDC, 0, 0, SRCCOPY);
	}
	else
	{
		// the first call to PaintStyle will create all the bitmaps here
		PaintStack(pDC, &gdi);
	}

	
	pDevC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),
		pDC, rect.left, rect.top, SRCCOPY);

	//delete m_pOld;
	//delete pDC;

}

void CTTEdit::PaintStack(CDC* pDC, Graphics* pGraphics)
{
	// set the Graphics object
	m_LayerpGdi = pGraphics;

	// set the MemDC object
	m_LayerMemDC = pDC;

	SetBorderClip(m_LayerpGdi, m_FrameRect);
	
	m_LayerRect = m_FrameRect;
	Gdiplus::SolidBrush br(Color::Green);
	m_LayerpGdi->FillRectangle(&br, m_LayerRect);

	m_LayerpGdi->ResetClip();
	
	// paint the borders
	PaintBorders(m_LayerpGdi, m_FrameRect, Color::Red, Color::Blue, 1);
	
	// delete dc in case of regeneration
	m_LayerDC.DeleteDC();
	
	int x = m_FrameRect.X;
	int y = m_FrameRect.Y;
	int width = m_FrameRect.Width;
	int height = m_FrameRect.Height;

	// store into m_dc
	CBitmap bmp;
	m_LayerDC.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, width, height);
	m_LayerDC.SelectObject(&bmp);
	m_LayerDC.BitBlt(0, 0, width, height, pDC, x, y, SRCCOPY);
	bmp.DeleteObject();

	m_bUseBitmap = TRUE;	
}

void CTTEdit::PaintBorders(Graphics* pGraphics, Rect r, Color ulclr, Color brclr, int width)
{
	int i, left, top, bottom, right;

	// disable smoothing mode
	SmoothingMode oldMode = pGraphics->GetSmoothingMode();
	pGraphics->SetSmoothingMode(SmoothingModeNone);

	// define the upper left pen
	Pen ulpen(ulclr, 1); ulpen.SetAlignment(PenAlignmentCenter);

	// define the bottom right pen
	Pen brpen(brclr, 1); brpen.SetAlignment(PenAlignmentCenter);

	// not sure why
	Rect rc(r);

	for (i = 0; i < width; i++)
	{
		left = rc.X; top = rc.Y; bottom = rc.GetBottom() - 1; right = rc.GetRight() - 1;

		// left
		pGraphics->DrawLine(&ulpen, left, top, left, bottom);

		// top
		pGraphics->DrawLine(&ulpen, left, top, right, top);

		// right
		pGraphics->DrawLine(&brpen, right, top + 1, right, bottom);

		// bottom
		pGraphics->DrawLine(&brpen, left + 1, bottom, right, bottom);

		rc.Inflate(-1, -1);
	}
}

void CTTEdit::SetBorderClip(Graphics* pGraphics, Rect rc, CombineMode mode)
{
	int radius = 5;
	CRgn frgn; int dia;

	dia = 2 * radius;

	/*if (shape == UNDEFINED)
	{
		// the border needs a shape
		ASSERT(FALSE);
	}
	if (shape == RECTANGLE)
	{*/
		frgn.CreateRectRgn(rc.X, rc.Y, rc.GetRight() + 1, rc.GetBottom() + 1);
	/*}
	else if (shape == ELLIPSE)
	{
		frgn.CreateEllipticRgn(rc.X - 1, rc.Y - 1, rc.GetRight() + 2, rc.GetBottom() + 2);
	}
	// handle this special case - TODO: add for Height < dia
	else if (rc.Width < dia)
	{
		rc.Width++; rc.Height++;
		Rect lrc(rc); lrc.Width = 2 * dia;
		Rect rrc(rc); rrc.Width = 2 * dia; rrc.X = rrc.X - (rrc.Width - rc.Width);
		CRgn lrgn, rrgn;
		lrgn.CreateRoundRectRgn(lrc.X, lrc.Y, lrc.GetRight() + 1, lrc.GetBottom() + 1, dia + 1, dia + 1);
		rrgn.CreateRoundRectRgn(rrc.X, rrc.Y, rrc.GetRight() + 1, rrc.GetBottom() + 1, dia + 1, dia + 1);
		frgn.CreateRectRgn(0, 0, 0, 0);
		frgn.CombineRgn(&lrgn, &rrgn, RGN_AND);
	}
	else
	{
		frgn.CreateRoundRectRgn(rc.X, rc.Y, rc.GetRight() + 1, rc.GetBottom() + 1, dia + 1, dia + 1);
	}*/

	Region grgn(frgn);

	pGraphics->SetClip(&grgn, mode);

}

void CTTEdit::GetpDC(CDC* pDC, CRect rect, BOOL bCopyFirst)
{/*
	//pDC;
	m_MemDCrect = rect;

	bool bStorageDC = FALSE;

	// true if printing
	BOOL m_bPrintDC = pDC->IsPrinting();

	//if (!m_bPrintDC)
	//{
	// Create a compatible DC
	m_MemDC.CreateCompatibleDC(pDC);

	// create a compatible bitmap
	m_bitmap.CreateCompatibleBitmap(pDC, m_MemDCrect.Width(), m_MemDCrect.Height());

	// select bitmap into dc
	m_pOld = m_MemDC.SelectObject(&m_bitmap);

	// set window origin
	m_MemDC.SetWindowOrg(m_MemDCrect.left, m_MemDCrect.top);

	// needed for transparency support
	if (bCopyFirst)
	{
		m_MemDC.BitBlt(m_MemDCrect.left, m_MemDCrect.top, m_MemDCrect.Width(), m_MemDCrect.Height(),
			pDC, m_MemDCrect.left, m_MemDCrect.top, SRCCOPY);
	}
	//return m_MemDC;
	/*}
	else
	{
	// Make a copy of the relevent parts of the current DC for printing
	m_bPrinting = m_pDC->m_bPrinting;
	m_hAttribDC = m_pDC->m_hAttribDC;
	m_hDC = m_pDC->m_hDC;
	}

	m_bCreated = TRUE;*/
}

void CTTEdit::DeletepDC(CDC* pDC)
{
	

	// Swap back the original bitmap.
	//m_MemDC.SelectObject(m_pOld);
	//delete m_pOld;
}

BEGIN_MESSAGE_MAP(CTTEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_CONTROL_REFLECT(EN_UPDATE, OnUpdate)
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTTEdit message handlers

HBRUSH CTTEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	TRACE(_T("CtlCOLOR\n"));
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_TextColor);
	return (HBRUSH)m_HollowBrush;
}

BOOL CTTEdit::OnEraseBkgnd(CDC* pDC)
{
	SetPosition(0, m_OffsetY); //m_Style.SetPosition(0, m_OffsetY); //
	Paint(pDC, m_EditRect); //m_Style.PaintStyle(pDC, m_EditRect); // Paint(pDC, m_EditRect)
	return TRUE;
}

void CTTEdit::OnUpdate()
{
	Invalidate();
}

// center text vertically
void CTTEdit::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	GetWindowRect(m_EditRect);
	m_EditRect.OffsetRect(-m_EditRect.left, -m_EditRect.top);
	m_FrameRect.X = m_EditRect.top;
	m_FrameRect.Y = m_EditRect.left;
	m_FrameRect.Width = m_EditRect.Width();
	m_FrameRect.Height = m_EditRect.Height();

	CRect rectWnd, rectClient;

	//calculate client area height needed for a font
	CFont *pFont = GetFont();
	CRect rectText;
	rectText.SetRectEmpty();

	CDC *pDC = GetDC();

	CFont *pOld = pDC->SelectObject(pFont);
	pDC->DrawText(_T("Eb"), rectText, DT_CALCRECT | DT_LEFT);
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

	m_OffsetY = -(int)uiCenterOffset;

	lpncsp->rgrc[0].top += uiCenterOffset;
	lpncsp->rgrc[0].bottom -= uiCenterOffset;

	lpncsp->rgrc[0].left += uiCX;
	lpncsp->rgrc[0].right -= uiCY;
}

BOOL CTTEdit::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
	if (m_OffsetY == -1)
	{
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
	}

	return CEdit::OnChildNotify(message, wParam, lParam, pLResult);
}

void CTTEdit::OnNcPaint()
{
	Default();

	CWindowDC dc(this);

	CString str;
	GetWindowText(str);
	SetPosition(0, 0); //m_Style.SetPosition(0, 0);
	Paint(&dc, m_EditRect); //m_Style.PaintStyle(&dc, m_EditRect);
	SetWindowText(str);
}
