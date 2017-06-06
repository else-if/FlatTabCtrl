#include "stdafx.h"
#include "DrawingHelper.h"

double DrawingHelper::m_DpiScaleFactor = -1;

DrawingHelper::DrawingHelper()
{
}

DrawingHelper::~DrawingHelper()
{
}

void DrawingHelper::GetRoundRectPath(Gdiplus::GraphicsPath *pPath, Gdiplus::Rect r, int dia)
{
	// diameter can't exceed width or height
	if (dia > r.Width)	dia = r.Width;
	if (dia > r.Height)	dia = r.Height;

	// define a corner 
	Gdiplus::Rect Corner(r.X, r.Y, dia, dia);

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

void DrawingHelper::GetRoundRectPath(Gdiplus::GraphicsPath *pPath, Gdiplus::Rect rBounds, int dia, int borderSize)
{
	Gdiplus::Rect r(rBounds);

	r.Inflate(-borderSize / 2, -borderSize / 2);

	// diameter can't exceed width or height
	if (dia > r.Width)	dia = r.Width;
	if (dia > r.Height)	dia = r.Height;

	// define a corner 
	Gdiplus::Rect Corner(r.X, r.Y, dia, dia);

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

void DrawingHelper::DrawRectArea(Gdiplus::Rect &rc, Gdiplus::Graphics &graphics, COLORREF color, int cornerRadius, int penWidth)
{
	Gdiplus::Color penColor(0, 0, 0);
	penColor.SetFromCOLORREF(color);

	Gdiplus::GraphicsPath path;
	GetRoundRectPath(&path, rc, cornerRadius);

	Gdiplus::Pen pen(penColor, (Gdiplus::REAL)penWidth);
	pen.SetLineJoin(Gdiplus::LineJoinRound);
	graphics.DrawPath(&pen, &path);
}

double DrawingHelper::GetDpiScaleFactor()
{
	if (m_DpiScaleFactor = -1)
	{
		HDC screen = ::GetDC(NULL);
		m_DpiScaleFactor = GetDeviceCaps(screen, LOGPIXELSX) / 96.0;
		::ReleaseDC(NULL, screen);
	}

	return m_DpiScaleFactor;
}

int DrawingHelper::GetDpiScaledValue(int value)
{
	return (int)(DrawingHelper::GetDpiScaleFactor() * value);
}