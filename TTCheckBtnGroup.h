#pragma once
#include "ControlsColorMap.h"
#include <vector>
#include "afxwin.h"

using namespace std;

typedef struct _HLSColor
{
	double H;
	double L;
	double S;
	double A;
} HLSColor;

class CTTCheckBtnGroup : public CButton
{
public:
	CTTCheckBtnGroup();
	virtual ~CTTCheckBtnGroup();

	void SetDrawingProperties(int borderPenWidth, int cornerRadius);

	void AddButton(CString text, bool isChecked);
	
	void RecalcLayout();

	void SetPressColor(COLORREF cl);

	bool IsChecked(int btnId);
	void SetProperWidth(int maxWidth);
private:
	struct BtnInfo
	{
		CString m_text;
		bool m_checked;
		bool m_howered;
		CRect m_rect;
	};

	int m_BorderPenWidth;
	int m_CornerRadius;
	
	vector<BtnInfo> m_Buttons;

	ControlsColorMap m_ColorMap;

	bool m_bTracking;
	void InvalidateButtonRect(BtnInfo* pBtnInfo);
	ControlState GetButtonState(BtnInfo* pBtnInfo);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg BOOL OnBnClicked();
private:
	int m_defaultButtonWidth;
	void DrawGradient(CDC* pDC, CRect t);

	COLORREF GetLumColor(COLORREF baseCl, double lumOffset, double minLum = 0.02, double maxLum = 0.98);
	COLORREF HLSToRGB(HLSColor hls);
	double SetColor(double t1, double t2, double t3);
	HLSColor RgbToHls(COLORREF rgbColor);
};

