
// FlatTabCtrl.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FlatTabCtrl.h"
#include "FlatTabCtrlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFlatTabCtrlApp

using namespace Gdiplus;

BEGIN_MESSAGE_MAP(CFlatTabCtrlApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CFlatTabCtrlApp construction

CFlatTabCtrlApp::CFlatTabCtrlApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CFlatTabCtrlApp object

CFlatTabCtrlApp theApp;


// CFlatTabCtrlApp initialization

BOOL CFlatTabCtrlApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CFlatTabCtrlDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CFlatTabCtrlApp::ExitInstance()
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken);

	return CWinApp::ExitInstance();
}

int DrawGlow(CDC* pDC, Gdiplus::GraphicsPath* gp)
{
	// Use the path to construct a brush.
	Gdiplus::PathGradientBrush pthGrBrush(gp);//path);
		
	Gdiplus::Color cb_colors[3] = { Gdiplus::Color::Transparent,
		Gdiplus::Color::MakeARGB(180, GetRValue(Gdiplus::Color::DimGray), GetGValue(Gdiplus::Color::DimGray), GetBValue(Gdiplus::Color::DimGray)),
		Gdiplus::Color::MakeARGB(180, GetRValue(Gdiplus::Color::DimGray), GetGValue(Gdiplus::Color::DimGray), GetBValue(Gdiplus::Color::DimGray)) };
	Gdiplus::REAL pos[3] = { 0.0, 0.1, 1 };

	pthGrBrush.SetInterpolationColors(cb_colors, pos, 3);
		
	Gdiplus::Graphics graphics(pDC->GetSafeHdc());
	
	graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
	graphics.FillPath(&pthGrBrush, gp);// path);// 0, 0, 140, 70);
	return 0;
}

void RectangleDropShadow(Gdiplus::Graphics& tg, Gdiplus::Rect& rc, Gdiplus::Color& shadowColor, int depth, int maxOpacity)
{

	//calculate the opacities

	Gdiplus::Color darkShadow =
		Gdiplus::Color::MakeARGB(maxOpacity, shadowColor.GetR(), shadowColor.GetG(), shadowColor.GetB());

	Gdiplus::Color lightShadow = 
		Gdiplus::Color::MakeARGB(0, shadowColor.GetR(), shadowColor.GetG(), shadowColor.GetB());

	//Create a brush that will create a softshadow circle

	Gdiplus::GraphicsPath gp;

	gp.AddEllipse(0, 0, 2 * depth, 2 * depth);

	Gdiplus::PathGradientBrush pgb(&gp);

	pgb.SetCenterColor(darkShadow);

	Gdiplus::Color cb_colors[1] = { lightShadow };	
	int count = 1;
	pgb.SetSurroundColors(cb_colors, &count);

	//generate a softshadow pattern that can be used to paint the shadow

	Gdiplus::Bitmap patternbm(2 * depth, 2 * depth);
	Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(&patternbm);

	g->FillEllipse(&pgb, 0, 0, 2 * depth, 2 * depth);

	Gdiplus::SolidBrush sb(Gdiplus::Color::MakeARGB(maxOpacity, shadowColor.GetR(), shadowColor.GetG(), shadowColor.GetB()));

	//tg.FillRectangle(&sb, rc.GetLeft() + depth, rc.GetTop() + depth, rc.Width - (2 * depth), rc.Height - (2 * depth));

	//top left corner

	tg.DrawImage(&patternbm, Gdiplus::Rect(rc.GetLeft(), rc.GetTop(), depth, depth), 0, 0, depth, depth, Gdiplus::Unit::UnitPixel);

	//top side

	tg.DrawImage(&patternbm, Gdiplus::Rect(rc.GetLeft() + depth, rc.GetTop(), rc.Width - (2 * depth), depth), depth, 0, 1, depth, Gdiplus::Unit::UnitPixel);

	//top right corner

	tg.DrawImage(&patternbm, Gdiplus::Rect(rc.GetRight() - depth, rc.GetTop(), depth, depth), depth, 0, depth, depth, Gdiplus::Unit::UnitPixel);

	//right side

	tg.DrawImage(&patternbm, Gdiplus::Rect(rc.GetRight() - depth, rc.GetTop() + depth, depth, rc.Height - (2 * depth)), depth, depth, depth, 1, Gdiplus::Unit::UnitPixel);

	//bottom left corner

	tg.DrawImage(&patternbm, Gdiplus::Rect(rc.GetRight() - depth, rc.GetBottom() - depth, depth, depth), depth, depth, depth, depth, Gdiplus::Unit::UnitPixel);

	//bottom side

	tg.DrawImage(&patternbm, Gdiplus::Rect(rc.GetLeft() + depth, rc.GetBottom() - depth, rc.Width - (2 * depth), depth), depth, depth, 1, depth, Gdiplus::Unit::UnitPixel);

	//bottom left corner

	tg.DrawImage(&patternbm, Gdiplus::Rect(rc.GetLeft(), rc.GetBottom() - depth, depth, depth), 0, depth, depth, depth, Gdiplus::Unit::UnitPixel);

	//left side

	tg.DrawImage(&patternbm, Gdiplus::Rect(rc.GetLeft(), rc.GetTop() + depth, depth, rc.Height - (2 * depth)), 0, depth, depth, 1, Gdiplus::Unit::UnitPixel);	
}

void DrawBlurredRectangle(Gdiplus::Graphics& tg, Gdiplus::Rect& rc)
{
	Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0), 4);
	//pen.SetAlignment(PenAlignment::PenAlignmentInset);
	//pen.SetLineJoin(Gdiplus::LineJoinRound);
	Rect r(rc);	
	Gdiplus::Rect sourceRect(0, 0, r.Width, r.Height);
	Gdiplus::Bitmap patternbm(sourceRect.Width, sourceRect.Height);
	Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(&patternbm);
	g->SetInterpolationMode(InterpolationMode::InterpolationModeHighQualityBilinear);
	g->SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);
	g->SetPixelOffsetMode(PixelOffsetMode::PixelOffsetModeHalf);
	Gdiplus::SolidBrush brush(Gdiplus::Color::White);

	g->FillRectangle(&brush, 0, 0, sourceRect.Width, sourceRect.Height);
	g->DrawRectangle(&pen, 0, 0, sourceRect.Width, sourceRect.Height);

	Gdiplus::Blur blur;
	Gdiplus::BlurParams blurParams;
	blurParams.expandEdge = TRUE;
	blurParams.radius = 4.0f;
	blur.SetParameters(&blurParams);	
				
	patternbm.ApplyEffect(&blur, NULL);
//   	patternbm.RotateFlip(Rotate90FlipX);
//   	patternbm.ApplyEffect(&blur, NULL);
//   	patternbm.RotateFlip(Rotate90FlipY);
// 	patternbm.ApplyEffect(&blur, NULL);

	//int newHeight = patternbm.GetHeight();

	tg.DrawImage(&patternbm, rc, 0, 0, sourceRect.Width, sourceRect.Height, Unit::UnitPixel);
}