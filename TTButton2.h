#pragma once
#include "TTButton.h"

class CTTButton2 : public CTTButton
{
	DECLARE_DYNAMIC(CTTButton2)

	public:
		CTTButton2();
		virtual ~CTTButton2();

		virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	DECLARE_MESSAGE_MAP()
};
