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
	virtual void PreSubclassWindow();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	afx_msg LRESULT OnMsgViewexpand(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewcollapce(WPARAM wParam, LPARAM lParam);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


