#include "editobject_dialog.h"
#include "../shared_functions.h"
#include <algorithm>

//=============================================================================
//
// Dialog Window
//
//=============================================================================

//=============================================================================
// Constructors
//=============================================================================

EditObjectDialog::EditObjectDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap, 
HWND inParentHandle, bool inEditObject) : EditDialogBase(inMainWindow, inParentHandle),
gameMap(inGameMap), descriptionsTab(0), qualitiesTab(0), effectsTab(0), locationsTab(0),
isEditObject(inEditObject) {
}

//=============================================================================
// Accessors
//=============================================================================

///----------------------------------------------------------------------------
/// getAlteredObject - Get a copy of the object that is being built by this
/// dialog window.
/// @return a Copy of a GameObject::Builder object.
///----------------------------------------------------------------------------

GameObject::Builder EditObjectDialog::getAlteredObject() {
    return newObject;
}

///----------------------------------------------------------------------------
/// isEditingObject - Returns whether this dialog is editing an object that
/// already exists, or is creating a new one.
/// @return true if it is editing an object, or false if it is creating a new
/// one.
///----------------------------------------------------------------------------

const bool EditObjectDialog::isEditingObject() const {
    return isEditObject;
}

//=============================================================================
// Mutators
//=============================================================================

///----------------------------------------------------------------------------
/// setObjectToEdit - Copy the object being edited into the dialog so the copy
/// can be worked on.
/// @param a constant reference of the desired GameObject
///----------------------------------------------------------------------------

void EditObjectDialog::setObjectToEdit(const GameObject& gameObject) {

    if(descriptionsTab) {
        newObject = GameObject::Builder(gameObject);
        descriptionsTab->populateFields(gameObject, *gameMap);
        qualitiesTab->populateFields(gameObject, *gameMap);
        effectsTab->populateFields(gameObject, *gameMap);
        locationsTab->populateFields(gameObject, *gameMap);
    }

}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnClose - Processes the WM_CLOSE message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void EditObjectDialog::OnClose() {

    // First we'll see if we can actually close the dialog.
    if(!tryClose()) {
        return;
    }

    // Then we'll end the dialog and inform the parent window
    // that we are done.
    
    endModal(&MainWindowInterface::finishedEditObjectDialog);

}

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

BOOL EditObjectDialog::OnCommand(WPARAM wParam, LPARAM lParam) {

    const WORD ctrlID = LOWORD(wParam);
    const WORD notifyCode = HIWORD(wParam);

    if(lParam && notifyCode == BN_CLICKED) {
        if(ctrlID == IDOK || ctrlID == IDCANCEL || 
            ctrlID == DefControlIDs::IDAPPLY) {

            dialogButtonPressed(ctrlID);
            return TRUE;

        }
    }

    return FALSE;

}

LRESULT EditObjectDialog::OnNotify(WPARAM wParam, LPARAM lParam) {

    NMHDR* notifyHDR = reinterpret_cast<NMHDR*>(lParam);

    if(notifyHDR->hwndFrom == tabControl.GetHwnd() && notifyHDR->code == TCN_SELCHANGE) {
        // Update Misc Tab if necessary
        locationsTab->canBeHeldByEntities(!qualitiesTab->isFixedFlagChecked());
    }

    return 0; 
}

///----------------------------------------------------------------------------
/// onSize - Processes the WM_SIZE message for the dialog window
///----------------------------------------------------------------------------

LRESULT EditObjectDialog::onSize(WPARAM& wParam, LPARAM& lParam) {

    /*
    const WORD newWidth = LOWORD(lParam);
    const WORD newHeight = HIWORD(lParam);

    bool hasHorz = false;
    bool hasVert = false;
    int whichBars = 0;

    if(newWidth < contentSize.cx) {
        hasHorz = true;
    }

    if(newHeight < contentSize.cy) {
        hasVert = true;
    }

    // Check to see if we need a horizontal one after turning
    // on the vertical one
    if(hasHorz == false && hasVert == true) {
        if(newWidth < contentSize.cx - 17) {
            hasHorz = true;
        }

    }
    else if(hasVert == false && hasHorz == true) {
        if(newHeight < contentSize.cy - 17) {
            hasVert = true;
        }
    }

    if(hasHorz && hasVert) {
        ShowScrollBar(SB_BOTH, TRUE);
    }
    else if(hasHorz) {
        ShowScrollBar(SB_HORZ, TRUE);
        ShowScrollBar(SB_VERT, FALSE);
    }
    else if(hasVert) {
        ShowScrollBar(SB_VERT, TRUE);
        ShowScrollBar(SB_HORZ, FALSE);
    }
    else {
        ShowScrollBar(SB_BOTH, FALSE);
    }
    */

    return 0;

}



///----------------------------------------------------------------------------
/// WndProc - Window Procedure for the Frame.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

LRESULT EditObjectDialog::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {

    if(msg == WM_SIZE) {
        //return OnSize(wParam, lParam);
    }

    return WndProcDefault(msg, wParam, lParam);
}

///----------------------------------------------------------------------------
/// PreTranslateMessage - Intercept messages, and check if the tab needs
/// to process them.
///----------------------------------------------------------------------------

BOOL EditObjectDialog::PreTranslateMessage(MSG &msg) {

    if (IsDialogMessage(msg)) {
        return TRUE;
    }

    return CWnd::PreTranslateMessage(msg);

}

//=============================================================================
// Protected Functions
//=============================================================================

///----------------------------------------------------------------------------
/// notifyChangeMade - Change the apply button to be usable.
///----------------------------------------------------------------------------

void EditObjectDialog::notifyChangeMade() {
    if(isEditObject) {
        btnDialogControl[2].EnableWindow(TRUE);
    }
}

///----------------------------------------------------------------------------
/// notifyChangesSaved - Change the apply button to be unusable.
///----------------------------------------------------------------------------

void EditObjectDialog::notifyChangesSaved() {
    if(isEditObject) {
        btnDialogControl[2].EnableWindow(FALSE);
    }
}

///----------------------------------------------------------------------------
/// trySaveData - Confirm that data in the dialog (in this case, each tab page)
/// is valid, and if it is, save it. This function should not be called
/// directly.
/// @return true if the data was valid, false if it was not.
///----------------------------------------------------------------------------

bool EditObjectDialog::trySaveData() {

    std::vector<EOTabViewBase*> tabPages;
    
    const size_t numTabs = 4;
    tabPages.reserve(numTabs);
    tabPages.push_back(descriptionsTab);
    tabPages.push_back(qualitiesTab);
    tabPages.push_back(effectsTab);
    tabPages.push_back(locationsTab);

    // Iterate over the tabs, and validate their fields.
    for(size_t i = 0; i < numTabs; ++i) {

        EOTabViewBase& tabPage = *tabPages[i];
        const InputValidator* validatorFailed = tabPage.validateFields();

        if(validatorFailed) {

            std::string errorMessage;
            std::string errorTitle;

            processValidatorError(errorMessage, errorTitle, validatorFailed);
            displayErrorMessage(errorMessage, errorTitle);

            tabControl.SelectPage(i);

            // TODO: Return an ID instead of a HWND
            // Also, tell the tab what action to take (IE: if it's a textbox,
            // hilight it)
            if(validatorFailed->getErrorCode() != errorCodes::ControlNotFound) {
                validatorFailed->getWindow()->SetFocus();
            }
            
            return false;

        }

    }

    descriptionsTab->insertData(newObject);
    qualitiesTab->insertData(newObject);
    effectsTab->insertData(newObject);
    locationsTab->insertData(newObject);

    return true;

}