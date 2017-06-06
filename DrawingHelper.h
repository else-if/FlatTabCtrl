#pragma once

class DrawingHelper
{
public:
	DrawingHelper();
	~DrawingHelper();

private:
	static double m_DpiScaleFactor;

public:

	static double GetDpiScaleFactor();

	static void GetRoundRectPath(Gdiplus::GraphicsPath *pPath, Gdiplus::Rect r, int dia);

	static void GetRoundRectPath(Gdiplus::GraphicsPath *pPath, Gdiplus::Rect rBounds, int dia, int borderSize);

	static void DrawRectArea(Gdiplus::Rect &rc, Gdiplus::Graphics &graphics, COLORREF color, int cornerRadius, int penWidth);

	static int GetDpiScaledValue(int value);
};

