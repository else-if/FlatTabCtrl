#pragma once

#include "stdafx.h"
#include "TTComboBox.h"
#include "common.h"


using namespace Gdiplus;

CTTComboBox::CTTComboBox()
{
	m_bTracking = false;
	m_bButtonPress = false;

	m_fBorderPenWidth = 2;

	m_CornerRadius = 10;

	m_ControlState = Normal;
	m_ArrowButtonState = Normal;

	m_ColorMap.SetDefaultColors();

	//m_ColorMap.SetColor(Focus, Border, RGB(153, 152, 158));
}

CTTComboBox::~CTTComboBox()
{	
}

BEGIN_MESSAGE_MAP(CTTComboBox, CComboBoxEx)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, &CTTComboBox::OnCbnKillfocus)
	ON_CONTROL_REFLECT(CBN_SETFOCUS, &CTTComboBox::OnCbnSetfocus)
END_MESSAGE_MAP()

void CTTComboBox::UpdateControlState()
{
	if (m_bTracking || 
		m_edit.IsMouseover() ||
		GetDroppedState())
		m_ControlState = Mouseover;
	else if (m_bHasFocus)
		m_ControlState = Focus;
	else
		m_ControlState = Normal;
	

	if (m_bOnButton || GetDroppedState())
	{
		m_ArrowButtonState = Mouseover;

		if (m_bButtonPress)
			m_ArrowButtonState = Press;
	}
	else
		m_ArrowButtonState = Normal;
}

BOOL CTTComboBox::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
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
		Invalidate(false);
	}
	else
	{
		m_bOnButton = false;
		Invalidate(false);
	}

	CComboBoxEx::OnMouseMove(nFlags, point);	
}

void CTTComboBox::OnMouseLeave()
{
	m_bTracking = m_edit.IsMouseover();
	m_bOnButton = false;
	
	Invalidate(false);

	CComboBoxEx::OnMouseLeave();	
}

void CTTComboBox::OnPaint()
{
	CRect cRect;
	GetClientRect(&cRect);
	CPaintDC dc(this); 
	CMemDC memDC(dc, cRect);

	Graphics graphics(memDC.GetDC().GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	
	DrawThemeParentBackground(GetSafeHwnd(), memDC.GetDC().GetSafeHdc(), cRect);

	UpdateControlState();

	if (!m_iComboBoxStyle)
		m_iComboBoxStyle = GetStyle() & CBS_DROPDOWNLIST;

	switch (m_iComboBoxStyle)
	{
	case CBS_DROPDOWN:
		DrawDropDown(cRect, memDC, graphics);
		break;
	case CBS_DROPDOWNLIST:
		DrawDropDownList(cRect, memDC, graphics);
		break;
	case CBS_SIMPLE:
		DrawSimple(cRect, memDC, graphics);
		break;
	}	
}

void CTTComboBox::DrawSimple(CRect &cRect, CMemDC &memDC, Gdiplus::Graphics &graphics)
{
	
}

void CTTComboBox::DrawDropDown(CRect &cRect, CMemDC &memDC, Gdiplus::Graphics &graphics)
{
	FillRectRegion(cRect, memDC, RGB(255, 255, 255, 0), m_CornerRadius);
	
	Gdiplus::Rect BorderRect(cRect.left, cRect.top, cRect.right, cRect.bottom);

	Gdiplus::Rect LightRect(cRect.left, cRect.top, cRect.right, cRect.bottom);
	LightRect.Inflate(-1, -1);

	float lightPenWidth = 2.0f;//(m_ControlState == Press || m_ControlState == Focus) ? 2.0f : 1.0f;

	// Background
	/*Draw4ColorsGradientRect(cRect, memDC,
		m_ColorMap.GetColor(m_ControlState, BackgroundTopGradientStart),
		m_ColorMap.GetColor(m_ControlState, BackgroundTopGradientFinish),
		m_ColorMap.GetColor(m_ControlState, BackgroundBottomGradientStart),
		m_ColorMap.GetColor(m_ControlState, BackgroundBottomGradientFinish),
		m_CornerRadius);*/

	// Border
	DrawRectArea(BorderRect, graphics, m_ColorMap.GetColor(m_ControlState, Border),
		m_CornerRadius, m_fBorderPenWidth);

	/*
	// Arrow button
	CRect arrowRect = GetArrowButtonRect();
	DrawArrowButton(arrowRect, memDC, graphics);


	// Highlight area
	DrawRectArea(LightRect, graphics, m_ColorMap.GetColor(m_ControlState, BorderLight),
		m_CornerRadius, lightPenWidth);

	//vertical line
	Gdiplus::Color borderColor;
	borderColor.SetFromCOLORREF(m_ColorMap.GetColor(m_ControlState, Border));
	Gdiplus::Pen borderPen(borderColor, 1);
	
	graphics.DrawLine(&borderPen, arrowRect.left, arrowRect.top, arrowRect.left, arrowRect.bottom);
	*/

}

void CTTComboBox::DrawDropDownList(CRect &cRect, CMemDC &memDC, Gdiplus::Graphics &graphics)
{

}

void CTTComboBox::DrawArrowButton(CRect &cRect, CMemDC &memDC, Gdiplus::Graphics &graphics)
{
	Draw4ColorsGradientRect(cRect, memDC,
		m_ColorMap.GetColor(m_ArrowButtonState, BackgroundTopGradientStart),
		m_ColorMap.GetColor(m_ArrowButtonState, BackgroundTopGradientFinish),
		m_ColorMap.GetColor(m_ArrowButtonState, BackgroundBottomGradientStart),
		m_ColorMap.GetColor(m_ArrowButtonState, BackgroundBottomGradientFinish),
		0);
	
	// Border
	//DrawRectArea(BorderRect, graphics, Gdiplus::Color::Green, 1, 1);//m_ColorMap.GetColor(buttonState, Border)

	Color cl;
	cl.SetFromCOLORREF(GetSysColor(COLOR_BTNTEXT));

	Pen pen(cl, 1);
	
	PointF points[3];
	points[0] = PointF(cRect.left + ((double)cRect.Width() * .25) - 1, (int)((double)cRect.Height() * .45));
	points[2] = PointF(cRect.left + ((double)cRect.Width() * .85) - 1, (int)((double)cRect.Height() * .45));
	points[1] = PointF(cRect.left + ((double)cRect.Width() * .55) - 1, (int)((double)cRect.Height() * .68));

	graphics.DrawLine(&pen, points[0], points[1]);
	graphics.DrawLine(&pen, points[1], points[2]);
	
}

CRect CTTComboBox::GetArrowButtonRect()
{
	int buttonWidth = GetSystemMetrics(SM_CXVSCROLL);
	
	CRect btnRect;
	GetClientRect(&btnRect);
	btnRect.left = btnRect.right - buttonWidth;
	btnRect.DeflateRect(-3, 2, 2, 2);

	return btnRect;
}

HBRUSH CTTComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_EDIT)
	{
		// Edit control
		if (m_edit.GetSafeHwnd() == NULL)
		{
			m_edit.SubclassWindow(pWnd->GetSafeHwnd());
			m_edit.SetMouseEventsNotification(m_iComboBoxStyle == CBS_SIMPLE);
		}
	}
	else if (nCtlColor == CTLCOLOR_LISTBOX)
	{
		// ListBox control
		if (m_listbox.GetSafeHwnd() == NULL)
		{
			m_listbox.SubclassWindow(pWnd->GetSafeHwnd());
			m_listbox.SetMouseEventsNotification(TRUE);
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
		Invalidate(false);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		m_bButtonPress = false;
		Invalidate(false);
		break;
	}
	

	return CComboBoxEx::PreTranslateMessage(pMsg);
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

