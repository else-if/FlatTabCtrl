#pragma once

class CTTButton : public CButton
{

public:
	CTTButton();

protected:

	enum mode
	{
		normal  = 0,
		select  = 1,
		focus   = 2,
		disable = 3
	};
		
protected:
		
	COLORREF m_NormalStateGradientStart;
	COLORREF m_NormalStateGradientFinish;

	COLORREF m_BorderNormalStateGradientStart;
	COLORREF m_BorderNormalStateGradientFinish;

	COLORREF m_SelectStateGradientStart;
	COLORREF m_SelectStateGradientFinish;

	COLORREF m_FocusedStateGradientStart;
	COLORREF m_FocusedStateGradientFinish;

	COLORREF m_DisabledStateGradientStart;
	COLORREF m_DisabledStateGradientFinish;

	COLORREF m_OutlineColor;	

	int m_CornerRadius;

	bool m_bCheckBox;
	bool m_bChecked;
	CFont m_TextFont;
	bool m_bTracking;

public:
	
	void SetNormalStateGradients(COLORREF start, COLORREF finish);
	void SetSelectedStateGradients(COLORREF start, COLORREF finish);
	void SetFocusedStateGradients(COLORREF start, COLORREF finish);
	void SetDisabledStateGradients(COLORREF start, COLORREF finish);

	void SetOutlineColor(COLORREF color);

	void ActAsCheckBox(bool fActAsCheckBox);

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);	

	void DrawButtonBackground(CRect &rc, CMemDC &dc, COLORREF colorStart, COLORREF colorFinish, int cornerRadius);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	virtual ~CTTButton();
	
protected:

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);	
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	
	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();	
};