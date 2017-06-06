#include "stdafx.h"
#include "TTComboBox.h"
#include "CommonDrawing.h"

using namespace Gdiplus;

CTTComboBox::CTTComboBox() :
m_oldParentRect(0, 0, 0, 0)
{
	m_bTracking = false;
	m_bButtonPress = false;
	m_bFilledArrow = false;
	m_bHasFocus = false;
	m_bOnButton = false;

	m_bUseBmp = false;

	m_iComboBoxStyle = 0;

	SetDrawingProperties(1, DrawingHelper::GetDpiScaledValue(5));

	m_ControlState = Normal;
	m_ArrowButtonState = Normal;

	m_ColorMap.SetDefaultColors();

	m_ColorMap.SetColor(Press, BackgroundTopGradientStart, RGB(250, 213, 103));
	m_ColorMap.SetColor(Press, BackgroundTopGradientFinish, RGB(244, 181, 51));
	m_ColorMap.SetColor(Press, BackgroundBottomGradientStart, RGB(255, 241, 148));
	m_ColorMap.SetColor(Press, BackgroundBottomGradientFinish, RGB(250, 213, 103));

	m_nBegin = m_nEnd = -1;
	m_LeftMouseButtonIsDown = false;
	m_bMultiSelection = false;
}

CTTComboBox::~CTTComboBox()
{
}

void CTTComboBox::SetDrawingProperties(int borderPenWidth, int cornerRadius)
{
	m_borderPenWidth = borderPenWidth;
	m_CornerRadius = cornerRadius;
}

void CTTComboBox::SetMultiSelectionMode(bool bEnableMultiSelection)
{
	m_bMultiSelection = bEnableMultiSelection;
	if (!bEnableMultiSelection)
		ClearSelectedItems();
}

bool CTTComboBox::MultiSelectionEnabled()
{
	return m_bMultiSelection;
}

void CTTComboBox::ClearSelectedItems()
{
	m_SelectedItems.clear();
	UpdateText();
}

void CTTComboBox::AddSelectedItem(int itemId)
{
	m_SelectedItems.insert(itemId);
	UpdateText();
}

void CTTComboBox::RemoveSelectedItem(int itemId)
{
	m_SelectedItems.erase(m_SelectedItems.find(itemId));
	UpdateText();
}

bool CTTComboBox::IsSelected(int itemId)
{
	return m_SelectedItems.find(itemId) != m_SelectedItems.end();
}

CString CTTComboBox::GetSelectedItemsText()
{
	CString cResultStr = _T("");

	if (MultiSelectionEnabled())
	{
		if (!m_SelectedItems.empty())
		{
			int nSelectedCount = 0;

			for (int nIndex = 0; nIndex < GetCount(); nIndex++)
			{
				if (m_SelectedItems.find(nIndex) != m_SelectedItems.end())
				{
					if (nSelectedCount > 0)
						cResultStr += ", ";
					CString cItemText;
					GetLBText(nIndex, cItemText);
					cResultStr += cItemText;
					nSelectedCount++;
				}
			}
		}
	}
	else
	{
		int nSelected = GetCurSel();
		if (nSelected != -1)
			GetLBText(nSelected, cResultStr);
	}

	return cResultStr;
}

void CTTComboBox::UpdateText()
{
	if (GetSafeHwnd() != NULL)
	{
		CString cText = GetSelectedItemsText();
		SetWindowText(cText);
	}	
}

set<int> CTTComboBox::GetMultiPersSelection()
{
	return m_SelectedItems;
}

void CTTComboBox::SetSelection(set<int> selItems)
{
	m_SelectedItems = selItems;
	UpdateText();
}

void CTTComboBox::UpdateControlState()
{
	if (!IsWindowEnabled())
		m_ControlState = Disable;
	else if (m_bTracking || m_edit.IsMouseover() || m_bHasFocus)
		m_ControlState = Mouseover;
	else if (GetDroppedState() && !(m_iComboBoxStyle & CBS_SIMPLE))
		m_ControlState = Mouseover;
	else if (GetFocus() == this || GetFocus() == &m_edit)
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

void CTTComboBox::SetEditboxPosition()
{
	if (m_edit.GetSafeHwnd() == NULL)
		return;

	CFont *pFont = GetFont();
	CRect rectText;
	rectText.SetRectEmpty();

	CDC *pDC = GetDC();

	CFont *pOld = pDC->SelectObject(pFont);
	pDC->DrawText(_T("Eb"), rectText, DT_CALCRECT | DT_LEFT);

	pDC->SelectObject(pOld);
	ReleaseDC(pDC);

	CRect cRect;
	GetClientRect(cRect);

	if (m_iComboBoxStyle == CBS_SIMPLE)
	{
		CRect cEditRect;
		m_edit.GetWindowRect(&cEditRect);
		ScreenToClient(&cEditRect);

		cRect.bottom = cEditRect.bottom + cEditRect.top;
	}

	int y = max((int)floor((cRect.Height() - rectText.Height()) / 2), m_borderPenWidth);
	int cy = min(rectText.Height(), cRect.Height() - 2 * m_borderPenWidth);
	int horizontalOffset = m_borderPenWidth + 2;

	m_edit.SetWindowPos(NULL,
		cRect.left + horizontalOffset,
		y,
		cRect.Width() - horizontalOffset - GetSystemMetrics(SM_CXVSCROLL),
		cy,
		SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOACTIVATE);
}

void CTTComboBox::DrawComboBoxControl(CDC* pDC, CRect* pRect, CRect* pClipRect, int nStyle, ControlState controlState,
	ControlState backgrougState, ControlState arrowButtonState, const CString& itemText, CFont* pFont, int cornerRadius,
	int borderWidth, ControlsColorMap* pColorMap)
{
	switch (nStyle)
	{
	case CBS_DROPDOWN:
		DrawDropDown(pDC, pRect, pClipRect, controlState,
			arrowButtonState, cornerRadius, borderWidth, pColorMap);
		break;
	case CBS_DROPDOWNLIST:
		DrawDropDownList(pDC, pRect, pClipRect, controlState, backgrougState,
			arrowButtonState, itemText, pFont, cornerRadius, borderWidth, pColorMap);
		break;
	case CBS_SIMPLE:
		DrawSimple(pDC, pRect, pClipRect, controlState, cornerRadius, borderWidth, pColorMap);
		break;
	}
}

void CTTComboBox::DrawSimple(CDC* pDC, CRect* pRect, CRect* pClipRect, ControlState controlState,
	int cornerRadius, int borderWidth, ControlsColorMap* pColorMap)
{
	if (pDC == NULL)
		return;

	if (pRect == NULL || pClipRect == NULL)
		return;

	int nSave = pDC->SaveDC();

	Graphics graphics(pDC->GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	bool bDeleteColorMap = false;
	if (pColorMap == NULL)
	{
		pColorMap = new ControlsColorMap();
		bDeleteColorMap = true;
	}

	CRgn clipRgn;
	CreateRectRgnInDevicePoints(pDC, &clipRgn, *pClipRect);

	pDC->SelectClipRgn(&clipRgn);

	CRect cRect(pRect);

	// Clear background
	FillRectRegion(cRect, *pDC, RGB(255, 255, 255), cornerRadius);

	// Borders
	Gdiplus::Rect BorderRect(cRect.left, cRect.top, cRect.Width(), cRect.Height());

	DrawRectArea(BorderRect, graphics, pColorMap->GetColor(controlState, Border),
		cornerRadius, borderWidth);

	pDC->RestoreDC(nSave);

	if (bDeleteColorMap && pColorMap != NULL)
	{
		delete pColorMap;
		pColorMap = NULL;
	}
}

void CTTComboBox::DrawDropDown(CDC* pDC, CRect* pRect, CRect* pClipRect, ControlState controlState,
	ControlState arrowButtonState, int cornerRadius, int borderWidth, ControlsColorMap* pColorMap)
{
	if (pDC == NULL)
		return;

	if (pRect == NULL || pClipRect == NULL)
		return;

	int nSave = pDC->SaveDC();

	Graphics graphics(pDC->GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	bool bDeleteColorMap = false;
	if (pColorMap == NULL)
	{
		pColorMap = new ControlsColorMap();
		bDeleteColorMap = true;
	}

	CRgn clipRgn;
	CreateRectRgnInDevicePoints(pDC, &clipRgn, *pClipRect);

	pDC->SelectClipRgn(&clipRgn);

	CRect cRect(pRect);

	// Clear background
	FillRectRegion(cRect, *pDC, RGB(255, 255, 255), cornerRadius);

	// Borders
	Gdiplus::Rect BorderRect(cRect.left, cRect.top, cRect.Width(), cRect.Height());

	DrawRectArea(BorderRect, graphics, pColorMap->GetColor(controlState, Border),
		cornerRadius, borderWidth);

	pDC->RestoreDC(nSave);

	CRect btnRect = GetArrowButtonRect(&cRect);
	CTTComboBox::DrawArrowButon(pDC, &btnRect, &btnRect, arrowButtonState, controlState,
		cornerRadius, borderWidth, pColorMap, FALSE);

	if (bDeleteColorMap && pColorMap != NULL)
	{
		delete pColorMap;
		pColorMap = NULL;
	}
}

void CTTComboBox::DrawDropDownList(CDC* pDC, CRect* pRect, CRect* pClipRect, ControlState controlState,
	ControlState backgrougState, ControlState arrowButtonState, const CString& itemText, CFont* pFont,
	int cornerRadius, int borderWidth, ControlsColorMap* pColorMap)
{
	if (pDC == NULL)
		return;

	if (pRect == NULL || pClipRect == NULL)
		return;

	int nSave = pDC->SaveDC();

	Graphics graphics(pDC->GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	bool bDeleteColorMap = false;
	if (pColorMap == NULL)
	{
		pColorMap = new ControlsColorMap();
		bDeleteColorMap = true;
	}

	CRgn clipRgn;
	CreateRectRgnInDevicePoints(pDC, &clipRgn, *pClipRect);

	pDC->SelectClipRgn(&clipRgn);

	CRect cRect(pRect);

	// Draw background
	Draw4ColorsGradientRect(cRect, *pDC,
		pColorMap->GetColor(backgrougState, BackgroundTopGradientStart),
		pColorMap->GetColor(backgrougState, BackgroundTopGradientFinish),
		pColorMap->GetColor(backgrougState, BackgroundBottomGradientStart),
		pColorMap->GetColor(backgrougState, BackgroundBottomGradientFinish),
		cornerRadius);

	// Borders
	Gdiplus::Rect BorderRect(cRect.left, cRect.top, cRect.Width(), cRect.Height());

	DrawRectArea(BorderRect, graphics, pColorMap->GetColor(controlState, Border),
		cornerRadius, borderWidth);

	// Selected item text
	DrawSelectedItemText(pDC, &cRect, itemText, controlState, borderWidth, pFont);

	pDC->RestoreDC(nSave);

	CRect btnRect = GetArrowButtonRect(&cRect);
	CTTComboBox::DrawArrowButon(pDC, &btnRect, &btnRect, arrowButtonState, controlState,
		cornerRadius, borderWidth, pColorMap, FALSE);

	if (bDeleteColorMap && pColorMap != NULL)
	{
		delete pColorMap;
		pColorMap = NULL;
	}
}

void CTTComboBox::DrawSelectedItemText(CDC* pDC, CRect* pRect, const CString& itemText, ControlState controlState,
	int borderWidth, CFont* pFont)
{
	if (pDC == NULL)
		return;

	if (pRect == NULL)
		return;

	if (itemText.IsEmpty())
		return;

	COLORREF textColor = controlState == Disable ? GetSysColor(COLOR_GRAYTEXT) : GetSysColor(COLOR_CAPTIONTEXT);

	CRect cTextRect(pRect);
	CRect btnRect = GetArrowButtonRect(pRect);
	cTextRect.right -= btnRect.Width();
	cTextRect.DeflateRect(borderWidth + 2, 0);
	DrawText(cTextRect, *pDC, *pFont, textColor, itemText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
}

CRect CTTComboBox::GetArrowButtonRect(CRect* clientRect)
{
	int buttonWidth = GetSystemMetrics(SM_CXVSCROLL);

	CRect btnRect(clientRect);
	btnRect.left = btnRect.right - buttonWidth;

	return btnRect;
}

void CTTComboBox::DrawArrowButon(CDC *pDC, CRect* pRect, CRect* pClipRect, ControlState arrowButtonState,
	ControlState controlState, int cornerRadius, int borderWidth, ControlsColorMap* pColorMap, BOOL filledArrow)
{
	if (pDC == NULL)
		return;

	int nSave = pDC->SaveDC();

	Graphics graphics(pDC->GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	bool bDeleteColorMap = false;
	if (pColorMap == NULL)
	{
		pColorMap = new ControlsColorMap();
		bDeleteColorMap = true;
	}

	COLORREF arrowColorStart = RGB(96, 96, 96);
	COLORREF arrowColorFinish = RGB(96, 96, 96);

	CRgn clipRgn;
	CreateRectRgnInDevicePoints(pDC, &clipRgn, *pClipRect);

	// Select button region
	CRect btnRect(pRect);
	CRgn btnRgn, RoundRectRgn;
	CreateRectRgnInDevicePoints(pDC, &btnRgn, btnRect.left, btnRect.top, btnRect.right + 1, btnRect.bottom + 1);
	CreateRectRgnInDevicePoints(pDC, &RoundRectRgn, pRect->left, pRect->top, pRect->right + 1, pRect->bottom + 1, cornerRadius);

	// CRgn btnRgn;
	btnRgn.CombineRgn(&btnRgn, &RoundRectRgn, RGN_AND);
	btnRgn.CombineRgn(&btnRgn, &clipRgn, RGN_AND);

	pDC->SelectClipRgn(&btnRgn);

	// Draw button background
	Draw4ColorsGradientRect(btnRect, *pDC,
		pColorMap->GetColor(arrowButtonState, BackgroundTopGradientStart),
		pColorMap->GetColor(arrowButtonState, BackgroundTopGradientFinish),
		pColorMap->GetColor(arrowButtonState, BackgroundBottomGradientStart),
		pColorMap->GetColor(arrowButtonState, BackgroundBottomGradientFinish),
		0, TRUE);

	// Draw borders
	if (borderWidth > 0)
	{
		CRgn RectRgn;
		CreateRectRgnInDevicePoints(pDC, &RectRgn, btnRect.left, btnRect.top, btnRect.right, btnRect.bottom);
		RectRgn.CombineRgn(&RectRgn, &clipRgn, RGN_AND);

		graphics.SetClip((HRGN)RectRgn);

		// Overwrite combo border
		Gdiplus::Rect BorderRect(btnRect.left - cornerRadius, btnRect.top, btnRect.Width() + cornerRadius, btnRect.Height());

		DrawRectArea(BorderRect, graphics, pColorMap->GetColor(controlState, Border),
			cornerRadius, borderWidth);

		// Draw left vertical border

		Color penColor;
		penColor.SetFromCOLORREF(pColorMap->GetColor(controlState, Border));
		Pen pen(penColor, 1);

		graphics.DrawLine(&pen, btnRect.left, btnRect.top, btnRect.left, btnRect.bottom);
	}

	// Draw arrow
	if (filledArrow)
	{
		// Filled Arrow
		int iLeft = (int)(btnRect.left + 0.25 * (double)btnRect.Width() + 1);
		int iRight = (int)(btnRect.left + 0.75 * (double)btnRect.Width() + 1);
		int iHeight = (iRight - iLeft) / 2;
		int iTop = (btnRect.top + btnRect.Height() / 2 - iHeight / 2) + 1;
		int iBottom = iTop + iHeight;
		int iMiddle = iLeft + ((iRight - iLeft) / 2);

		CRect ArrowRect(iLeft, iTop, iRight, iBottom);

		CPoint points[3];
		points[0] = CPoint(iLeft, iTop);
		points[1] = CPoint(iRight, iTop);
		points[2] = CPoint(iMiddle, iBottom);

		CRgn ArrowWhiteRgn;
		CreatePolygonRgnInDevicePoints(pDC, &ArrowWhiteRgn, points, 3, ALTERNATE);
		ArrowWhiteRgn.CombineRgn(&ArrowWhiteRgn, &clipRgn, RGN_AND);

		pDC->SelectClipRgn(&ArrowWhiteRgn);
		FillRectRegion(btnRect, *pDC, RGB(255, 255, 255), 1, TRUE);

		CRgn ArrowRgn;
		CreatePolygonRgnInDevicePoints(pDC, &ArrowRgn, points, 3, ALTERNATE);
		ArrowRgn.OffsetRgn(-1, 0);

		ArrowRgn.CombineRgn(&ArrowRgn, &ArrowWhiteRgn, RGN_AND);
		ArrowRgn.CombineRgn(&ArrowWhiteRgn, &clipRgn, RGN_AND);

		pDC->SelectClipRgn(&ArrowRgn);

		Draw2ColorsGradientRect(ArrowRect, *pDC, arrowColorStart, arrowColorFinish, 0, TRUE);
	}
	else
	{
		// Unfilled arrow, like "V"
		int iLeft = (int)(btnRect.left + 0.25 * (double)btnRect.Width());
		int iRight = (int)(btnRect.left + 0.75 * (double)btnRect.Width() + 1);
		int iHeight = (iRight - iLeft) / 2;
		int iTop = (btnRect.top + btnRect.Height() / 2 - iHeight / 2) + 1;
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
		CreatePolygonRgnInDevicePoints(pDC, &ArrowRgn, points, 4, ALTERNATE);

		points[0].x += 2;
		points[1].x -= 2;
		points[2].y -= 2;
		points[3].y -= 2;

		CRgn ArrowInnerRgn;
		CreatePolygonRgnInDevicePoints(pDC, &ArrowInnerRgn, points, 4, ALTERNATE);

		ArrowRgn.CombineRgn(&ArrowRgn, &ArrowInnerRgn, RGN_DIFF);
		ArrowRgn.CombineRgn(&ArrowRgn, &clipRgn, RGN_AND);

		pDC->SelectClipRgn(&ArrowRgn);

		Draw2ColorsGradientRect(ArrowRect, *pDC, arrowColorStart, arrowColorFinish, 0, TRUE);
	}

	pDC->RestoreDC(nSave);

	if (bDeleteColorMap && pColorMap != NULL)
		delete pColorMap;
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
	ON_MESSAGE(CB_SETCURSEL, &CTTComboBox::OnCbSetcursel)
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_MESSAGE(CB_SETITEMHEIGHT, &CTTComboBox::OnCbSetitemheight)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CTTComboBox::OnCbnSelchange)
END_MESSAGE_MAP()

BOOL CTTComboBox::OnEraseBkgnd(CDC* pDC)
{
	if (!m_bUseBmp){
		CRect Rect;
		GetWindowRect(&Rect);
		CWnd *pParent = GetParent();
		pParent->ScreenToClient(&Rect);
		CDC *pParentDC = pParent->GetDC();

		m_dc.DeleteDC();
		// store into m_dc
		CBitmap bmp;
		m_dc.CreateCompatibleDC(pParentDC);
		bmp.CreateCompatibleBitmap(pParentDC, Rect.Width(), Rect.Height());
		m_dc.SelectObject(&bmp);
		m_dc.BitBlt(0, 0, Rect.Width(), Rect.Height(), pParentDC, Rect.left, Rect.top, SRCCOPY);
		bmp.DeleteObject();

		m_bUseBmp = true;

		pParent->ReleaseDC(pParentDC);
	}

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

	CRect clientRect;
	GetClientRect(&clientRect);
	if (GetArrowButtonRect(&clientRect).PtInRect(point))
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
	CWnd *pWnd = GetParent();
	if (pWnd != NULL)
	{
		GetWindowRect(m_oldParentRect);
		::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&m_oldParentRect, 2);
	}

	CRect cRect;
	GetClientRect(&cRect);

	if (m_iComboBoxStyle == CBS_SIMPLE)
	{
		// Calculate borders rect
		if (!m_edit)
			return;

		CRect cEditRect;
		m_edit.GetWindowRect(&cEditRect);
		ScreenToClient(&cEditRect);

		cRect.bottom = cEditRect.bottom + cEditRect.top;
	}

	CPaintDC dc(this);
	CMemDC memDC(dc, cRect);

	UpdateControlState();

	if (m_bUseBmp)
	{
		memDC.GetDC().BitBlt(cRect.left, cRect.top, cRect.Width(), cRect.Height(),
			&m_dc, 0, 0, SRCCOPY);
	}

	ControlState backgroundState = m_ControlState;
	if (m_ControlState != Disable && (!m_bTracking || m_bOnButton))
		backgroundState = Normal;

	CString cSelText = GetSelectedItemsText();
	
	LOGFONT logFont;
	GetFont()->GetLogFont(&logFont);
	CFont font;
	font.CreateFontIndirect(&logFont);

	DrawComboBoxControl(&memDC.GetDC(), &cRect, &cRect, m_iComboBoxStyle, m_ControlState, backgroundState,
		m_ArrowButtonState, cSelText, &font, m_CornerRadius, m_borderPenWidth, &m_ColorMap);

}

HBRUSH CTTComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_EDIT)
	{
		// Edit control
		if (m_edit.GetSafeHwnd() == NULL)
		{
			m_edit.SubclassWindow(pWnd->GetSafeHwnd());

			// for Simple type combo box we need Invalidate on Edit's mouse leave
			m_edit.InvalidateParentOnMouseLeave((GetStyle() & CBS_DROPDOWNLIST) == CBS_SIMPLE);

			SetEditboxPosition();

			Invalidate();
		}
	}
	else if (nCtlColor == CTLCOLOR_LISTBOX)
	{
		// ListBox control
		if (m_listbox.GetSafeHwnd() == NULL)
		{
			m_listbox.m_pParent = this;
			m_listbox.SubclassWindow(pWnd->GetSafeHwnd());
			Invalidate();
		}

		CRect cCurRect;
		HWND hWnd = m_listbox.GetSafeHwnd();
		::GetWindowRect(hWnd, cCurRect);

		CRect cNewRect(cCurRect);
		int droppedWidth = GetDroppedWidth();

		CRect cParentRect;
		GetParent()->GetWindowRect(&cParentRect);

		CRect cWndRect;
		GetWindowRect(cWndRect);

		int screenWidth = GetSystemMetrics(SM_CXSCREEN);

		if (cWndRect.left + droppedWidth > min(cParentRect.right, screenWidth) &&
			cWndRect.right - droppedWidth > 0)
		{
			// if dropped list goes out of right bound of the parent window - expand it on the left side
			cNewRect.left = min(cWndRect.left, cWndRect.right - droppedWidth);
			cNewRect.right = cWndRect.right;
		}
		else
		{
			// expand dropped list on the right side 
			cNewRect.left = cWndRect.left;
			cNewRect.right = max(cWndRect.left + droppedWidth, cWndRect.right);
		}

		if (!cCurRect.EqualRect(cNewRect))
			::MoveWindow(hWnd, cNewRect.left, cNewRect.top, cNewRect.Width(), cNewRect.Height(), TRUE);

	}

	hbr = (HBRUSH)GetStockObject(WHITE_BRUSH);

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
		m_LeftMouseButtonIsDown = true;
		m_bButtonPress = m_bOnButton;
		break;
	case WM_LBUTTONUP:
		m_LeftMouseButtonIsDown = false;
		m_bButtonPress = false;
		break;
	}

	if (!GetDroppedState())
		m_nBegin = m_nEnd = -1;

	if (!MultiSelectionEnabled())
		return __super::PreTranslateMessage(pMsg);

	// Handling messages for Multi selected mode
	if (pMsg->message == WM_KEYDOWN)
	{
		UINT nChar = pMsg->wParam;

		if (GetDroppedState())
		{
			if (nChar == VK_RETURN || nChar == VK_ESCAPE || nChar == VK_TAB)
			{	
				if (!m_SelectedItems.empty())
				{
					if (m_SelectedItems.size() == 1)
						SetCurSel(*m_SelectedItems.begin());
					
					ShowDropDown(FALSE);
					if (nChar == VK_TAB)
						return __super::PreTranslateMessage(pMsg);
					
					return TRUE;
				}
			}

			if ((GetKeyState(VK_LSHIFT) & 0x8000) || (GetKeyState(VK_RSHIFT) & 0x8000))
			{
				if (m_nBegin == -1)
				{
					m_nBegin = GetCurSel();
					m_nEnd = m_nBegin;
				}

				if (nChar == VK_UP)
				{
					if ((m_nEnd - 1) > 0)
						m_nEnd--;
					if (m_nEnd <= m_nBegin)
					{
						for (int i = m_nEnd; i <= m_nBegin; i++)
							if (i != -1)
								m_SelectedItems.insert(i);
					}
					else
					{
						for (int i = m_nBegin; i <= m_nEnd; i++)
							if (i != -1)
								m_SelectedItems.insert(i);
					}
					UpdateText();

					m_listbox.SetCurSel(m_nEnd);
					m_listbox.Invalidate();
					return TRUE;
				}
				else if (nChar == VK_DOWN)
				{
					if ((m_nEnd + 1) < GetCount())
						m_nEnd++;
					if (m_nBegin <= m_nEnd)
					{
						for (int i = m_nBegin; i <= m_nEnd; i++)
							if (i != -1)
								m_SelectedItems.insert(i);
					}
					else
					{
						for (int i = m_nEnd; i <= m_nBegin; i++)
							if (i != -1)
								m_SelectedItems.insert(i);
					}
					UpdateText();
					m_listbox.SetCurSel(m_nEnd);
					m_listbox.Invalidate();					
					return TRUE;
				}
			}

			// - Check if any CTRL-button is down to select an item.
			else if ((GetKeyState(VK_LCONTROL) & 0x8000) || (GetKeyState(VK_RCONTROL) & 0x8000)
				&& m_LeftMouseButtonIsDown)
			{
				// Only handle CTRL-selections if the left mouse button is down simultaneously
				if (!m_LeftMouseButtonIsDown)
				{
					return TRUE;
				}

				if (m_nBegin == -1)
				{
					m_nBegin = GetCurSel();
					m_nEnd = m_nBegin;
					if (m_nBegin != -1)
					{
						m_SelectedItems.insert(m_nBegin);
						m_listbox.SetCurSel(m_nBegin);
					}
				}
				if (nChar == VK_UP)
				{
					if ((m_nEnd - 1) > 0)
						m_nEnd--;

					m_listbox.SetCurSel(m_nEnd);
					m_listbox.Invalidate();					
					return TRUE;
				}
				else if (nChar == VK_DOWN)
				{
					if ((m_nEnd + 1) < GetCount())
						m_nEnd++;

					m_listbox.SetCurSel(m_nEnd);
					m_listbox.Invalidate();					
					return TRUE;
				}
				else if (nChar == VK_SPACE)
				{
					m_nBegin = GetCurSel();
					m_nEnd = m_nBegin;
					int itemId = m_nBegin;
					if (itemId != -1)
					{
						if (m_SelectedItems.find(itemId) != m_SelectedItems.end())
							m_SelectedItems.erase(m_SelectedItems.find(itemId));
						else
							m_SelectedItems.insert(itemId);
					}
					UpdateText();
					m_listbox.SetCurSel(m_nBegin);
					m_listbox.Invalidate();					
					return TRUE;
				}
			}
			else
			{
				m_nBegin = m_nEnd = -1;
				if (m_SelectedItems.size() > 0)
					ClearSelectedItems();
				m_listbox.Invalidate();
			}
		}
	}

	return __super::PreTranslateMessage(pMsg);
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

void CTTComboBox::PreSubclassWindow()
{
	m_iComboBoxStyle = GetStyle() & CBS_DROPDOWNLIST;

	CFont* font = GetParent()->GetFont();

	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(font);
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	SetItemHeight(-1, tm.tmHeight + 2);

	CComboBox::PreSubclassWindow();
}

afx_msg LRESULT CTTComboBox::OnCbSetcursel(WPARAM wParam, LPARAM lParam)
{
	LRESULT Result = Default();
	
	if (MultiSelectionEnabled())
		UpdateText();

	Invalidate(FALSE);

	return Result;
}

void CTTComboBox::OnMove(int x, int y)
{
	if (!::IsWindow(GetSafeHwnd()))
		return;

	SetEditboxPosition();

	// Invalidate current client region
	m_bUseBmp = false;
	Invalidate();

	// Invalidate changed parent regions
	CWnd *pWnd = GetParent();
	if (pWnd != NULL)
	{
		CRect oldWindowRect, curWindowRect;

		GetWindowRect(curWindowRect);
		oldWindowRect.CopyRect(m_oldParentRect);

		::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&curWindowRect, 2);

		InvalidateRectRegions(pWnd, oldWindowRect, curWindowRect, RGN_XOR);
	}
}

void CTTComboBox::OnSize(UINT nType, int cx, int cy)
{
	CComboBox::OnSize(nType, cx, cy);

	SetEditboxPosition();

	// Invalidate current client region
	m_bUseBmp = false;
	Invalidate();

	// Invalidate changed parent region
	CWnd *pWnd = GetParent();
	if (pWnd != NULL)
	{
		CRect oldWindowRect, curWindowRect;

		GetWindowRect(curWindowRect);
		oldWindowRect.CopyRect(m_oldParentRect);

		::MapWindowPoints(HWND_DESKTOP, pWnd->GetSafeHwnd(), (LPPOINT)&curWindowRect, 2);

		InvalidateRectRegions(pWnd, oldWindowRect, curWindowRect, RGN_XOR);
	}
}

afx_msg LRESULT CTTComboBox::OnCbSetitemheight(WPARAM wParam, LPARAM lParam)
{
	LRESULT result = Default();

	SetEditboxPosition();

	m_bUseBmp = false;
	Invalidate();

	return result;
}

void CTTComboBox::OnCbnSelchange()
{
	Default();

	if (MultiSelectionEnabled())
	{
		m_SelectedItems.clear();
		int itemId = GetCurSel();

		m_nBegin = m_nEnd = itemId;
		if (itemId != -1)
		{
			AddSelectedItem(itemId);
			Invalidate();
		}
	}
}

void CTTComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->CtlType = ODT_LISTBOX && lpDrawItemStruct->itemID < GetCount())
	{
		int nIndex = lpDrawItemStruct->itemID;
		CString cStr;
		GetLBText(lpDrawItemStruct->itemID, cStr);

		CDC dc;

		dc.Attach(lpDrawItemStruct->hDC);

		// Save these value to restore them when done drawing.
		COLORREF crOldTextColor = dc.GetTextColor();
		COLORREF crOldBkColor = dc.GetBkColor();

		// If this item is selected, set the background color 
		// and the text color to appropriate values. Erase
		// the rect by filling it with the background color.
		if ((lpDrawItemStruct->itemAction & (ODA_SELECT | ODA_FOCUS)) && (lpDrawItemStruct->itemState & ODS_SELECTED))
		{
			dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
			dc.SetBkColor(::GetSysColor(COLOR_HOTLIGHT));
			dc.FillSolidRect(&lpDrawItemStruct->rcItem, ::GetSysColor(COLOR_HOTLIGHT));
		}
		else if (m_SelectedItems.find(nIndex) != m_SelectedItems.end())
		{
			dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
			dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
			dc.FillSolidRect(&lpDrawItemStruct->rcItem, ::GetSysColor(COLOR_HIGHLIGHT));
			if (m_nBegin != -1 && m_nEnd == nIndex)
				dc.DrawFocusRect(&lpDrawItemStruct->rcItem);
		}
		else if (m_nBegin != -1 && m_nEnd == nIndex)
		{
			dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);
			dc.DrawFocusRect(&lpDrawItemStruct->rcItem);
		}
		else
		{
			dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);
		}

		// Draw the text.
		dc.DrawText(cStr, &lpDrawItemStruct->rcItem, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

		// Reset the background color and the text color back to their
		// original values.
		dc.SetTextColor(crOldTextColor);
		dc.SetBkColor(crOldBkColor);

		dc.Detach();
	}
}
