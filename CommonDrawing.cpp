#include "stdafx.h"
#include "CommonDrawing.h"

void GetRoundRectPath(GraphicsPath *pPath, Rect r, int dia)
{
    // diameter can't exceed width or height
    if (dia > r.Width)	dia = r.Width;
    if (dia > r.Height)	dia = r.Height;

    // define a corner 
    Rect Corner(r.X, r.Y, dia, dia);

    // begin path
    pPath->Reset();

    // top left
    pPath->AddArc(Corner, 180, 90);

    // tweak needed for radius of 10 (dia of 20)
    if (dia == 20)
    {
        Corner.Width += 1;
        Corner.Height += 1;
        r.Width -= 1;
        r.Height -= 1;
    }

    // top right
    Corner.X += (r.Width - dia - 1);
    pPath->AddArc(Corner, 270, 90);

    // bottom right
    Corner.Y += (r.Height - dia - 1);
    pPath->AddArc(Corner, 0, 90);

    // bottom left
    Corner.X -= (r.Width - dia - 1);
    pPath->AddArc(Corner, 90, 90);

    // end path
    pPath->CloseFigure();
}

void GetRoundRectPath(GraphicsPath *pPath, Rect rBounds, int dia, int borderSize)
{
    Rect r(rBounds);

    r.Inflate(-borderSize / 2, -borderSize / 2);

    // diameter can't exceed width or height
    if (dia > r.Width)	dia = r.Width;
    if (dia > r.Height)	dia = r.Height;

    // define a corner 
    Rect Corner(r.X, r.Y, dia, dia);

    // begin path
    pPath->Reset();

    // top left
    pPath->AddArc(Corner, 180, 90);

    // tweak needed for radius of 10 (dia of 20)
    if (dia == 20)
    {
        Corner.Width += 1;
        Corner.Height += 1;
        r.Width -= 1;
        r.Height -= 1;
    }

    // top right
    Corner.X += (r.Width - dia - 1);
    pPath->AddArc(Corner, 270, 90);

    // bottom right
    Corner.Y += (r.Height - dia - 1);
    pPath->AddArc(Corner, 0, 90);

    // bottom left
    Corner.X -= (r.Width - dia - 1);
    pPath->AddArc(Corner, 90, 90);

    // end path
    pPath->CloseFigure();
}

void Draw4ColorsGradientRect(CRect &rc, CMemDC &dc,
    COLORREF colorStart1, COLORREF colorFinish1,
    COLORREF colorStart2, COLORREF colorFinish2,
    int cornerRadius, BOOL drawOnCurrentRgn)
{
    Draw4ColorsGradientRect(rc, dc.GetDC(), colorStart1, colorFinish1,
        colorStart2, colorFinish2, cornerRadius, drawOnCurrentRgn);
}

void Draw4ColorsGradientRect(CRect &rc, CDC &dc,
    COLORREF colorStart1, COLORREF colorFinish1,
    COLORREF colorStart2, COLORREF colorFinish2,
    int cornerRadius, BOOL drawOnCurrentRgn)
{
    CRect oldRect;
    dc.GetClipBox(&oldRect);
    CRgn oldRgn;
    CreateRectRgnInDevicePoints(&dc, &oldRgn, oldRect.left, oldRect.top, oldRect.right, oldRect.bottom);

    CDrawingManager drawingManager(dc);

    if (!drawOnCurrentRgn)
    {
        CRgn rgn;
        CreateRectRgnInDevicePoints(&dc, &rgn, rc.left, rc.top, rc.right, rc.bottom, cornerRadius);
        dc.SelectClipRgn(&rgn);
    }

    drawingManager.Fill4ColorsGradient(rc, colorStart1, colorFinish1,
        colorStart2, colorFinish2);

    if (!drawOnCurrentRgn)
        dc.SelectClipRgn(&oldRgn);
}

void Draw2ColorsGradientRect(CRect &rc, CMemDC &dc,
    COLORREF colorStart, COLORREF colorFinish,
    int cornerRadius, BOOL drawOnCurrentRgn)
{
    Draw2ColorsGradientRect(rc, dc.GetDC(), colorStart, colorFinish,
        cornerRadius, drawOnCurrentRgn);
}

void Draw2ColorsGradientRect(CRect &rc, CDC &dc,
    COLORREF colorStart, COLORREF colorFinish,
    int cornerRadius, BOOL drawOnCurrentRgn)
{
    CRect oldRect;
    dc.GetClipBox(&oldRect);
    CRgn oldRgn;
    CreateRectRgnInDevicePoints(&dc, &oldRgn, oldRect.left, oldRect.top, oldRect.right, oldRect.bottom);

    CDrawingManager drawingManager(dc);

    if (!drawOnCurrentRgn)
    {
        CRgn rgn;
        CreateRectRgnInDevicePoints(&dc, &rgn, rc.left, rc.top, rc.right, rc.bottom, cornerRadius);
        dc.SelectClipRgn(&rgn);
    }

    drawingManager.FillGradient(rc, colorStart, colorFinish);

    if (!drawOnCurrentRgn)
        dc.SelectClipRgn(&oldRgn);
}

void FillRectRegion(CRect &rc, CMemDC &dc, COLORREF backgroundColor, int cornerRadius, BOOL drawOnCurrentRgn)
{
    FillRectRegion(rc, dc.GetDC(), backgroundColor, cornerRadius, drawOnCurrentRgn);
}

void FillRectRegion(CRect &rc, CDC &dc, COLORREF backgroundColor, int cornerRadius, BOOL drawOnCurrentRgn)
{
    CRect oldRect;
    dc.GetClipBox(&oldRect);
    CRgn oldRgn;
    CreateRectRgnInDevicePoints(&dc, &oldRgn, oldRect.left, oldRect.top, oldRect.right, oldRect.bottom);

    if (!drawOnCurrentRgn)
    {
        CRgn rgn;
        CreateRectRgnInDevicePoints(&dc, &rgn, rc.left, rc.top, rc.right, rc.bottom, cornerRadius);
        dc.SelectClipRgn(&rgn);
    }

    dc.FillSolidRect(rc, backgroundColor);

    if (!drawOnCurrentRgn)
        dc.SelectClipRgn(&oldRgn);
}

void DrawRectArea(Gdiplus::Rect &rc, Gdiplus::Graphics &graphics, COLORREF color, int cornerRadius, int penWidth)
{
    Gdiplus::Color penColor(0, 0, 0);
    penColor.SetFromCOLORREF(color);

    GraphicsPath path;
    GetRoundRectPath(&path, rc, cornerRadius);

    Gdiplus::Pen pen(penColor, (REAL)penWidth);
    pen.SetLineJoin(LineJoinRound);
    graphics.DrawPath(&pen, &path);
}

void DrawText(CRect &rc, CMemDC &dc, CFont &font, COLORREF textColor, CString text, UINT nFormat)
{
    DrawText(rc, dc.GetDC(), font, textColor, text, nFormat);
}

void DrawText(CRect &rc, CDC &dc, CFont &font, COLORREF textColor, CString text, UINT nFormat)
{
    int oldBkMode = dc.GetBkMode();
    COLORREF oldColor = dc.GetTextColor();
    CFont* pOldFont = dc.SelectObject(&font);

    dc.SetBkMode(TRANSPARENT);
    dc.SetTextColor(textColor);

    dc.DrawText(text, rc, nFormat);

    dc.SetBkMode(oldBkMode);
    dc.SetTextColor(oldColor);
    dc.SelectObject(&pOldFont);
}

void CreateRectRgnInDevicePoints(CDC *pDC, CRgn *rgn, CRect &rc, int cornerRadius)
{
    CRect cDevPtsRect(rc);
    pDC->LPtoDP(&cDevPtsRect);

	ASSERT(cDevPtsRect.EqualRect(&rc));

    if (cornerRadius == 0)
        rgn->CreateRectRgnIndirect(&cDevPtsRect);
    else
        rgn->CreateRoundRectRgn(cDevPtsRect.left, cDevPtsRect.top, cDevPtsRect.right, cDevPtsRect.bottom,
        cornerRadius, cornerRadius);
}

void CreateRectRgnInDevicePoints(CDC *pDC, CRgn *rgn, int left, int top, int right, int bottom, int cornerRadius)
{
    CRect cLogPtsRect(left, top, right, bottom);
    CreateRectRgnInDevicePoints(pDC, rgn, cLogPtsRect, cornerRadius);
}

void CreatePolygonRgnInDevicePoints(CDC *pDC, CRgn *rgn, CPoint points[], int nCount, int nMode)
{
    CPoint* devPts = new CPoint[nCount];
    for (int i = 0; i < nCount; i++)
        devPts[i] = CPoint(points[i]);

    pDC->LPtoDP(devPts, nCount);
    rgn->CreatePolygonRgn(devPts, nCount, nMode);

    delete devPts;
}

void InvalidateRectRegions(CWnd* pWnd, CRect &rect1, CRect &rect2, int nCombineMode)
{
	if (pWnd == NULL)
		return;

	CRgn invalidRgn;
	invalidRgn.CreateRectRgnIndirect(&rect1);

	CRgn rgn2;
	rgn2.CreateRectRgnIndirect(&rect2);

	invalidRgn.CombineRgn(&invalidRgn, &rgn2, nCombineMode);
	
	pWnd->InvalidateRgn(&invalidRgn);
}

// recreate the combo box by copying styles etc, and list items
// and applying them to a newly created control
BOOL RecreateComboBox(CComboBox* pCombo, LPVOID lpParam/*=0*/)
{
	if (pCombo == NULL)
		return FALSE;
	if (pCombo->GetSafeHwnd() == NULL)
		return FALSE;

	CWnd* pParent = pCombo->GetParent();
	if (pParent == NULL)
		return FALSE;

	// get current attributes
	DWORD dwStyle = pCombo->GetStyle();
	DWORD dwStyleEx = pCombo->GetExStyle();
	CRect rc;
	pCombo->GetDroppedControlRect(&rc);
	pParent->ScreenToClient(&rc);	// map to client co-ords
	UINT nID = pCombo->GetDlgCtrlID();
	CFont* pFont = pCombo->GetFont();
	CWnd* pWndAfter = pCombo->GetNextWindow(GW_HWNDPREV);

	// get the currently selected text (and whether it is a valid list selection)
	CString sCurText;
	int nCurSel = pCombo->GetCurSel();
	BOOL bItemSelValid = nCurSel != -1;
	if (bItemSelValid)
		pCombo->GetLBText(nCurSel, sCurText);
	else
		pCombo->GetWindowText(sCurText);

	// copy the combo box items into a temp combobox (not sorted)
	// along with each item's data
	CComboBox comboNew;
	if (!comboNew.CreateEx(dwStyleEx, _T("COMBOBOX"), _T(""),
		dwStyle, rc, pParent, nID, lpParam))
		return FALSE;
	comboNew.SetFont(pFont);
	int nNumItems = pCombo->GetCount();
	for (int n = 0; n < nNumItems; n++)
	{
		CString sText;
		pCombo->GetLBText(n, sText);
		int nNewIndex = comboNew.AddString(sText);
		comboNew.SetItemData(nNewIndex, pCombo->GetItemData(n));
	}
	// re-set selected text
	if (bItemSelValid)
		comboNew.SetCurSel(comboNew.FindStringExact(-1, sCurText));
	else
		comboNew.SetWindowText(sCurText);

	// destroy the existing window, then attach the new one
	HWND oldHwnd = pCombo->Detach();
	HWND hwnd = comboNew.Detach();
	pCombo->Attach(hwnd);
	pCombo->SubclassWindow(/*comboNew.GetSafeHwnd()*/hwnd);

	::DestroyWindow(oldHwnd);

	// position correctly in z-order
	pCombo->SetWindowPos(pWndAfter == NULL ?
		&CWnd::wndBottom :
		pWndAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return TRUE;
}