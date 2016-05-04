#pragma once

#include "ControlsColorMap.h"

class CTTStatic2 : public CStatic
{
    DECLARE_DYNAMIC(CTTStatic2)

public:
    CTTStatic2();
    virtual ~CTTStatic2();

    COLORREF GetBackgroundColor();
    void SetBackgroundColor(COLORREF backgroundColor);

    COLORREF GetTextColor();
    void SetTextColor(COLORREF textColor);

    COLORREF GetBorderColor();
    void SetBorderColor(COLORREF textColor);

    void DrawBorders(bool drawBorder);
    void SetDrawingProperties(int borderPenWidth, int cornerRadius, bool drawBorder = false);
    void SetColorProperties(COLORREF backgroundColor, COLORREF textColor, COLORREF borderColor);

protected:

    ControlState m_ControlState;
    ControlsColorMap m_ColorMap;

    COLORREF m_backgroundColor;
    COLORREF m_textColor;
    COLORREF m_borderColor;

    bool m_bDrawBorders;
    int m_borderPenWidth;
    int m_iCornerRadius;

    void UpdateControlState();

protected:
    afx_msg LRESULT OnSetText(WPARAM, LPARAM);
    afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnPaint();
};