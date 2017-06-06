#pragma once
#include "afxole.h"
#include "CustomMessages.h"
#include <exception>

template<typename DataType>
class CTTDropTarget : public COleDropTarget
{
public:
	CTTDropTarget();
	~CTTDropTarget();

	BOOL Register(CWnd* pWnd, FORMATETC cbFormat);

	typedef struct tagDragParameters				// data passed with drag event messages
	{
		DWORD			dwKeyState;					// WM_APP_DRAG_ENTER, WM_APP_DRAG_OVER
		CPoint			point;
		DROPEFFECT		dropEffect;					// dropDefault parameter with WM_APP_DROP_EX
		DROPEFFECT		dropList;					// with WM_APP_DROP_EX only
	} DragParameters;
	
	// Overrides
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);
	virtual DROPEFFECT OnDropEx(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point);

	LPVOID GetLpDataObject();

	DataType* GetDataObject();
	
	DWORD GetSize();

protected:

	FORMATETC  m_CBFormat;
	DataType* m_pDataObject;
	DataType m_DataObject;

	LPVOID m_lpDataObject;

	DWORD m_dwSize;

private:
	void ResolveDataObject(COleDataObject* pDataObject);
};

template<typename DataType>
CTTDropTarget<DataType>::CTTDropTarget()
{
	m_pDataObject = NULL;
	ZeroMemory(&m_DataObject, sizeof(DataType));
}

template<typename DataType>
CTTDropTarget<DataType>::~CTTDropTarget()
{
	if (m_pDataObject)
	{
		delete m_pDataObject;
		m_pDataObject = NULL;
	}
	ZeroMemory(&m_DataObject, sizeof(DataType));
}

template<typename DataType>
BOOL CTTDropTarget<DataType>::Register(CWnd* pWnd, FORMATETC cbFormat)
{
	if (pWnd == NULL)
		return E_FAIL;

	if (cbFormat.cfFormat == 0L)
		return E_FAIL;

	m_CBFormat = cbFormat;

	return COleDropTarget::Register(pWnd);
}

// Called by OLE when drag cursor first enters a registered window.
// Use this for:
// - Determination if target is able to drop data.
// - Optional highlight target.
// - Optional set drop description cursor type and text
template<typename DataType>
DROPEFFECT CTTDropTarget<DataType>::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	DROPEFFECT dwRet = DROPEFFECT_NONE;

	ResolveDataObject(pDataObject);

	/*if (m_pDataObject == NULL)
		return dwRet;*/

	DragParameters Params = { 0 };
	Params.dwKeyState = dwKeyState;
	Params.point = point;
	dwRet = static_cast<DROPEFFECT>(pWnd->SendMessage(
		TWM_DRAG_ENTER,		
		reinterpret_cast<WPARAM>(this),
		reinterpret_cast<LPARAM>(&Params)));

	return dwRet;
}

// Called when cursor is dragged over a registered window and
//  before OnDropEx/OnDrop when mouse button is released.
// Note: OnDragOver is like the WM_MOUSEMOVE message, it is called repeatedly
//   when the mouse moves inside the window.
// The mainly purpose of this function is to change the cursor according to
//  the key state.
// When dropping, the return value is passed to OnDropEx/OnDrop.
template<typename DataType>
DROPEFFECT CTTDropTarget<DataType>::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	DROPEFFECT dwRet = DROPEFFECT_NONE;

	/*if (m_pDataObject == NULL)
		return dwRet;*/

	DragParameters Params = { 0 };
	Params.dwKeyState = dwKeyState;
	Params.point = point;
	dwRet = static_cast<DROPEFFECT>(pWnd->SendMessage(
		TWM_DRAG_OVER,
		reinterpret_cast<WPARAM>(this),
		reinterpret_cast<LPARAM>(&Params)));

	return dwRet;
}

// Called when drag cursor leaves a registered window.
// Use this for cleanup (e.g. remove highlighting).
// NOTE: 
//  This is not called when OnDrop or OnDropEx are implemented in a window class and has been called!
//  So it may be necessary to perform a similar clean up at the end of OnDrop or OnDropEx of the window class!
template<typename DataType>
void CTTDropTarget<DataType>::OnDragLeave(CWnd* pWnd)
{
	pWnd->SendMessage(TWM_DRAG_LEAVE,
		reinterpret_cast<WPARAM>(this), 0L);

	m_pDataObject = NULL;
}

// Called when mouse button is released.
// Before this is called, OnDragOver() is called and the return value is passed in the dropDefault parameter.
// Note that this is always called even when OnDragOver() returned DROPEFFECT_NONE.
// So implementations should check dropDefault and do nothing if it is DROPEFFECT_NONE.
// They may optionally return -1 in this case to get OnDragLeave() called.
template<typename DataType>
DROPEFFECT CTTDropTarget<DataType>::OnDropEx(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point)
{
	// Skip this handler and let "OnDrop" handle an event 
	return (DROPEFFECT)-1;
}

// Called when mouse button is released, OnDropEx() returns -1 and dropEffect != DROPEFFECT_NONE.
// dropEffect is the value returned by OnDragOver() called just before this.
// OnDragLeave() is not called. So it may be necessary to perform cleanup here.
// Return TRUE when data has been dropped.
// The drop effect passed to the DoDragDrop() call of the source is dropEffect 
//  when returning TRUE here or DROPEFFECT_NONE when returning FALSE.
template<typename DataType>
BOOL CTTDropTarget<DataType>::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	BOOL bRet = FALSE;

	/*if (m_pDataObject == NULL)
		return bRet;*/

	//ResolveDataObject(pDataObject);

	DragParameters Params = { 0 };
	Params.dropEffect = dropEffect;
	Params.point = point;
	bRet = static_cast<BOOL>(pWnd->SendMessage(TWM_DROP,
		reinterpret_cast<WPARAM>(this),
		reinterpret_cast<LPARAM>(&Params)));

	return bRet;
}

// This is called before OnDragEnter() and OnDragOver().
// Implemented functions should return:
// - DROPEFFECT_NONE:   
//    When not scrolling or not handling the event.
// - DROPEFFECT_SCROLL with DROPEFFECT_COPY, DROPEFFECT_MOVE, or DROPEFFECT_LINK. 
//    When scrolling is active.
// NOTE: 
//  When the value returned here has the DROPEFFECT_SCROLL bit set indicating
//   that scrolling is active, OnDragEnter() and OnDragOver() are not called afterwards.
template<typename DataType>
DROPEFFECT CTTDropTarget<DataType>::OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point)
{
	DROPEFFECT dwEffect = DROPEFFECT_NONE;

	/*if (m_pDataObject == NULL)
		return dwEffect;*/

	DragParameters Params = { 0 };
	Params.dwKeyState = dwKeyState;
	Params.point = point;

	dwEffect = static_cast<DROPEFFECT>(pWnd->SendMessage(
		TWM_DRAG_SCROLL,
		reinterpret_cast<WPARAM>(this),
		reinterpret_cast<LPARAM>(&Params)));

	return dwEffect;
}

template<typename DataType>
DataType* CTTDropTarget<DataType>::GetDataObject()
{
	return m_pDataObject;
	//return &m_DataObject;
}

template<typename DataType>
LPVOID CTTDropTarget<DataType>::GetLpDataObject()
{
	return m_lpDataObject;
}

template<typename DataType>
void CTTDropTarget<DataType>::ResolveDataObject(COleDataObject* pDataObject)
{
	/*if (m_pDataObject)
		delete m_pDataObject;*/

	//ZeroMemory(&m_DataObject, sizeof(DataType));
	m_pDataObject = NULL;
	m_dwSize = 0;

	if (pDataObject == NULL)
		return;

	if (!pDataObject->IsDataAvailable(m_CBFormat.cfFormat))
		return;

	/*STGMEDIUM iMedium;
	ZeroMemory(&iMedium, sizeof(STGMEDIUM));*/

	HGLOBAL hGlobal = pDataObject->GetGlobalData(m_CBFormat.cfFormat);

	/*if (dwSize != sizeof(DataType))
		return;*/

	if (hGlobal && false)
	{
		TRACE(_T("HGLOBAL after: %p\n"), hGlobal);

		DWORD dwSize = GlobalSize(hGlobal);

		m_dwSize = dwSize;

		LPVOID lpGlobal = GlobalLock(hGlobal);		

		ASSERT(AfxIsValidAddress(lpGlobal, dwSize, FALSE));

		//ASSERT(sizeof(DataType) == dwSize);

		unsigned char* b = new unsigned char [dwSize];
		
		CopyMemory(b, lpGlobal, dwSize);

		m_pDataObject = reinterpret_cast<DataType*>(b);

		//swapPtr(m_pDataObject, b);		

		//m_pDataObject = new DataType();
		//m_pDataObject = static_cast<DataType*>(lpGlobal);
		//CopyMemory(b, lpGlobal, dwSize);

		
		//m_pDataObject = static_cast<DataType*>(static_cast<void*>(b));

		/*m_lpDataObject = malloc(dwSize);
		//m_pDataObject = new DataType();
		//BYTE* pByte = new BYTE[dwSize];
		CopyMemory(m_lpDataObject, lpGlobal, dwSize);*/
		
		//m_pDataObject = reinterpret_cast<DataType*>(pByte);

		//delete pByte;
		//m_pDataObject = new DataType();
		//CopyMemory(m_pDataObject, lpGlobal, dwSize);
		
		GlobalUnlock(hGlobal);		
	}

	/*if (!pDataObject->GetData(m_CBFormat.cfFormat, &iMedium, &m_CBFormat))
		return;

	DWORD dwSize = ::GlobalSize(iMedium.hGlobal);

	if (dwSize != sizeof(DataType))
		return;

	::GlobalLock(iMedium.hGlobal);
	::CopyMemory(&m_DataObject, m_pDataObject, dwSize);

	::GlobalUnlock(iMedium.hGlobal);

	if (iMedium.pUnkForRelease != NULL)
		iMedium.pUnkForRelease->Release();*/
}

template<typename DataType>
DWORD CTTDropTarget<DataType>::GetSize()
{
	return m_dwSize;
}

/*void swapPtr(void*& p1, void*& p2)
{
	p1 = p2;
}*/