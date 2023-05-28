#include "editdialog_base.h"
#include "../util/languagemapper.h"

//=============================================================================
// Constructors
//=============================================================================

EditDialogBase::EditDialogBase(MainWindowInterface* inMainWindow, HWND inParentWindow) : 
mainWindow(inMainWindow), parentWindow(inParentWindow), optionChosen(IDCLOSE), 
changeMade(false), areSavedChanges(false), originalWindowTitle("") {}

//=============================================================================
// Mutators
//=============================================================================

///----------------------------------------------------------------------------
/// setDefaultDialogTitle - Not only sets the dialog's title, also keeps a copy
/// of it so it can append a * when unsaved changes are made, and restore it
/// when those changes are reverted.
/// @param a reference to a CString containing the title to be used
///----------------------------------------------------------------------------

void EditDialogBase::setDefaultDialogTitle(const CString& inTitle) {
    originalWindowTitle = inTitle;
    SetWindowText(originalWindowTitle);
}

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// PreCreate - Override Window creation for the dialog
///----------------------------------------------------------------------------

void EditDialogBase::PreCreate(CREATESTRUCT& cs) {
    if(cs.style & WS_VISIBLE) {
        cs.style &= ~WS_VISIBLE;
    }
}

//=============================================================================
// Public Functions
//=============================================================================

///----------------------------------------------------------------------------
/// changesSaved - Mark the changes are being saved, and reset the dialog title
///----------------------------------------------------------------------------

void EditDialogBase::changesSaved() {
    changeMade = false;
    areSavedChanges = true;
    SetWindowText(originalWindowTitle);
}

///----------------------------------------------------------------------------
/// dialogButtonPressed - do default processing when Ok, Cancel, or Apply are
/// pressed.
/// @param an integer that contains IDOK, IDCANCEL, or DefControlIDs::IDAPPLY
///----------------------------------------------------------------------------

void EditDialogBase::dialogButtonPressed(const int& which) {

    switch(which) {

        case IDOK:
            if(trySave()) {
                optionChosen = IDOK;
                Close();
            }
            break;

        case IDCANCEL:
            optionChosen = IDCANCEL;
            Close();
            break;

        case DefControlIDs::IDAPPLY:
            trySave();
            // IDCLOSE is the default action, since
            // apply doesn't close the window, we'll
            // just set the option back to the default
            optionChosen = IDCLOSE;
            break;

        #ifdef _DEBUG
        default:
            MessageBox(L"Warning: Invalid dialog button value was supplied.", L"Invalid Button", MB_OK | MB_ICONWARNING);
            break;
        #endif // _DEBUG

    }

}

///----------------------------------------------------------------------------
/// endModal - Enables the parent window and closes the dialog window.
///----------------------------------------------------------------------------

void EditDialogBase::endModal(void (MainWindowInterface::*finishFunction)() ) {

    if(parentWindow != NULL) {
        ::EnableWindow(parentWindow, TRUE);
    }

    CWnd::OnClose();

    // Finish function cannot be NULL. It *must* be specified.
    assert(finishFunction);
    (mainWindow->*finishFunction)();

}

///----------------------------------------------------------------------------
/// goModal - Attempts to make the Dialog window modal.
/// @return true if the Dialog was able to go modal, false if not.
///----------------------------------------------------------------------------

bool EditDialogBase::goModal() {

    if(parentWindow != NULL) {
        ::EnableWindow(parentWindow, FALSE);
        return true;
    }

    return false;
}

///----------------------------------------------------------------------------
/// madeChange - If the dialog isn't already marked as having a change made,
/// do so and inform the classes that derive from it that something happened.
///----------------------------------------------------------------------------

void EditDialogBase::madeChange() {
    if(!changeMade) {
        changeMade = true;
        SetWindowText(originalWindowTitle + L"*");
    }
}

///----------------------------------------------------------------------------
/// tryClose - Attempt to close the window, giving the user the option to
/// save any unsaved changes before closing.
/// @return true if closing should be done, false if it should not.
///----------------------------------------------------------------------------

bool EditDialogBase::tryClose() {
    
    if(optionChosen != IDCANCEL) {

        if(changeMade) {
            LanguageMapper& langMap = LanguageMapper::getInstance();

            const int retVal = askYesNoQuestion(langMap.get(LanguageConstants::UnsavedChangesMessage),
                                                langMap.get(LanguageConstants::UnsavedChangesTitle),
                                                true);
            
            if(retVal == GenericInterfaceResponses::Yes) {
                 // We will now act as if the use pressed the OK button.

                if(!trySave()) {
                    return false;
                }

            }
            else if(retVal == GenericInterfaceResponses::No) {
                // If they select no, we'll simplly act as if they had
                // pressed cancel button on the dialog, not the message box.
                optionChosen = IDCANCEL;
            }
            else {
                // The use selected "cancel" on the message box, so do not do anything!
                return false;
            }

        }

    }

    // The user has made their choice.
    return true;

}

///----------------------------------------------------------------------------
/// trySave - Attempt to save the data up to this point.
/// @return true if saving was successful, false if it was not.
///----------------------------------------------------------------------------

bool EditDialogBase::trySave() {

    if(!trySaveData()) {
        return false;
    }

    changesSaved();
    return true;

}

//=============================================================================
// Public Interface Functions
//=============================================================================

int EditDialogBase::askYesNoQuestion(const std::string& inQuestion, 
                                     const std::string& inTitle, bool allowCancel) {
    
    const CString question      = AtoW(inQuestion.c_str(), CP_UTF8);
    const CString title         = AtoW(inTitle.c_str(), CP_UTF8);
    const UINT messageBoxFlags  = MB_ICONQUESTION | (allowCancel ? MB_YESNOCANCEL : MB_YESNO);

    const int retVal = MessageBox(question, title, messageBoxFlags);

    if(retVal == IDYES) {
        return GenericInterfaceResponses::Yes;
    }
    else if(retVal == IDNO) {
        return GenericInterfaceResponses::No;
    }

    return GenericInterfaceResponses::Cancel;

}

void EditDialogBase::displayErrorMessage(const std::string &inMessage, 
                                         const std::string& inTitle) {

    const CString message       = AtoW(inMessage.c_str(), CP_UTF8);
    const CString title         = AtoW(inTitle.c_str(), CP_UTF8);
    const UINT messageBoxFlags  = MB_ICONERROR;

    MessageBox(message, title, messageBoxFlags);

}