#include <wxx_commondlg.h>
#include "../shared_functions.h"
#include "../../model/gamecharacter.h"
#include "editcharacter_tabviews.h"

using namespace EditCharacterDialogConstants::DescriptionsTab;

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message. See the Win32++ documentation
/// for more information
///----------------------------------------------------------------------------

BOOL EditCharacterDescriptionsTab::OnCommand(WPARAM wParam, LPARAM lParam) {
    
    if(lParam) {

        const WORD ctrlID = LOWORD(wParam);
        const WORD ctrlAction = HIWORD(wParam);

        if(ctrlID == ControlIDs::BrowseIcon || ctrlID == ControlIDs::BrowseSound) {
            if(ctrlAction == BN_CLICKED) {

                const bool findIcon     = ctrlID == ControlIDs::BrowseIcon ? true : false;
                const int whichControl  = findIcon ? 4 : 5;

                if (dlgOnBrowseForMedia(*this, txtDescriptions[whichControl], findIcon)) {
                    parentWindow->madeChange();
                }

                return TRUE;
            }
        }
        else if(ctrlID >= ControlIDs::NameText && ctrlID <= ControlIDs::OnDeathText) {
            if(ctrlAction == EN_CHANGE) {
                parentWindow->madeChange();
            }
        }

    }

    return FALSE;
}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for dialog
///----------------------------------------------------------------------------

void EditCharacterDescriptionsTab::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditCharacterDescriptionsTab";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

//=============================================================================
// Public Functions
//=============================================================================



///----------------------------------------------------------------------------
/// insertData - Takes the data given by the user, and inputs it into the
/// builder object.
///----------------------------------------------------------------------------

void EditCharacterDescriptionsTab::insertData(GameCharacter::Builder& builder) {

    CString wideDesc = txtDescriptions[0].GetWindowText().Left(GameCharacterConstants::MaxCharacterName);
    builder.description(WtoA(wideDesc).c_str(), 0);

    wideDesc = txtDescriptions[1].GetWindowText().Left(GameCharacterConstants::MaxDescriptionLength);
    builder.description(WtoA(wideDesc).c_str(), 1);

    wideDesc = txtDescriptions[2].GetWindowText().Left(GameCharacterConstants::MaxFightText);
    builder.description(WtoA(wideDesc).c_str(), 2);

    wideDesc = txtDescriptions[3].GetWindowText().Left(GameCharacterConstants::MaxDeathText);
    builder.description(WtoA(wideDesc).c_str(), 3);

    wideDesc = txtDescriptions[4].GetWindowText();
    builder.description(WtoA(wideDesc).c_str(), 4);

    wideDesc = txtDescriptions[5].GetWindowText();
    builder.description(WtoA(wideDesc).c_str(), 5);

}



///----------------------------------------------------------------------------
/// populateFields - Fill the tab page with the relevant data from the
/// Game Character being worked on.
/// @param a constant reference to the Game Character containing the data to be
/// used to fill out the fields.
///----------------------------------------------------------------------------

void EditCharacterDescriptionsTab::populateFields(const GameCharacter& gameCharacter) {

    CString caption;

    for(int i = 0; i < GameCharacterDescriptions::NumAllDescriptions; ++i) {
        SetWindowTextFromStr(gameCharacter.getDescription(i), txtDescriptions[i], caption);
    }

}

///----------------------------------------------------------------------------
/// validateFields - Ensures that the data given by the user is valid, and if
/// is not, gives the user a chance to correct it.
/// @return NULL if no errors occurred, or a pointer to an input validator
/// if something was wrong
///----------------------------------------------------------------------------

InputValidator* EditCharacterDescriptionsTab::validateFields() {
   
    // For this tab page, only the last two text boxes need to be validated.

    if(txtDescriptions[4].GetTextLength() != 0) {
        if(!iconDescValidator.validate()) {
            return &iconDescValidator;
        }
    }

    if(txtDescriptions[5].GetTextLength() != 0) {
        if(!soundDescValidator.validate()) {
            return &soundDescValidator;
        }   
    }

    return NULL;
}