#include "resizeworld_dialog.h"
#include "../shared_functions.h"
#include "../../util/win32pp_extras.h"
#include "../../adventuregamer_constants.h"
#include "../../util/languagemapper.h"

namespace ControlIDs {
    const WORD txtWidth = 101;
    const WORD txtHeight = 102;
}

//=============================================================================
// Constructors
//=============================================================================

ResizeWorldDialog::ResizeWorldDialog(MainWindowInterface* inMainWindow, HWND inParentHandle) :
EditDialogBase(inMainWindow, inParentHandle, &MainWindowInterface::finishedResizeWorldDialog) {
}

//=============================================================================
// Mutators
//=============================================================================

///----------------------------------------------------------------------------
/// setWorldDimensions - Set what the current width/height of the Game World is
/// @param an integer specifying the width of the current world
/// @param an integer specifying the height of the current world
///----------------------------------------------------------------------------

void ResizeWorldDialog::setWorldDimensions(const int& currentWidth, const int& currentHeight) {
    
    worldWidth = currentWidth;
    worldHeight = currentHeight;

    CString caption;

}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

BOOL ResizeWorldDialog::OnCommand(WPARAM wParam, LPARAM lParam) {

    const WORD ctrlID = LOWORD(wParam);
    const WORD notifyCode = HIWORD(wParam);

    if (lParam) {
        if (ctrlID == IDOK || ctrlID == IDCANCEL || ctrlID == DefControlIDs::IDAPPLY) {
            if (notifyCode == BN_CLICKED) {
                dialogButtonPressed(ctrlID);
                return TRUE;
            }
        } else if (ctrlID >= ControlIDs::txtWidth && ctrlID <= ControlIDs::txtHeight) {
            if (notifyCode == EN_CHANGE) {
                madeChange();
                return TRUE;
            }
        }
    }

    return FALSE;
}

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the Dialog window, and create remaining
/// child controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int ResizeWorldDialog::OnCreate(CREATESTRUCT& cs) {

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    for (int i = 0; i < 2; ++i) {
        lblDimenions[i].Create(*this, 0, SS_SIMPLE);
        txtDimensions[i].Create(*this, 0, WS_TABSTOP | ES_AUTOHSCROLL);
        txtDimensions[i].SetExStyle(WS_EX_CLIENTEDGE);
        txtDimensions[i].SetDlgCtrlID(ControlIDs::txtWidth + i);
        txtDimensions[i].SetLimitText(2);

    }

    dimensionValidator[0] = IntegerValidator(&txtDimensions[0], 
                                             AdventureGamerConstants::MinWorldWidth,
                                             AdventureGamerConstants::MaxWorldWidth);

    dimensionValidator[1] = IntegerValidator(&txtDimensions[1],
                                             AdventureGamerConstants::MinWorldHeight,
                                             AdventureGamerConstants::MaxWorldHeight);

    lblDimenions[0].SetWindowText(L"Width");
    lblDimenions[1].SetWindowText(L"Height");
    
    createDefaultDialogButtons(true);

    // TODO: Font code and the resize code here it in every dialog, it might
    // Be possible to turn that into it's own function to reduce code reuse

    HFONT dialogFont = windowMetrics.GetCurrentFont();
    EnumChildWindows(*this, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);

    const LONG contentWidth = calculateWindowWidth();

    // TODO: Finish calculating dimensions
    RECT rc ={ 0, 0,
        contentWidth + (windowMetrics.GetControlSpacing().XWINDOW_MARGIN * 2),
        0 };

    AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());

    SetWindowPos(0, 0, 0, rc.right + abs(rc.left), rc.bottom + abs(rc.top),
                 SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREPOSITION);

    moveControls();

    return CWnd::OnCreate(cs);
}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for the dialog
///----------------------------------------------------------------------------

void ResizeWorldDialog::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"ResizeWorldDialog";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

//=============================================================================
// Protected Functions
//=============================================================================

///----------------------------------------------------------------------------
/// moveControls - Move the controls into their proper positions
///----------------------------------------------------------------------------

void ResizeWorldDialog::moveControls() {

    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD   = windowMetrics.GetControlDimensions();

    const int maxRowWidth  = GetClientRect().right - (CS.XWINDOW_MARGIN * 2);

    const CSize defaultLabelSize(maxRowWidth, CD.YLABEL);
    const CSize defaultEditSize(maxRowWidth, CD.YTEXTBOX_ONE_LINE_ALONE);

    CPoint cPos(CS.XWINDOW_MARGIN, CS.YWINDOW_MARGIN);

    for (int i = 0; i < 2; ++i) {

        lblDimenions[i].MoveWindow(cPos.x, cPos.y,
                                   defaultLabelSize.cx, defaultLabelSize.cy);
        
        cPos.Offset(0, defaultLabelSize.cy + CS.YLABELASSOC_MARGIN);

        txtDimensions[i].MoveWindow(cPos.x, cPos.y,
                                    defaultEditSize.cx, defaultEditSize.cy);

        cPos.Offset(0, defaultEditSize.cy + CS.YRELATED_MARGIN);

    }

    cPos.Offset(0, CS.YUNRELATED_MARGIN);

    CPoint btnPos(GetClientRect().right - CS.XWINDOW_MARGIN, cPos.y + CS.YUNRELATED_MARGIN);
    btnPos.Offset(-(CD.XBUTTON), 0);

    for (int i = 2; i >= 0; --i) {
        btnDialog[i].MoveWindow(btnPos.x, btnPos.y, CD.XBUTTON, CD.YBUTTON);
        btnPos.Offset(-(CD.XBUTTON + CS.XBUTTON_MARGIN), 0);
    }

    btnPos.Offset(0, CD.YBUTTON);

    // TODO: Finish calculating dimensions
    RECT rc ={ 0, 0,
        maxRowWidth + (CS.XWINDOW_MARGIN * 2),
        btnPos.y + (CS.YWINDOW_MARGIN * 2) };

    AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());

    SetWindowPos(0, 0, 0, rc.right + abs(rc.left), rc.bottom + abs(rc.top),
                 SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREPOSITION);

}

///----------------------------------------------------------------------------
/// trySaveData - Confirm data is valid, and if it is save it. This function
/// should not be called directly and instead trySave should be called instead.
/// @return true if the data was valid, false if it was not.
///----------------------------------------------------------------------------

bool ResizeWorldDialog::trySaveData() {

    const InputValidator* validationFailed = validateFields();

    if (validationFailed) {
        std::string errorMessage;
        std::string errorTitle;

        processValidatorError(errorMessage, errorTitle, validationFailed);
        displayErrorMessage(errorMessage, errorTitle);

        // TODO: Return an ID instead of a HWND
        // Also, tell the tab what action to take (IE: if it's a textbox,
        // highlight it)
        if (validationFailed->getErrorCode() != errorCodes::ControlNotFound) {
            validationFailed->getWindow()->SetFocus();
        }

        return false;
    }


    worldWidth = std::stol(WtoA(txtDimensions[0].GetWindowText()).c_str());
    worldHeight = std::stol(WtoA(txtDimensions[1].GetWindowText()).c_str());

    return true;
}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// calculateWindowWidth - Calculates how wide the content of the window
/// needs to be.
/// @return a LONG integer containing the width of the content.
///----------------------------------------------------------------------------

LONG ResizeWorldDialog::calculateWindowWidth() {

    const WindowMetrics::ControlSpacing CS      = windowMetrics.GetControlSpacing();
    const WindowMetrics::ControlDimensions CD   = windowMetrics.GetControlDimensions();

    LONG contentWidth = 0;

    // The Width will be the size of the widest control, in this case it'll likely
    // be the buttons.

    const LONG minSize = (CD.XBUTTON * 4) + (CS.XBUTTON_MARGIN * 3) + (CS.XWINDOW_MARGIN * 2);

    for (int i = 0; i < 2; ++i) {
        contentWidth = std::max(windowMetrics.CalculateStringWidth(
            lblDimenions[i].GetWindowTextW().c_str()), contentWidth);
    }

    // Include margins
    contentWidth += CS.XWINDOW_MARGIN * 2;
    
    // Now check if the labels are wider than the minimum size needed to display the buttons.
    contentWidth = std::max(contentWidth, minSize);  

    return contentWidth;
}

///----------------------------------------------------------------------------
/// validateFields - Ensures that the data given by the user is valid, and if
/// is not, gives the user a chance to correct it.
/// @return NULL if no errors occurred, or a pointer to an input validator
/// if something was wrong
///----------------------------------------------------------------------------

InputValidator* ResizeWorldDialog::validateFields() {

    for (int i = 0; i < 2; ++i) {

        if (!dimensionValidator[i].validate()) {
            return &dimensionValidator[i];
        }

    }

    return NULL;

}