#pragma once

#include "stdafx.h"
#include "TTComboEdit.h"

CTTComboEdit::CTTComboEdit()	
{		
	m_bTracking = false;
	m_bSendInvalidate = false;
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

	CEdit::OnMouseMove(nFlags, point);
}

void CTTComboEdit::OnMouseLeave()
{
	m_bTracking = false;	
	if (m_bSendInvalidate)
		GetParent()->Invalidate();

	CEdit::OnMouseLeave();	
}

void CTTComboEdit::InvalidateParentOnMouseLeave(bool bInvalidate)
{
	m_bSendInvalidate = bInvalidate;
}