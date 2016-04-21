#pragma once

#include "stdafx.h"
#include "TTComboListBox.h"

CTTComboListBox::CTTComboListBox() : CTTComboListBox(false)
{
}

CTTComboListBox::CTTComboListBox(bool sendMouseLeaveMessage) :
m_bSendMouseEvents(sendMouseLeaveMessage),
m_bTracking(false)
{
}


CTTComboListBox::~CTTComboListBox()
{
}

BEGIN_MESSAGE_MAP(CTTComboListBox, CListBox)	
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

void CTTComboListBox::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = GetSafeHwnd();
		m_bTracking = ::_TrackMouseEvent(&tme) ? true : false;
	}

	/*if (m_bSendMouseEvents)
		GetParent()->SendMessage(WM_MOUSEMOVE, nFlags, MAKELPARAM(point.x, point.y));
*/
	CListBox::OnMouseMove(nFlags, point);
}


void CTTComboListBox::OnMouseLeave()
{
	m_bTracking = false;
	/*if (m_bSendMouseEvents)
		GetParent()->SendMessage(WM_MOUSELEAVE, 0, 0);
*/
	CListBox::OnMouseLeave();
}
