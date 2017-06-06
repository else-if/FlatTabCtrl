#include "stdafx.h"
#include "TTAutoComplete.h"

CTTAutoComplete::CTTAutoComplete()
{
	InternalInit();
}

CTTAutoComplete::CTTAutoComplete(const CSimpleArray<CString>& p_sItemList)
{
	InternalInit();
	SetList(p_sItemList);
}

CTTAutoComplete::~CTTAutoComplete()
{
	m_asList.RemoveAll();

	if (m_pac)
		m_pac.Release();
}


bool CTTAutoComplete::SetList(const CSimpleArray<CString>& p_sItemList)
{
	ATLASSERT(p_sItemList.GetSize() != 0);

	Clear();
	m_asList = p_sItemList;

	return true;
}

bool CTTAutoComplete::Bind(HWND p_hWndEdit, DWORD p_dwOptions, LPCTSTR p_lpszFormatString)
{
	ATLASSERT(::IsWindow(p_hWndEdit));

	if ((m_fBound) || (m_pac))
		return false;

	HRESULT hr = S_OK;

	hr = m_pac.CoCreateInstance(CLSID_AutoComplete);

	if (SUCCEEDED(hr))
	{
		if (p_dwOptions)
		{
			CComQIPtr<IAutoComplete2> pAC2(m_pac);

			ATLASSERT(pAC2);

			hr = pAC2->SetOptions(p_dwOptions);			// This never fails?
			pAC2.Release();
		}

		hr = m_pac->Init(p_hWndEdit, this, NULL, p_lpszFormatString);

		if (SUCCEEDED(hr))
		{
			m_fBound = true;
			return true;
		}
	}

	return false;
}

void CTTAutoComplete::Unbind()
{
	if (!m_fBound)
		return;

	ATLASSERT(m_pac);

	if (m_pac)
	{
		m_pac.Release();
		m_fBound = false;
	}
}

bool CTTAutoComplete::AddItem(CString& p_sItem)
{
	if (p_sItem.GetLength() != 0)
	{
		if (m_asList.Find(p_sItem) == -1)
		{
			m_asList.Add(p_sItem);
			return true;
		}
	}

	return false;
}

bool CTTAutoComplete::AddItem(LPCTSTR p_lpszItem)
{
	return AddItem(CString(p_lpszItem));
}

int CTTAutoComplete::GetItemCount()
{
	return m_asList.GetSize();
}

bool CTTAutoComplete::RemoveItem(CString& p_sItem)
{
	if (p_sItem.GetLength() != 0)
	{
		if (m_asList.Find(p_sItem) != -1)
		{
			m_asList.Remove(p_sItem);
			return true;
		}
	}

	return false;
}

bool CTTAutoComplete::RemoveItem(LPCTSTR p_lpszItem)
{
	return RemoveItem(CString(p_lpszItem));
}

bool CTTAutoComplete::Clear()
{
	if (m_asList.GetSize() != 0)
	{
		m_asList.RemoveAll();
		return true;
	}

	return false;
}

bool CTTAutoComplete::Disable()
{
	if ((!m_pac) || (!m_fBound))
		return false;

	return SUCCEEDED(EnDisable(false));
}

bool CTTAutoComplete::Enable()
{
	if ((!m_pac) || (m_fBound))
		return false;

	return SUCCEEDED(EnDisable(true));
}

void CTTAutoComplete::InternalInit()
{
	m_nCurrentElement = 0;
	m_nRefCount = 0;
	m_fBound = false;
}

HRESULT CTTAutoComplete::EnDisable(BOOL p_fEnable)
{
	HRESULT hr = S_OK;

	ATLASSERT(m_pac);

	hr = m_pac->Enable(p_fEnable);

	if (SUCCEEDED(hr))
		m_fBound = p_fEnable;

	return hr;
}


//	IUnknown implementation

STDMETHODIMP_(ULONG) CTTAutoComplete::AddRef()
{
	return ::InterlockedIncrement(reinterpret_cast<LONG*>(&m_nRefCount));
}

STDMETHODIMP_(ULONG) CTTAutoComplete::Release()
{
	ULONG nCount = 0;
	nCount = (ULONG) ::InterlockedDecrement(reinterpret_cast<LONG*>(&m_nRefCount));

	if (nCount == 0)
		delete this;

	return nCount;
}

STDMETHODIMP CTTAutoComplete::QueryInterface(REFIID riid, void** ppvObject)
{
	HRESULT hr = E_NOINTERFACE;

	if (ppvObject != NULL)
	{
		*ppvObject = NULL;

		if (IID_IUnknown == riid)
			*ppvObject = static_cast<IUnknown*>(this);

		if (IID_IEnumString == riid)
			*ppvObject = static_cast<IEnumString*>(this);

		if (*ppvObject != NULL)
		{
			hr = S_OK;
			((LPUNKNOWN)*ppvObject)->AddRef();
		}
	}
	else
	{
		hr = E_POINTER;
	}

	return hr;
}


//	IEnumString implementation

STDMETHODIMP CTTAutoComplete::Next(ULONG celt, LPOLESTR* rgelt, ULONG* pceltFetched)
{
	HRESULT hr = S_FALSE;

	if (!celt)
		celt = 1;

	ULONG i;
	for (i = 0; i < celt; i++)
	{
		if (m_nCurrentElement == (ULONG)m_asList.GetSize())
			break;

		rgelt[i] = (LPWSTR)::CoTaskMemAlloc((ULONG) sizeof(WCHAR) * (m_asList[m_nCurrentElement].GetLength() + 1));
		lstrcpy(rgelt[i], m_asList[m_nCurrentElement]);

		if (pceltFetched)
			*pceltFetched++;

		m_nCurrentElement++;
	}

	if (i == celt)
		hr = S_OK;

	return hr;
}

STDMETHODIMP CTTAutoComplete::Skip(ULONG celt)
{
	m_nCurrentElement += celt;

	if (m_nCurrentElement > (ULONG)m_asList.GetSize())
		m_nCurrentElement = 0;

	return S_OK;
}

STDMETHODIMP CTTAutoComplete::Reset(void)
{
	m_nCurrentElement = 0;
	return S_OK;
}

STDMETHODIMP CTTAutoComplete::Clone(IEnumString** ppenum)
{
	if (!ppenum)
		return E_POINTER;

	CTTAutoComplete* pnew = new CTTAutoComplete();

	pnew->AddRef();
	*ppenum = pnew;

	return S_OK;
}