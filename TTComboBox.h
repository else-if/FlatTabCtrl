#pragma once
#include "ControlsColorMap.h"
#include "TTComboEdit.h"
#include "TTComboListBox.h"
#include "TTStatic.h"

class CTTComboBox : public CComboBox
{

public:
    CTTComboBox();
    virtual ~CTTComboBox();

    void SetDrawingProperties(int borderPenWidth, int cornerRadius);

    static void CTTComboBox::DrawComboBoxControl(CDC* pDC, CRect* pRect, CRect* pClipRect, int nStyle,
        ControlState controlState = Normal, ControlState backgrougControlState = Normal, ControlState arrowButtonState = Normal,
        const CString& itemText = _T(""), CFont* pFont = NULL, int cornerRadius = 5, int borderWidth = 1, ControlsColorMap* pColorMap = NULL);

    static void DrawDropDown(CDC* pDC, CRect* pRect, CRect* pClipRect, ControlState controlState = Normal,
        ControlState arrowButtonState = Normal, int cornerRadius = 5, int borderWidth = 1,
        ControlsColorMap* pColorMap = NULL);

    static void DrawDropDownList(CDC* pDC, CRect* pRect, CRect* pClipRect, ControlState controlState = Normal,
        ControlState backgrougControlState = Normal, ControlState arrowButtonState = Normal, const CString& itemText = _T(""),
        CFont* pFont = NULL, int cornerRadius = 5, int borderWidth = 1, ControlsColorMap* pColorMap = NULL);

    static void DrawArrowButon(CDC* pDC, CRect* pRect, CRect* pClipRect,
        ControlState arrowButtonState = Normal, ControlState controlState = Normal,
        int cornerRadius = 5, int borderWidth = 1, ControlsColorMap* pColorMap = NULL,
        BOOL filledArrow = FALSE);

    static void DrawSimple(CDC* pDC, CRect* pRect, CRect* pClipRect, ControlState controlState,
        int cornerRadius = 5, int borderWidth = 1, ControlsColorMap* pColorMap = NULL);

    static void DrawSelectedItemText(CDC* pDC, CRect* pRect, const CString& itemText, ControlState controlState = Normal,
        int borderWidth = 1, CFont* pFont = NULL);

    static CRect GetArrowButtonRect(CRect* clientRect);

protected:

    CDC m_dc;
    bool m_bUseBmp;

    CTTComboEdit m_edit;
    CTTComboListBox m_listbox;
    CTTStatic m_static;

    ControlState m_ControlState;
    ControlState m_ArrowButtonState;
    ControlsColorMap m_ColorMap;

    DWORD m_iComboBoxStyle;

    int m_borderPenWidth;
    int m_CornerRadius;
    bool m_bTracking;
    bool m_bHasFocus;
    bool m_bOnButton;
    bool m_bButtonPress;
    bool m_bFilledArrow;

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
protected:
    afx_msg LRESULT OnCbSetcursel(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnMove(int x, int y);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
};

