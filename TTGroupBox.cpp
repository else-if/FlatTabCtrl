#include "stdafx.h"
#include "TTGroupBox.h"
#include "CommonDrawing.h"

CTTGroupBox::CTTGroupBox()
{
	SetDrawingProperties(1, 5);

	m_CaptionTextColor = RGB(0, 0, 0);//GetSysColor(COLOR_CAPTIONTEXT);

	m_ControlState = Normal;
}

CTTGroupBox::~CTTGroupBox()
{	
}

void CTTGroupBox::SetDrawingProperties(int borderPenWidth, int cornerRadius)
{
	m_BorderPenWidth = borderPenWidth;
	m_CornerRadius = cornerRadius;
}

void CTTGroupBox::UpdateControlState()
{
	if (!IsWindowEnabled())
		m_ControlState = Disable;
	else 
		m_ControlState = Normal;
}

BEGIN_MESSAGE_MAP(CTTGroupBox, CButton)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CTTGroupBox::OnPaint()
{
	CPaintDC dc(this);
	
	CRect cRect;
	GetWindowRect(&cRect);
	ScreenToClient(&cRect);

	UpdateControlState();

	CString captionText;
	GetWindowText(captionText);
	captionText.Format(_T(" %s "), captionText);
	CSize captionSize = dc.GetTextExtent(captionText);

	int iTopOffset = (captionSize.cy / 2);

	CRect captionRect(cRect.left + OFS_LEFT, cRect.top, cRect.left + OFS_LEFT + captionSize.cx + 1, cRect.top + captionSize.cy + 1);
	CRgn captionRgn;
	captionRgn.CreateRectRgnIndirect(&captionRect);

	CRect bordRect(cRect.left, cRect.top + iTopOffset, cRect.right + 1, cRect.bottom + 1);
	CRgn borderRgn;
	borderRgn.CreateRectRgnIndirect(&bordRect);

	borderRgn.CombineRgn(&borderRgn, &captionRgn, RGN_DIFF);

	dc.SelectClipRgn(&borderRgn);

	//Borders
	CPen pen(PS_SOLID, m_BorderPenWidth, m_ColorMap.GetColor(m_ControlState, Border));
	CBrush* pOldBrush;
	CPen* pOldPen;
	pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	pOldPen = dc.SelectObject(&pen);

	dc.RoundRect(&bordRect, CPoint(m_CornerRadius, m_CornerRadius));

	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);

	// Caption
	dc.SelectClipRgn(&captionRgn);

	LOGFONT logFont;
	GetFont()->GetLogFont(&logFont);
	CFont font;	
	font.CreateFontIndirect(&logFont);

	dc.SelectObject(font);
	//dc.SetBkMode(TRANSPARENT);

	COLORREF textColor = m_ControlState == Disable ? GetSysColor(COLOR_GRAYTEXT) : m_CaptionTextColor;
	dc.SetTextColor(textColor);
	
	dc.DrawText(captionText, captionRect, DT_VCENTER | DT_LEFT | DT_SINGLELINE | DT_NOCLIP);

	/*
	DrawThemeParentBackground(GetSafeHwnd(), memDC.GetDC().GetSafeHdc(), cRect);

	UpdateButtonState(lpDrawItemStruct->itemState);

	Gdiplus::Rect BorderRect(cRect.left, cRect.top, cRect.Width(), cRect.Height());

	Gdiplus::Rect LightRect(cRect.left, cRect.top, cRect.Width(), cRect.Height());
	LightRect.Inflate(-1, -1);

	cRect.DeflateRect(1, 1, 1, 1);

	int lightPenWidth = (m_ButtonState == Press || m_ButtonState == Focus) ? 2 : 1;

	// Background
	Draw4ColorsGradientRect(cRect, memDC,
		m_ColorMap.GetColor(m_ButtonState, BackgroundTopGradientStart),
		m_ColorMap.GetColor(m_ButtonState, BackgroundTopGradientFinish),
		m_ColorMap.GetColor(m_ButtonState, BackgroundBottomGradientStart),
		m_ColorMap.GetColor(m_ButtonState, BackgroundBottomGradientFinish),
		m_CornerRadius);

	// Highlight area
	DrawRectArea(LightRect, graphics, m_ColorMap.GetColor(m_ButtonState, BorderLight),
		m_CornerRadius, lightPenWidth);

	// Border
	DrawRectArea(BorderRect, graphics, m_ColorMap.GetColor(m_ButtonState, Border),
		m_CornerRadius, m_BorderPenWidth);

	// Button text
	CString buttonText;
	GetWindowText(buttonText);
	COLORREF textColor = m_ButtonState == Disable ? GetSysColor(COLOR_GRAYTEXT) : m_CaptionTextColor;

	DrawText(cRect, memDC, m_TextFont, textColor, buttonText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	/*

	CRect rc;
	CPoint ptStart, ptEnd;
	CSize seText;
	CString sText, sTemp;
	CPen pnFrmDark, pnFrmLight, *ppnOldPen;
	int iUpDist, nSavedDC;
	DWORD dwStyle, dwExStyle;

	CPaintDC dc(this); // device context for painting
	
	// save dc state
	nSavedDC = dc.SaveDC();

	// window rect
	GetWindowRect(&rc);
	ScreenToClient(&rc);

	CMemDC memDC(dc, rc);

	

	CPen pen;
	CBrush* pOldBrush;
	CPen* pOldPen;
	if (!pen.CreatePenIndirect(&m_logpen))
		return;
	
	pOldPen = memDC.GetDC().SelectObject(&pen);
	dc.RoundRect(rc, CPoint(5, 5));
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);
	return;

	// determine text length
	GetWindowText(sTemp);
	sText.Format(_T(" %s "), sTemp); // looks better with a blank on each side	
	seText = dc.GetTextExtent(sText);

	// distance from window top to group rect
	iUpDist = (seText.cy / 2);

	// calc rect and start/end points
	dwStyle = GetStyle();
	dwExStyle = GetExStyle();

	// handle the extended windows styles
	if (dwExStyle & WS_EX_DLGMODALFRAME)
	{
		rc.DeflateRect(2 * GetSystemMetrics(SM_CXEDGE), 2 * GetSystemMetrics(SM_CYEDGE));
		rc.left--;
	}
	if (dwExStyle & WS_EX_CLIENTEDGE)
	{
		rc.DeflateRect(GetSystemMetrics(SM_CXEDGE) + 1, GetSystemMetrics(SM_CYEDGE) + 1);
		rc.left--;
		if (dwExStyle & WS_EX_DLGMODALFRAME)
		{
			rc.right++;
		}
	}
	if (dwExStyle & WS_EX_STATICEDGE)
	{
		rc.DeflateRect(2 * GetSystemMetrics(SM_CXBORDER), 2 * GetSystemMetrics(SM_CYBORDER));
		rc.left--;
	}

	// handle text alignment (Caution: BS_CENTER == BS_LEFT|BS_RIGHT!!!)
	ptStart.y = ptEnd.y = rc.top + iUpDist;
	if ((dwStyle & BS_CENTER) == BS_RIGHT) // right aligned
	{
		ptEnd.x = rc.right - OFS_X;
		ptStart.x = ptEnd.x - seText.cx;
	}
	else if ((!(dwStyle & BS_CENTER)) || ((dwStyle & BS_CENTER) == BS_LEFT))// left aligned	/ default
	{
		ptStart.x = rc.left + OFS_X;
		ptEnd.x = ptStart.x + seText.cx;
	}
	else if ((dwStyle & BS_CENTER) == BS_CENTER) // text centered
	{
		ptStart.x = (rc.Width() - seText.cx) / 2;
		ptEnd.x = ptStart.x + seText.cx;
	}


	if (dwStyle & BS_FLAT) // "flat" frame
	{
		VERIFY(pnFrmDark.CreatePen(PS_SOLID, 1, RGB(0, 0, 0)));
		VERIFY(pnFrmLight.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DHILIGHT)));

		ppnOldPen = dc.SelectObject(&pnFrmDark);

		dc.MoveTo(ptStart);
		dc.LineTo(rc.left, ptStart.y);
		dc.LineTo(rc.left, rc.bottom);
		dc.LineTo(rc.right, rc.bottom);
		dc.LineTo(rc.right, ptEnd.y);
		dc.LineTo(ptEnd);

		dc.SelectObject(&pnFrmLight);

		dc.MoveTo(ptStart.x, ptStart.y + 1);
		dc.LineTo(rc.left + 1, ptStart.y + 1);
		dc.LineTo(rc.left + 1, rc.bottom - 1);
		dc.LineTo(rc.right - 1, rc.bottom - 1);
		dc.LineTo(rc.right - 1, ptEnd.y + 1);
		dc.LineTo(ptEnd.x, ptEnd.y + 1);

	}
	else // 3D frame
	{
		VERIFY(pnFrmDark.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW)));
		VERIFY(pnFrmLight.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DHILIGHT)));

		ppnOldPen = dc.SelectObject(&pnFrmDark);
		
		dc.MoveTo(ptStart);
		dc.LineTo(rc.left, ptStart.y);
		dc.LineTo(rc.left, rc.bottom - 1);
		dc.LineTo(rc.right - 1, rc.bottom - 1);
		dc.LineTo(rc.right - 1, ptEnd.y);
		dc.LineTo(ptEnd);

		dc.SelectObject(&pnFrmLight);

		dc.MoveTo(ptStart.x, ptStart.y + 1);
		dc.LineTo(rc.left + 1, ptStart.y + 1);
		dc.LineTo(rc.left + 1, rc.bottom - 1);
		dc.MoveTo(rc.left, rc.bottom);
		dc.LineTo(rc.right, rc.bottom);
		dc.LineTo(rc.right, ptEnd.y - 1);
		dc.MoveTo(rc.right - 2, ptEnd.y + 1);
		dc.LineTo(ptEnd.x, ptEnd.y + 1);
	}

	// draw text (if any)
	if (!sText.IsEmpty() && !(dwExStyle & (BS_ICON | BS_BITMAP)))
	{
		if (!IsWindowEnabled())
		{
			ptStart.y -= iUpDist;
			dc.DrawState(ptStart, seText, sText, DSS_DISABLED, TRUE, 0, (HBRUSH)NULL);
		}
		else
		{
			dc.SetBkMode(TRANSPARENT);
			dc.DrawText(sText, CRect(ptStart, ptEnd), DT_VCENTER | DT_LEFT | DT_SINGLELINE | DT_NOCLIP);
		}
	}

	// cleanup
	dc.SelectObject(ppnOldPen);
	dc.RestoreDC(nSavedDC);*/
}
