#include "stdafx.h"
#include "TTBarTabCtrl.h"
#include "CommonDrawing.h"

CTTBarTabCtrl::CTTBarTabCtrl()
{
	m_bUseDefaultColor = false;
	SetDrawingProperties(1, 5);
	InitColorMap();	
}

CTTBarTabCtrl::~CTTBarTabCtrl()
{
}

void CTTBarTabCtrl::SetDrawingProperties(int borderPenWidth, int cornerRadius)
{
	m_nBorderSize = borderPenWidth;
	m_CornerRadius = cornerRadius;
}

void CTTBarTabCtrl::InitColorMap()
{
	m_ColorMap.SetDefaultColors();

	if (!m_bUseDefaultColor)
	{
		m_ColorMap.SetColor(Normal, BackgroundTopGradientStart, RGB(217, 220, 224));
		m_ColorMap.SetColor(Normal, BackgroundTopGradientFinish, RGB(241, 242, 245));
		m_ColorMap.SetColor(Normal, BackgroundBottomGradientStart, RGB(217, 220, 224));
		m_ColorMap.SetColor(Normal, BackgroundBottomGradientFinish, RGB(217, 220, 224));

		m_ColorMap.SetColor(Mouseover, BackgroundTopGradientStart, RGB(254, 215, 104));
		m_ColorMap.SetColor(Mouseover, BackgroundTopGradientFinish, RGB(255, 220, 129));
		m_ColorMap.SetColor(Mouseover, BackgroundBottomGradientStart, RGB(254, 240, 175));
		m_ColorMap.SetColor(Mouseover, BackgroundBottomGradientFinish, RGB(254, 215, 104));

		m_ColorMap.SetColor(Press, BackgroundTopGradientStart, RGB(254, 169, 4));
		m_ColorMap.SetColor(Press, BackgroundTopGradientFinish, RGB(255, 223, 161));
		m_ColorMap.SetColor(Press, BackgroundBottomGradientStart, RGB(255, 221, 110));
		m_ColorMap.SetColor(Press, BackgroundBottomGradientFinish, RGB(254, 169, 4));
	}
}

void CTTBarTabCtrl::DrawTabButton(CDC& dc, int iButtonIdx, BOOL bDrawPressedButton)
{
	CMFCTabInfo* pTab = (CMFCTabInfo*)m_arTabs[iButtonIdx];
	ASSERT_VALID(pTab);

	CRect rectBtn = pTab->GetRect();

	if (rectBtn.IsRectEmpty())
	{
		return;
	}

	BOOL bIsHighlighted = (iButtonIdx == m_iHighlighted);
	BOOL bIsPressed = (iButtonIdx == m_iPressed) && bDrawPressedButton;
	BOOL bIsActive = (iButtonIdx == m_iActiveTab);

	if (IsMode2003() && bIsActive)
	{
		bIsPressed = TRUE;
	}

	COLORREF clrBtnText = GetGlobalData()->clrBarText;
	
	ControlState ctrlState = Normal;
	if (bIsPressed)
		ctrlState = Press;
	else if (bIsHighlighted)
		ctrlState = Mouseover;	

	Draw4ColorsGradientRect(rectBtn, dc,
		m_ColorMap.GetColor(ctrlState, BackgroundTopGradientStart),
		m_ColorMap.GetColor(ctrlState, BackgroundTopGradientFinish),
		m_ColorMap.GetColor(ctrlState, BackgroundBottomGradientStart),
		m_ColorMap.GetColor(ctrlState, BackgroundBottomGradientFinish),
		0, TRUE);

	CRect borderRect(rectBtn);
	borderRect.InflateRect(1, 1, 0, 0);
	dc.Draw3dRect(borderRect, m_ColorMap.GetColor(Normal, Border), m_ColorMap.GetColor(Normal, Border));

	//---------------
	// Draw tab icon:
	//---------------
	CSize sizeImage = GetImageSize();
	UINT uiIcon = GetTabIcon(iButtonIdx);
	HICON hIcon = GetTabHicon(iButtonIdx);

	if (uiIcon == (UINT)-1 && hIcon == NULL)
	{
		sizeImage.cx = 0;
	}

	if (sizeImage.cx + CMFCBaseTabCtrl::AFX_TAB_IMAGE_MARGIN <= rectBtn.Width())
	{
		CRect rectImage = rectBtn;

		rectImage.top += (rectBtn.Height() - sizeImage.cy) / 2;
		rectImage.bottom = rectImage.top + sizeImage.cy;

		rectImage.left += AFX_IMAGE_MARGIN;
		rectImage.right = rectImage.left + sizeImage.cx;

		if (hIcon != NULL)
		{
			//---------------------
			// Draw the tab's icon:
			//---------------------
			dc.DrawState(rectImage.TopLeft(), rectImage.Size(), hIcon, DSS_NORMAL, (HBRUSH)NULL);
		}
		else
		{
			const CImageList* pImageList = GetImageList();
			if (pImageList != NULL && uiIcon != (UINT)-1)
			{
				ASSERT_VALID(pImageList);

				//----------------------
				// Draw the tab's image:
				//----------------------
				((CImageList*)pImageList)->Draw(&dc, uiIcon, rectImage.TopLeft(), ILD_TRANSPARENT);
			}
		}
	}

#define AFX_TEXT_MARGIN 4
#define AFX_GRIPPER_MARGIN 4

	//---------------
	// Draw tab text:
	//---------------
	dc.SetTextColor(clrBtnText);

	CRect rcText = pTab->GetRect();

	if (IsTabDetachable(iButtonIdx) && !IsMode2003())
	{
		rcText.right -= AFX_CX_GRIPPER + AFX_GRIPPER_MARGIN * 2;
	}

	rcText.left += sizeImage.cx + 2 * AFX_TEXT_MARGIN;

	UINT uiDTFlags = DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;

	if (IsMode2003())
	{
		uiDTFlags |= DT_LEFT;
	}
	else
	{
		switch (m_nPageButtonTextAlign)
		{
		case TA_LEFT:
			uiDTFlags |= DT_LEFT;
			break;

		case TA_CENTER:
		default:
			uiDTFlags |= DT_CENTER;
			break;

		case TA_RIGHT:
			uiDTFlags |= DT_RIGHT;
			break;
		}
	}

	CString tabLabel;
	GetTabLabel(iButtonIdx, tabLabel);
	dc.DrawText(tabLabel, rcText, uiDTFlags);

	if (IsTabDetachable(iButtonIdx) && !IsMode2003())
	{
		//--------------
		// Draw gripper:
		//--------------
		CRect rectGripper = pTab->GetRect();
		rectGripper.left = rcText.right;
		rectGripper.DeflateRect(AFX_GRIPPER_MARGIN, 2);

		CBasePane bar;
		CMFCVisualManager::GetInstance()->OnDrawBarGripper(&dc, rectGripper, TRUE, &bar);
	}
}

BEGIN_MESSAGE_MAP(CTTBarTabCtrl, CMFCOutlookBarTabCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CTTBarTabCtrl::OnPaint()
{
	CPaintDC dcPaint(this); // device context for painting
	CMemDC memDC(dcPaint, this);
	CDC& dc = memDC.GetDC();

	Graphics graphics(dc.GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	graphics.SetTextRenderingHint(TextRenderingHintSingleBitPerPixel);

	int nVisibleTabsNum = GetVisibleTabsNum();

	CRect rectClient;
	GetClientRect(rectClient);
	

	DrawThemeParentBackground(GetSafeHwnd(), dc.GetSafeHdc(), rectClient);
	
	//-------------
	// Draw border:
	//-------------
	if (m_nBorderSize > 0)
	{
		rectClient.DeflateRect(m_nBorderSize, m_nBorderSize);
	}

	CPen penDrak(PS_SOLID, 1, GetGlobalData()->clrBarShadow);
	CPen* pOldPen = (CPen*)dc.SelectObject(&penDrak);
	ENSURE(pOldPen != NULL);

	CMFCOutlookBar* pOutlookBar = DYNAMIC_DOWNCAST(CMFCOutlookBar, GetParent());

	CFont* pOldFont = (CFont*)dc.SelectObject(pOutlookBar != NULL && pOutlookBar->GetButtonsFont() != NULL ? pOutlookBar->GetButtonsFont() : &(GetGlobalData()->fontRegular));
	dc.SetBkMode(TRANSPARENT);

	CRgn clipRgn;
	CreateRectRgnInDevicePoints(&dc, &clipRgn, rectClient, m_CornerRadius);

	int nSave = dc.SaveDC();
	dc.SelectClipRgn(&clipRgn);

	if (nVisibleTabsNum > 1 || !IsHideSingleTab())
	{
		for (int i = 0; i < m_iTabsNum; i++)
		{
			DrawTabButton(dc, i);
		}
	}

	dc.RestoreDC(nSave);

	// Border 
	Gdiplus::Rect gdiRectClient(rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height());
	DrawRectArea(gdiRectClient, graphics, m_ColorMap.GetColor(Normal, Border), m_CornerRadius, m_nBorderSize);

	if (!m_rectCaption.IsRectEmpty())
	{
		// Draw caption:
		CRect rectTop = m_rectCaption;
		rectTop.right++;

		rectTop.top -= 5;
		rectTop.bottom = rectTop.top + 5;

		dc.FillRect(rectTop, &(GetGlobalData()->brBarFace));

		COLORREF clrText = GetGlobalData()->clrBarText;
		CMFCVisualManager::GetInstance()->OnFillOutlookBarCaption(&dc, m_rectCaption, clrText);

		CString strActivePage;
		GetTabLabel(m_iActiveTab, strActivePage);

		CRect rcText = m_rectCaption;
		rcText.DeflateRect(CMFCBaseTabCtrl::AFX_TAB_TEXT_MARGIN, 0);

		UINT uiDTFlags = DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;

		dc.SetTextColor(clrText);
		dc.DrawText(strActivePage, rcText, uiDTFlags);
	}

	if (!m_rectSplitter.IsRectEmpty())
	{
		// Draw splitter:
		CMFCVisualManager::GetInstance()->OnDrawOutlookBarSplitter(&dc, m_rectSplitter);
	}

	// Draw scroll buttons:
	if (m_bScrollButtons && !IsMode2003())
	{
		if (m_iActiveTab == m_iTabsNum - 1)
		{
			CRect rectFill = rectClient;
			rectFill.top = rectFill.bottom - m_nTabsHeight;

			dc.FillRect(rectFill, &(GetGlobalData()->brBarFace));
		}
	}

	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldFont);
}

BOOL CTTBarTabCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

BOOL CTTBarTabCtrl::SetActiveTab(int iTab)
{
	bool result = __super::SetActiveTab(iTab);
	ModifyStyle(WS_CLIPCHILDREN, 0, SWP_NOREDRAW);
	return result;
}

BOOL CTTBarTabCtrl::Create(const CRect& rect, CWnd* pParentWnd, UINT nID)
{
	BOOL result = __super::Create(rect, pParentWnd, nID);
	ModifyStyle(WS_CLIPCHILDREN, 0, SWP_NOREDRAW);
	return result;
}