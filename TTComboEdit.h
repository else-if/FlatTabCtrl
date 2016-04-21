#pragma once

class CTTComboEdit : public CEdit
{
public:
	CTTComboEdit();
	CTTComboEdit(bool sendMouseLeaveMessage);
	virtual ~CTTComboEdit();
	bool IsMouseover() { return m_bTracking; }
	void SetMouseEventsNotification(bool DoSend) { m_bSendMouseEvents = DoSend; }

protected:
	bool m_bTracking;
	bool m_bSendMouseEvents;
	//bool m_bHasFocus;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
};
