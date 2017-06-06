#pragma once

#include "stdafx.h"

public class IView{

public:

	virtual HWND GetViewHwnd() = 0;

	virtual CString GetViewName() = 0;

	CScrollBar* pVScrollBar = NULL;
	CScrollBar* pHScrollBar = NULL;
	virtual SCROLLINFO GetVertScrollBarInfo() = 0;
	virtual void HandleHScroll(int nPos) = 0;
	virtual void HandleVScroll(int nPos) = 0;

	// Override this for "static" views that should have fixed height
	virtual int GetDesiredViewHeight() { return -1; }
};