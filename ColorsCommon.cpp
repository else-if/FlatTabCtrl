#include "stdafx.h"
#include "ColorsCommon.h"

//macros
#define HSB(h,s,b) ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(s))<<8))|(((DWORD)(BYTE)(h))<<16)))
#define LUMINANCE(r,g,b) ((30*r+59*g+11*b)/255)

#define GetLValue(hsb)	 ((BYTE)(hsb))
#define GetSValue(hsb)	 ((BYTE)(((WORD)(hsb)) >> 8))
#define GetHValue(hsb)	 ((BYTE)((DWORD)(hsb)>>16))
#define	HUE_UNDEF	 0x000000FF

//-------------------------------------------------------------------------//
// Convert the HSB components into RGB components                          //
//                                                                         //
// red, green, blue values are from 0 to 255                               //
// hue = [0,240], sat = [0,100]%, bri = [0,100]%                           //
//                                                                         //
//-------------------------------------------------------------------------//
void ColorsCommon::SfxHSBtoRGB(int nHue, int nSat, int nBri, int *nRed, int *nGreen, int *nBlue)
{
	int		nSector;
	double	dFract, dVal1, dVal2;
	double	dRed, dGreen, dBlue;
	double	dHue, dSat, dBri;

	if ((nSat == 0) || (nHue == HUE_UNDEF))
	{
		// achromatic (grey, black or white)
		*nRed = *nGreen = *nBlue = (nBri * 255) / 100;
		return;
	}

	dHue = (double)nHue;
	dSat = (double)nSat / 100.0;
	dBri = (double)nBri / 100.0;

	dHue /= 40.0;
	nSector = (int)floor(dHue);			// sector 0 to 5
	dFract = dHue - floor(dHue);		// factional part of hue
	if (!(nSector & 1)) dFract = 1.0 - dFract;
	dVal1 = dBri * (1.0 - dSat);
	dVal2 = dBri * (1.0 - dSat * dFract);
	switch (nSector)
	{
	case 0: dRed = dBri;	dGreen = dVal2;		dBlue = dVal1;	break;
	case 1: dRed = dVal2;	dGreen = dBri;		dBlue = dVal1;	break;
	case 2: dRed = dVal1;	dGreen = dBri;		dBlue = dVal2;	break;
	case 3: dRed = dVal1;	dGreen = dVal2;		dBlue = dBri;	break;
	case 4: dRed = dVal2;	dGreen = dVal1;		dBlue = dBri;	break;
	case 5: dRed = dBri;	dGreen = dVal1;		dBlue = dVal2;	break;
	}
	*nRed = (int)(dRed * 255.0);
	*nGreen = (int)(dGreen * 255.0);
	*nBlue = (int)(dBlue * 255.0);
}

//-------------------------------------------------------------------------//
// Convert the RGB components into HSB components                          //
//                                                                         //
// red, green, blue values are from 0 to 255                               //
// hue = [0,240], sat = [0,100]%, bri = [0,100]%                           //
//                                                                         //
//-------------------------------------------------------------------------//
void ColorsCommon::SfxRGBtoHSB(int nRed, int nGreen, int nBlue, int *nHue, int *nSat, int *nBri)
{
	double	dRed, dGreen, dBlue;
	double	dHue, dSat, dBri;
	double	dMin, dMax, dDelta;

	dRed = (double)nRed / 255.0;
	dGreen = (double)nGreen / 255.0;
	dBlue = (double)nBlue / 255.0;
	dMin = min(dRed, dGreen);
	dMin = min(dMin, dBlue);
	dMax = max(dRed, dGreen);
	dMax = max(dMax, dBlue);
	dBri = dMax;
	if (dMax == dMin)
	{
		// achromatic color (i.e. grey, black or white)
		// r = g = b = x		
		// sat = 0, bri = x*100, hue is undefined
		*nSat = 0;
		*nHue = HUE_UNDEF;
		*nBri = (int)(dBri * 100.0);
		return;
	}
	dDelta = dMax - dMin;
	dSat = dDelta / dMax;
	if (dRed == dMax) dHue = (dGreen - dBlue) / dDelta;				// between yellow & magenta
	else if (dGreen == dMax) dHue = 2.0 + (dBlue - dRed) / dDelta;		// between cyan & yellow
	else dHue = 4.0 + (dRed - dGreen) / dDelta;							// between magenta & cyan
	dHue *= 40.0;
	if (dHue < 0) dHue += 240.0;
	*nHue = (int)dHue;
	*nSat = (int)(dSat * 100.0);
	*nBri = (int)(dBri * 100.0);
}

//-------------------------------------------------------------------------//
// Convert an RGB color-space format to HSB color-space format             //
//                                                                         //
// RGB format 0x00BBGGRR                                                   //
// HSB format 0x00HHSSBB                                                   //
//                                                                         //
//-------------------------------------------------------------------------//
COLORREF ColorsCommon::SfxColorRGBtoHSB(COLORREF color)
{
	int		nHue, nSat, nBri;

	SfxRGBtoHSB(GetRValue(color), GetGValue(color), GetBValue(color),
		&nHue, &nSat, &nBri);

	return HSB(nHue, nSat, nBri);
}

//-------------------------------------------------------------------------//
// Convert an HSB color-space format to RGB color-space format             //
//                                                                         //
// RGB format 0x00BBGGRR                                                   //
// HSB format 0x00HHSSBB                                                   //
//                                                                         //
//-------------------------------------------------------------------------//
COLORREF ColorsCommon::SfxColorHSBtoRGB(COLORREF color)
{
	int		nRed, nGreen, nBlue;

	SfxHSBtoRGB(GetHValue(color), GetSValue(color), GetLValue(color),
		&nRed, &nGreen, &nBlue);
	return RGB(nRed, nGreen, nBlue);
}

//-------------------------------------------------------------------------//
// Create a color that contrast a given color                              //
// This algorithm is taken from my research on color science               //
//-------------------------------------------------------------------------//
COLORREF ColorsCommon::SfxContrastingColor(COLORREF color, int nThreshold)
{
	int		nOrigLum, nCalcLum, nLoop;
	int		nHue, nSat, nBri;
	int		nRed, nGreen, nBlue;

	nRed = GetRValue(color);
	nGreen = GetGValue(color);
	nBlue = GetBValue(color);
	nOrigLum = LUMINANCE(nRed, nGreen, nBlue);

	SfxRGBtoHSB(nRed, nGreen, nBlue, &nHue, &nSat, &nBri);

	if (nHue == HUE_UNDEF)
	{
		nRed = nGreen = nBlue = 0;
		if (nBri < 50) nRed = nGreen = nBlue = 255;
	}
	else
	{
		nHue = (nHue + 120) % 240;
		nLoop = 20;
		while (nLoop)
		{
			SfxHSBtoRGB(nHue, nSat, nBri, &nRed, &nGreen, &nBlue);
			nCalcLum = LUMINANCE(nRed, nGreen, nBlue);
			if (abs(nOrigLum - nCalcLum) >= nThreshold) break;
			if (nOrigLum <= 50)
			{
				nSat -= 5;
				if (nSat < 0) nSat += 5;
				nBri += 10;
				if (nBri > 100) nBri = 100;
			}
			else
			{
				nSat += 5;
				if (nSat > 100) nSat = 100;
				nBri -= 5;
				if (nBri < 10) nBri = 10;
			}
			nLoop--;
		}
	}
	return RGB(nRed, nGreen, nBlue);
}

//-------------------------------------------------------------------------//
// Retrieve a Luminance from RGB color                                     //
//-------------------------------------------------------------------------//
int ColorsCommon::SfxGetLuminance(COLORREF crRGB)
{
	return 	LUMINANCE(GetRValue(crRGB), GetGValue(crRGB), GetBValue(crRGB));
}
