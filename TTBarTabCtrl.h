#pragma once
#include "afxoutlookbartabctrl.h"
#include "ControlsColorMap.h"

class CTTBarTabCtrl :	public CMFCOutlookBarTabCtrl
{
public:
	CTTBarTabCtrl();
	virtual ~CTTBarTabCtrl();

	void SetDrawingProperties(int borderPenWidth, int cornerRadius);

	void DrawTabButton(CDC& dc, int iButtonIdx, BOOL bDrawPressedButton = TRUE);

	void SetUseDefaultColors(bool bUseDefault) { m_bUseDefaultColor = bUseDefault; InitColorMap(); Invalidate(); };
	bool GetUseDefaultColors(bool bUseDefault) { return m_bUseDefaultColor; };

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();

public:
	virtual BOOL Create(const CRect& rect, CWnd* pParentWnd, UINT nID) override;

	virtual BOOL SetActiveTab(int iTab) override;

protected:
	ControlsColorMap m_ColorMap;

	int m_CornerRadius;

private:
	bool m_bUseDefaultColor;

	void InitColorMap();
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

