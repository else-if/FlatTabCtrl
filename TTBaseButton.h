#pragma once

/// Control's type mask
#define BASEBTN_BS_TYPEMASK 0x0000000FL

/////////////////////////////////////////////////////////////////////////////
// CTTBaseButton window

class CTTBaseButton : public CButton
{
public:
    CTTBaseButton();
    virtual ~CTTBaseButton();

protected:
    virtual void PreSubclassWindow();

protected:
    afx_msg UINT OnGetDlgCode();
    afx_msg LRESULT OnSetStyle(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()

private:
    // Data members
    BOOL m_bCanBeDefault;	/* TRUE to enable default state handling */
    BOOL m_bDefault;		/* Set to TRUE when control has default state */
    UINT m_nTypeStyle;		/* Type of control */
    BOOL m_bIsCheckBox;    /* Set to TRUE when control is a check button */
    int  m_iCheckState;

protected:
    // Interface

    /// Use to enable or disable default state handling	
    void EnableDefault(BOOL bEnable);

    /// Use to know whether the control has a default state
    BOOL IsDefault() const;

    /// Use to know the type of control to draw
    UINT GetControlType() const;

    /// Use to know whether the control has a default state
    BOOL IsCheckBox() const;

public:
    /// Use to set the dialog's default pushbutton
    static void SetDefID(CDialog* pDialog, const UINT nID);

    LRESULT OnSetCheck(WPARAM wParam, LPARAM lParam);
    LRESULT OnGetCheck(WPARAM wParam, LPARAM lParam);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};