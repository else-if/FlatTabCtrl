#pragma once

#include "ControlsColorMap.h"

// CTTEdit

class CTTEdit : public CEdit
{

public:
    CTTEdit();
    virtual ~CTTEdit();

    void SetDrawingProperties(int borderPenWidth, int cornerRadius);

protected:

    bool m_bUseBaseMessageHandlers;
    bool m_bNcSizeIsCalculated;
    bool m_bPainted;

    Gdiplus::Rect m_ClientRect;

    ControlState m_ControlState;
    ControlsColorMap m_ColorMap;

    int m_borderPenWidth;
    int m_CornerRadius;

    bool m_bHover;
    bool m_bFocused;

    CDC m_dc;
    bool m_bUseBitmap;

    void UpdateControlState();
    void Paint(CDC* pDC);

    int m_OffsetY;
    UINT uiCX, uiCY;

    CRect m_oldWndRect;

    void SetPosition(int x, int y);
    void Trace(CString cMsg);

    DECLARE_MESSAGE_MAP()
public:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
    virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
    afx_msg void OnNcPaint();

    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnMouseLeave();
    afx_msg void OnEnUpdate();
    virtual void PreSubclassWindow();
    afx_msg void OnPaint();
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnMove(int x, int y);
};


