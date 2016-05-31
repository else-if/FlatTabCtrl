#pragma once
#include "ControlsColorMap.h"
#include "TTBaseButton.h"

class CTTButton : public CTTBaseButton
{

public:
    CTTButton();
    virtual ~CTTButton();

    void SetDrawingProperties(int borderPenWidth, int cornerRadius);

    static void DrawButtonControl(CDC* pDC, CRect* pRect, CRect* pClipRect, int nStyle, ControlState buttonState = Normal,
        CString buttonText = _T(""), CFont* pFont = NULL, int cornerRadius = 5, int borderWidth = 1,
        ControlsColorMap* pColorMap = NULL, COLORREF captionTextColor = COLOR_BTNTEXT, bool bIsDefalut = false);

    static void DrawPushButton(CDC* pDC, CRect* pRect, CRect* pClipRect, ControlState buttonState = Normal,
        CString buttonText = _T(""), CFont* pFont = NULL, int cornerRadius = 5, int borderWidth = 1,
        ControlsColorMap* pColorMap = NULL, COLORREF captionTextColor = COLOR_BTNTEXT, bool bIsDefalut = false);

    static void DrawCheckBox(CDC* pDC, CRect* pRect, CRect* pClipRect, ControlState buttonState = Normal,
        CString buttonText = _T(""), CFont* pFont = NULL, int cornerRadius = 5, int borderWidth = 1,
        ControlsColorMap* pColorMap = NULL, COLORREF captionTextColor = COLOR_BTNTEXT);

    static void DrawCaptionText(CDC* pDC, CRect* pRect, CRect* pClipRect, CString buttonText = _T(""), int nFormat = -1,
        CFont * pFont = NULL, COLORREF textColor = COLOR_BTNTEXT);

protected:

    ControlState m_ButtonState;
    ControlsColorMap m_ColorMap;

    COLORREF m_CaptionTextColor;

    int m_BorderPenWidth;
    int m_CornerRadius;
    bool m_bTracking;

	CRect m_oldParentRect;

    void UpdateButtonState(UINT state);

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
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};