#pragma once


// CPaneChildDlg dialog

class CPaneChildDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPaneChildDlg)

public:
	CPaneChildDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneChildDlg();

// Dialog Data
	enum { IDD = IDD_PANE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};


class CTTDragFrameImpl : public CMFCDragFrameImpl
{
public:
	CTTDragFrameImpl(CWnd* pParentWnd)
	{	
		m_pWndDummy = new CDockablePaneAdapter();
		m_pWndDummy->CreateEx(0, _T(""), pParentWnd, CRect(0, 0, 0, 0), FALSE, -2, WS_CHILD);
		i = rand() % 100;
	}
	virtual ~CTTDragFrameImpl()
	{		
		if (m_pWndDummy)
		{
			//m_pWndDummy = NULL;
			m_pWndDummy->DestroyWindow();
			delete m_pWndDummy;
			m_pWndDummy = NULL;
		}
	}

	int i = 0;
};

class CTTPaneFrameWnd : public CPaneFrameWnd
{
	DECLARE_SERIAL(CTTPaneFrameWnd)

public:
	CTTPaneFrameWnd()
	{
		m_impl = NULL;
	}
	virtual ~CTTPaneFrameWnd()
	{		
		*&m_dragFrameImpl = m_OrigImpl;
		//CTTDragFrameImpl* pttimpl = dynamic_cast<CTTDragFrameImpl*>(&m_dragFrameImpl);
		//delete pttimpl;
		if (m_impl)
		{
			delete m_impl;
			m_impl = NULL;
			//*&m_dragFrameImpl = *m_impl;
			//m_impl = NULL;
		}
	}

	virtual BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, CCreateContext* pContext = NULL) override
	{
		*&m_OrigImpl = m_dragFrameImpl;

		m_impl = new CTTDragFrameImpl(pParentWnd);
		*&m_dragFrameImpl = *m_impl;

		BOOL result = __super::Create(lpszWindowName, dwStyle, rect, pParentWnd, pContext);
		
		return result;
	}

	void SetMainFrame(CFrameWndEx* pMainFrame){ m_pMainFrame = pMainFrame; }

private:
	CMFCDragFrameImpl m_OrigImpl;
	CTTDragFrameImpl* m_impl;
	CFrameWndEx* m_pMainFrame;
};

/***********************************************************************/
class CDialogPane : public CDockablePane
{
	DECLARE_DYNAMIC(CDialogPane)
	DECLARE_MESSAGE_MAP()
public:
	CDialogPane();
	virtual ~CDialogPane();

	//virtual BOOL CanFloat() const override { return FALSE; }
	
	CFrameWnd* GetTopLevelFrame() const;

	CTTDragFrameImpl* m_customImpl;

	void SetMainFrame(CFrameWndEx* pMainFrame){ m_pMainFrame = pMainFrame; }

	virtual CPaneFrameWnd* CreateDefaultMiniframe(CRect rectInitial) override
	{
		ASSERT_VALID(this);

		CRect rectVirtual = rectInitial;

		CPaneFrameWnd* pMiniFrame =
			(CPaneFrameWnd*)m_pMiniFrameRTC->CreateObject();

		if (pMiniFrame != NULL)
		{
			// it must have valid CFrameEx window as parent
			CWnd* pParentFrame = m_pMainFrame;
			ASSERT_VALID(pParentFrame);

			pMiniFrame->SetDockingManager(afxGlobalUtils.GetDockingManager(GetDockSiteFrameWnd()));

			if (!pMiniFrame->Create(NULL, WS_POPUP, rectVirtual, pParentFrame))
			{
				TRACE0("Failed to create miniframe");
				delete pMiniFrame;
				return NULL;
			}
		}
		else
		{
			TRACE0("Failed to create miniframe using runtime class information \n");
			ASSERT(FALSE);
		}
		return pMiniFrame;
	}

	/*CFrameWnd* AFX_CDECL AFXGetParentFrame(const CWnd* pWnd)
	{
		return m_pMainFrame;
	}*/

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lp);
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	CMFCDragFrameImpl m_OrigImpl;
	CPaneChildDlg m_wndDlg;
	CFrameWndEx* m_pMainFrame;
};