#pragma once

class CTTComboEdit : public CEdit
{
public:
	CTTComboEdit();
	virtual ~CTTComboEdit();
	bool IsMouseover() { return m_bTracking; }
	
protected:
	bool m_bTracking;
	
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
};
