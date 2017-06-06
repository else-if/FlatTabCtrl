#pragma once
#include "ControlsColorMap.h"
#include "TTComboEdit.h"
#include "TTComboListBox.h"
#include "DrawingHelper.h"
#include <set>

class CTTComboBox : public CComboBox
{

public:
	CTTComboBox();
	virtual ~CTTComboBox();

	void SetDrawingProperties(int borderPenWidth, int cornerRadius);
	void SetEditboxPosition();

	void SetMultiSelectionMode(bool bEnableMultiSelection = true);
	bool MultiSelectionEnabled();

	void AddSelectedItem(int itemId);
	void RemoveSelectedItem(int itemId);
	bool IsSelected(int itemId);
	void ClearSelectedItems();

	set<int> GetMultiPersSelection();
	void SetSelection(set<int> selItems);

	virtual CString GetSelectedItemsText();
	virtual void UpdateText();

	static void CTTComboBox::DrawComboBoxControl(CDC* pDC, CRect* pRect, CRect* pClipRect, int nStyle,
		ControlState controlState = Normal, ControlState backgrougControlState = Normal, ControlState arrowButtonState = Normal,
		const CString& itemText = _T(""), CFont* pFont = NULL, int cornerRadius = DrawingHelper::GetDpiScaledValue(5),
		int borderWidth = 1, ControlsColorMap* pColorMap = NULL);

	static void DrawDropDown(CDC* pDC, CRect* pRect, CRect* pClipRect, ControlState controlState = Normal,
		ControlState arrowButtonState = Normal, int cornerRadius = DrawingHelper::GetDpiScaledValue(5), int borderWidth = 1,
		ControlsColorMap* pColorMap = NULL);

	static void DrawDropDownList(CDC* pDC, CRect* pRect, CRect* pClipRect, ControlState controlState = Normal,
		ControlState backgrougControlState = Normal, ControlState arrowButtonState = Normal, const CString& itemText = _T(""),
		CFont* pFont = NULL, int cornerRadius = DrawingHelper::GetDpiScaledValue(5), int borderWidth = 1, ControlsColorMap* pColorMap = NULL);

	static void DrawArrowButon(CDC* pDC, CRect* pRect, CRect* pClipRect,
		ControlState arrowButtonState = Normal, ControlState controlState = Normal,
		int cornerRadius = DrawingHelper::GetDpiScaledValue(5), int borderWidth = 1, ControlsColorMap* pColorMap = NULL,
		BOOL filledArrow = FALSE);

	static void DrawSimple(CDC* pDC, CRect* pRect, CRect* pClipRect, ControlState controlState,
		int cornerRadius = DrawingHelper::GetDpiScaledValue(5), int borderWidth = 1, ControlsColorMap* pColorMap = NULL);

	static void DrawSelectedItemText(CDC* pDC, CRect* pRect, const CString& itemText, ControlState controlState = Normal,
		int borderWidth = 1, CFont* pFont = NULL);

	static CRect GetArrowButtonRect(CRect* clientRect);

	int m_nBegin;
	int m_nEnd;

protected:

	CDC m_dc;
	bool m_bUseBmp;

	CTTComboEdit m_edit;
	CTTComboListBox m_listbox;
	
	ControlState m_ControlState;
	ControlState m_ArrowButtonState;
	ControlsColorMap m_ColorMap;

	DWORD m_iComboBoxStyle;

	CRect m_oldParentRect;

	int m_borderPenWidth;
	int m_CornerRadius;
	bool m_bTracking;
	bool m_bHasFocus;
	bool m_bOnButton;
	bool m_bButtonPress;
	bool m_bFilledArrow;

	bool m_LeftMouseButtonIsDown;
	bool m_bMultiSelection;
	set<int> m_SelectedItems;

	void UpdateControlState();

protected:

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCbnKillfocus();
	afx_msg void OnCbnSetfocus();
	virtual void PreSubclassWindow();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCbnSelchange();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
protected:
	afx_msg LRESULT OnCbSetcursel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCbSetitemheight(WPARAM wParam, LPARAM lParam);	
};

