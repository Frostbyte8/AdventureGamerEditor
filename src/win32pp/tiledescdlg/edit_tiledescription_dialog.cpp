#include "edit_tiledescription_dialog.h"
#include "../../util/languagemapper.h"
#include "../shared_functions.h"

namespace ControlIDs {
    const WORD Name                 = 101;
    const WORD Description          = 102;
}


//=============================================================================
// Constructors
//=============================================================================

EditTileDescriptionDialog::EditTileDescriptionDialog(MainWindowInterface* inMainWindow, HWND inParentHandle) : 
EditDialogBase(inMainWindow, inParentHandle, &MainWindowInterface::finishedEditTileDescriptionDialog) {
}

//=============================================================================
// Accessors
//=============================================================================

///----------------------------------------------------------------------------
/// getTileName - Returns a copy of the tile's new name.
///----------------------------------------------------------------------------

const std::string EditTileDescriptionDialog::getTileName() const {
    return tileName;
}

///----------------------------------------------------------------------------
/// getTileDescription - Returns a copy of the tile's new description.
///----------------------------------------------------------------------------

const std::string EditTileDescriptionDialog::getTileDescription() const {
    return tileDescription;
}

//=============================================================================
// Mutators
//=============================================================================

///----------------------------------------------------------------------------
/// setTileDescription - Give the dialog a copy of the the name and description
/// the tile already has set.
/// @param a constant reference to the tile's name.
/// @param a constant reference to the tile's description.
///----------------------------------------------------------------------------

void EditTileDescriptionDialog::setTileDescription(const std::string& inName, const std::string& inDescription) {
    if(txtTileName.IsWindow()) {
        txtTileName.SetWindowText(AtoW(inName.c_str()));
        txtTileDescription.SetWindowText(AtoW(inDescription.c_str()));
    }
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

BOOL EditTileDescriptionDialog::OnCommand(WPARAM wParam, LPARAM lParam) {

    if(lParam) {
        const WORD ctrlID = LOWORD(wParam);
        const WORD notifyCode = HIWORD(wParam);

        if(ctrlID == ControlIDs::Name || ctrlID == ControlIDs::Description) {
            if(notifyCode == EN_CHANGE) {
                madeChange();
                return TRUE;
            }
        }
        else if(ctrlID == IDOK || ctrlID == IDCANCEL || 
            ctrlID == DefControlIDs::IDAPPLY) {

            dialogButtonPressed(ctrlID);
            return TRUE;

        }
    }

    return FALSE;
}

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the Dialog window, and create remaining
/// child controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int EditTileDescriptionDialog::OnCreate(CREATESTRUCT& cs) {

    const int retVal = CWnd::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    lblTileName.Create(*this, 0, SS_SIMPLE);
    SetWindowTextFromLangMapString("TileNameLabel", lblTileName, caption, langMap);

    txtTileName.Create(*this, 0, ES_AUTOHSCROLL);
    txtTileName.SetExStyle(txtTileName.GetExStyle() | WS_EX_CLIENTEDGE);
    txtTileName.SetDlgCtrlID(ControlIDs::Name);
    txtTileName.SetLimitText(GameMapConstants::MaxTileName);

    lblTileDescription.Create(*this, 0, SS_SIMPLE);
    SetWindowTextFromLangMapString("TileDescriptionLabel", lblTileDescription, caption, langMap);

    txtTileDescription.Create(*this, 0, ES_MULTILINE | WS_VSCROLL);
    txtTileDescription.SetExStyle(txtTileDescription.GetExStyle() | WS_EX_CLIENTEDGE);
    txtTileDescription.SetDlgCtrlID(ControlIDs::Description);
    txtTileDescription.SetLimitText(GameMapConstants::MaxTileDescription);

    for(int i = 0; i < 3; ++i) {
        btnDialogButtons[i].Create(*this, 0, BS_PUSHBUTTON);
    }

    SetWindowTextFromLangMapString("OKButton", btnDialogButtons[0], caption, langMap);
    SetWindowTextFromLangMapString("CancelButton", btnDialogButtons[1], caption, langMap);
    SetWindowTextFromLangMapString("ApplyButton", btnDialogButtons[2], caption, langMap);

    btnDialogButtons[0].SetStyle(btnDialogButtons[0].GetStyle() | BS_DEFPUSHBUTTON);
    btnDialogButtons[0].SetDlgCtrlID(IDOK);
    btnDialogButtons[1].SetDlgCtrlID(IDCANCEL);
    btnDialogButtons[2].SetDlgCtrlID(DefControlIDs::IDAPPLY);
    btnDialogButtons[2].EnableWindow(FALSE);

    HFONT dialogFont = windowMetrics.GetCurrentFont();
    EnumChildWindows(*this, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);

    moveControls();

    return retVal;
}


///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for the dialog
///----------------------------------------------------------------------------

void EditTileDescriptionDialog::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditTileDescriptionDialog";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

//=============================================================================
// Protected Functions
//=============================================================================

///----------------------------------------------------------------------------
/// notifyChangeMade - Change the apply button to be useable.
///----------------------------------------------------------------------------

void EditTileDescriptionDialog::notifyChangeMade() {
    btnDialogButtons[2].EnableWindow(TRUE);
}

///----------------------------------------------------------------------------
/// notifyChangesSaved - Change the apply button to be unusable.
///----------------------------------------------------------------------------

void EditTileDescriptionDialog::notifyChangesSaved() {
    btnDialogButtons[2].EnableWindow(FALSE);
}

///----------------------------------------------------------------------------
/// trySaveData - Confirm that data in the dialog is valid, and if it is, save
/// it. This function should not be called directly.
/// @return Always true
///----------------------------------------------------------------------------

bool EditTileDescriptionDialog::trySaveData() {

    tileName = WtoA(txtTileName.GetWindowText().Left(GameMapConstants::MaxTileName));
    tileDescription = WtoA(txtTileDescription.GetWindowText().Left(GameMapConstants::MaxTileDescription));
    
    return true;
}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// moveControls - Move the controls into their proper positions
///----------------------------------------------------------------------------

void EditTileDescriptionDialog::moveControls() {

    // TODO: At some point, make this dialog resizable, also figure out actual width

    const WindowMetrics::ControlDimensions  CD = windowMetrics.GetControlDimensions();
    const WindowMetrics::ControlSpacing     CS = windowMetrics.GetControlSpacing();

    const int minWidth = (CD.XBUTTON * 4) + (CS.XBUTTON_MARGIN * 3) + (CS.XWINDOW_MARGIN * 2);
    const int innerWidth = minWidth - (CS.XWINDOW_MARGIN * 2);

    CPoint cPos(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN);

    lblTileName.MoveWindow(cPos.x, cPos.y, innerWidth, CD.YLABEL);
    cPos.Offset(0, CD.YLABEL + CS.YRELATED_MARGIN);
    
    txtTileName.MoveWindow(cPos.x, cPos.y, innerWidth, CD.YTEXTBOX_ONE_LINE_ALONE);
    cPos.Offset(0, CD.YTEXTBOX_ONE_LINE_ALONE + CS.YRELATED_MARGIN);

    lblTileDescription.MoveWindow(cPos.x, cPos.y, innerWidth, CD.YLABEL);
    cPos.Offset(0, CD.YLABEL + CS.YRELATED_MARGIN);

    txtTileDescription.MoveWindow(cPos.x, cPos.y, innerWidth, (CD.YTEXTBOX_ONE_LINE_ALONE) + (CD.YTEXTBOX_ADDITIONAL_LINES * 5));
    cPos.Offset(innerWidth - (CD.XBUTTON),
                txtTileDescription.GetClientRect().Height() + CS.YUNRELATED_MARGIN);

    for(int i = 2; i >= 0; --i) {
        btnDialogButtons[i].MoveWindow(cPos.x, cPos.y, CD.XBUTTON, CD.YBUTTON);
        cPos.Offset(-(CD.XBUTTON + CS.XBUTTON_MARGIN), 0);
    }

    // TODO: Finish calculating dimensions
    RECT rc = {0, 0, minWidth, cPos.y + CS.YWINDOW_MARGIN + CD.YBUTTON};

    AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());

    SetWindowPos(0, 0, 0, rc.right + abs(rc.left), rc.bottom + abs(rc.top),
                 SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREPOSITION);

}
