#pragma once

#include "stdafx.h"

using namespace Gdiplus;

void GetRoundRectPath(GraphicsPath *pPath, Rect r, int dia);

void GetRoundRectPath(GraphicsPath *pPath, Rect rBounds, int dia, int borderSize);

void Draw4ColorsGradientRect(CRect &rc, CMemDC &dc,
    COLORREF colorStart1, COLORREF colorFinish1,
    COLORREF colorStart2, COLORREF colorFinish2,
    int cornerRadius, BOOL drawOnCurrentRgn = FALSE);

void Draw4ColorsGradientRect(CRect &rc, CDC &dc,
    COLORREF colorStart1, COLORREF colorFinish1,
    COLORREF colorStart2, COLORREF colorFinish2,
    int cornerRadius, BOOL drawOnCurrentRgn = FALSE);

void Draw2ColorsGradientRect(CRect &rc, CMemDC &dc,
    COLORREF colorStart, COLORREF colorFinish,
    int cornerRadius, BOOL drawOnCurrentRgn = FALSE);

void Draw2ColorsGradientRect(CRect &rc, CDC &dc,
    COLORREF colorStart, COLORREF colorFinish,
    int cornerRadius, BOOL drawOnCurrentRgn = FALSE);

void DrawRectArea(Gdiplus::Rect &rc, Gdiplus::Graphics &graphics, COLORREF color, int cornerRadius, int penWidth);

void DrawText(CRect &rc, CDC &dc, CFont &font, COLORREF textColor, CString text, UINT nFormat);
void DrawText(CRect &rc, CMemDC &dc, CFont &font, COLORREF textColor, CString text, UINT nFormat);

void FillRectRegion(CRect &rc, CDC &dc, COLORREF backgroundColor, int cornerRadius, BOOL drawOnCurrentRgn = FALSE);

void FillRectRegion(CRect &rc, CMemDC &dc, COLORREF backgroundColor, int cornerRadius, BOOL drawOnCurrentRgn = FALSE);

void CreateRectRgnInDevicePoints(CDC *pDC, CRgn *rgn, CRect &rc, int cornerRadius = 0);
void CreateRectRgnInDevicePoints(CDC *pDC, CRgn *rgn, int left, int top, int right, int bottom, int cornerRadius = 0);
void CreatePolygonRgnInDevicePoints(CDC *pDC, CRgn *rgn, CPoint points[], int nCount, int nMode);

void InvalidateRectRegions(CWnd* pWnd, CRect &rect1, CRect &rect2, int nCombineMode);