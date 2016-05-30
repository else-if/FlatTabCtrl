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

    m_bUseBmp = false;

    m_iComboBoxStyle = 0;

    SetDrawingProperties(1, 5);

    m_ControlState = Normal;
    m_ArrowButtonState = Normal;

    m_ColorMap.SetDefaultColors();

    m_ColorMap.SetColor(Press, BackgroundTopGradientStart, RGB(250, 213, 103));
    m_ColorMap.SetColor(Press, BackgroundTopGradientFinish, RGB(244, 181, 51));
    m_ColorMap.SetColor(Press, BackgroundBottomGradientStart, RGB(255, 241, 148));
    m_ColorMap.SetColor(Press, BackgroundBottomGradientFinish, RGB(250, 213, 103));
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
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
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

    ControlState backgrougState = m_ControlState;
    if (m_ControlState != Disable && (!m_bTracking || m_bOnButton))
        backgrougState = Normal;

    CString cSelText = _T("");
    int	nSel = GetCurSel();    
    if (nSel != -1)
    {
        int iLen = GetLBTextLen(nSel);
        GetLBText(nSel, cSelText.GetBuffer(iLen));
        cSelText.ReleaseBuffer();
    }
        
    LOGFONT logFont;
    GetFont()->GetLogFont(&logFont);
    CFont font;
    font.CreateFontIndirect(&logFont);

    DrawComboBoxControl(&memDC.GetDC(), &cRect, &cRect, m_iComboBoxStyle, m_ControlState, backgrougState,
        m_ArrowButtonState, cSelText, &font, m_CornerRadius, m_borderPenWidth, &m_ColorMap);

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

        CRect cCurRect;
        HWND hWnd = m_listbox.GetSafeHwnd();
        ::GetWindowRect(hWnd, cCurRect);

        CRect cNewRect(cCurRect);
        int droppedWidth = GetDroppedWidth();

        CRect cParentRect;
        GetParent()->GetWindowRect(&cParentRect);

        CRect cWndRect;
        GetWindowRect(cWndRect);

        if (cWndRect.left + droppedWidth > cParentRect.right)
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
    else if (IsWindowEnabled())
    {
        pDC->SetBkMode(TRANSPARENT);
        return (HBRUSH)GetStockObject(NULL_BRUSH);
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
    TRACE(CString("Combo SetFocus\n"));
    m_bHasFocus = true;
    Invalidate();
}

void CTTComboBox::PreSubclassWindow()
{
    m_iComboBoxStyle = GetStyle() & CBS_DROPDOWNLIST;

    CComboBox::PreSubclassWindow();
}

afx_msg LRESULT CTTComboBox::OnCbSetcursel(WPARAM wParam, LPARAM lParam)
{
    LRESULT Result = Default();
    Invalidate(FALSE);

    return Result;
}

void CTTComboBox::OnMove(int x, int y)
{
    CComboBox::OnMove(x, y);

    m_bUseBmp = false;
    Invalidate(TRUE);
}

void CTTComboBox::OnSetFocus(CWnd* pOldWnd)
{
    TRACE(CString("Combo WM_SETFOCUS\n"));
    CComboBox::OnSetFocus(pOldWnd);

    m_bHasFocus = true;
    Invalidate();
}


void CTTComboBox::OnKillFocus(CWnd* pNewWnd)
{
    TRACE(CString("Combo WM_KILLFOCUS\n"));
    CComboBox::OnKillFocus(pNewWnd);

    m_bHasFocus = false;
    //m_bTracking = false;
    //m_bOnButton = false;
    m_bButtonPress = false;
    Invalidate();
}