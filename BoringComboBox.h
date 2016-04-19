#pragma once


// CBoringComboBox

class CBoringComboBox : public CComboBoxEx
{
	DECLARE_DYNAMIC(CBoringComboBox)

public:
	CBoringComboBox();
	virtual ~CBoringComboBox();

	COLORREF BackgroundColor();
	void BackgroundColor(COLORREF backgroundColor);

protected:
	bool m_bTracking;
	bool m_bOnButton;
	CEdit m_edit;
	COLORREF m_backgroundColor;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	virtual void PreSubclassWindow();
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


