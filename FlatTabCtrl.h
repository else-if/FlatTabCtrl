
// FlatTabCtrl.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CFlatTabCtrlApp:
// See FlatTabCtrl.cpp for the implementation of this class
//

class CFlatTabCtrlApp : public CWinApp
{
public:
	CFlatTabCtrlApp();
	ULONG_PTR m_gdiplusToken;
// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CFlatTabCtrlApp theApp;

void DrawGlow(CDC* pDC, CRect cRect);
int DrawGlow(CDC* pDC, Gdiplus::GraphicsPath* gp);
void RectangleDropShadow(Gdiplus::Graphics& tg, Gdiplus::Rect& rc, Gdiplus::Color& shadowColor, int depth, int maxOpacity);
void DrawBlurredRectangle(Gdiplus::Graphics& tg, Gdiplus::Rect& rc);