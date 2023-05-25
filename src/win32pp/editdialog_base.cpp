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