#pragma once

class CTTComboListBox : public CListBox
{
public:
	CTTComboListBox();
	CTTComboListBox(bool sendMouseLeaveMessage);
	virtual ~CTTComboListBox();
	bool IsMouseover() { return m_bTracking; }
	void SetMouseEventsNotification(bool DoSend) { m_bSendMouseEvents = DoSend; }

protected:
	bool m_bTracking;
	bool m_bSendMouseEvents;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
};
