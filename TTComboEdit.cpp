#pragma once

#include "stdafx.h"
#include "TTComboEdit.h"

CTTComboEdit::CTTComboEdit() : CTTComboEdit(false)
{	
}

CTTComboEdit::CTTComboEdit(bool sendMouseLeaveMessage) :
m_bSendMouseEvents(sendMouseLeaveMessage),
m_bTracking(false)
{	
}

CTTComboEdit::~CTTComboEdit()
{
}

BEGIN_MESSAGE_MAP(CTTComboEdit, CEdit)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

void CTTComboEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = GetSafeHwnd();
		m_bTracking = ::_TrackMouseEvent(&tme) ? true : false;
	}

	//if (m_bSendMouseEvents)
		//GetParent()->SendMessage(WM_MOUSEMOVE, nFlags, MAKELPARAM(point.x, point.y));
	GetParent()->SendMessage(WM_PAINT, 0, 0);

	CEdit::OnMouseMove(nFlags, point);
}

void CTTComboEdit::OnMouseLeave()
{
	m_bTracking = false;	
	/*if (m_bSendMouseEvents)
	*///	GetParent()->SendMessage(WM_MOUSELEAVE, 0, 0);
	GetParent()->SendMessage(WM_PAINT, 0, 0);
	CEdit::OnMouseLeave();	
}
