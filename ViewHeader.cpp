#include "stdafx.h"
#include "ViewHeader.h"
#include "CustomMessages.h"
#include "DrawingHelper.h"

using namespace Gdiplus;

CViewHeader::CViewHeader(IView* view)
{
	m_pView = view;
	m_bCollapsed = false;

	m_nBtnItemOffset = DrawingHelper::GetDpiScaledValue(3);
	m_nButtonSize = DrawingHelper::GetDpiScaledValue(12);

	m_CurMousePt.SetPoint(0, 0);
	m_ExColBtnRect.SetRectEmpty();
	m_CloseBtnRect.SetRectEmpty();
}

CViewHeader::~CViewHeader()
{
}

void CViewHeader::SetCollapsed(bool collapsed)
{
	m_bCollapsed = collapsed;
}

void CViewHeader::InvalidateBtnRect(CRect &btnRect)
{
	CRect invRect(btnRect);
	invRect.InflateRect(1, 1);
	InvalidateRect(&invRect);
}

BEGIN_MESSAGE_MAP(CViewHeader, CStatic)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

void CViewHeader::OnPaint()
{
	CPaintDC dc(this);
	CMemDC MemDC(dc, this);

	Gdiplus::Graphics graphics(MemDC.GetDC().GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	CRect clientRect;
	GetClientRect(&clientRect);

	Gdiplus::Rect cliRect(clientRect.left, clientRect.top, clientRect.Width() - 1, clientRect.Height() - 1);

	Color backColor(255, 255, 225);
	Color borderColor(44, 61, 90);

	SolidBrush bkgndBrush(backColor);
	Gdiplus::Pen BorderPen(borderColor);
	graphics.FillRectangle(&bkgndBrush, cliRect);
	graphics.DrawRectangle(&BorderPen, cliRect);

	Gdiplus::Pen RedPen(Gdiplus::Color::Red);
	Gdiplus::Point lb(clientRect.left, clientRect.bottom-1);
	Gdiplus::Point rb(clientRect.right, clientRect.bottom-1);
	graphics.DrawLine(&RedPen, lb, rb);

	Gdiplus::Pen btnItemPen(Color::Black, 1.0f);

	m_CloseBtnRect.left = max(clientRect.left, clientRect.right - m_nButtonSize - 1);
	m_CloseBtnRect.right = m_CloseBtnRect.left + m_nButtonSize;
	m_CloseBtnRect.top = clientRect.top + 1;
	m_CloseBtnRect.bottom = m_CloseBtnRect.top + m_nButtonSize;

	m_ExColBtnRect.SetRect(clientRect.left, clientRect.top, clientRect.left + m_nButtonSize, clientRect.top + m_nButtonSize);
	m_ExColBtnRect.OffsetRect(1, 1);

	if (m_ExColBtnRect.right >= m_CloseBtnRect.left)
		m_ExColBtnRect.SetRectEmpty();

	// Draw expand / collapse button
	if (m_ExColBtnRect.PtInRect(m_CurMousePt))
	{
		Gdiplus::Rect btnRect(m_ExColBtnRect.left, m_ExColBtnRect.top, m_ExColBtnRect.Width(), m_ExColBtnRect.Height());

		Color btnBkgndColor(255, 252, 244);
		Color btnBorderColor(229, 195, 101);
		SolidBrush btnBkgndBrush(btnBkgndColor);
		Pen btnBorderPen(btnBorderColor);
		graphics.FillRectangle(&btnBkgndBrush, btnRect);
		graphics.DrawRectangle(&btnBorderPen, btnRect);
	}

	if (m_bCollapsed)
	{
		Point pt1(m_ExColBtnRect.left + m_nBtnItemOffset, m_ExColBtnRect.top + m_nBtnItemOffset);
		Point pt2(m_ExColBtnRect.right - m_nBtnItemOffset, m_ExColBtnRect.top + m_ExColBtnRect.Height() / 2);
		Point pt3(m_ExColBtnRect.left + m_nBtnItemOffset, m_ExColBtnRect.bottom - m_nBtnItemOffset);

		graphics.DrawLine(&btnItemPen, pt1, pt2);
		graphics.DrawLine(&btnItemPen, pt2, pt3);
	}
	else
	{
		Point pt1(m_ExColBtnRect.left + m_nBtnItemOffset, m_ExColBtnRect.top + m_nBtnItemOffset);
		Point pt2(m_ExColBtnRect.left + m_ExColBtnRect.Width() / 2, m_ExColBtnRect.bottom - m_nBtnItemOffset);
		Point pt3(m_ExColBtnRect.right - m_nBtnItemOffset, m_ExColBtnRect.top + m_nBtnItemOffset);

		graphics.DrawLine(&btnItemPen, pt1, pt2);
		graphics.DrawLine(&btnItemPen, pt2, pt3);
	}

	// Draw title
	Gdiplus::Font f(_T("Segoe UI"), DrawingHelper::GetDpiScaledValue(11), Gdiplus::FontStyle::FontStyleRegular, Gdiplus::Unit::UnitPixel);
	Color textColor(0, 122, 224);
	SolidBrush textBr(textColor);

	Gdiplus::RectF textLayoutRect(m_ExColBtnRect.right, clientRect.top, m_CloseBtnRect.left, clientRect.bottom);

	graphics.DrawString(m_pView->GetViewName(), m_pView->GetViewName().GetLength(), &f, textLayoutRect, Gdiplus::StringFormat::GenericDefault(), &textBr);

	// Draw close button
	if (m_CloseBtnRect.PtInRect(m_CurMousePt))
	{
		Gdiplus::Rect btnRect(m_CloseBtnRect.left, m_CloseBtnRect.top, m_CloseBtnRect.Width(), m_CloseBtnRect.Height());

		Color btnBkgndColor(255, 252, 244);
		Color btnBorderColor(229, 195, 101);
		SolidBrush btnBkgndBrush(btnBkgndColor);
		Pen btnBorderPen(btnBorderColor);
		graphics.FillRectangle(&btnBkgndBrush, btnRect);
		graphics.DrawRectangle(&btnBorderPen, btnRect);
	}

	Point pt1(m_CloseBtnRect.left + m_nBtnItemOffset, m_CloseBtnRect.top + m_nBtnItemOffset);
	Point pt2(m_CloseBtnRect.right - m_nBtnItemOffset, m_CloseBtnRect.bottom - m_nBtnItemOffset);
	Point pt3(m_CloseBtnRect.left + m_nBtnItemOffset, m_CloseBtnRect.bottom - m_nBtnItemOffset);
	Point pt4(m_CloseBtnRect.right - m_nBtnItemOffset, m_CloseBtnRect.top + m_nBtnItemOffset);

	graphics.DrawLine(&btnItemPen, pt1, pt2);
	graphics.DrawLine(&btnItemPen, pt3, pt4);

}

void CViewHeader::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_ExColBtnRect.PtInRect(point))
	{
		if (m_bCollapsed)
			GetParent()->SendMessage(TWM_VIEWEXPAND, 0, (LPARAM)m_pView);
		else
			GetParent()->SendMessage(TWM_VIEWCOLLAPCE, 0, (LPARAM)m_pView);

		m_bCollapsed = !m_bCollapsed;
		InvalidateBtnRect(m_ExColBtnRect);
	}

	CStatic::OnLButtonUp(nFlags, point);

	if (m_CloseBtnRect.PtInRect(point))
		GetParent()->SendMessage(TWM_VIEWCLOSE, 0, (LPARAM)m_pView);
}

void CViewHeader::OnMouseMove(UINT nFlags, CPoint point)
{
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = m_hWnd;
	tme.dwHoverTime = HOVER_DEFAULT;
	::TrackMouseEvent(&tme);

	if (m_CloseBtnRect.PtInRect(point) != m_CloseBtnRect.PtInRect(m_CurMousePt))
		InvalidateBtnRect(m_CloseBtnRect);

	if (m_ExColBtnRect.PtInRect(point) != m_ExColBtnRect.PtInRect(m_CurMousePt))
		InvalidateBtnRect(m_ExColBtnRect);

	m_CurMousePt = point;

	CStatic::OnMouseMove(nFlags, point);
}

void CViewHeader::OnMouseLeave()
{
	m_CurMousePt.SetPoint(0, 0);
	InvalidateBtnRect(m_CloseBtnRect);
	InvalidateBtnRect(m_ExColBtnRect);

	CStatic::OnMouseLeave();
}
