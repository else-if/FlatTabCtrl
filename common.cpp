#include "stdafx.h"
#include "common.h"

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

	r.Inflate(-borderSize/2, -borderSize/2);

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
	int cornerRadius)
{
	CRect oldRect;
	dc.GetDC().GetClipBox(&oldRect);
	CRgn oldRgn;
	oldRgn.CreateRectRgn(oldRect.left, oldRect.top, oldRect.right, oldRect.bottom);

	CDrawingManager drawingManager(dc.GetDC());

	CRgn rgn;
	rgn.CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, cornerRadius, cornerRadius);
	dc.GetDC().SelectClipRgn(&rgn);

	drawingManager.Fill4ColorsGradient(rc, colorStart1, colorFinish1,
		colorStart2, colorFinish2);

	dc.GetDC().SelectClipRgn(&oldRgn);
}

void Draw2ColorsGradientRect(CRect &rc, CMemDC &dc,
	COLORREF colorStart, COLORREF colorFinish,
	int cornerRadius)
{
	CDrawingManager drawingManager(dc.GetDC());

	CRgn rgn;
	rgn.CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, cornerRadius, cornerRadius);
	dc.GetDC().SelectClipRgn(&rgn);

	drawingManager.FillGradient(rc, colorStart, colorFinish);		
}

void FillRectRegion(CRect &rc, CMemDC &dc, COLORREF backgroundColor, int cornerRadius)
{
	CRect oldRect;
	dc.GetDC().GetClipBox(&oldRect);
	CRgn oldRgn;
	oldRgn.CreateRectRgn(oldRect.left, oldRect.top, oldRect.right, oldRect.bottom);

	CRgn rgn;
	rgn.CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, cornerRadius, cornerRadius);
	dc.GetDC().SelectClipRgn(&rgn);

	dc.GetDC().FillSolidRect(rc, backgroundColor);

	dc.GetDC().SelectClipRgn(&oldRgn);
}

void DrawRectArea(Gdiplus::Rect &rc, Gdiplus::Graphics &graphics, COLORREF color, int cornerRadius, float penWidth)
{
	Gdiplus::Color penColor(0, 0, 0);
	penColor.SetFromCOLORREF(color);

	GraphicsPath path;
	GetRoundRectPath(&path, rc, cornerRadius);

	Gdiplus::Pen pen(penColor, penWidth);
	pen.SetLineJoin(LineJoinRound);
	graphics.DrawPath(&pen, &path);
}

void DrawText(CRect &rc, CMemDC &dc, CFont &font, COLORREF textColor, CString text, UINT nFormat)
{
	int oldBkMode = dc.GetDC().GetBkMode();
	COLORREF oldColor = dc.GetDC().GetTextColor();
	CFont* pOldFont = dc.GetDC().SelectObject(&font);

	dc.GetDC().SetBkMode(TRANSPARENT);
	dc.GetDC().SetTextColor(textColor);		

	dc.GetDC().DrawText(text, rc, nFormat);

	dc.GetDC().SetBkMode(oldBkMode);
	dc.GetDC().SetTextColor(oldColor);
	dc.GetDC().SelectObject(&pOldFont);
}
