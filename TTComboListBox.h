#pragma once

class CTTComboListBox : public CListBox
{
public:
    CTTComboListBox();
    virtual ~CTTComboListBox();
    bool IsMouseover() { return m_bTracking; }

protected:
    bool m_bTracking;

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnMouseLeave();
};
