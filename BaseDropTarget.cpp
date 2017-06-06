#include "stdafx.h"
#include "BaseDropTarget.h"

CBaseDropTarget::CBaseDropTarget()
	: m_DropTargetWnd(NULL),
	m_dwRefCount(1),
	m_RegisterType(0),
	m_KeyState(0L),
	m_Data(NULL)
{
	OleInitialize(NULL);

	//	required: these MUST be strong locked
	CoLockObjectExternal(this, TRUE, 0);
}

CBaseDropTarget::~CBaseDropTarget()
{
	//	unlock
	CoLockObjectExternal(this, FALSE, 0);

	OleUninitialize();
}

HRESULT CBaseDropTarget::QueryInterface(REFIID iid, void **ppvObject)
{
	if (ppvObject == NULL)
		return E_FAIL;

	if (iid == IID_IUnknown)
	{
		AddRef();
		(*ppvObject) = this;
		return S_OK;
	}
	
	if (iid == IID_IDropTarget)
	{
		AddRef();
		(*ppvObject) = this;
		return S_OK;
	}

	return E_FAIL;
}

ULONG CBaseDropTarget::AddRef(void)
{
	m_dwRefCount++;

	return m_dwRefCount;
}

ULONG CBaseDropTarget::Release(void)
{
	m_dwRefCount--;
	
	return m_dwRefCount;
}

BOOL CBaseDropTarget::Register(CWnd* pWnd, UINT pDataType)
{
	if (NULL == pWnd)
		return E_FAIL;

	if (0L == pDataType)
		return E_FAIL;
	
	//	keep
	m_DropTargetWnd = pWnd;
	m_RegisterType = pDataType;

	//	this is ok, we have it
	DWORD hRes = ::RegisterDragDrop(m_DropTargetWnd->m_hWnd, this);
	if (SUCCEEDED(hRes))
		return TRUE;

	//	wont accept data now
	return FALSE;
}

void CBaseDropTarget::Revoke()
{
	if (NULL == m_DropTargetWnd)
		return;

	RevokeDragDrop(m_DropTargetWnd->m_hWnd);
}

HRESULT	CBaseDropTarget::DragEnter(struct IDataObject *pDataObject, unsigned long grfKeyState, struct _POINTL pMouse, unsigned long * pDropEffect)
{
	if (pDataObject == NULL)
		return E_FAIL;	//	must have data

	//	keep point
	m_DropPoint.x = pMouse.x;
	m_DropPoint.y = pMouse.y;

	//	keep key
	m_KeyState = grfKeyState;

	//	call top
	*pDropEffect = GotEnter();

	return S_OK;
}

HRESULT	CBaseDropTarget::DragOver(unsigned long grfKeyState, struct _POINTL pMouse, unsigned long *pEffect)
{
	//	keep point
	m_DropPoint.x = pMouse.x;
	m_DropPoint.y = pMouse.y;

	//	keep key
	m_KeyState = grfKeyState;

	//	call top
	*pEffect = GotDrag();

	return S_OK;
}

HRESULT	CBaseDropTarget::DragLeave(void)
{
	GotLeave();

	return S_OK;
}

HRESULT	CBaseDropTarget::Drop(struct IDataObject *pDataObject, unsigned long grfKeyState, struct _POINTL pMouse, unsigned long *pdwEffect)
{
	if (NULL == pDataObject)
		return E_FAIL;

	//	do final effect
	*pdwEffect = DROPEFFECT_COPY;

	//	Check the data
	FORMATETC iFormat;
	ZeroMemory(&iFormat, sizeof(FORMATETC));

	STGMEDIUM iMedium;
	ZeroMemory(&iMedium, sizeof(STGMEDIUM));

	//	data
	iFormat.cfFormat = m_RegisterType;	//	its my type
	iFormat.dwAspect = DVASPECT_CONTENT;
	iFormat.lindex = -1;			//	give me all
	iFormat.tymed = TYMED_HGLOBAL;	//	want mem

	HRESULT hRes = pDataObject->GetData(&iFormat, &iMedium);
	if (FAILED(hRes))
	{
		return hRes;
	}

	//	we have the data, get it		
	//BYTE *iMem = (BYTE *)::GlobalLock(iMedium.hGlobal);
	LPVOID *iMem = (LPVOID *)::GlobalLock(iMedium.hGlobal);
	DWORD iLen = ::GlobalSize(iMedium.hGlobal);

	//	pass over
	m_Data = iMem;
	m_Len = iLen;

	//	keep point
	m_DropPoint.x = pMouse.x;
	m_DropPoint.y = pMouse.y;

	//	keep key
	m_KeyState = grfKeyState;

	//	notify parent of drop
	GotDrop();

	::GlobalUnlock(iMedium.hGlobal);

	//	free data
	if (iMedium.pUnkForRelease != NULL)
		iMedium.pUnkForRelease->Release();

	return S_OK;
}

void CBaseDropTarget::GotDrop()
{
}

DWORD CBaseDropTarget::GotDrag(void)
{
	return DROPEFFECT_LINK;
}

void CBaseDropTarget::GotLeave(void)
{
}

DWORD CBaseDropTarget::GotEnter(void)
{
	return DROPEFFECT_LINK;
}

