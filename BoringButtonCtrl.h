#pragma once


// BoringButtonCtrl

class BoringButtonCtrl : public CButton
{
	DECLARE_DYNAMIC(BoringButtonCtrl)

public:
	BoringButtonCtrl();
	virtual ~BoringButtonCtrl();

protected:
	COLORREF m_backgroundColor;
	DECLARE_MESSAGE_MAP()

	virtual void DrawItem(
		LPDRAWITEMSTRUCT lpDrawItemStruct
		);

public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};  


