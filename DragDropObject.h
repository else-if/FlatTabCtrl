#pragma once
#include "BaseDropTarget.h"
class CDragDropObject :	public CBaseDropTarget
{
public:
	CDragDropObject();
	~CDragDropObject();

	void	GotDrop(void);
	DWORD	GotDrag(void);
	void	GotLeave(void);
	DWORD	GotEnter(void);
};

