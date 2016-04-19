#include "stdafx.h"
#include "WBButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CWBButton::CWBButton()
{
	m_TopHeight = 8;
	m_BottomHeight = 8;
	m_LeftWidth = 8;
	m_RightWidth = 17;
	m_State = notInited;
	m_RcId = 0;
	m_NumofPics = 0;
	
	m_TransparentColor = RGB(255, 0, 255);

	m_bTracking = false;
	m_bCheckBox = false;
	m_bChecked = false;

	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
	m_TextFont.CreateFontIndirect(&ncm.lfStatusFont);
}

CWBButton::~CWBButton()
{
	NormalBitmap.DeleteObject();
	SelectBitmap.DeleteObject();
	FocusBitmap.DeleteObject();
	DisableBitmap.DeleteObject();
}

BEGIN_MESSAGE_MAP(CWBButton, CButton)
	//{{AFX_MSG_MAP(CWBButton)
	ON_WM_ERASEBKGND()	
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CWBButton::ActAsCheckBox(bool fActAsCheckBox)
{
	m_bCheckBox = fActAsCheckBox;
}

void CWBButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC xdc;
	xdc.Attach(lpDrawItemStruct->hDC);
	CRect rc;
	GetClientRect(rc);
	CMemDC dc(xdc, rc);

	UINT state = lpDrawItemStruct->itemState;
	bool IsDisable = false;

	if (m_bCheckBox) {
		if (m_bChecked || (state & ODS_FOCUS && state & ODS_SELECTED)) {
			DrawBitmap(&dc.GetDC(), select);			
		}
		else if (state & ODS_DISABLED)	{
			IsDisable = true;
			DrawBitmap(&dc.GetDC(), disable);
		}
		else {
			if (m_bChecked) {
				DrawBitmap(&dc.GetDC(), select);
				
			}
			else DrawBitmap(&dc.GetDC(), normal);
		}
	}
	else
	{
		if (state & ODS_FOCUS)
		{
			DrawBitmap(&dc.GetDC(), focus);
			if (state & ODS_SELECTED)
			{
				DrawBitmap(&dc.GetDC(), select);				
			}
		}
		else if (m_bTracking)
		{
			DrawBitmap(&dc.GetDC(), focus);
		}
		else if (state & ODS_DISABLED)
		{
			IsDisable = true;
			DrawBitmap(&dc.GetDC(), disable);
		}
		else
		{
			if (m_bCheckBox && m_bChecked)
			{
				DrawBitmap(&dc.GetDC(), select);				
			}
			else
			{
				DrawBitmap(&dc.GetDC(), normal);
			}
		}
	}

	int imode = dc.GetDC().SetBkMode(TRANSPARENT);
	//CFont *pOldFnt = dc.GetDC().SelectObject(&font);
	//COLORREF oldColor;
// 	if (IsDisable)
// 		oldColor = dc.SetTextColor(GetSysColor(COLOR_GRAYTEXT));
// 	else
// 		oldColor = dc.SetTextColor(m_TextFont->GetFontColor()); */
// 
// 		oldColor = dc.SetTextColor(m_TextFont->GetFontColor()); */
	dc.GetDC().SelectObject(&m_TextFont);
	CString txt;
	GetWindowText(txt);
	dc.GetDC().DrawText(txt, rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);	
}

bool CWBButton::LoadBitmaps(UINT id, int count, int TopHeight, int BottomHeight, int LeftWidth, int RightWidth)
{
	m_TopHeight = TopHeight;
	m_BottomHeight = BottomHeight;
	m_LeftWidth = LeftWidth;
	m_RightWidth = RightWidth;

	m_RcId = id;       // Resource ID
	m_NumofPics = count;

	CBitmap bmp;
	if (!bmp.LoadBitmap(id)) return false;

	if (!InitBitmap(bmp, NormalBitmap, 0, count)) return false;
	if (!InitBitmap(bmp, SelectBitmap, 1, count)) return false;
	if (!InitBitmap(bmp, DisableBitmap, 2, count)) return false;
	if (!InitBitmap(bmp, FocusBitmap, 3, count)) return false;

	bmp.DeleteObject();

	return true;
}

bool CWBButton::InitBitmap(CBitmap & src, CBitmap & dist, int index, int count)
{
	CDC * pDC = GetDC();

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	CDC srcDC;
	srcDC.CreateCompatibleDC(pDC);

	CBitmap* pOldBitmap1;
	pOldBitmap1 = srcDC.SelectObject(&src);

	BITMAP bmpinfo;
	src.GetBitmap(&bmpinfo);
	int bmpWidth = bmpinfo.bmWidth / count;
	int bmpHeight = bmpinfo.bmHeight;
	int orix = bmpWidth * index;

	CRect rc;
	GetClientRect(rc);

	CBitmap* pOldBitmap2;
	dist.DeleteObject();
	dist.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
	pOldBitmap2 = memDC.SelectObject(&dist);

	// lefttop,leftbottom,righttop,rightbottom
	if (!memDC.BitBlt(0, 0, m_LeftWidth, m_TopHeight, &srcDC, orix, 0, SRCCOPY)) return false;

	if (!memDC.BitBlt(0, rc.bottom - m_BottomHeight, m_LeftWidth, m_BottomHeight,
		&srcDC, orix, bmpHeight - m_BottomHeight, SRCCOPY)) return false;

	if (!memDC.BitBlt(rc.right - m_RightWidth, 0, m_RightWidth, m_TopHeight,
		&srcDC, orix + bmpWidth - m_RightWidth, 0, SRCCOPY)) return false;

	if (!memDC.BitBlt(rc.right - m_RightWidth, rc.bottom - m_BottomHeight, m_RightWidth, m_BottomHeight,
		&srcDC, orix + bmpWidth - m_RightWidth, bmpHeight - m_BottomHeight, SRCCOPY)) return false;

	// entireimage
	memDC.StretchBlt(m_LeftWidth,
		m_TopHeight,
		rc.Width() - (m_LeftWidth + m_RightWidth),
		rc.Height() - (m_TopHeight + m_BottomHeight),
		&srcDC,
		orix + m_LeftWidth,
		m_TopHeight,
		bmpWidth - (m_LeftWidth + m_RightWidth),
		bmpHeight - (m_TopHeight + m_BottomHeight), SRCCOPY);

	// topbar,bottombar( Stretch )
	memDC.StretchBlt(m_LeftWidth, 0, rc.Width() - (m_LeftWidth + m_RightWidth), m_TopHeight,
		&srcDC, orix + m_LeftWidth, 0, bmpWidth - (m_LeftWidth + m_RightWidth), m_TopHeight, SRCCOPY);

	memDC.StretchBlt(m_LeftWidth, rc.bottom - m_BottomHeight, rc.Width() - (m_LeftWidth + m_RightWidth), m_BottomHeight,
		&srcDC, orix + m_LeftWidth, bmpHeight - m_BottomHeight, bmpWidth - (m_LeftWidth + m_RightWidth), m_BottomHeight, SRCCOPY);

	// sidebar ( STretch? )
	memDC.StretchBlt(0, m_TopHeight, m_LeftWidth, rc.bottom - m_TopHeight - m_BottomHeight,
		&srcDC, orix, m_TopHeight, m_LeftWidth, bmpHeight - (m_TopHeight + m_BottomHeight), SRCCOPY);

	memDC.StretchBlt(rc.right - m_RightWidth, m_TopHeight, m_RightWidth, rc.bottom - m_TopHeight - m_BottomHeight,
		&srcDC, orix + bmpWidth - m_RightWidth, m_TopHeight, m_RightWidth, bmpHeight - m_TopHeight - m_BottomHeight, SRCCOPY);

	srcDC.SelectObject(pOldBitmap1);
	memDC.SelectObject(pOldBitmap2);

	ReleaseDC(pDC);
	ReleaseDC(&srcDC);
	ReleaseDC(&memDC);

	m_State = FileLoaded;

	return true;
}

void CWBButton::DrawBitmap(CDC * pDC, int mode)
{
	if (m_State < FileLoaded) return;

	CRect rc;
	GetClientRect(rc);
	
	COLORREF crOldText = pDC->SetTextColor(RGB(0, 0, 0));
	CDC dcImage, dcTrans, dcOffScr;
		
	// Create two memory dcs for the image and the mask
	dcImage.CreateCompatibleDC(pDC);
	dcTrans.CreateCompatibleDC(pDC);
	dcOffScr.CreateCompatibleDC(pDC);

	// Select the image into the appropriate dc
	CBitmap* pOldBitmapImage;
	switch (mode)
	{
	case normal:
		pOldBitmapImage = dcImage.SelectObject(&NormalBitmap);
		break;
	case select:
		pOldBitmapImage = dcImage.SelectObject(&SelectBitmap);
		break;
	case focus:
		pOldBitmapImage = dcImage.SelectObject(&FocusBitmap);
		break;
	case disable:
		pOldBitmapImage = dcImage.SelectObject(&DisableBitmap);
		break;
	default:
		return;
	}

	// Create the mask bitmap
	CBitmap bitmapTrans, OffScr;
	CBitmap* pOldBitmapOffScr;
	int nWidth = rc.Width();
	int nHeight = rc.Height();

	OffScr.CreateBitmap(nWidth, nHeight, (BYTE)pDC->GetDeviceCaps(PLANES), (BYTE)pDC->GetDeviceCaps(BITSPIXEL), NULL);
	pOldBitmapOffScr = dcOffScr.SelectObject(&OffScr);

	// Select the mask bitmap into the appropriate dc
	bitmapTrans.CreateBitmap(nWidth, nHeight, 1, 1, NULL);
	CBitmap* pOldBitmapTrans = dcTrans.SelectObject(&bitmapTrans);

	DrawThemeParentBackground(GetSafeHwnd(), dcOffScr.GetSafeHdc(), rc);

	// Build mask based on transparent colour
	COLORREF oldColor = dcImage.SetBkColor(m_TransparentColor);
	dcTrans.BitBlt(0, 0, nWidth, nHeight, &dcImage, 0, 0, SRCCOPY);	// create monochrome bitmap

	dcOffScr.SetBkColor(oldColor);
	dcOffScr.BitBlt(0, 0, nWidth, nHeight, &dcImage, 0, 0, SRCINVERT);
	dcOffScr.BitBlt(0, 0, nWidth, nHeight, &dcTrans, 0, 0, SRCAND);
	dcOffScr.BitBlt(0, 0, nWidth, nHeight, &dcImage, 0, 0, SRCINVERT);

	pDC->BitBlt(0, 0, nWidth, nHeight, &dcOffScr, 0, 0, SRCCOPY);

	// Restore settings
	dcImage.SelectObject(pOldBitmapImage);
	dcTrans.SelectObject(pOldBitmapTrans);
	dcOffScr.SelectObject(pOldBitmapOffScr);
	
	//pDC->SetTextColor(crOldText);
}

BOOL CWBButton::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void CWBButton::SetButtonDef(int TopHeight, int BottomHeight, int LeftWidth, int RightWidth)
{
	m_TopHeight = TopHeight;
	m_BottomHeight = BottomHeight;
	m_LeftWidth = LeftWidth;
	m_RightWidth = RightWidth;

	if (m_RcId != 0 && m_NumofPics != 0)
	{
		LoadBitmaps(m_RcId, m_NumofPics, TopHeight, BottomHeight, LeftWidth, RightWidth);
	}
}

BOOL CWBButton::PreTranslateMessage(MSG* pMsg)
{
	// treat double-click like single click
	if (pMsg->message == WM_LBUTTONDBLCLK) {
		pMsg->message = WM_LBUTTONDOWN;
	}
	return CButton::PreTranslateMessage(pMsg);
}

void CWBButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bCheckBox) {
		CRect rectButton;
		GetClientRect(&rectButton);
		if (rectButton.PtInRect(point))
			if (!m_bChecked)
				m_bChecked = !m_bChecked;
	}
	CButton::OnLButtonUp(nFlags, point);
	Invalidate();
	/*	 HWND hWnd = GetSafeHwnd();
	  if ((hWnd != NULL) && ::IsWindow (hWnd))
	  Invalidate();*/
}

void CWBButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = GetSafeHwnd();
		m_bTracking = ::_TrackMouseEvent(&tme) ? true : false;
		Invalidate(false);
	}

	CButton::OnMouseMove(nFlags, point);
}

void CWBButton::OnMouseLeave()
{
	m_bTracking = false;
	Invalidate(false);

	CButton::OnMouseLeave();
}