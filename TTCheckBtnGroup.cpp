#include "stdafx.h"
#include "TTCheckBtnGroup.h"
#include "CommonDrawing.h"
#include "TTButton.h"

CTTCheckBtnGroup::CTTCheckBtnGroup()
{
	SetDrawingProperties(1, 5);

	m_bTracking = false;
	m_Buttons.clear();
	m_defaultButtonWidth = 150;
}

CTTCheckBtnGroup::~CTTCheckBtnGroup()
{
}

void CTTCheckBtnGroup::SetDrawingProperties(int borderPenWidth, int cornerRadius)
{
	m_BorderPenWidth = borderPenWidth;
	m_CornerRadius = cornerRadius;
}

void CTTCheckBtnGroup::AddButton(CString text, bool isChecked)
{
	BtnInfo btnInfo;
	btnInfo.m_text = text;
	btnInfo.m_checked = isChecked;
	btnInfo.m_howered = false;
	btnInfo.m_rect.SetRectEmpty();

	m_Buttons.push_back(btnInfo);

	RecalcLayout();

	Invalidate(FALSE);
}

void CTTCheckBtnGroup::SetPressColor(COLORREF cl)
{
	m_ColorMap.SetColor(Press, BackgroundTopGradientStart, cl);
	m_ColorMap.SetColor(Press, BackgroundTopGradientFinish, cl);
	m_ColorMap.SetColor(Press, BackgroundBottomGradientStart, cl);
	m_ColorMap.SetColor(Press, BackgroundBottomGradientFinish, cl);
	Invalidate(FALSE);
}

BEGIN_MESSAGE_MAP(CTTCheckBtnGroup, CButton)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_CONTROL_REFLECT_EX(BN_CLICKED, &CTTCheckBtnGroup::OnBnClicked)
END_MESSAGE_MAP()

void CTTCheckBtnGroup::RecalcLayout()
{
	if (GetSafeHwnd() == NULL)
		return;

	CDC* pDC = GetDC();
	int totalWidth = 0;
	vector<int> buttonsWidth;
	for (int btnId = 0; btnId < m_Buttons.size(); ++btnId)
	{
		int textExt = pDC->GetTextExtent(m_Buttons[btnId].m_text).cx + 10;
		int desiredWidth = max(textExt, m_defaultButtonWidth);
		buttonsWidth.push_back(desiredWidth);
		totalWidth += desiredWidth;
	}

	CRect clientRect;
	GetClientRect(clientRect);
	clientRect.DeflateRect(1, 1);

	for (int btnId = 0; btnId < m_Buttons.size(); ++btnId)
	{
		int left = btnId == 0 ? clientRect.left : (m_Buttons[btnId - 1].m_rect.right);
		int right = (btnId == m_Buttons.size() - 1) ? clientRect.right : (left + (int)((float)buttonsWidth[btnId] / (float)totalWidth * (float)clientRect.Width()));

		m_Buttons[btnId].m_rect.SetRect(left, clientRect.top, right, clientRect.bottom);
	}
}

void CTTCheckBtnGroup::SetProperWidth(int maxWidth)
{
	if (GetSafeHwnd() == NULL)
		return;

	CDC* pDC = GetDC();
	int totalWidth = 0;
	for (int btnId = 0; btnId < m_Buttons.size(); ++btnId)
	{
		int textExt = pDC->GetTextExtent(m_Buttons[btnId].m_text).cx + 10;
		int desiredWidth = max(textExt, m_defaultButtonWidth);
		totalWidth += desiredWidth;
	}

	CRect clientRect;
	GetClientRect(clientRect);

	int newWidth = min(maxWidth, totalWidth);
	SetWindowPos(NULL, 0, 0, newWidth, clientRect.Height(), SWP_NOSENDCHANGING | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
	Invalidate();
}

bool CTTCheckBtnGroup::IsChecked(int btnId)
{
	if (btnId < 0 || btnId >= m_Buttons.size())
		return false;

	return m_Buttons[btnId].m_checked;
}

void CTTCheckBtnGroup::InvalidateButtonRect(BtnInfo* pBtnInfo)
{
	CRect invalidRect(pBtnInfo->m_rect);
	invalidRect.right++;
	invalidRect.bottom++;
	InvalidateRect(invalidRect, FALSE);
}

ControlState CTTCheckBtnGroup::GetButtonState(BtnInfo* pBtnInfo)
{
	ControlState state = Normal;
	if (pBtnInfo->m_checked)
		state = Press;
	else if (pBtnInfo->m_howered)
		state = Mouseover;

	return state;
}

void CTTCheckBtnGroup::OnPaint()
{
	CPaintDC dc(this); 

	CRect clientRect;
	GetClientRect(clientRect);

	if (clientRect.Width() <= 0 || clientRect.Height() <= 0)
		return;

	CMemDC memDC(dc, clientRect);

	CDC* pDC = &memDC.GetDC();

	DrawThemeParentBackground(GetSafeHwnd(), memDC.GetDC().GetSafeHdc(), clientRect);

	Graphics graphics(pDC->GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	graphics.SetTextRenderingHint(TextRenderingHintSingleBitPerPixel);

	// clear background
	CRgn bkgndRgn;
	CRect bkgndRect(clientRect);
	bkgndRect.left += 1;
	bkgndRect.top += 1;
	CreateRectRgnInDevicePoints(pDC, &bkgndRgn, bkgndRect, m_CornerRadius);
	pDC->SelectClipRgn(&bkgndRgn);

	pDC->FillSolidRect(&clientRect, RGB(255, 255, 255));

	CRgn clientRgn;
	CreateRectRgnInDevicePoints(pDC, &clientRgn, clientRect);
	pDC->SelectClipRgn(&clientRgn);

	// Whole border
	Gdiplus::Rect borderRect(clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height());
	borderRect.Inflate(-1, -1);

	if (m_Buttons.size() == 0)
	{
		COLORREF borderColor = m_ColorMap.GetColor(Normal, Border);
		DrawRectArea(borderRect, graphics, borderColor, m_CornerRadius, m_BorderPenWidth);
	}

	for (int btnId = 0; btnId < m_Buttons.size(); ++btnId)
	{
		BtnInfo* pBtnInfo = &m_Buttons[btnId];

		CRgn btnRgn;
		CRect t(pBtnInfo->m_rect);
		t.right += 1; t.bottom += 1;
		CreateRectRgnInDevicePoints(pDC, &btnRgn, t);
		btnRgn.CombineRgn(&btnRgn, &bkgndRgn, RGN_AND);
		pDC->SelectClipRgn(&btnRgn);
		
		ControlState buttonState = GetButtonState(pBtnInfo);

		// Highlight
		if (buttonState != Press)		
			pDC->FillSolidRect(pBtnInfo->m_rect, m_ColorMap.GetColor(buttonState, BorderLight));

		// Border
		graphics.SetClip(btnRgn);
		COLORREF borderColor = m_ColorMap.GetColor(buttonState, Border);
		DrawRectArea(borderRect, graphics, borderColor, m_CornerRadius, m_BorderPenWidth);
		graphics.ResetClip();

		CRgn offsetRgn1;
		offsetRgn1.CreateRectRgn(0, 0, 0, 0);
		offsetRgn1.CopyRgn(&btnRgn);
		offsetRgn1.OffsetRgn(m_BorderPenWidth + 1, m_BorderPenWidth + 1);

		CRgn offsetRgn2;
		offsetRgn2.CreateRectRgn(0, 0, 0, 0);
		offsetRgn2.CopyRgn(&btnRgn);
		offsetRgn2.OffsetRgn(-(m_BorderPenWidth + 1), -(m_BorderPenWidth + 1));

		CRgn backRgn;
		backRgn.CreateRectRgn(0, 0, 0, 0);
		backRgn.CombineRgn(&offsetRgn1, &offsetRgn2, RGN_AND);
		pDC->SelectClipRgn(&backRgn);

		if (buttonState != Mouseover)
		{
			DrawGradient(pDC, t);
			/*Draw4ColorsGradientRect(t, *pDC,
				m_ColorMap.GetColor(buttonState, BackgroundTopGradientStart),
				m_ColorMap.GetColor(buttonState, BackgroundTopGradientFinish),
				m_ColorMap.GetColor(buttonState, BackgroundBottomGradientStart),
				m_ColorMap.GetColor(buttonState, BackgroundBottomGradientFinish),
				m_CornerRadius, TRUE);*/
		}

		COLORREF textColor = RGB(80,80,80);//COLOR_GRAYTEXT;//COLOR_BTNTEXT;
		/*if (buttonState == Press)
			textColor = RGB(0, 122, 224);*/

		pDC->SelectClipRgn(&btnRgn);
		CFont* pFont = GetFont();
		
		int nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS;
		CTTButton::DrawCaptionText(pDC, &pBtnInfo->m_rect, &pBtnInfo->m_rect, pBtnInfo->m_text, nFormat, pFont, textColor);
	}

	pDC->SelectClipRgn(&clientRgn);
	for (int btnId = 1; btnId < m_Buttons.size(); ++btnId)
	{
		BtnInfo* pPrevBtnInfo = &m_Buttons[btnId - 1];
		BtnInfo* pCurBtnInfo = &m_Buttons[btnId];

		ControlState prevBtnState = GetButtonState(pPrevBtnInfo);
		ControlState curBtnState = GetButtonState(pCurBtnInfo);

		ControlState state = Normal;
		if (prevBtnState == Press || curBtnState == Press)
			state = Press;
		else if (prevBtnState == Mouseover || curBtnState == Mouseover)
			state = Mouseover;

		COLORREF borderColor = m_ColorMap.GetColor(state, Border);

		CPen borderPen(PS_SOLID, m_BorderPenWidth, borderColor);
		pDC->SelectObject(borderPen);

		pDC->MoveTo(pPrevBtnInfo->m_rect.right, pPrevBtnInfo->m_rect.top);
		pDC->LineTo(pPrevBtnInfo->m_rect.right, pPrevBtnInfo->m_rect.bottom);
	}
}

BOOL CTTCheckBtnGroup::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CTTCheckBtnGroup::OnSize(UINT nType, int cx, int cy)
{
	CButton::OnSize(nType, cx, cy);
	RecalcLayout();
	Invalidate(FALSE);
}

void CTTCheckBtnGroup::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = GetSafeHwnd();
		m_bTracking = ::_TrackMouseEvent(&tme);		
	}

	bool foundedHower = false;
	for (int btnId = 0; btnId < m_Buttons.size(); ++btnId)
	{
		BtnInfo* pBtnInfo = &m_Buttons[btnId];
		
		bool oldHower = pBtnInfo->m_howered;
		pBtnInfo->m_howered = !foundedHower && pBtnInfo->m_rect.PtInRect(point);

		if (oldHower != pBtnInfo->m_howered)
			InvalidateButtonRect(pBtnInfo);
		
		foundedHower = foundedHower || pBtnInfo->m_howered;
	}

	CButton::OnMouseMove(nFlags, point);
}

void CTTCheckBtnGroup::OnMouseLeave()
{
	m_bTracking = false;

	for (int btnId = 0; btnId < m_Buttons.size(); ++btnId)
	{
		BtnInfo* pBtnInfo = &m_Buttons[btnId];
		if (pBtnInfo->m_howered)
		{
			pBtnInfo->m_howered = false;
			InvalidateButtonRect(pBtnInfo);
		}
	}

	CButton::OnMouseLeave();
}

BOOL CTTCheckBtnGroup::OnBnClicked()
{
	POINT pt;
	if (!GetCursorPos(&pt))
		return TRUE;

	ScreenToClient(&pt);

	for (int btnId = 0; btnId < m_Buttons.size(); ++btnId)
	{
		BtnInfo* pBtnInfo = &m_Buttons[btnId];
		if (pBtnInfo->m_rect.PtInRect(pt))
		{
			pBtnInfo->m_howered = false;
			pBtnInfo->m_checked = !pBtnInfo->m_checked;
			InvalidateButtonRect(pBtnInfo);
			return FALSE;
		}
	}

	return TRUE;
}

void CTTCheckBtnGroup::DrawGradient(CDC* pDC, CRect rect)
{
	Color gdipBaseColor, gdipLightColor;

	COLORREF baseColor = m_ColorMap.GetColor(Press, BackgroundTopGradientStart);
	HLSColor hlsBaseColor = RgbToHls(baseColor);
	if (hlsBaseColor.L > 0.8)
	{
	
		COLORREF darkColor = GetLumColor(baseColor, -0.3);

		gdipBaseColor.SetFromCOLORREF(baseColor);
		gdipLightColor.SetFromCOLORREF(darkColor);
	}
	else
	{
		COLORREF lightColor = GetLumColor(baseColor, 0.3);

		gdipBaseColor.SetFromCOLORREF(baseColor);
		gdipLightColor.SetFromCOLORREF(lightColor);
	}	

	Graphics graphics(pDC->GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	// Draw frag rectangle
	Rect boundsRect(rect.left, rect.top, rect.Width(), rect.Height());

	GraphicsPath path;
	GetRoundRectPath(&path, boundsRect, 8);
	
	Rect upperRect(boundsRect);
	upperRect.Height = boundsRect.Height / 2;

	Rect lowerRect(boundsRect);
	lowerRect.Y = upperRect.GetBottom() - 1;
	lowerRect.Height = boundsRect.Height - upperRect.Height;

	
	Rect upGradRect(upperRect);
	upGradRect.Inflate(0, 1);

	Rect loGradRect(lowerRect);
	loGradRect.Inflate(0, 1);

	LinearGradientBrush brush1(upGradRect, gdipLightColor, gdipBaseColor, LinearGradientMode::LinearGradientModeVertical);
	LinearGradientBrush brush2(loGradRect, gdipBaseColor, gdipBaseColor, LinearGradientMode::LinearGradientModeVertical);

	//SolidBrush brush(baseColor);

	//graphics.FillRectangle(&brush, upperRect);
	//graphics.FillPath(&brush1, &path);

	Region wholeRgn(&path);

	Region upperRgn(upperRect);
	upperRgn.Intersect(&wholeRgn);

	Region lowerRgn(lowerRect);
	lowerRgn.Intersect(&wholeRgn);

	graphics.FillRegion(&brush1, &upperRgn);
	graphics.FillRegion(&brush2, &lowerRgn);

	//SolidBrush brush(gdipFragColor);
	//graphics.FillPath(&brush, &path);
}

COLORREF CTTCheckBtnGroup::GetLumColor(COLORREF baseCl, double lumOffset, double minLum, double maxLum)
{
	HLSColor hlsResult = RgbToHls(baseCl);

	hlsResult.L += lumOffset;
	if (hlsResult.L > maxLum)
		hlsResult.L = maxLum;

	if (hlsResult.L < minLum)
		hlsResult.L = minLum;

	return HLSToRGB(hlsResult);
}

// Converts a HSL color to an RGB color
// Returns an RGB color object equivalent to the HSL color object passed in
COLORREF CTTCheckBtnGroup::HLSToRGB(HLSColor hls)
{
	int R, G, B, A = 0;

	/* If S = 0, that means we are dealing with a shade
	* of gray. So, we set R, G, and B to L and exit. */

	// Special case: Gray
	if (hls.S == 0)
	{
		R = (int)(hls.L * 255);
		G = (int)(hls.L * 255);
		B = (int)(hls.L * 255);
		A = (int)(hls.A * 255);
		return RGB(R, G, B);
	}

	double t1;
	if (hls.L < 0.5)
	{
		t1 = hls.L*(1.0 + hls.S);
	}
	else
	{
		t1 = hls.L + hls.S - (hls.L * hls.S);
	}

	double t2 = 2.0*hls.L - t1;

	// Convert H from degrees to a percentage
	double h = hls.H / 360;

	// Set colors as percentage values
	double tR = h + (1.0 / 3.0);
	double r = SetColor(t1, t2, tR);

	double tG = h;
	double g = SetColor(t1, t2, tG);

	double tB = h - (1.0 / 3.0);
	double b = SetColor(t1, t2, tB);

	R = (int)(r * 255);
	G = (int)(g * 255);
	B = (int)(b * 255);
	A = (int)(A * 255);

	return RGB(R, G, B);
}

double CTTCheckBtnGroup::SetColor(double t1, double t2, double t3)
{
	if (t3 < 0) t3 += 1.0;
	if (t3 > 1) t3 -= 1.0;

	double color;
	if (6.0 * t3 < 1)
	{
		color = t2 + (t1 - t2) * 6.0 * t3;
	}
	else if (2.0 * t3 < 1)
	{
		color = t1;
	}
	else if (3.0*t3 < 2)
	{
		color = t2 + (t1 - t2) * ((2.0 / 3.0) - t3) * 6.0;
	}
	else
	{
		color = t2;
	}

	return color;
}

// Converts a WPF RGB color to an HSL color
// Returns an HSL color object equivalent to the RGB color object passed in
HLSColor CTTCheckBtnGroup::RgbToHls(COLORREF rgbColor)
{
	// Initialize result
	HLSColor hls;

	// Convert RGB values to percentages
	double r = (double)GetRValue(rgbColor) / 255;
	double g = (double)GetGValue(rgbColor) / 255;
	double b = (double)GetBValue(rgbColor) / 255;
	double a = (double)(LOBYTE((rgbColor) >> 24)) / 255;

	// Find min and max RGB values
	double min = __min(r, __min(g, b));
	double max = __max(r, __max(g, b));
	double delta = max - min;

	/* If max and min are equal, that means we are dealing with
	* a shade of gray. So we set H and S to zero, and L to either
	* max or min (it doesn't matter which), and  then we exit. */

	//Special case: Gray
	if (max == min)
	{
		hls.H = 0;
		hls.S = 0;
		hls.L = max;
		return hls;
	}

	/* If we get to this point, we know we don't have a shade of gray. */

	// Set L
	hls.L = (min + max) / 2;

	// Set S
	if (hls.L < 0.5)
	{
		hls.S = delta / (max + min);
	}
	else
	{
		hls.S = delta / (2.0 - max - min);
	}

	// Set H
	if (r == max) hls.H = (g - b) / delta;
	if (g == max) hls.H = 2.0 + (b - r) / delta;
	if (b == max) hls.H = 4.0 + (r - g) / delta;
	hls.H *= 60;
	if (hls.H < 0) hls.H += 360;

	// Set A
	hls.A = a;

	// Set return value
	return hls;
}
