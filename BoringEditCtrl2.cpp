// BoringEditCtrl2.cpp : implementation file
//

#include "stdafx.h"
#include "FlatTabCtrl.h"
#include "BoringEditCtrl2.h"


// BoringEditCtrl2

IMPLEMENT_DYNAMIC(BoringEditCtrl2, BoringStaticCtrl)

BoringEditCtrl2::BoringEditCtrl2()
{
}


BoringEditCtrl2::~BoringEditCtrl2()
{
}


BEGIN_MESSAGE_MAP(BoringEditCtrl2, BoringStaticCtrl)
END_MESSAGE_MAP()



// BoringEditCtrl2 message handlers

void BoringEditCtrl2::PreSubclassWindow()
{
	BoringStaticCtrl::PreSubclassWindow();

	SetWindowText(L"");
	ModifyStyleEx(0, WS_EX_TRANSPARENT);

	CRect cRect;
	GetClientRect(&cRect);
	cRect.DeflateRect(5, 5);
	m_edit.Create(ES_LEFT | WS_VISIBLE, cRect, this, 10001);
	m_edit.SetFont(GetFont());
}
