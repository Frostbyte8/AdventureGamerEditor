#include "editdialog_base.h"
#include "../util/languagemapper.h"
#include "shared_functions.h"

//=============================================================================
// Constructors
//=============================================================================

EditDialogBase::EditDialogBase(MainWindowInterface* inMainWindow, HWND inParentWindow, void (MainWindowInterface::*finishDialogFunc)()) :
mainWindow(inMainWindow), parentWindow(inParentWindow), optionChosen(IDCLOSE), 
changeMade(false), areSavedChanges(false), dialogReady(false), hasApplyButton(false),
originalWindowTitle(""), finishFunc(finishDialogFunc) {
    assert(finishFunc); // Finish function CANNOT be NULL.
}

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
/// OnClose - Called when the dialog is trying to be closed
///----------------------------------------------------------------------------

void EditDialogBase::OnClose() {

    if (!tryClose()) {
        return;
    }

    endModal();
}

///----------------------------------------------------------------------------
/// PreCreate - Override Window creation for the dialog
///----------------------------------------------------------------------------

void EditDialogBase::PreCreate(CREATESTRUCT& cs) {
    if(cs.style & WS_VISIBLE) {
        cs.style &= ~WS_VISIBLE;
    }
}

///----------------------------------------------------------------------------
/// PreTranslateMessage - Intercept messages, and check if the tab needs
/// to process them.
///----------------------------------------------------------------------------

BOOL EditDialogBase::PreTranslateMessage(MSG &msg) {

    if (IsDialogMessage(msg)) {
        return TRUE;
    }

    return CWnd::PreTranslateMessage(msg);

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
    notifyChangesSaved();
}

///----------------------------------------------------------------------------
/// createDefaultDialogButtons - Create the Ok, Cancel, and optionally Apply
/// buttons found on every dialog.
/// @param if true, the dialog has an apply button, if false it does not.
///----------------------------------------------------------------------------

void EditDialogBase::createDefaultDialogButtons(const bool hasApply) {

    hasApplyButton = hasApply;
    const int NUM_DIALOG_BUTTONS = hasApply ? 3 : 2;

    for (int i = 0; i < NUM_DIALOG_BUTTONS; ++i) {
        btnDialog[i].Create(*this, 0, WS_TABSTOP | BS_PUSHBUTTON);
    }

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    SetWindowTextFromLangMapString("OKButton", btnDialog[0], caption, langMap);
    SetWindowTextFromLangMapString("CancelButton", btnDialog[1], caption, langMap);

    btnDialog[0].SetStyle(btnDialog[0].GetStyle() | BS_DEFPUSHBUTTON);
    btnDialog[0].SetDlgCtrlID(IDOK);
    btnDialog[1].SetDlgCtrlID(IDCANCEL);

    if (hasApply) {
        SetWindowTextFromLangMapString("ApplyButton", btnDialog[2], caption, langMap);
        btnDialog[2].SetDlgCtrlID(DefControlIDs::IDAPPLY);
        btnDialog[2].EnableWindow(FALSE);
    }

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

void EditDialogBase::endModal() {

    dialogReady = false;

    CWnd::OnClose();

    (mainWindow->*finishFunc)();

}

///----------------------------------------------------------------------------
/// goModal - Attempts to make the Dialog window modal.
/// @return true if the Dialog was able to go modal, false if not.
///----------------------------------------------------------------------------

bool EditDialogBase::goModal() {

    if(parentWindow != NULL) {
        ::EnableWindow(parentWindow, FALSE);
        dialogReady = true;
        return true;
    }

    return false;
}

///----------------------------------------------------------------------------
/// notifyChangeMade - A field on the dialog was changed. Override this to be
/// notified of changes in a dialog. By default, it will enable the apply
/// button if one exists.
///----------------------------------------------------------------------------

void EditDialogBase::notifyChangeMade() {
    if (hasApplyButton) {
        btnDialog[2].EnableWindow(TRUE);
    }
}

///----------------------------------------------------------------------------
/// notifyChangesSaved - Changes are being saved. Override this to be notified
/// of such. By default it will disable the apply button if one exists.
///----------------------------------------------------------------------------

void EditDialogBase::notifyChangesSaved() {
    if (hasApplyButton) {
        btnDialog[2].EnableWindow(FALSE);
    }
}

///----------------------------------------------------------------------------
/// madeChange - If the dialog isn't already marked as having a change made,
/// do so and inform the classes that derive from it that something happened.
///----------------------------------------------------------------------------

void EditDialogBase::madeChange() {
    if(dialogReady && !changeMade) {
        changeMade = true;
        SetWindowText(originalWindowTitle + L"*");
        notifyChangeMade();
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

            const int retVal = askYesNoQuestion(langMap.get("UnsavedChangesText"), 
                                                langMap.get("UnsavedChangesTitle"), 
                                                true);
            
            if(retVal == GenericInterfaceResponses::Yes) {
                 // We will now act as if the use pressed the OK button.

                if(!trySave()) {
                    return false;
                }

            }
            else if(retVal == GenericInterfaceResponses::No) {
                // If they select no, we'll simply act as if they had
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