#pragma once

class CTTAutoComplete : public IEnumString
{

private:

	CSimpleArray<CString> m_asList;
	CComPtr<IAutoComplete> m_pac;

	ULONG m_nCurrentElement;
	ULONG m_nRefCount;
	bool m_fBound;

public:

	CTTAutoComplete();
	CTTAutoComplete(const CSimpleArray<CString>& p_sItemList);
	~CTTAutoComplete();

	bool SetList(const CSimpleArray<CString>& p_sItemList);

	bool Bind(HWND p_hWndEdit, DWORD p_dwOptions = 0, LPCTSTR p_lpszFormatString = NULL);
	void Unbind();

	bool AddItem(CString& p_sItem);
	bool AddItem(LPCTSTR p_lpszItem);
	bool RemoveItem(CString& p_sItem);
	bool RemoveItem(LPCTSTR p_lpszItem);
	int GetItemCount();
	bool Clear();
	
	bool Disable();
	bool Enable();

private:

	void InternalInit();
	HRESULT EnDisable(BOOL p_fEnable);

public:

	//	IUnknown methods
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);

	//	IEnumString methods	
	STDMETHODIMP Next(ULONG celt, LPOLESTR* rgelt, ULONG* pceltFetched);
	STDMETHODIMP Skip(ULONG celt);
	STDMETHODIMP Reset(void);
	STDMETHODIMP Clone(IEnumString** ppenum);
};