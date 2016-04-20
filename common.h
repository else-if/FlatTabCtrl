#include "stdafx.h"

using namespace Gdiplus;

void GetRoundRectPath(GraphicsPath *pPath, Rect r, int dia);

void GetRoundRectPath(GraphicsPath *pPath, Rect rBounds, int dia, int borderSize);

void Draw4ColorsGradientRect(CRect &rc, CMemDC &dc,
	COLORREF colorStart1, COLORREF colorFinish1,
	COLORREF colorStart2, COLORREF colorFinish2,
	int cornerRadius);

void Draw2ColorsGradientRect(CRect &rc, CMemDC &dc,
	COLORREF colorStart, COLORREF colorFinish,
	int cornerRadius);

void DrawRectArea(Gdiplus::Rect &rc, Gdiplus::Graphics &graphics, COLORREF color, int cornerRadius, float penWidth);

void DrawText(CRect &rc, CMemDC &dc, CFont &font, COLORREF textColor, CString text, UINT nFormat);