#pragma once
#include "BoringStaticCtrl.h"

// BoringEditCtrl2

class BoringEditCtrl2 : public BoringStaticCtrl
{
	DECLARE_DYNAMIC(BoringEditCtrl2)

public:
	BoringEditCtrl2();
	virtual ~BoringEditCtrl2();

	virtual void PreSubclassWindow();

protected:
	CEdit m_edit;
	DECLARE_MESSAGE_MAP()
};


