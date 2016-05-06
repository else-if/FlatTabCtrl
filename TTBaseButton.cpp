#include "stdafx.h"
#include "TTBaseButton.h"

/////////////////////////////////////////////////////////////////////////////
// CTTBaseButton

CTTBaseButton::CTTBaseButton()
{
    m_bDefault = FALSE;
    m_bCanBeDefault = FALSE;
    m_bIsCheckBox = FALSE;
    m_iCheckState = BST_UNCHECKED;

    // invalid value, since type still unknown
    m_nTypeStyle = BASEBTN_BS_TYPEMASK;
}

CTTBaseButton::~CTTBaseButton()
{
}

BEGIN_MESSAGE_MAP(CTTBaseButton, CButton)
    ON_WM_GETDLGCODE()
    ON_MESSAGE(BM_SETSTYLE, OnSetStyle)
    ON_MESSAGE(BM_SETCHECK, OnSetCheck)
    ON_MESSAGE(BM_GETCHECK, OnGetCheck)
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTTBaseButton message handlers

void CTTBaseButton::PreSubclassWindow()
{
    // set initial control type
    m_nTypeStyle = GetStyle() & BASEBTN_BS_TYPEMASK;

    // set initial default state flag
    if (m_nTypeStyle == BS_DEFPUSHBUTTON)
    {
        // enable default state handling for push buttons
        m_bCanBeDefault = TRUE;

        // set default state for a default button
        m_bDefault = TRUE;

        // adjust style for default button
        m_nTypeStyle = BS_PUSHBUTTON;
    }
    else if (m_nTypeStyle == BS_PUSHBUTTON)
    {
        // enable default state handling for push buttons
        m_bCanBeDefault = TRUE;
    }

    if ((m_nTypeStyle & BS_CHECKBOX)
        || (m_nTypeStyle & BS_AUTOCHECKBOX))
        m_bIsCheckBox = TRUE;

    // you should not set the Owner Draw before this call
    // (don't use the resource editor "Owner Draw" or
    // ModifyStyle(0, BS_OWNERDRAW) before calling PreSubclassWindow() )
    ASSERT(m_nTypeStyle != BS_OWNERDRAW);

    // switch to owner-draw
    ModifyStyle(BASEBTN_BS_TYPEMASK, BS_OWNERDRAW, SWP_FRAMECHANGED);

    CButton::PreSubclassWindow();
}

/// WM_GETDLGCODE message handler, indicate to the system whether we want to handle default state
UINT CTTBaseButton::OnGetDlgCode()
{
    UINT nCode = CButton::OnGetDlgCode();

    // handle standard control types
    switch (GetControlType())
    {
    case BS_RADIOBUTTON:
    case BS_AUTORADIOBUTTON:
        nCode |= DLGC_RADIOBUTTON;
        break;

    case BS_GROUPBOX:
        nCode = DLGC_STATIC;
        break;
    }

    // tell the system if we want default state handling
    // (losing default state always allowed)
    if (m_bCanBeDefault || m_bDefault)
        nCode |= (m_bDefault ? DLGC_DEFPUSHBUTTON : DLGC_UNDEFPUSHBUTTON);

    return nCode;
}

/// BM_SETSTYLE message handler, update internal default state data member
LRESULT CTTBaseButton::OnSetStyle(WPARAM wParam, LPARAM lParam)
{
    UINT nNewType = (wParam & BASEBTN_BS_TYPEMASK);

    // update default state flag
    if (nNewType == BS_DEFPUSHBUTTON)
    {
        // we must like default state at this point
        ASSERT(m_bCanBeDefault);

        m_bDefault = TRUE;
    }
    else if (nNewType == BS_PUSHBUTTON)
    {
        // losing default state always allowed
        m_bDefault = FALSE;
    }

    // can't change control type after owner-draw is set.
    // let the system process changes to other style bits
    // and redrawing, while keeping owner-draw style
    return DefWindowProc(BM_SETSTYLE,
        (wParam & ~BASEBTN_BS_TYPEMASK) | BS_OWNERDRAW, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// CTTBaseButton methods

/// <summary>
///     Get the type of control to draw
/// </summary>
/// <returns>
///     The type of control
/// </returns>
UINT CTTBaseButton::GetControlType() const
{
    return m_nTypeStyle;
}

/// <summary>
///     Get the control's default state
/// </summary>
/// <returns>
///     TRUE if control has a default state, FALSE otherwise
/// </returns>
BOOL CTTBaseButton::IsDefault() const
{
    // if we have default state, we must like it!
    ASSERT((m_bCanBeDefault && m_bDefault) == m_bDefault);

    return m_bDefault;
}

/// <summary>
///     Enable default state handling
/// </summary>
/// <param name="bEnable">TRUE to enable default state handling, FALSE to disable</param>
void CTTBaseButton::EnableDefault(BOOL bEnable)
{
    m_bCanBeDefault = bEnable;

    // disabling default when control has default state
    // needs removing the default state
    if (!bEnable && m_bDefault)
    {
        // remove default state
        SendMessage(BM_SETSTYLE, (GetStyle() & ~BASEBTN_BS_TYPEMASK) | BS_PUSHBUTTON, MAKELPARAM(TRUE, 0));
        ASSERT(m_bDefault == FALSE);

        // update default button
        CWnd* pParent = GetParent();
        if (pParent)
        {
            LRESULT lRes = pParent->SendMessage(DM_GETDEFID);
            if (HIWORD(lRes) == DC_HASDEFID)
            {
                pParent->SendMessage(DM_SETDEFID, LOWORD(lRes));
            }
        }
    }
}

/// <summary>
///     Set the dialog's default pushbutton
/// </summary>
/// <param name="pDialog">Dialog to set the default pushbutton</param>
/// <param name="nID">Button ID to make default</param>
void CTTBaseButton::SetDefID(CDialog* pDialog, const UINT nID)
{
    if (!pDialog || !::IsWindow(pDialog->m_hWnd))
    {
        ASSERT(FALSE); // Bad pointer or dialog is not a window
        return;
    }

    // get the current default button
    const DWORD dwPrevDefID = pDialog->GetDefID();
    const UINT nPrevID = (HIWORD(dwPrevDefID) == DC_HASDEFID) ? LOWORD(dwPrevDefID) : 0;

    
    // Set the new default ID in the dialog
    pDialog->SetDefID(nID);

    // Make sure the previous default button doesn't have the default state anymore
    
    // check previous ID is a default-compatible button
    // and it has the default state
    LRESULT lRes = (nPrevID == 0) ? 0 : pDialog->SendDlgItemMessage(nPrevID, WM_GETDLGCODE);
    if ((lRes & DLGC_BUTTON) &&
        (lRes & DLGC_DEFPUSHBUTTON))
    {
        pDialog->SendDlgItemMessage(nPrevID, BM_SETSTYLE,
            BS_PUSHBUTTON, MAKELPARAM(TRUE, 0));
    }

    //	Make sure the new default button receives the default state
    
    // check new ID is a button
    lRes = (nID == 0) ? 0 : pDialog->SendDlgItemMessage(nID, WM_GETDLGCODE);
    if (lRes & DLGC_BUTTON)
    {
        // exception: current focused button should keep its default state (IMHO)
        CWnd* pFocusWnd = GetFocus();
        LRESULT lResFocus = (pFocusWnd == NULL) ? 0 : pFocusWnd->SendMessage(WM_GETDLGCODE);

        // check focused control is a default-compatible button
        if ((lResFocus & DLGC_BUTTON) &&
            (lResFocus & (DLGC_DEFPUSHBUTTON | DLGC_UNDEFPUSHBUTTON)))
        {
            // remove default state (if needed)
            if ((lRes & DLGC_DEFPUSHBUTTON) &&
                (nID != (UINT)pFocusWnd->GetDlgCtrlID()))
            {
                pDialog->SendDlgItemMessage(nID, BM_SETSTYLE,
                    BS_PUSHBUTTON, MAKELPARAM(TRUE, 0));
            }

            // set default state (if needed)
            if (lResFocus & DLGC_UNDEFPUSHBUTTON)
            {
                pFocusWnd->SendMessage(BM_SETSTYLE,
                    BS_DEFPUSHBUTTON, MAKELPARAM(TRUE, 0));
            }
        }
        else if (lRes & DLGC_UNDEFPUSHBUTTON)
        {
            // not default-compatible button has the focus
            // set default state
            pDialog->SendDlgItemMessage(nID, BM_SETSTYLE,
                BS_DEFPUSHBUTTON, MAKELPARAM(TRUE, 0));
        }
    }
}

/// <summary>
///     Get the control's check box state
/// </summary>
/// <returns>
///     TRUE if control is a check box, FALSE otherwise
/// </returns>
BOOL CTTBaseButton::IsCheckBox() const
{
    return m_bIsCheckBox;
}

LRESULT CTTBaseButton::OnSetCheck(WPARAM wParam, LPARAM lParam) {
    m_iCheckState = wParam;
    return 0;
}

LRESULT CTTBaseButton::OnGetCheck(WPARAM wParam, LPARAM lParam) {
    return m_iCheckState;
}

void CTTBaseButton::OnLButtonUp(UINT nFlags, CPoint point)
{
    BOOL buttonPressed = GetState() & 4;
    if (buttonPressed) m_iCheckState = (GetStyle() & BS_PUSHLIKE) ? !m_iCheckState : 0;
    
    CButton::OnLButtonUp(nFlags, point);
}
