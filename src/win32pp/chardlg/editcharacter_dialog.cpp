#include "editcharacter_dialog.h"
#include "../shared_functions.h"

//=============================================================================
// Constructors
//=============================================================================

EditCharacterDialog::EditCharacterDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap,
HWND inParentHandle, bool inEditCharacter) : EditDialogBase(inMainWindow, inParentHandle),
gameMap(inGameMap), descriptionsTab(NULL), attributesTab(NULL), qualitiesTab(NULL), miscTab(NULL),
isEditCharacter(inEditCharacter) {
}

//=============================================================================
// Accessors
//=============================================================================

///----------------------------------------------------------------------------
/// getAlteredCharacter - Get a copy of the object that is being built by this
/// dialog window.
/// @return a Copy of a GameCharacter::Builder object.
///----------------------------------------------------------------------------

GameCharacter::Builder EditCharacterDialog::getAlteredCharacter() {
    return newCharacter;
}

///----------------------------------------------------------------------------
/// isEditingCharacter - Checks if the dialog is editing a character that
/// exists.
/// @return try if it is editing a character, false if it's creating one
///----------------------------------------------------------------------------

const bool EditCharacterDialog::isEditingCharacter() const {
    return isEditCharacter;
}

//=============================================================================
// Mutators
//=============================================================================

void EditCharacterDialog::setCharacterToEdit(const GameCharacter& gameCharacter) {
    
    if(descriptionsTab) {

        newCharacter = GameCharacter::Builder(gameCharacter);

        descriptionsTab->populateFields(gameCharacter);
        qualitiesTab->populateFields(gameCharacter);
        attributesTab->populateFields(gameCharacter);
        miscTab->populateFields(gameCharacter);

    }
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnClose - Processes the WM_CLOSE message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void EditCharacterDialog::OnClose() {

    if(!tryClose()) {
        return;
    }
    
    endModal(&MainWindowInterface::finishedEditCharacterDialog);

}

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

BOOL EditCharacterDialog::OnCommand(WPARAM wParam, LPARAM lParam) {

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

///----------------------------------------------------------------------------
/// PreTranslateMessage - Intercept messages, and check if the tab needs
/// to process them.
///----------------------------------------------------------------------------

BOOL EditCharacterDialog::PreTranslateMessage(MSG &msg) {

    if (IsDialogMessage(msg)) {
        return TRUE;
    }

    return CWnd::PreTranslateMessage(msg);

}

///----------------------------------------------------------------------------
/// WndProc - Window Procedure for the Frame.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

LRESULT EditCharacterDialog::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {
    return WndProcDefault(msg, wParam, lParam);
}

//=============================================================================
// Protected Functions
//=============================================================================

///----------------------------------------------------------------------------
/// notifyChangeMade - Change the apply button to be usable.
///----------------------------------------------------------------------------

void EditCharacterDialog::notifyChangeMade() {
    if(isEditCharacter) {
        btnDialogControl[2].EnableWindow(TRUE);
    }
}

///----------------------------------------------------------------------------
/// notifyChangesSaved - Change the apply button to be unusable.
///----------------------------------------------------------------------------

void EditCharacterDialog::notifyChangesSaved() {
    if(isEditCharacter) {
        btnDialogControl[2].EnableWindow(FALSE);
    }
}

///----------------------------------------------------------------------------
/// trySaveData - Confirm that data in the dialog (in this case, each tab page)
/// is valid, and if it is, save it.
/// @return true if the data was valid, false if it was not.
///----------------------------------------------------------------------------

bool EditCharacterDialog::trySaveData() {

    std::vector<ECTabViewBase*> tabPages;

    const size_t numTabs = 4;
    tabPages.reserve(numTabs);
    tabPages.push_back(descriptionsTab);
    tabPages.push_back(qualitiesTab);
    tabPages.push_back(attributesTab);
    tabPages.push_back(miscTab);

    for(size_t i = 0; i < numTabs; ++i) {

        ECTabViewBase& tabPage = *tabPages[i];
        const InputValidator* validatorFailed = tabPage.validateFields();

        if(validatorFailed) {

            std::string     errorMessage;
            std::string     errorTitle;
            processValidatorError(errorMessage, errorTitle, validatorFailed);
            displayErrorMessage(errorMessage, errorTitle);

            tabControl.SelectPage(i);

            // TODO: see EditObjectDialog's TODO
            if(validatorFailed->getErrorCode() != errorCodes::ControlNotFound) {
                validatorFailed->getWindow()->SetFocus();
            }
            
            return false;

        }
    }

    descriptionsTab->insertData(newCharacter);
    qualitiesTab ->insertData(newCharacter);
    attributesTab->insertData(newCharacter);
    miscTab->insertData(newCharacter);

    return true;

}