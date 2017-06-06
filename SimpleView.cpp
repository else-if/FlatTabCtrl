#include "stdafx.h"
#include "SimpleView.h"
#include "DrawingHelper.h"
#include "AnyClass.h"
#include "CustomMessages.h"
#include <vector>


#define CF_CUSTOMFORMAT	_T("Vector<int>")

using namespace std;
using namespace Gdiplus;

CSimpleView::CSimpleView(bool hasVScroll)
{
	m_hScroll = 0;
	m_vScroll = 0;
	m_bHasVScroll = hasVScroll;
	mainColor = RGB(0, 0, 0);

	//m_TargetDrop = new CDragDropObject();

	UINT nFormat = ::RegisterClipboardFormat(CF_CUSTOMFORMAT);
	//nFormat = CF_TEXT;
	m_CBFormat = { nFormat, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

	m_bDragging = false;
	m_bLBtnmDown = false;

	m_bIsStatic = false;

	m_Brush.CreateSolidBrush(RGB(255, 255, 255));
}

CSimpleView::~CSimpleView()
{
	/*if (m_TargetDrop)
	{
		//	stop drag and drop
		m_TargetDrop->Revoke();

		//	KILL!
		delete m_TargetDrop;
	}
	m_TargetDrop = NULL;*/
}

void CSimpleView::SetName(CString& name)
{
	m_name = name;
}

CString CSimpleView::GetViewName()
{
	return m_name;
}

HWND CSimpleView::GetViewHwnd()
{
	return GetSafeHwnd();
}

void CSimpleView::HandleHScroll(int nPos)
{
	m_hScroll = nPos;
	UpdateWindowStr();
}

void CSimpleView::HandleVScroll(int nPos)
{
	m_vScroll = nPos;
	UpdateWindowStr();
}

void CSimpleView::UpdateWindowStr()
{
	CString str;
	str.Format(_T("\tStatic control\nHoryzontal scroll. Current position %d"), m_hScroll);

	str = (m_bIsStatic ? _T("STATIC VIEW\n") : _T("")) + str;

	if (m_bHasVScroll)
	{
		CString temp = str;
		str.Format(_T("%s\nVertical scroll. Current position %d"), temp, m_vScroll);
	}

	SetWindowText(str);
}

SCROLLINFO CSimpleView::GetVertScrollBarInfo()
{
	SCROLLINFO info;

	if (m_bHasVScroll)
	{
		info.cbSize = sizeof(SCROLLINFO);
		info.fMask = SIF_ALL;
		info.nMin = 0;
		info.nMax = 20;
		info.nPage = 1;
		info.nPos = 0;
		info.nTrackPos = 1;
	}
	else
	{
		info.fMask = 0;
	}

	return info;
}

BEGIN_MESSAGE_MAP(CSimpleView, CStatic)		
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_MESSAGE(TWM_DRAG_ENTER, &CSimpleView::OnTwmDragEnter)
	ON_MESSAGE(TWM_DRAG_LEAVE, &CSimpleView::OnTwmDragLeave)
	ON_MESSAGE(TWM_DRAG_OVER, &CSimpleView::OnTwmDragOver)
	ON_MESSAGE(TWM_DROP, &CSimpleView::OnTwmDrop)
	ON_MESSAGE(TWM_DRAG_SCROLL, &CSimpleView::OnTwmDragScroll)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(884, &CSimpleView::OnRadioBtnClicked)
END_MESSAGE_MAP()

void CSimpleView::OnKeyDown(UINT keyCode, UINT nRepCnt, UINT nFlags)
{
	if (keyCode == VK_DOWN)
	{
		CWnd* pWnd = GetParent();
		//pWnd->GetScrollBarCtrl(SB_VERT)->SetScrollPos(10);
		pWnd->SetScrollPos(SB_VERT, 10);
	}

	CStatic::OnKeyDown(keyCode, nRepCnt, nFlags);
}

void CSimpleView::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	
	//	keep start point
	m_StartPoint = point;

	//	start a timer
	//m_TimerID = SetTimer(1, 100, NULL);
	m_bLBtnmDown = true;

	/*bool bHandled = false;
	HBITMAP hBitmap = GetBitmap();
	COleDataSource *pDataSrc = new COleDataSource();
	
	CAnyClass2 data;
	FORMATETC FormatEtc = { m_Format, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, sizeof(data));

	STGMEDIUM StgMedium;
	StgMedium.tymed = TYMED_HGLOBAL;
	StgMedium.hGlobal = hGlobal;
	StgMedium.pUnkForRelease = NULL;

	LPBYTE lpGlobal = static_cast<LPBYTE>(::GlobalLock(hGlobal));

	::CopyMemory(lpGlobal, &data, sizeof(data));
	::GlobalUnlock(hGlobal);

	pDataSrc->CacheData(static_cast<CLIPFORMAT>(m_Format), &StgMedium, &FormatEtc);

	if (pDataSrc)
	{
		COleDropSource dataSrc;
		
		
		m_bDragging = true;							// to know when dragging over our own window
		//pDataSrc->DoDragDropEx(DROPEFFECT_COPY);
		CRect moveRect(point, point);
		moveRect.InflateRect(5, 5);
		DROPEFFECT dwEffect = pDataSrc->DoDragDrop(DROPEFFECT_MOVE, &moveRect, &dataSrc);
		TRACE("DROPEFFECT: %d\n", dwEffect);
		pDataSrc->InternalRelease();
		m_bDragging = false;
		bHandled = dwEffect != 0;//true;
	}
	
	if (!bHandled)*/
		__super::OnLButtonDown(nFlags, point);	
}

BOOL CSimpleView::PreTranslateMessage(MSG* pMsg)
{
	return __super::PreTranslateMessage(pMsg);
}

UINT CSimpleView::OnGetDlgCode()
{
	UINT result = __super::OnGetDlgCode();
	result = result | DLGC_WANTALLKEYS;
	return result;
}

void CSimpleView::OnDestroy()
{
	__super::OnDestroy();

	m_DropTarget.Revoke();

}

void CSimpleView::InitDrag()
{
	//	register us as a drag/drop client
	//if (m_DropTarget)
	m_DropTarget.Register(this, m_CBFormat);
}

void CSimpleView::OnTimer(UINT_PTR nIDEvent)
{
	//	check if mouse is still in rect
	if (nIDEvent == 1)
	{
		POINT pt;
		::GetCursorPos(&pt);
		CRect iRect;
		GetWindowRect(iRect);
		if (!(iRect.PtInRect(pt)))
		{
			KillTimer(nIDEvent);
			m_TimerID = 0;
		}
	}

	__super::OnTimer(nIDEvent);
}

void CSimpleView::OnLButtonUp(UINT nFlags, CPoint point)
{
	/*m_StartPoint.x = -100;
	m_StartPoint.y = -100;*/

	m_bLBtnmDown = false;

	if (m_TimerID)
	{
		KillTimer(m_TimerID);
		m_TimerID = 0;
	}

	__super::OnLButtonUp(nFlags, point);
}

void CSimpleView::OnMouseMove(UINT nFlags, CPoint point)
{
	//if (m_TimerID > 0)
	if (m_bLBtnmDown)
	{
		//	check if we really moved enough
		int iX = m_StartPoint.x - point.x;
		int iY = m_StartPoint.y - point.y;
		if ((iX*iX + iY*iY) > 100)
		{
			/*if (m_TimerID)
			{
				KillTimer(m_TimerID);
				m_TimerID = 0;
			}*/

			COleDataSource*	pSource = new COleDataSource();
			if (pSource)
			{
				//CAnyClass2 data;
				//vector<int> data;
				testStr t;
				t.data.push_back(rand() % (100));
				
				/*int size = (rand() % (int)(10));
				CString iText = _T("");
				for (int i = 0; i < size;i++)
				{
					int val = rand() % (100);
					data.push_back(val);

					CString t = iText;
					iText.Format(_T("%s, %d"), iText, val);
				}*/

				/*STGMEDIUM StgMedium;
				StgMedium.hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, sizeof(vector<int>));

				if (StgMedium.hGlobal)
				{
					LPVOID pData = static_cast<LPVOID>(::GlobalLock(StgMedium.hGlobal));
					*pData = &data;
					::GlobalUnlock(StgMedium.hGlobal);
					StgMedium.tymed = TYMED_HGLOBAL;
					StgMedium.pUnkForRelease = NULL;
					bool bSet = SUCCEEDED(pSource->SetData(&FormatEtc, &StgMedium, TRUE));
					if (!bSet)
						::GlobalFree(StgMedium.hGlobal);
				}*/

				DWORD dwSize = sizeof(testStr*);
				HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwSize);
				
				/*STGMEDIUM StgMedium;
				StgMedium.tymed = TYMED_HGLOBAL;
				StgMedium.hGlobal = hGlobal;
				StgMedium.pUnkForRelease = NULL;*/

				testStr* pT = &t;

				LPVOID lpGlobal = GlobalLock(hGlobal);					
				CopyMemory(lpGlobal, &pT, dwSize);
				GlobalUnlock(hGlobal);
				
				//EmptyClipboard();
				//::SetClipboardData(m_CBFormat.cfFormat, hGlobal);
				//::CloseClipboard();

				pSource->CacheGlobalData(m_CBFormat.cfFormat, hGlobal);

				CString cStr;
				cStr.Format(_T("Bytes:%s\nValue: %d"), print_bytes(&pT, dwSize), t.data[0]);

				SetWindowText(cStr);
				//TRACE(cStr);

				//	Do drag and drop!
				//pSource->DoDragDrop();			

				m_bLBtnmDown = false;

				//	free source
				pSource->InternalRelease();	
				GlobalFree(hGlobal);				
			}
		}
	}

	__super::OnMouseMove(nFlags, point);
}

int CSimpleView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitDrag();

	//m_Anchor.Init(GetSafeHwnd(), true);
	
	CRect clientRect;
	GetClientRect(clientRect);

	m_TestBtn.Create(_T("Test 1"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, CRect(20, 20, 140, 40), this, 884);
	m_TestBtn2.Create(_T("Test 2"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON , CRect(20, 40, 140, 60), this, 992);
	//m_Anchor.Add(m_TestBtn.GetSafeHwnd(), ANCHOR_BOTTOMRIGHT);

	return 0;
}

afx_msg LRESULT CSimpleView::OnTwmDragEnter(WPARAM wParam, LPARAM lParam)
{
	//TRACE("DRAG ENTER \n");
	return DROPEFFECT_COPY;
}

afx_msg LRESULT CSimpleView::OnTwmDragLeave(WPARAM wParam, LPARAM lParam)
{
	//TRACE("DRAG LEAVE \n");
	return 0;
}

afx_msg LRESULT CSimpleView::OnTwmDragOver(WPARAM wParam, LPARAM lParam)
{
	//COleDropTarget* pDropTarget = reinterpret_cast<COleDropTarget*>(wParam);

	/*vector<int>* ac = m_DropTarget.GetDataObject();
	vector<int> data;

	memcpy(&data[0], ac, )

	//int* ac = m_DropTarget.GetDataObject();
	if (ac != NULL && (ac)->size() > 0)
	{
		int val = (ac)->operator[](0);
		TRACE("DRAG OVER: %d\n", val);
	}*/

	return DROPEFFECT_COPY;
}

afx_msg LRESULT CSimpleView::OnTwmDrop(WPARAM wParam, LPARAM lParam)
{
	//TRACE("DROP \n");

	CTTDropTarget<CAnyClass2>::DragParameters *pParams = reinterpret_cast<CTTDropTarget<CAnyClass2>::DragParameters*>(lParam);

	COleDropTarget* pDropTarget = reinterpret_cast<COleDropTarget*>(wParam);

	//CAnyClass2* ac = NULL;
	//vector<int>* ac = NULL;
	//CString* ac = NULL;
	//int* ac = NULL;
	/*if (pDropTarget == &m_DropTarget)
		ac = m_DropTarget.GetDataObject();

	if (ac == NULL || (ac)->size() == 0)
		return 0;*/

	testStr* t = NULL;
	if (m_DropTarget.GetDataObject() != NULL)
		 t = *m_DropTarget.GetDataObject();

	if (!t)
		return FALSE;

	//TRACE(_T("POINTER: %p\n"), t);
	/*vector<int> data;
	CopyMemory(&data, m_DropTarget.GetLpDataObject(), m_DropTarget.GetSize());*/

	//if ((ac) && (m_DropTargetWnd))
	//{
	/*	CPoint iPoint = m_DropPoint;

		//	lets check if this one wants use
		CWnd *iPossibleWnd = CWnd::WindowFromPoint(iPoint);
		if (NULL == iPossibleWnd)
			return;	//	nope*/

	//CString iText = _T("");
	//iText.Format(_T("%d"), t->data[0]);
	/*for (vector<int>::iterator it(ac->begin()); it != ac->end(); it++)
	{
		CString t = iText;
		int val = *it;
		iText.Format(_T("%s\r\n%d"), t, val);
	}*/
	//iText.Format(_T("Value %.2f"), ac->value);

	//TRACE(_T("DROPPED %s\n"), iText);

	//if (dynamic_cast<IView*>(iPossibleWnd) != NULL)

	CString cStr;
	cStr.Format(_T("Bytes:%s\nValue: %d"), print_bytes(m_DropTarget.GetDataObject(), m_DropTarget.GetSize()), 0/*(*m_DropTarget.GetDataObject())->data[0]*/);

	SetWindowText(cStr);

	return 0;
}

afx_msg LRESULT CSimpleView::OnTwmDragScroll(WPARAM wParam, LPARAM lParam)
{
	//TRACE("DRAG SCROLL \n");
	return /*DROPEFFECT_SCROLL |*/ DROPEFFECT_COPY;
}


CString CSimpleView::print_bytes(LPVOID obj, size_t size)
{
	unsigned char* data = new unsigned char[size];
	size_t i;
	memcpy(data, obj, size);
	CString cStr = _T("");
	for (i = 0; i < size; ++i)
	{
		CString t = cStr;
		cStr.Format(_T("%s %02x"), t, data[i]);
	}
	return cStr;
}

int CSimpleView::GetDesiredViewHeight()
{
	if (m_bIsStatic)
		return 60;
	else
		return IView::GetDesiredViewHeight();
}

void CSimpleView::SetStatic(bool bIsStatic)
{
	m_bIsStatic = bIsStatic;
}

void CSimpleView::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	//m_Anchor.OnSize();
}

BOOL CSimpleView::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam)
{
	// TODO: Add your specialized code here and/or call the base class

	BOOL res = __super::CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, lpParam);

	CRect clientRect;
	GetClientRect(clientRect);

	return res;
}

void CSimpleView::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	__super::PreSubclassWindow();
}

void CSimpleView::OnPaint()
{
	CPaintDC dc(this); 

	CRect clientRect;
	GetClientRect(&clientRect);
	dc.FillSolidRect(clientRect, RGB(255, 255, 255));

	Graphics graphics(dc.GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	Rect boundsRect(200, 30, 300, 20);

	Color gdipFragColor(127, 255, 0, 0);
	
	GraphicsPath path;
	DrawingHelper::GetRoundRectPath(&path, boundsRect, 7);

	SolidBrush solidBrush(gdipFragColor);

	/*Pen pen(Color(127, 0, 255, 0));
	graphics.DrawPath(&pen, &path);*/
	
	Region fragRgn(&path);
	graphics.FillRegion(&solidBrush, &fragRgn);
	
	Gdiplus::Matrix m;
	m.Rotate(90, Gdiplus::MatrixOrder::MatrixOrderAppend);
	path.Transform(&m);
	
	Region fragRgn2(&path);
	Color gdipFragColor(127, 255, 0, 0);
	SolidBrush solidBrush2(gdipFragColor2);
	graphics.FillRegion(&solidBrush2, &fragRgn2);

	/*Pen tempPen(Color::White);
	path.Widen(&tempPen);
	fragRgn.Union(&path);*/

	
	//graphics.FillPath(&solidBrush, &path);

	/*Point prevPt(clientRect.left, clientRect.bottom);

	GraphicsPath path;
	path.AddLine(prevPt, prevPt);

	for (int i = clientRect.left; i < clientRect.right; i += 60)
	{
		int y1 = rand() % clientRect.bottom;
		
		Point startPt(i + 20, y1);
		Point bezPt1((startPt.X + prevPt.X) / 2, prevPt.Y);
		Point bezPt2((startPt.X + prevPt.X) / 2, startPt.Y);

		path.AddBezier(prevPt, bezPt1, bezPt2, startPt);

		Point endPt(i + 40, y1);
		path.AddLine(startPt, endPt);
		
		//prevPt = startPt;
		prevPt = endPt;
	}

	Pen p(Color::Blue);
	graphics.DrawPath(&p, &path);*/
}

HBRUSH CSimpleView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	//TRACE(_T("View CtlColor: %d\n"), pWnd->GetDlgCtrlID());
	HBRUSH hbr = m_Brush;
	pDC->SetBkMode(TRANSPARENT);
	/*CRect clRect;
	pWnd->GetClientRect(clRect);
	pDC->FillSolidRect(clRect, RGB(255, 255, 255));*/

	return hbr;
}

void CSimpleView::OnRadioBtnClicked()
{
	//TRACE(_T("CAtched!\n"));
}