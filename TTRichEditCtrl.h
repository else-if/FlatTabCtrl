#pragma once

#include "ControlsColorMap.h"

class CTTRichEditCtrl : public CRichEditCtrl
{
public:
    CTTRichEditCtrl();
    virtual ~CTTRichEditCtrl();

    void SetDrawingProperties(int borderPenWidth, int cornerRadius);
    virtual void PreSubclassWindow();

protected:
    bool m_bNcSizeIsCalculated;

    Gdiplus::Rect m_ClientRect;
    Gdiplus::Rect m_BorderRect;
    CRect m_OffsetRect;
    CRect m_oldParentRect;

    ControlState m_ControlState;
    ControlsColorMap m_ColorMap;

    int m_borderPenWidth;
    int m_CornerRadius;

    CDC m_dc;
    bool m_bUseBitmap;

    void UpdateControlState();

public:
    DECLARE_MESSAGE_MAP()
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
    afx_msg void OnNcPaint();
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnMove(int x, int y);
};

