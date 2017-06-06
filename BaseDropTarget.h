#pragma once
#include "oleidl.h"

interface CBaseDropTarget : public IDropTarget
{
public:
	CBaseDropTarget();
	~CBaseDropTarget();

	//	basic IUnknown stuff
	HRESULT	STDMETHODCALLTYPE	QueryInterface(REFIID iid, void ** ppvObject);
	ULONG	STDMETHODCALLTYPE	AddRef(void);
	ULONG	STDMETHODCALLTYPE	Release(void);

	HRESULT	STDMETHODCALLTYPE	DragEnter(struct IDataObject *, unsigned long, struct _POINTL, unsigned long *);
	HRESULT	STDMETHODCALLTYPE	DragOver(unsigned long, struct _POINTL, unsigned long *);
	HRESULT	STDMETHODCALLTYPE	DragLeave(void);
	HRESULT	STDMETHODCALLTYPE	Drop(struct IDataObject *, unsigned long, struct _POINTL, unsigned long *);

	//	called by parents
	BOOL						Register(CWnd* pWnd, UINT pDataType);
	void						Revoke();

	//	call parent
	virtual	void				GotDrop(void);
	virtual	DWORD				GotDrag(void);
	virtual	void				GotLeave(void);
	virtual	DWORD				GotEnter(void);

public:
	/*BYTE*/LPVOID			*m_Data;
	DWORD m_Len;
	CPoint			m_DropPoint;
	DWORD			m_KeyState;

protected:
	CWnd*			m_DropTargetWnd;
	UINT			m_RegisterType;
	DWORD			m_dwRefCount;
};

