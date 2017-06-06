#pragma once

#include "stdafx.h"
#include "TTComboListBox.h"
#include "CommonDrawing.h"
#include "TTComboBox.h"

using namespace Gdiplus;

CTTComboListBox::CTTComboListBox()
{
	m_bTracking = false;
	m_pParent = NULL;
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

BOOL CTTComboListBox::PreTranslateMessage(MSG* pMsg)
{
	if (m_pParent == NULL || !m_pParent->MultiSelectionEnabled())
		return __super::PreTranslateMessage(pMsg);

	if (pMsg->message == WM_LBUTTONDOWN)
	{
		if ((GetKeyState(VK_LSHIFT) & 0x8000) || (GetKeyState(VK_RSHIFT) & 0x8000))
		{
			if (m_pParent->m_nBegin == -1)
			{
				m_pParent->m_nBegin = m_pParent->GetCurSel();
				m_pParent->m_nEnd = m_pParent->m_nBegin;
			}
			else
			{
				m_pParent->m_nEnd = m_pParent->GetCurSel();
				if (m_pParent->m_nEnd <= m_pParent->m_nBegin)
				{
					if (!((GetKeyState(VK_LCONTROL) & 0x8000) || (GetKeyState(VK_RCONTROL) & 0x8000)))
						m_pParent->ClearSelectedItems();
					for (int i = m_pParent->m_nEnd; i <= m_pParent->m_nBegin; i++)
					{
						int itemId = i;
						if (itemId != -1)
							m_pParent->AddSelectedItem(itemId);
					}
				}
				else
				{
					if (!((GetKeyState(VK_LCONTROL) & 0x8000) || (GetKeyState(VK_RCONTROL) & 0x8000)))
						m_pParent->ClearSelectedItems();
					for (int i = m_pParent->m_nBegin; i <= m_pParent->m_nEnd; i++)
					{
						int itemId = i;
						if (itemId != -1)
							m_pParent->AddSelectedItem(itemId);
					}
				}
				
				m_pParent->UpdateText();
				m_pParent->SetCurSel(m_pParent->m_nEnd);
				Invalidate();
				
				return TRUE;
			}
		}
		else if ((GetKeyState(VK_LCONTROL) & 0x8000) || (GetKeyState(VK_RCONTROL) & 0x8000))
		{
			m_pParent->m_nBegin = m_pParent->GetCurSel();
			m_pParent->m_nEnd = m_pParent->m_nBegin;
			int itemId = m_pParent->m_nBegin;
			if (itemId != -1)
			{
				if (m_pParent->IsSelected(itemId))
					m_pParent->RemoveSelectedItem(itemId);
				else
					m_pParent->AddSelectedItem(itemId);
			}
			
			m_pParent->UpdateText();
			m_pParent->SetCurSel(m_pParent->m_nBegin);
			Invalidate();
			
			return TRUE;
		}
	}

	return __super::PreTranslateMessage(pMsg);	
}

void CTTComboListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	m_pParent->DrawItem(lpDrawItemStruct);
}
