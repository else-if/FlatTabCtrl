#include "stdafx.h"
#include "DragDropObject.h"
#include "IView.h"
#include "AnyClass.h"

CDragDropObject::CDragDropObject()
{
}

CDragDropObject::~CDragDropObject()
{
}

// ************************************************************
//	GotDrag
//		Dragging in the control.
//
//		We just check the runtime class to see if its
//		one of us. We could then call members to check if the 
//		drop is acceptable, ...
//
//		In our case, just check the type and give some graphics
// ************************************************************
DWORD CDragDropObject::GotDrag(void)
{
	if (m_DropTargetWnd)
	{
		CPoint iPoint = m_DropPoint;
		//	lets check if this one wants use
		CWnd *iPossibleWnd = CWnd::WindowFromPoint(iPoint);
		if (NULL == iPossibleWnd)
			return DROPEFFECT_NONE;	//	nope

		if (dynamic_cast<IView*>(iPossibleWnd) != NULL)
			//return DROPEFFECT_LINK;
			//return DROPEFFECT_COPY;
			return DROPEFFECT_MOVE;
	}

	return DROPEFFECT_NONE;
}

// ************************************************************
//	GotLeave
// ************************************************************
void CDragDropObject::GotLeave(void)
{
}

// ************************************************************
//	GotEnter
// ************************************************************
DWORD CDragDropObject::GotEnter(void)
{
	//return DROPEFFECT_COPY;
	//return DROPEFFECT_LINK;
	return DROPEFFECT_MOVE;
}

// ************************************************************
//	GotDrop
//		Called if we have a drop text drop here.
//	
//		We could now begin sending messages to the controls
//		to check if they want the drag and drop; ....
//
//		This sample just presumes that since they registered,
//		they probably do
// ************************************************************
void CDragDropObject::GotDrop(void)
{
	//	value contains the material itself
	if (m_Data)
	{
		//	get the text
		//char *iText = (char *)m_Data;
		//CAnyClass* ac = static_cast<CAnyClass*>(m_Data);
		if (m_Len != sizeof(CAnyClass2))
			return;

		CAnyClass2* ac = reinterpret_cast<CAnyClass2*>(m_Data);		

		if ((ac) && (m_DropTargetWnd))
		{
			CPoint iPoint = m_DropPoint;

			//	lets check if this one wants use
			CWnd *iPossibleWnd = CWnd::WindowFromPoint(iPoint);
			if (NULL == iPossibleWnd)
				return;	//	nope

			CString iText;
			iText.Format(_T("Value %.2f"), ac->value);

			TRACE(_T("DROPPED %.2f\n"), ac->value);

			//if (dynamic_cast<IView*>(iPossibleWnd) != NULL)
			iPossibleWnd->SetWindowText(CString(iText));

			/*//	check if this is one of us...
			if (iPossibleWnd->IsKindOf(RUNTIME_CLASS(CDragDropButton)))
			{
				((CDragDropButton *)iPossibleWnd)->SetWindowText(iText);	//	Ha the cast. The terrible cast!
			}
			else if (iPossibleWnd->IsKindOf(RUNTIME_CLASS(CDragDropComboBox)))
			{
				//	could check if its already present...
				((CDragDropComboBox *)iPossibleWnd)->AddString(iText);	//	Ha the cast. The terrible cast!
			}
			else if (iPossibleWnd->IsKindOf(RUNTIME_CLASS(CDragDropListBox)))
			{
				//	could check if its already present...
				((CDragDropListBox *)iPossibleWnd)->AddString(iText);	//	Ha the cast. The terrible cast!
			}*/
		}
	}
}
