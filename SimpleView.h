#pragma once

#include "afxwin.h"
#include "IView.h"
//#include "DragDropObject.h"
#include "TTDropTarget.h"
#include "AnyClass.h"
#include <vector>
#include "anchor.h"

using namespace std;

struct testStr
{
	vector<int> data;
};

class CSimpleView : public CStatic, public IView
{
public:
	CSimpleView(bool hasVScroll = false);
	virtual ~CSimpleView();

	void SetName(CString& name);

	CString GetViewName();
	HWND GetViewHwnd();

	void HandleHScroll(int nPos);
	void HandleVScroll(int nPos);
	SCROLLINFO GetVertScrollBarInfo();

	COLORREF mainColor;

	void InitDrag();

	CString print_bytes(LPVOID obj, size_t size);


	virtual int GetDesiredViewHeight() override;

	void SetStatic(bool bIsStatic);

private:
	int m_hScroll;
	int m_vScroll;
	bool m_bHasVScroll;
	CString m_name;

	bool m_bLBtnmDown;

	bool m_bDragging;

	void UpdateWindowStr();

	CButton m_TestBtn;
	CButton m_TestBtn2;
	CBrush m_Brush;
	CDlgAnchor m_Anchor;

	//CDragDropObject* m_TargetDrop;
	//CTTDropTarget<vector<int>> m_DropTarget;
	//CTTDropTarget<int> m_DropTarget;
	CTTDropTarget<testStr*> m_DropTarget;	

	CPoint					m_StartPoint;
	UINT					m_TimerID;

	//UINT m_Format;

	FORMATETC m_CBFormat;

	bool m_bIsStatic;
public:
	DECLARE_MESSAGE_MAP()	
protected:
	afx_msg void OnKeyDown(UINT keyCode, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRadioBtnClicked();
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	afx_msg LRESULT OnTwmDragEnter(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTwmDragLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTwmDragOver(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTwmDrop(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTwmDragScroll(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam = NULL);
	virtual void PreSubclassWindow();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

