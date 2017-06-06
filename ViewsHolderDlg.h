#pragma once

#include <vector>
#include "IView.h"
#include "ViewHeader.h"
#include "resource.h"
#include "TTCheckBtnGroup.h"

// CViewsHolderDlg dialog

using namespace std;

struct ViewInfo
{
	IView* View;

	bool IsCollapsed;
	bool IsClosed;
	bool ShowHeader;

	double nHeightProc;

	CRect ViewRect;
	CRect HeaderRect;
	CRect VScrollRect;

	CScrollBar* pVScrollBar;

	ViewInfo(IView* view, bool isCollapsed, bool isClosed)
		: ViewInfo(view, isCollapsed, isClosed, true)
	{
	}

	ViewInfo(IView* view, bool isCollapsed, bool isClosed, bool showHeader)
	{
		View = view;
		IsCollapsed = isCollapsed;
		IsClosed = isClosed;
		ShowHeader = showHeader;

		pVScrollBar = NULL;

		if (isCollapsed || isClosed)
			nHeightProc = 0.0f;
		else
			nHeightProc = 1.0f;
	}
};

enum StaticViewAlign
{
	Top,
	Bottom
};

struct StaticViewInfo : ViewInfo
{
	StaticViewAlign Align;
	StaticViewInfo(IView* pView, StaticViewAlign viewAlign)
		: ViewInfo(pView, false, false, false)
	{		
		Align = viewAlign;
	}
};

struct SplitterInfo
{
	ViewInfo* UpperViewInfo;
	ViewInfo* LowerViewInfo;
	CRect SplitterRect;
};

class CViewsHolderDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CViewsHolderDlg)

public:
	CViewsHolderDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CViewsHolderDlg();

	// Dialog Data
	enum { IDD = IDD_VIEWS_HOLDER };

	void AddView(IView* view);
	void AddView(IView* view, bool isCollapsed, bool isClosed, float heigthProc);
	void AddView(IView* view, bool isCollapsed, bool isClosed, float heigthProc, bool showHeader);
	void RemoveView(IView* pView);
	void RemoveAllViews();
	void PlaceViews();

	void HScrollViews(int nPos);

	void InvalidateViews(BOOL bErase = TRUE);

	IView* GetFocusedView();
	bool IsViewClosed(IView* pView);

private:

	HACCEL m_hAccelTable;

	vector<pair<CViewHeader*, ViewInfo>> m_Views;
	vector<SplitterInfo> m_Splitters;

	vector<StaticViewInfo> m_StaticViews;

	int m_nSplitterOffset;

	bool m_bResizing;
	int m_nHoveredSplitterId;
	int m_nHeaderHeight;

	void SetViewHeightProc(ViewInfo* pViewInfo, float nHeightProc);
	void SpreadSpace();

	int GetExpandedViewsCount();
	int GetClosedViewsCount();
	int GetShowedHeadersCount();
	float GetTotalProc();

	ViewInfo* GetCurrentViewInfo();
	ViewInfo* GetExpandedUpperViewInfo(int splitterId);
	ViewInfo* GetExpandedLowerViewInfo(int splitterId);

	void PlaceStaticViews();
	int GetStaticViewsHeight(StaticViewAlign viewAlign);
	void SetViewsGroupButtonWidth();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

protected:
	afx_msg LRESULT OnMsgViewExpand(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewCollapce(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewShow(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewClose(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void AddStaticView(IView* view, StaticViewAlign align);
	CTTCheckBtnGroup m_ViewsGroup;
	afx_msg void OnBnClickedButton1();
	void UpdateColor(COLORREF cl);
};
