#pragma once

#include "stdafx.h"
#include "TTComboListBox.h"
#include "CommonDrawing.h"

using namespace Gdiplus;

CTTComboListBox::CTTComboListBox()
{
    m_bTracking = false;
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

    CListBox::OnMouseMove(nFlags, point);
}

void CTTComboListBox::OnMouseLeave()
{
    m_bTracking = false;

    CListBox::OnMouseLeave();
}