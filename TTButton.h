#pragma once
#include "ControlsColorMap.h"
#include "TTBaseButton.h"

class CTTButton : public CTTBaseButton
{

public:
    CTTButton();
    virtual ~CTTButton();

    void SetDrawingProperties(int borderPenWidth, int cornerRadius);

protected:

    ControlState m_ButtonState;
    ControlsColorMap m_ColorMap;

    COLORREF m_CaptionTextColor;

    int m_BorderPenWidth;
    int m_CornerRadius;
    bool m_bTracking;

    CFont m_TextFont;

    void UpdateButtonState(UINT state);

    void DrawPushButton(LPDRAWITEMSTRUCT lpDrawItemStruct);
    void DrawCheckBox(LPDRAWITEMSTRUCT lpDrawItemStruct);

public:

    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:

    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

    DECLARE_MESSAGE_MAP()
public:

    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnMouseLeave();
    virtual void PreSubclassWindow();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};