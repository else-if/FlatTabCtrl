#include "stdafx.h"
#include "ViewsHolderDlg.h"
#include "afxdialogex.h"
#include "PrintPreview.h"
#include "CustomMessages.h"

IMPLEMENT_DYNAMIC(CViewsHolderDlg, CDialogEx)

CViewsHolderDlg::CViewsHolderDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CViewsHolderDlg::IDD, pParent)
{
	m_nHoveredSplitterId = -1;
	m_nSplitterOffset = 5;
	m_nHeaderHeight = 15;

	m_bResizing = false;
}

CViewsHolderDlg::~CViewsHolderDlg()
{
	RemoveAllViews();

	if (m_hAccelTable)
	{
		DestroyAcceleratorTable(m_hAccelTable);
		m_hAccelTable = NULL;
	}
}

void CViewsHolderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_ViewsGroup);
}

void CViewsHolderDlg::AddView(IView* view)
{
	AddView(view, false, false, 1.0);
}

void CViewsHolderDlg::AddView(IView* view, bool isCollapsed, bool isClosed, float heigthProc)
{
	AddView(view, isCollapsed, isClosed, heigthProc, false);
}

void CViewsHolderDlg::AddView(IView* view, bool isCollapsed, bool isClosed, float heigthProc, bool showHeader)
{
	if (view == NULL)
		return;

	ViewInfo vi(view, isCollapsed, isClosed, showHeader);

	CViewHeader* vh = new CViewHeader(view);
	CRect emptyRect;
	vh->Create(view->GetViewName(), WS_CHILD | SS_NOTIFY, emptyRect, this);

	if (view->pVScrollBar == NULL)
	{
		SCROLLINFO sbInfo = view->GetVertScrollBarInfo();
		if (sbInfo.fMask != 0)
		{
			CRect emptyRect;
			vi.pVScrollBar = new CScrollBar();
			vi.pVScrollBar->Create(SBS_VERT | SBS_LEFTALIGN | WS_CHILD, emptyRect, this, 10000);
			vi.pVScrollBar->SetScrollInfo(&sbInfo);

			vi.View->pVScrollBar = vi.pVScrollBar;
		}
	}

	m_Views.push_back(make_pair(vh, vi));

	m_ViewsGroup.AddButton(view->GetViewName(), !isClosed);
	SetViewsGroupButtonWidth();

	SetViewHeightProc(&m_Views[m_Views.size() - 1].second, heigthProc);
	SpreadSpace();

	if (GetSafeHwnd() != NULL)
		PlaceViews();
}

void CViewsHolderDlg::AddStaticView(IView* view, StaticViewAlign align)
{
	if (view == NULL)
		return;

	StaticViewInfo vi(view, align);

	if (view->pVScrollBar == NULL)
	{
		SCROLLINFO sbInfo = view->GetVertScrollBarInfo();
		if (sbInfo.fMask != 0)
		{
			CRect emptyRect;
			vi.pVScrollBar = new CScrollBar();
			vi.pVScrollBar->Create(SBS_VERT | SBS_LEFTALIGN | WS_CHILD, emptyRect, this, 10000);
			vi.pVScrollBar->SetScrollInfo(&sbInfo);

			vi.View->pVScrollBar = vi.pVScrollBar;
		}
	}

	m_StaticViews.push_back(vi);

	if (GetSafeHwnd() != NULL)
		PlaceViews();
}

void CViewsHolderDlg::SetViewHeightProc(ViewInfo* pViewInfo, float nHeightProc)
{
	if (pViewInfo == NULL)
		return;

	pViewInfo->nHeightProc = nHeightProc;

	int expCount = GetExpandedViewsCount() - 1;

	if (pViewInfo->IsCollapsed || pViewInfo->IsClosed || nHeightProc <= 0.0 || expCount == 0 || nHeightProc > expCount + 1)
		return;

	float leftToSpread = 1 - pViewInfo->nHeightProc;
	int iHandled = 0;

	for (int i = 0; i < m_Views.size(); i++)
	{
		ViewInfo* pView = &m_Views[i].second;
		if (pView->IsCollapsed || pView->IsClosed)
			continue;

		if (pView == pViewInfo)
			continue;

		float delta = 0;

		if (iHandled == expCount - 1)
			delta = leftToSpread;
		else
			delta = (1 - nHeightProc) * pView->nHeightProc / expCount;

		pView->nHeightProc += delta;
		leftToSpread -= delta;
		iHandled++;
	}
}

void CViewsHolderDlg::SpreadSpace()
{
	int openViewsCount = GetExpandedViewsCount();
	float totalProc = GetTotalProc();

	if (openViewsCount == 0)
		return;

	for (int i = 0; i < m_Views.size(); i++)
	{
		ViewInfo* pViewInfo = &m_Views[i].second;
		if (pViewInfo->View->GetViewHwnd() &&
			!pViewInfo->IsClosed && !pViewInfo->IsCollapsed)
		{
			pViewInfo->nHeightProc = pViewInfo->nHeightProc / totalProc * openViewsCount;
		}
	}
}

void CViewsHolderDlg::PlaceStaticViews()
{
	if (m_StaticViews.empty())
		return;

	CRect clientRect;
	GetClientRect(&clientRect);
	int mostTop = clientRect.top + 25;
	int mostBottom = clientRect.bottom;

	for (int i = 0; i < m_StaticViews.size(); i++)
	{
		StaticViewInfo* pViewInfo = &m_StaticViews[i];
		HWND hWnd = pViewInfo->View->GetViewHwnd();

		if (hWnd == NULL)
			continue;

		int availableHeight = min(pViewInfo->View->GetDesiredViewHeight(), mostBottom - mostTop);
		if (availableHeight <= 0)
			continue;

		int sbWidth = 0;
		if (pViewInfo->pVScrollBar)
			sbWidth = GetSystemMetrics(SM_CXVSCROLL);

		if (pViewInfo->Align == StaticViewAlign::Top)
		{
			pViewInfo->ViewRect.SetRect(clientRect.left, mostTop, clientRect.right - sbWidth, mostTop + availableHeight);
			mostTop += availableHeight;
		}
		else
		{
			pViewInfo->ViewRect.SetRect(clientRect.left, mostBottom - availableHeight, clientRect.right - sbWidth, mostBottom);
			mostBottom -= availableHeight;
		}

		CWnd* pWnd = CWnd::FromHandle(hWnd);
		if (!pWnd->IsWindowVisible())
			pWnd->ShowWindow(SW_SHOW);

		pWnd->MoveWindow(pViewInfo->ViewRect, 1);

		if (pViewInfo->pVScrollBar)
		{
			pViewInfo->VScrollRect.SetRect(pViewInfo->ViewRect.right, pViewInfo->ViewRect.top,
				pViewInfo->ViewRect.right + sbWidth, pViewInfo->ViewRect.bottom);

			if (!pViewInfo->pVScrollBar->IsWindowVisible())
				pViewInfo->pVScrollBar->ShowWindow(SW_SHOW);

			pViewInfo->pVScrollBar->MoveWindow(pViewInfo->VScrollRect, 1);
		}
	}
}

int CViewsHolderDlg::GetStaticViewsHeight(StaticViewAlign viewAlign)
{
	int result = 0;

	for (int i = 0; i < m_StaticViews.size(); i++)
	{
		StaticViewInfo* pViewInfo = &m_StaticViews[i];
		if (pViewInfo->Align != viewAlign)
			continue;

		int desiredHeight = pViewInfo->View->GetDesiredViewHeight();
		if (desiredHeight > 0)
			result += desiredHeight;
	}

	return result;
}

void CViewsHolderDlg::SetViewsGroupButtonWidth()
{
	CRect clientRect;
	GetClientRect(clientRect);
	m_ViewsGroup.SetProperWidth(clientRect.Width());
}

void CViewsHolderDlg::PlaceViews()
{
	PlaceStaticViews();

	m_Splitters.clear();

	if (m_Views.empty())
		return;

	CRect clientRect;
	GetClientRect(&clientRect);
	clientRect.top += GetStaticViewsHeight(StaticViewAlign::Top) + 25;
	clientRect.bottom -= GetStaticViewsHeight(StaticViewAlign::Bottom);
	clientRect.bottom = max(clientRect.top, clientRect.bottom);

	int closedViews = GetClosedViewsCount();
	int showedViews = m_Views.size() - closedViews;
	int expandedViews = GetExpandedViewsCount();
	int headersCount = GetShowedHeadersCount();

	int minViewHeight = 10;

	float defaultViewHeight = 0.0;
	if (expandedViews != 0)
	{
		// calculate an amount of space that should be spread between sizable views
		float freeSpace = clientRect.Height()			// whole client height
			- headersCount * m_nHeaderHeight			// without space reserved for headers
			- m_nSplitterOffset * (showedViews - 1)		// without space for splitters 
			- minViewHeight * expandedViews;			// without space that every view should have

		defaultViewHeight = freeSpace / (float)expandedViews;
	}

	int prevBottom = clientRect.top;
	int placedViews = 0;
	ViewInfo* pPrevViewInfo = NULL;

	for (int i = 0; i < m_Views.size(); i++)
	{
		ViewInfo* pViewInfo = &m_Views[i].second;
		HWND hWnd = pViewInfo->View->GetViewHwnd();

		if (hWnd == NULL)
			continue;

		if (pViewInfo->IsClosed)
		{
			CWnd* pHeaderWnd = m_Views[i].first;
			if (pHeaderWnd && pHeaderWnd->IsWindowVisible())
				pHeaderWnd->ShowWindow(SW_HIDE);

			CWnd* pSbWnd = pViewInfo->pVScrollBar;
			if (pSbWnd && pSbWnd->IsWindowVisible())
				pSbWnd->ShowWindow(SW_HIDE);

			CWnd* pWnd = CWnd::FromHandle(hWnd);
			if (pWnd && pWnd->IsWindowVisible())
				pWnd->ShowWindow(SW_HIDE);

			continue;
		}

		int sbWidth = 0;
		if (pViewInfo->pVScrollBar)
			sbWidth = GetSystemMetrics(SM_CXVSCROLL);

		int top = prevBottom + (placedViews == 0 ? 0 : m_nSplitterOffset);
		int headerHeight = 0;
		if (pViewInfo->ShowHeader)
		{
			headerHeight = m_nHeaderHeight;
			pViewInfo->HeaderRect.SetRect(clientRect.left, top, clientRect.right, min(top + headerHeight, clientRect.bottom));

			CWnd* pHeaderWnd = m_Views[i].first;
			pHeaderWnd->ShowWindow(SW_SHOW);
			pHeaderWnd->MoveWindow(pViewInfo->HeaderRect, 1);
		}
		else
		{
			headerHeight = 0;
			pViewInfo->HeaderRect.SetRectEmpty();

			CWnd* pHeaderWnd = m_Views[i].first;
			pHeaderWnd->ShowWindow(SW_HIDE);
		}

		int height = 0;
		if (placedViews == showedViews - 1)
			height = clientRect.bottom - top - headerHeight; // use all space left
		else
			height = round(defaultViewHeight * pViewInfo->nHeightProc) + minViewHeight;; // calculate view height

		if (!pViewInfo->IsCollapsed)
		{
			pViewInfo->ViewRect.SetRect(clientRect.left, top + headerHeight, clientRect.right - sbWidth, min(top + headerHeight + height, clientRect.bottom));

			CWnd* pWnd = CWnd::FromHandle(hWnd);
			if (!pWnd->IsWindowVisible())
				pWnd->ShowWindow(SW_SHOW);

			pWnd->MoveWindow(pViewInfo->ViewRect, 1);

			if (pViewInfo->pVScrollBar)
			{
				pViewInfo->VScrollRect.SetRect(pViewInfo->ViewRect.right, pViewInfo->ViewRect.top,
					pViewInfo->ViewRect.right + sbWidth, pViewInfo->ViewRect.bottom);

				if (!pViewInfo->pVScrollBar->IsWindowVisible())
					pViewInfo->pVScrollBar->ShowWindow(SW_SHOW);

				pViewInfo->pVScrollBar->MoveWindow(pViewInfo->VScrollRect, 1);
			}
		}
		else
		{
			CWnd* pWnd = CWnd::FromHandle(hWnd);
			if (pWnd->IsWindowVisible())
				pWnd->ShowWindow(SW_HIDE);

			if (pViewInfo->pVScrollBar && pViewInfo->pVScrollBar->IsWindowVisible())
				pViewInfo->pVScrollBar->ShowWindow(SW_HIDE);

			pViewInfo->ViewRect.SetRectEmpty();
			pViewInfo->VScrollRect.SetRectEmpty();			
		}

		if (pPrevViewInfo != NULL)
		{
			SplitterInfo si;
			si.UpperViewInfo = pPrevViewInfo;
			si.LowerViewInfo = pViewInfo;
			si.SplitterRect.SetRect(clientRect.left, prevBottom,
				clientRect.right, top);
			m_Splitters.push_back(si);
		}

		if (!pViewInfo->IsCollapsed)
		{
			pPrevViewInfo = pViewInfo;
			prevBottom = pViewInfo->ViewRect.bottom;
		}
		else
		{
			prevBottom = top + headerHeight;
		}		

		placedViews++;		
	}
}

void CViewsHolderDlg::RemoveView(IView* pView)
{
	// Check sizable views
	for (int i = 0; i < m_Views.size(); i++)
	{
		if (pView == m_Views[i].second.View)
		{
			CWnd* pWnd = CWnd::FromHandle(pView->GetViewHwnd());
			if (pWnd->IsWindowVisible())
				pWnd->ShowWindow(SW_HIDE);

			if (m_Views[i].first != NULL)
			{
				delete m_Views[i].first;
				m_Views[i].first = NULL;
			}

			if (m_Views[i].second.pVScrollBar)
			{
				delete m_Views[i].second.pVScrollBar;
				m_Views[i].second.pVScrollBar = NULL;
				m_Views[i].second.View->pVScrollBar = NULL;
			}

			m_Views.erase(m_Views.begin() + i);

			break;
		}
	}

	// Check static views
	for (int i = 0; i < m_StaticViews.size(); i++)
	{
		if (pView == m_StaticViews[i].View)
		{
			CWnd* pWnd = CWnd::FromHandle(pView->GetViewHwnd());
			if (pWnd->IsWindowVisible())
				pWnd->ShowWindow(SW_HIDE);
			
			if (m_StaticViews[i].pVScrollBar)
			{
				delete m_StaticViews[i].pVScrollBar;
				m_StaticViews[i].pVScrollBar = NULL;
				m_StaticViews[i].View->pVScrollBar = NULL;
			}

			m_StaticViews.erase(m_StaticViews.begin() + i);

			break;
		}
	}

	SpreadSpace();
	PlaceViews();
}

void CViewsHolderDlg::RemoveAllViews()
{
	for each (pair<CViewHeader*, ViewInfo> viewData in m_Views)
	{
		if (viewData.first != NULL)
		{
			delete viewData.first;
			viewData.first = NULL;
		}

		if (viewData.second.pVScrollBar)
		{
			delete viewData.second.pVScrollBar;
			viewData.second.pVScrollBar = NULL;
			viewData.second.View->pVScrollBar = NULL;
		}
	}
	m_Views.clear();

	for each (StaticViewInfo viewInfo in m_StaticViews)
	{
		if (viewInfo.pVScrollBar)
		{
			delete viewInfo.pVScrollBar;
			viewInfo.pVScrollBar = NULL;
			viewInfo.View->pVScrollBar = NULL;
		}
	}
	m_StaticViews.clear();
}

ViewInfo* CViewsHolderDlg::GetExpandedUpperViewInfo(int splitterId)
{
	for (int i = splitterId; i >= 0; i--)
	{
		if (m_Splitters[i].UpperViewInfo->View->GetViewHwnd() != NULL &&
			!m_Splitters[i].UpperViewInfo->IsClosed &&
			!m_Splitters[i].UpperViewInfo->IsCollapsed)
			return m_Splitters[i].UpperViewInfo;
	}

	return NULL;
}

ViewInfo* CViewsHolderDlg::GetExpandedLowerViewInfo(int splitterId)
{
	for (int i = splitterId; i < m_Splitters.size(); i++)
	{
		if (m_Splitters[i].LowerViewInfo->View->GetViewHwnd() != NULL &&
			!m_Splitters[i].LowerViewInfo->IsClosed &&
			!m_Splitters[i].LowerViewInfo->IsCollapsed)
			return m_Splitters[i].LowerViewInfo;
	}

	return NULL;
}

int CViewsHolderDlg::GetShowedHeadersCount()
{
	int result = 0;

	for (int i = 0; i < m_Views.size(); i++)
	{
		ViewInfo* pViewInfo = &m_Views[i].second;
		if (pViewInfo->View->GetViewHwnd() &&
			!pViewInfo->IsClosed && pViewInfo->ShowHeader)
			result++;
	}

	return result;
}

int CViewsHolderDlg::GetExpandedViewsCount()
{
	int result = 0;

	for (int i = 0; i < m_Views.size(); i++)
	{
		ViewInfo* pViewInfo = &m_Views[i].second;
		if (pViewInfo->View->GetViewHwnd() &&
			!pViewInfo->IsClosed && !pViewInfo->IsCollapsed)
			result++;
	}

	return result;
}

int CViewsHolderDlg::GetClosedViewsCount()
{
	int result = 0;

	for (int i = 0; i < m_Views.size(); i++)
	{
		ViewInfo* pViewInfo = &m_Views[i].second;
		if (pViewInfo->View->GetViewHwnd() && pViewInfo->IsClosed)
			result++;
	}

	return result;
}

float CViewsHolderDlg::GetTotalProc()
{
	float result = 0;

	for (int i = 0; i < m_Views.size(); i++)
	{
		ViewInfo* pViewInfo = &m_Views[i].second;
		if (pViewInfo->View->GetViewHwnd() &&
			!pViewInfo->IsClosed && !pViewInfo->IsCollapsed)
			result += pViewInfo->nHeightProc;
	}

	return result;
}

ViewInfo* CViewsHolderDlg::GetCurrentViewInfo()
{
	CPoint curPt;
	GetCursorPos(&curPt);
	ScreenToClient(&curPt);

	for (int i = 0; i < m_Views.size(); i++)
	{
		ViewInfo* pViewInfo = &m_Views[i].second;
		if (!pViewInfo->IsClosed && !pViewInfo->IsCollapsed &&
			(pViewInfo->ViewRect.PtInRect(curPt) || pViewInfo->HeaderRect.PtInRect(curPt)))
			return pViewInfo;
	}

	for (int i = 0; i < m_StaticViews.size(); i++)
	{
		ViewInfo* pViewInfo = &m_StaticViews[i];
		if (!pViewInfo->IsClosed && !pViewInfo->IsCollapsed &&
			(pViewInfo->ViewRect.PtInRect(curPt) || pViewInfo->HeaderRect.PtInRect(curPt)))
			return pViewInfo;
	}

	return NULL;
}

void CViewsHolderDlg::InvalidateViews(BOOL bErase /*= TRUE*/)
{
	for each (pair<CViewHeader*, ViewInfo> viewData in m_Views)
	{
		if (!viewData.second.View)
			continue;

		HWND hWnd = viewData.second.View->GetViewHwnd();
		if (hWnd == NULL)
			continue;

		CWnd* pWnd = CWnd::FromHandle(hWnd);
		pWnd->Invalidate(bErase);
	}

	for each (StaticViewInfo viewinfo in m_StaticViews)
	{
		if (!viewinfo.View)
			continue;

		HWND hWnd = viewinfo.View->GetViewHwnd();
		if (hWnd == NULL)
			continue;

		CWnd* pWnd = CWnd::FromHandle(hWnd);
		pWnd->Invalidate(bErase);
	}
}

IView* CViewsHolderDlg::GetFocusedView()
{
	CWnd* pWnd = GetFocus();
	if (!pWnd)
		return NULL;

	HWND hWnd = pWnd->GetSafeHwnd();

	for (int i = 0; i < m_Views.size(); i++)
	{
		IView* pView = m_Views[i].second.View;
		if (pView->GetViewHwnd() == hWnd)
			return pView;
	}

	for (int i = 0; i < m_StaticViews.size(); i++)
	{
		IView* pView = m_StaticViews[i].View;
		if (pView->GetViewHwnd() == hWnd)
			return pView;
	}

	return NULL;
}

bool CViewsHolderDlg::IsViewClosed(IView* pView)
{
	ASSERT(pView != NULL);

	for (int i = 0; i < m_Views.size(); i++)
	{
		if (pView == m_Views[i].second.View)
			return m_Views[i].second.IsClosed;
	}

	for (int i = 0; i < m_StaticViews.size(); i++)
	{
		if (pView == m_StaticViews[i].View)
			return m_StaticViews[i].IsClosed;
	}

	return false;
}

void CViewsHolderDlg::HScrollViews(int nPos)
{
	for (int i = 0; i < m_Views.size(); i++)
		m_Views[i].second.View->HandleHScroll(nPos);

	for (int i = 0; i < m_StaticViews.size(); i++)
		m_StaticViews[i].View->HandleHScroll(nPos);
}

BEGIN_MESSAGE_MAP(CViewsHolderDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(TWM_VIEWEXPAND, &CViewsHolderDlg::OnMsgViewExpand)
	ON_MESSAGE(TWM_VIEWCOLLAPCE, &CViewsHolderDlg::OnMsgViewCollapce)
	ON_MESSAGE(TWM_VIEWSHOW, &CViewsHolderDlg::OnMsgViewShow)
	ON_MESSAGE(TWM_VIEWCLOSE, &CViewsHolderDlg::OnMsgViewClose)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON1, &CViewsHolderDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

// CViewsHolderDlg message handlers

BOOL CViewsHolderDlg::PreTranslateMessage(MSG* pMsg)
{
	if (m_hAccelTable) {
		if (::TranslateAccelerator(GetParent()->GetSafeHwnd(), m_hAccelTable, pMsg)) {
			return(TRUE);
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CViewsHolderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_hAccelTable = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

	PlaceViews();

	return TRUE;
}

void CViewsHolderDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	SetViewsGroupButtonWidth();

	PlaceViews();
	Invalidate(FALSE);
}

void CViewsHolderDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int minpos;
	int maxpos;
	GetScrollRange(SB_HORZ, &minpos, &maxpos);

	int curpos = GetScrollPos(SB_HORZ);

	switch (nSBCode)
	{
	case SB_LEFT:      // Scroll to far left.
		curpos = minpos;
		break;

	case SB_RIGHT:      // Scroll to far right.
		curpos = maxpos;
		break;

	case SB_ENDSCROLL:   // End scroll.
		break;

	case SB_LINELEFT:      // Scroll left.
		if (curpos > minpos)
			curpos--;
		break;

	case SB_LINERIGHT:   // Scroll right.
		if (curpos < maxpos)
			curpos++;
		break;

	case SB_PAGELEFT:    // Scroll one page left.
	{
		// Get the page size.
		SCROLLINFO   info;
		GetScrollInfo(SB_HORZ, &info, SIF_ALL);

		if (curpos > minpos)
			curpos = max(minpos, curpos - (int)info.nPage);
	}
	break;

	case SB_PAGERIGHT:      // Scroll one page right.
	{
		// Get the page size.
		SCROLLINFO   info;
		GetScrollInfo(SB_HORZ, &info, SIF_ALL);

		if (curpos < maxpos)
			curpos = min(maxpos, curpos + (int)info.nPage);
	}
	break;

	case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		curpos = nPos;      // of the scroll box at the end of the drag operation.
		break;

	case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
		curpos = nPos;     // position that the scroll box has been dragged to.
		break;
	}

	SetScrollPos(SB_HORZ, curpos);
	HScrollViews(curpos);

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CViewsHolderDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar != NULL)
	{
		int minpos, maxpos;
		pScrollBar->GetScrollRange(&minpos, &maxpos);

		int curpos = pScrollBar->GetScrollPos();

		switch (nSBCode)
		{
		case SB_TOP:      // Scroll to top.
			curpos = minpos;
			break;

		case SB_BOTTOM:      // Scroll to bottom.
			curpos = maxpos;
			break;

		case SB_ENDSCROLL:   // End scroll.
			return;

		case SB_LINEUP:      // Scroll up.
			if (curpos > minpos)
				curpos--;
			break;

		case SB_LINEDOWN:   // Scroll down.
			if (curpos < maxpos)
				curpos++;
			break;

		case SB_PAGEUP:    // Scroll one page up.
		{
			// Get the page size.
			SCROLLINFO   info;
			pScrollBar->GetScrollInfo(&info, SIF_ALL);

			if (curpos > minpos)
				curpos = max(minpos, curpos - (int)info.nPage);
		}
		break;

		case SB_PAGEDOWN:      // Scroll one page down.
		{
			// Get the page size.
			SCROLLINFO   info;
			pScrollBar->GetScrollInfo(&info, SIF_ALL);

			if (curpos < maxpos)
				curpos = min(maxpos, curpos + (int)info.nPage);
		}
		break;

		case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
			curpos = nPos;      // of the scroll box at the end of the drag operation.
			break;

		case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
			curpos = nPos;     // position that the scroll box has been dragged to.
			break;
		}

		pScrollBar->SetScrollPos(curpos);

		for (int i = 0; i < m_Views.size(); i++)
			if (pScrollBar == m_Views[i].second.pVScrollBar)
				m_Views[i].second.View->HandleVScroll(curpos);

		for (int i = 0; i < m_StaticViews.size(); i++)
			if (pScrollBar == m_StaticViews[i].pVScrollBar)
				m_StaticViews[i].View->HandleVScroll(curpos);
	}
	else
	{
		CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
	}
}

void CViewsHolderDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bResizing && (nFlags & MK_LBUTTON))
	{
		SplitterInfo si = m_Splitters[m_nHoveredSplitterId];

		ViewInfo* upperViewInfo = GetExpandedUpperViewInfo(m_nHoveredSplitterId);
		ViewInfo* lowerViewInfo = GetExpandedLowerViewInfo(m_nHoveredSplitterId);

		if (upperViewInfo != NULL && lowerViewInfo != NULL)
		{
			/*int top = upperViewInfo->ViewRect.top;
			int bottom = lowerViewInfo->ViewRect.bottom;

			if (point.y > top && point.y < bottom)
			{
				float newProc = point.y * upperViewInfo->nHeightProc / upperViewInfo->ViewRect.bottom;
				lowerViewInfo->nHeightProc += upperViewInfo->nHeightProc - newProc;
				upperViewInfo->nHeightProc = newProc;
			}*/

			int yPos = si.SplitterRect.top + si.SplitterRect.Height() / 2;
			/*
			int delta = yPos - point.y;

			upperViewInfo->nHeightProc = (upperViewInfo->ViewRect.Height() - delta) * upperViewInfo->nHeightProc / upperViewInfo->ViewRect.Height();
			lowerViewInfo->nHeightProc = (lowerViewInfo->ViewRect.Height() + delta) * lowerViewInfo->nHeightProc / lowerViewInfo->ViewRect.Height();*/

			
			ViewInfo* baseView = yPos < point.y ? lowerViewInfo : upperViewInfo;

			float nProcChange = 0.0f;
			if (abs(yPos - point.y) >= baseView->ViewRect.Height())
				nProcChange = baseView->nHeightProc * (yPos < point.y ? -1 : 1);
			else
				nProcChange = (float)(yPos - point.y) / (float)baseView->ViewRect.Height() * baseView->nHeightProc;

			lowerViewInfo->nHeightProc += nProcChange;
			upperViewInfo->nHeightProc -= nProcChange;
			
			PlaceViews();

			InvalidateRect(si.SplitterRect);
		}
	}
	else
	{
		m_bResizing = false;
		m_nHoveredSplitterId = -1;

		for (int i = 0; i < m_Splitters.size(); i++)
		{
			if (m_Splitters[i].SplitterRect.PtInRect(point))
			{
				::SetCursor(::LoadCursor(NULL, IDC_SIZENS));
				m_nHoveredSplitterId = i;
			}
		}
	}

	CDialogEx::OnMouseMove(nFlags, point);
}

BOOL CViewsHolderDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	ViewInfo* pViewInfo = GetCurrentViewInfo();
	if (pViewInfo == NULL)
		return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);

	if (pViewInfo->View->pVScrollBar == NULL)
		return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);

	CScrollBar* pVScrollBar = pViewInfo->View->pVScrollBar;

	int newPos = pVScrollBar->GetScrollPos();

	newPos -= zDelta / 120;

	int minPos = 0, maxPos = 0;

	pVScrollBar->GetScrollRange(&minPos, &maxPos);

	if (newPos > maxPos) newPos = maxPos;
	if (newPos < minPos) newPos = minPos;

	pVScrollBar->SetScrollPos(newPos, TRUE);

	OnVScroll(SB_THUMBTRACK, newPos, pVScrollBar);

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}

void CViewsHolderDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_nHoveredSplitterId != -1)
	{
		SetCapture();
		m_bResizing = true;
		::SetCursor(::LoadCursor(NULL, IDC_SIZENS));
		InvalidateRect(m_Splitters[m_nHoveredSplitterId].SplitterRect);
	}
	else
	{
		ViewInfo* pViewInfo = GetCurrentViewInfo();
		if (pViewInfo && pViewInfo->View->GetViewHwnd() != NULL)
		{
			HWND hWnd = pViewInfo->View->GetViewHwnd();
			::SetFocus(hWnd);
		}
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}

void CViewsHolderDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bResizing)
	{
		InvalidateRect(m_Splitters[m_nHoveredSplitterId].SplitterRect);
		ReleaseCapture();
		m_nHoveredSplitterId = -1;
		m_bResizing = false;
	}

	CDialogEx::OnLButtonUp(nFlags, point);
}

afx_msg LRESULT CViewsHolderDlg::OnMsgViewExpand(WPARAM wParam, LPARAM lParam)
{
	IView* pView = (IView*)lParam;

	for (int i = 0; i < m_Views.size(); i++)
	{
		if (pView == m_Views[i].second.View)
		{
			m_Views[i].second.IsCollapsed = false;
			SetViewHeightProc(&m_Views[i].second, m_Views[i].second.nHeightProc);

			SpreadSpace();
			PlaceViews();
			break;
		}
	}

	return 0;
}

afx_msg LRESULT CViewsHolderDlg::OnMsgViewCollapce(WPARAM wParam, LPARAM lParam)
{
	IView* pView = (IView*)lParam;

	for (int i = 0; i < m_Views.size(); i++)
	{
		if (pView == m_Views[i].second.View)
		{
			m_Views[i].second.IsCollapsed = true;
			SpreadSpace();
			PlaceViews();
			break;
		}
	}

	return 0;
}

afx_msg LRESULT CViewsHolderDlg::OnMsgViewShow(WPARAM wParam, LPARAM lParam)
{
	IView* pView = (IView*)lParam;

	for (int i = 0; i < m_Views.size(); i++)
	{
		if (pView == m_Views[i].second.View)
		{
			m_Views[i].second.IsClosed = false;
			SetViewHeightProc(&m_Views[i].second, m_Views[i].second.nHeightProc);

			SpreadSpace();
			PlaceViews();
			break;
		}
	}

	return 0;
}

afx_msg LRESULT CViewsHolderDlg::OnMsgViewClose(WPARAM wParam, LPARAM lParam)
{
	IView* pView = (IView*)lParam;

	for (int i = 0; i < m_Views.size(); i++)
	{
		if (pView == m_Views[i].second.View)
		{
			m_Views[i].second.IsClosed = true;
			SpreadSpace();
			PlaceViews();
			break;
		}
	}

	return 0;
}

void CViewsHolderDlg::OnPaint()
{
	//__super::OnPaint();
	CPaintDC dc(this);
	CRect cliRect, wndRect;

	GetClientRect(&cliRect);
	GetWindowRect(&wndRect);
	if (cliRect.Width() <= 0 || cliRect.Height() <= 0)
		return;

	CMemDC memDC(dc, &cliRect);

	/*if (CTilWinGlobals::Instance().pGlobalPrintPreview)
	{
		CRect cRect;
		CTilWinGlobals::Instance().pGlobalPrintPreview->GetPaperRect(cRect);
		CTilWinGlobals::Instance().pGlobalPrintPreview->ClientToScreen(&cRect);
		ScreenToClient(&cRect);
		memDC.GetDC().ExcludeClipRect(&cRect);
	}*/

	/*int PanelColor = TTRuntime::DefaultSession->UserContext->UserSettings->ColourSetting32[_T("Plan_PanelCol")];
	if (TTRuntime::DefaultSession->CurrentUserState->IsMirrorPlanning())
	{
		PanelColor = TTRuntime::DefaultSession->UserContext->UserSettings->ColourSetting32[_T("Plan_MirrorPanelCol")];
		if (PanelColor == 0)
			PanelColor = RGB(255, 128, 128);
	}*/
	int PanelColor = RGB(255, 255, 255);

	memDC.GetDC().FillSolidRect(cliRect, PanelColor);

	for (int i = 0; i < m_Splitters.size(); i++)
	{
		CRect splRect(m_Splitters[i].SplitterRect);
		splRect.DeflateRect(0, 1);

		COLORREF cl = RGB(225, 225, 225);
		if (i == m_nHoveredSplitterId && m_bResizing)
			cl = RGB(190, 190, 190);

		memDC.GetDC().FillSolidRect(splRect, cl);

	}

	DefWindowProc(WM_PAINT, (WPARAM)memDC.GetDC().GetSafeHdc(), (LPARAM)0);
}

BOOL CViewsHolderDlg::OnEraseBkgnd(CDC* pDC)
{
	//return __super::OnEraseBkgnd(pDC);
	return TRUE;
}

void CViewsHolderDlg::OnBnClickedButton1()
{
	for (int i = 0; i < m_Views.size(); i++)
	{
		bool isChecked = m_ViewsGroup.IsChecked(i);
		if (m_Views[i].second.IsClosed == isChecked)
		{
			UINT msg = isChecked ? TWM_VIEWSHOW : TWM_VIEWCLOSE;
			PostMessage(msg, 0, (LPARAM)m_Views[i].second.View);
		}
	}
}

void CViewsHolderDlg::UpdateColor(COLORREF cl)
{
	m_ViewsGroup.SetPressColor(cl);
}
