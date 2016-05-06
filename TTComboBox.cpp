#pragma once

#include "stdafx.h"
#include "TTComboBox.h"
#include "CommonDrawing.h"

using namespace Gdiplus;

CTTComboBox::CTTComboBox()
{
	m_bTracking = false;
	m_bButtonPress = false;
	
	m_bFilledArrow = false;
    m_bHasFocus = false;
    m_bOnButton = false;

    m_iComboBoxStyle = 0;

	SetDrawingProperties(1, 5);

	m_ControlState = Normal;
	m_ArrowButtonState = Normal;

	m_ColorMap.SetDefaultColors();

	m_ColorMap.SetColor(Press, BackgroundTopGradientStart, RGB(250, 213, 103));
	m_ColorMap.SetColor(Press, BackgroundTopGradientFinish, RGB(244, 181, 51));
	m_ColorMap.SetColor(Press, BackgroundBottomGradientStart, RGB(255, 241, 148));
	m_ColorMap.SetColor(Press, BackgroundBottomGradientFinish, RGB(250, 213, 103));

	m_arrowColorStart = RGB(96, 96, 96);
	m_arrowColorFinish = RGB(96, 96, 96);
}

CTTComboBox::~CTTComboBox()
{	
}

void CTTComboBox::SetDrawingProperties(int borderPenWidth, int cornerRadius)
{
	m_borderPenWidth = borderPenWidth;
	m_CornerRadius = cornerRadius;
}

void CTTComboBox::UpdateControlState()
{
	if (!IsWindowEnabled())
		m_ControlState = Disable;
	else if (m_bTracking || m_edit.IsMouseover() || m_bHasFocus)
		m_ControlState = Mouseover;
	else if (GetDroppedState() && !(m_iComboBoxStyle & CBS_SIMPLE))
		m_ControlState = Mouseover;
	else
		m_ControlState = Normal;

	if (!IsWindowEnabled())
		m_ArrowButtonState = Disable;
	else if (m_bButtonPress)
		m_ArrowButtonState = Press;
	else if (m_bOnButton)
		m_ArrowButtonState = Mouseover;
	else
		m_ArrowButtonState = Normal;
}

void CTTComboBox::DrawSimple()
{
	CRect cRect;
	GetClientRect(&cRect);
	CPaintDC dc(this);

	// Calculate borders rect
	if (!m_edit)
		return;

	CRect cEditRect;
	m_edit.GetWindowRect(&cEditRect);
	ScreenToClient(&cEditRect);

	cRect.bottom = cEditRect.bottom + cEditRect.top;

	CMemDC memDC(dc, cRect);

	Graphics graphics(memDC.GetDC().GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	DrawThemeParentBackground(GetSafeHwnd(), memDC.GetDC().GetSafeHdc(), cRect);

	// Clear background
	FillRectRegion(cRect, memDC, RGB(255, 255, 255), m_CornerRadius);

	// Borders
	Gdiplus::Rect BorderRect(cRect.left, cRect.top, cRect.right, cRect.bottom);

	DrawRectArea(BorderRect, graphics, m_ColorMap.GetColor(m_ControlState, Border),
		m_CornerRadius, m_borderPenWidth);

}

void CTTComboBox::DrawDropDown()
{
	CRect cRect;
	GetClientRect(&cRect);
	CPaintDC dc(this);
	CMemDC memDC(dc, cRect);

	Graphics graphics(memDC.GetDC().GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	DrawThemeParentBackground(GetSafeHwnd(), memDC.GetDC().GetSafeHdc(), cRect);

	// Clear background
	FillRectRegion(cRect, memDC, RGB(255, 255, 255), m_CornerRadius);

	// Borders
	Gdiplus::Rect BorderRect(cRect.left, cRect.top, cRect.right, cRect.bottom);

	DrawRectArea(BorderRect, graphics, m_ColorMap.GetColor(m_ControlState, Border),
		m_CornerRadius, m_borderPenWidth);

	// Arrow button region
	DrawArrowButton(cRect, memDC, graphics);
}

void CTTComboBox::DrawDropDownList()
{
	CRect cRect;
	GetClientRect(&cRect);
	CPaintDC dc(this);
	CMemDC memDC(dc, cRect);

	Graphics graphics(memDC.GetDC().GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	DrawThemeParentBackground(GetSafeHwnd(), memDC.GetDC().GetSafeHdc(), cRect);

	ControlState backgrougControlState = m_ControlState;
	if (m_ControlState != Disable && (!m_bTracking || m_bOnButton))
		backgrougControlState = Normal;

	// Draw background
	Draw4ColorsGradientRect(cRect, memDC,
		m_ColorMap.GetColor(backgrougControlState, BackgroundTopGradientStart),
		m_ColorMap.GetColor(backgrougControlState, BackgroundTopGradientFinish),
		m_ColorMap.GetColor(backgrougControlState, BackgroundBottomGradientStart),
		m_ColorMap.GetColor(backgrougControlState, BackgroundBottomGradientFinish),
		m_CornerRadius);

	// Borders
	Gdiplus::Rect BorderRect(cRect.left, cRect.top, cRect.right, cRect.bottom);

	DrawRectArea(BorderRect, graphics, m_ColorMap.GetColor(m_ControlState, Border),
		m_CornerRadius, m_borderPenWidth);

	// Selected item text
	DrawSelectedItemText(cRect, memDC, graphics);

	// Arrow button region
	DrawArrowButton(cRect, memDC, graphics);

}

void CTTComboBox::DrawArrowButton(CRect &cRect, CMemDC &memDC, Gdiplus::Graphics &graphics)
{
	CRect oldRect;
	memDC.GetDC().GetClipBox(&oldRect);
	CRgn oldRgn;
	oldRgn.CreateRectRgn(oldRect.left, oldRect.top, oldRect.right, oldRect.bottom);

	// Select button region
	CRect btnRect = GetArrowButtonRect();
	CRgn btnRgn, RoundRectRgn;
	btnRgn.CreateRectRgn(btnRect.left, btnRect.top, btnRect.right + 1, btnRect.bottom + 1);
	RoundRectRgn.CreateRoundRectRgn(cRect.left, cRect.top, cRect.right + 1, cRect.bottom + 1,
		m_CornerRadius, m_CornerRadius);

	// CRgn btnRgn;
	btnRgn.CombineRgn(&btnRgn, &RoundRectRgn, RGN_AND);

	memDC.GetDC().SelectClipRgn(&btnRgn);

	// Draw button background
	Draw4ColorsGradientRect(btnRect, memDC,
		m_ColorMap.GetColor(m_ArrowButtonState, BackgroundTopGradientStart),
		m_ColorMap.GetColor(m_ArrowButtonState, BackgroundTopGradientFinish),
		m_ColorMap.GetColor(m_ArrowButtonState, BackgroundBottomGradientStart),
		m_ColorMap.GetColor(m_ArrowButtonState, BackgroundBottomGradientFinish),
		0, TRUE);

	// Draw borders
	CRgn RectRgn;
	RectRgn.CreateRectRgn(btnRect.left, btnRect.top, btnRect.right, btnRect.bottom);
	memDC.GetDC().SelectClipRgn(&RectRgn);

	// Overwrite combo border
	Gdiplus::Rect BorderRect(cRect.left, cRect.top, cRect.right, cRect.bottom);
	DrawRectArea(BorderRect, graphics, m_ColorMap.GetColor(m_ControlState, Border),
		m_CornerRadius, m_borderPenWidth);

	// Draw left vertical border
	Color penColor;
	penColor.SetFromCOLORREF(m_ColorMap.GetColor(m_ControlState, Border));
	Pen pen(penColor, 1);

	graphics.DrawLine(&pen, btnRect.left, btnRect.top, btnRect.left, btnRect.bottom);

	// Draw arrow
	// m_bFilledArrow = true;
	if (m_bFilledArrow)
	{
		// Filled Arrow
		int iLeft = (int)(btnRect.left + 0.25 * (double)btnRect.Width() + 1);
		int iRight = (int)(btnRect.left + 0.75 * (double)btnRect.Width() + 1);
		int iHeight = (iRight - iLeft) / 2;
		int iTop = ((btnRect.top + btnRect.Height()) / 2 - iHeight / 2) + 1;
		int iBottom = iTop + iHeight;
		int iMiddle = iLeft + ((iRight - iLeft) / 2);

		CRect ArrowRect(iLeft, iTop, iRight, iBottom);

		CPoint points[3];
		points[0] = CPoint(iLeft, iTop);
		points[1] = CPoint(iRight, iTop);
		points[2] = CPoint(iMiddle, iBottom);

		CRgn ArrowWhiteRgn;
		ArrowWhiteRgn.CreatePolygonRgn(points, 3, ALTERNATE);

		memDC.GetDC().SelectClipRgn(&ArrowWhiteRgn);
		FillRectRegion(btnRect, memDC, RGB(255, 255, 255), 1, TRUE);

		CRgn ArrowRgn;
		ArrowRgn.CreatePolygonRgn(points, 3, ALTERNATE);
		ArrowRgn.OffsetRgn(-1, 0);

		ArrowRgn.CombineRgn(&ArrowRgn, &ArrowWhiteRgn, RGN_AND);

		memDC.GetDC().SelectClipRgn(&ArrowRgn);

		Draw2ColorsGradientRect(ArrowRect, memDC,
			m_arrowColorStart,
			m_arrowColorFinish,
			0, TRUE);
	}
	else
	{
		// Unfilled arrow, like "V"
		int iLeft = (int)(btnRect.left + 0.25 * (double)btnRect.Width());
		int iRight = (int)(btnRect.left + 0.75 * (double)btnRect.Width() + 1);
		int iHeight = (iRight - iLeft) / 2;
		int iTop = ((btnRect.top + btnRect.Height()) / 2 - iHeight / 2) + 1;
		int iBottom = iTop + iHeight + 1;
		
		int iBottLeft, iBottRight;
		iBottLeft = iLeft + (int)floor((iRight - iLeft) / 2);
		iBottRight = iLeft + (int)ceil((iRight - iLeft) / 2);
		
		CRect ArrowRect(iLeft, iTop, iRight, iBottom);

		CPoint points[4];
		points[0] = CPoint(iLeft, iTop);
		points[1] = CPoint(iRight, iTop);
		points[2] = CPoint(iBottRight, iBottom);
		points[3] = CPoint(iBottLeft, iBottom);

		CRgn ArrowRgn;
		ArrowRgn.CreatePolygonRgn(points, 4, ALTERNATE);

		points[0].x += 2;
		points[1].x -= 2;
		points[2].y -= 2;
		points[3].y -= 2;

		CRgn ArrowInnerRgn;
		ArrowInnerRgn.CreatePolygonRgn(points, 4, ALTERNATE);

		ArrowRgn.CombineRgn(&ArrowRgn, &ArrowInnerRgn, RGN_DIFF);

		memDC.GetDC().SelectClipRgn(&ArrowRgn);

		Draw2ColorsGradientRect(ArrowRect, memDC,
			m_arrowColorStart,
			m_arrowColorFinish,
			0, TRUE);
	}

	memDC.GetDC().SelectClipRgn(&oldRgn);

}

void CTTComboBox::DrawSelectedItemText(CRect &cRect, CMemDC &memDC, Gdiplus::Graphics &graphics)
{
	int	nSel = GetCurSel();
	if (nSel == -1)
		return;

	CString cSelText;
	int iLen = GetLBTextLen(nSel);
	GetLBText(nSel, cSelText.GetBuffer(iLen));
	cSelText.ReleaseBuffer();

	LOGFONT logFont;
	GetFont()->GetLogFont(&logFont);
	CFont font;
	font.CreateFontIndirect(&logFont);

	COLORREF textColor = m_ControlState == Disable ? GetSysColor(COLOR_GRAYTEXT) : GetSysColor(COLOR_CAPTIONTEXT);

    CRect cTextRect(cRect);
    cTextRect.DeflateRect(m_borderPenWidth + 2, 0);
    DrawText(cTextRect, memDC, font, textColor, cSelText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

CRect CTTComboBox::GetArrowButtonRect()
{
	int buttonWidth = GetSystemMetrics(SM_CXVSCROLL);

	CRect btnRect;
	GetClientRect(&btnRect);
	btnRect.left = btnRect.right - buttonWidth;

	//btnRect.DeflateRect(0, 0, -1, -1);

	return btnRect;
}

BEGIN_MESSAGE_MAP(CTTComboBox, CComboBox)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, &CTTComboBox::OnCbnKillfocus)
	ON_CONTROL_REFLECT(CBN_SETFOCUS, &CTTComboBox::OnCbnSetfocus)
END_MESSAGE_MAP()

BOOL CTTComboBox::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CTTComboBox::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = GetSafeHwnd();
		m_bTracking = ::_TrackMouseEvent(&tme) ? true : false;
	}

	if (GetArrowButtonRect().PtInRect(point))
	{
		m_bOnButton = true;		
	}
	else
	{
		m_bOnButton = false;		
	}

	CComboBox::OnMouseMove(nFlags, point);
}

void CTTComboBox::OnMouseLeave()
{
	m_bTracking = false;
	m_bOnButton = false;
	Invalidate();

	CComboBox::OnMouseLeave();
}

void CTTComboBox::OnPaint()
{
	if (!m_iComboBoxStyle)
		m_iComboBoxStyle = GetStyle() & CBS_DROPDOWNLIST;

	UpdateControlState();

	switch (m_iComboBoxStyle)
	{
	case CBS_DROPDOWN:
		DrawDropDown();
		break;
	case CBS_DROPDOWNLIST:
		DrawDropDownList();
		break;
	case CBS_SIMPLE:
		DrawSimple();
		break;
	}	
}

HBRUSH CTTComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_EDIT)
	{
		// Edit control
		if (m_edit.GetSafeHwnd() == NULL)
		{
			m_edit.SubclassWindow(pWnd->GetSafeHwnd());

			// for Simple type combo box we need Invalidate on Edit's mouse leave
			m_edit.InvalidateParentOnMouseLeave((GetStyle() & CBS_DROPDOWNLIST) == CBS_SIMPLE);

			Invalidate();
		}
	}
	else if (nCtlColor == CTLCOLOR_LISTBOX)
	{
		// ListBox control
		if (m_listbox.GetSafeHwnd() == NULL)
		{
			m_listbox.SubclassWindow(pWnd->GetSafeHwnd());
			Invalidate();
		}
	}
	HBRUSH hbr = CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void CTTComboBox::OnDestroy()
{
	if (m_edit.GetSafeHwnd() != NULL)
		m_edit.UnsubclassWindow();
	if (m_listbox.GetSafeHwnd() != NULL)
		m_listbox.UnsubclassWindow();
	CComboBox::OnDestroy();
}

BOOL CTTComboBox::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		m_bButtonPress = m_bOnButton;
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		m_bButtonPress = false;
		break;
	}	

	return CComboBox::PreTranslateMessage(pMsg);
}

void CTTComboBox::OnCbnKillfocus()
{
	m_bHasFocus = false;
	m_bTracking = false;
	m_bOnButton = false;
	m_bButtonPress = false;
	Invalidate();
}

void CTTComboBox::OnCbnSetfocus()
{
	m_bHasFocus = true;
	Invalidate();
}

