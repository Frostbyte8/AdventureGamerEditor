#include "editdialog_base.h"

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
/// GoModal - Attempts to make the Dialog window modal.
/// @return true if the Dialog was able to go modal, false if not.
///----------------------------------------------------------------------------

bool EditDialogBase::GoModal() {

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
        //markedAsChanged
    }
}

///----------------------------------------------------------------------------
/// tryClose - Attempt to close the window, giving the user the option to
/// save any unsaved changes before closing.
/// @return true if closing should be done, false if it should not.
///----------------------------------------------------------------------------

bool EditDialogBase::tryClose() {
    
    if(optionChosen == IDCLOSE) {

        if(changeMade) {
            LanguageMapper& langMap = LanguageMapper::getInstance();

            const int retVal = askYesNoQuestion(langMap.get(LanguageConstants::UnsavedChangesMessage),
                                                langMap.get(LanguageConstants::UnsavedChangesTitle),
                                                true);
            
            if(retVal == GenericInterfaceResponses::Yes) {
                optionChosen = IDOK; // We will now act as if the use pressed the OK button.
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

//=============================================================================
// Public Interface Functions
//=============================================================================

int EditDialogBase::askYesNoQuestion(const std::string& question, const std::string& title, bool allowCancel) {
    
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