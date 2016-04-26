#pragma once
#include "ControlsColorMap.h"
#include "TTComboEdit.h"
#include "TTComboListBox.h"

class CTTComboBox : public CComboBox
{

public:
	CTTComboBox();
	virtual ~CTTComboBox();

	void SetDrawingProperties(int borderPenWidth, int cornerRadius);

protected:

	CTTComboEdit m_edit;
	CTTComboListBox m_listbox;

	ControlState m_ControlState;
	ControlState m_ArrowButtonState;
	ControlsColorMap m_ColorMap;

	COLORREF m_arrowColorStart;
	COLORREF m_arrowColorFinish;

	DWORD m_iComboBoxStyle;

	int m_borderPenWidth;
	int m_CornerRadius;
	bool m_bTracking;
	bool m_bHasFocus;
	bool m_bOnButton;
	bool m_bButtonPress;
	bool m_bFilledArrow;
	
	void UpdateControlState();
	
protected:	

	void DrawSimple();
	void DrawDropDown();
	void DrawDropDownList();
	void DrawArrowButton(CRect &cRect, CMemDC &memDC, Gdiplus::Graphics &graphics);
	void DrawSelectedItemText(CRect &cRect, CMemDC &memDC, Gdiplus::Graphics &graphics);
	CRect GetArrowButtonRect();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();	
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCbnKillfocus();
	afx_msg void OnCbnSetfocus();

};

