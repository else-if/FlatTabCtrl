#pragma once

class ColorsCommon
{
public:
	static void SfxHSBtoRGB(int nHue, int nSat, int nBri, int *nRed, int *nGreen, int *nBlue);
	static void SfxRGBtoHSB(int nRed, int nGreen, int nBlue, int *nHue, int *nSat, int *nBri);
	static COLORREF SfxColorRGBtoHSB(COLORREF color);
	static COLORREF SfxColorHSBtoRGB(COLORREF color);
	static COLORREF SfxContrastingColor(COLORREF color, int nThreshold);
	static int SfxGetLuminance(COLORREF crRGB);
};

