#pragma once


// BoringStaticCtrl

class BoringStaticCtrl : public CStatic
{
	DECLARE_DYNAMIC(BoringStaticCtrl)

public:
	BoringStaticCtrl();
	virtual ~BoringStaticCtrl();

	COLORREF BackgroundColor();
	void BackgroundColor(COLORREF backgroundColor);

protected:
	COLORREF m_backgroundColor;
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
};


