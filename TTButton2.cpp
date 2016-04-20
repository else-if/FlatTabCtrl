#pragma once

#include "stdafx.h"
#include "TTButton2.h"
#include "ControlsColorMap.h"
#include "common.h"

using namespace Gdiplus;

IMPLEMENT_DYNAMIC(CTTButton2, CTTButton)

CTTButton2::CTTButton2()
{
	m_fBorderPenWidth = 2;
	m_CornerRadius = 10;

	m_ColorMap.SetColor(Normal, Border, RGB(153, 152, 158));
	m_ColorMap.SetColor(Focus, Border, RGB(153, 152, 158));
}

CTTButton2::~CTTButton2()
{	
}

BEGIN_MESSAGE_MAP(CTTButton2, CTTButton)
END_MESSAGE_MAP()

void CTTButton2::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC xdc;
	xdc.Attach(lpDrawItemStruct->hDC);

	CRect cRect;
	GetClientRect(&cRect);

	CMemDC memDC(xdc, cRect);

	Gdiplus::Graphics graphics(memDC.GetDC().GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);


	UpdateButtonState(lpDrawItemStruct->itemState);

	DrawThemeParentBackground(GetSafeHwnd(), memDC.GetDC().GetSafeHdc(), cRect);

	cRect.DeflateRect(1, 1, 1, 1);

	Gdiplus::Rect BorderRect(cRect.left, cRect.top, cRect.Width(), cRect.Height());

	Gdiplus::Rect LightRect(cRect.left, cRect.top, cRect.Width(), cRect.Height());
	LightRect.Inflate(-1, -1);

	cRect.DeflateRect(1, 1, 1, 1);

	float lightPenWidth = (m_ButtonState == Press || m_ButtonState == Focus) ? 2.0f : 1.0f;

	// Background
	Draw4ColorsGradientRect(cRect, memDC,
		m_ColorMap.GetColor(m_ButtonState, BackgroundTopGradientStart),
		m_ColorMap.GetColor(m_ButtonState, BackgroundTopGradientFinish),
		m_ColorMap.GetColor(m_ButtonState, BackgroundBottomGradientStart),
		m_ColorMap.GetColor(m_ButtonState, BackgroundBottomGradientFinish),
		m_CornerRadius);

	// Border
	DrawRectArea(BorderRect, graphics, m_ColorMap.GetColor(m_ButtonState, Border),
		m_CornerRadius, m_fBorderPenWidth);

	// Highlight area
	DrawRectArea(LightRect, graphics, m_ColorMap.GetColor(m_ButtonState, BorderLight),
		m_CornerRadius, lightPenWidth);

	// Button text
	CString buttonText;
	GetWindowText(buttonText);
	COLORREF textColor = m_ButtonState == Disable ? GetSysColor(COLOR_GRAYTEXT) : m_CaptionTextColor;

	DrawText(cRect, memDC, m_TextFont, textColor, buttonText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

}