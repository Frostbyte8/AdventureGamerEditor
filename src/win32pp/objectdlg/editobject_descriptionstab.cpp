#include "editobject_tabviews.h"
#include "../../model/gameobject.h"
#include <algorithm>
#include <wxx_commondlg.h>

#include "../shared_functions.h"

using namespace EditObjectDialogConstants;

//=============================================================================
// Win32++ Functions
//=============================================================================

void EditObjectDescriptionsTab::PreCreate(CREATESTRUCT& cs) {
    cs.dwExStyle |= WS_EX_CONTROLPARENT;
}

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message. See the Win32++ documentation
/// for more information
///----------------------------------------------------------------------------

BOOL EditObjectDescriptionsTab::OnCommand(WPARAM wParam, LPARAM lParam) {
    
    if(lParam) {

        const WORD ctrlID = LOWORD(wParam);
        const WORD ctrlAction = HIWORD(wParam);

        if(ctrlID >= DescriptionsTab::ControlIDs::Name && ctrlID <= DescriptionsTab::ControlIDs::OnLastUse) {
            if(ctrlAction == EN_CHANGE) {
                parentWindow->madeChange();

            }
            return TRUE;
        }
        else if (ctrlID == DescriptionsTab::ControlIDs::BrowseIcon || ctrlID == DescriptionsTab::ControlIDs::BrowseSound) {
            if (ctrlAction == BN_CLICKED) {
                const bool findIcon     = ctrlID == DescriptionsTab::ControlIDs::BrowseIcon ? true : false;
                const int whichControl  = findIcon ? 4 : 5;

                if (dlgOnBrowseForMedia(*this, txtDescriptions[whichControl], findIcon)) {
                    parentWindow->madeChange();
                }

                return TRUE;
            }
        }

    }

    return FALSE;
}

//=============================================================================
// Public Functions
//=============================================================================

///----------------------------------------------------------------------------
/// insertData - Takes the data given by the user, and inputs it into the
/// builder object.
///----------------------------------------------------------------------------

void EditObjectDescriptionsTab::insertData(GameObject::Builder& builder) {

    // TODO: For the controller version of this, send the text and let the controller
    // do the string capping.

    CString wideDesc = txtDescriptions[0].GetWindowText().Left(GameObjectConstants::MaxNameLength);
    builder.description(WtoA(wideDesc).c_str(), 0);  

    for(int i = 1; i < 6; ++i) {
        wideDesc = txtDescriptions[i].GetWindowText().Left(GameObjectConstants::MaxDescriptionLength);
        builder.description(WtoA(wideDesc).c_str(), i);
    }

}



///----------------------------------------------------------------------------
/// populateFields - Take the data from the GameObject and fill in the fields
/// on the tab page.
/// @param a constant reference to GameObject object
///----------------------------------------------------------------------------

void EditObjectDescriptionsTab::populateFields(const GameObject &gameObject, const GameMap& gameMap) {

    for(int i = 0; i < GameObjectDescriptions::NumAllDescriptions; ++i) {

        CString caption;

        SetWindowTextFromStr(gameObject.getDescription(i),
                                            txtDescriptions[i], caption);
    }

}

///----------------------------------------------------------------------------
/// validateFields - Ensures that the data given by the user is valid, and if
/// is not, gives the user a chance to correct it.
/// @return NULL if no errors occurred, or a pointer to an input validator
/// if something was wrong
///----------------------------------------------------------------------------

InputValidator* EditObjectDescriptionsTab::validateFields() {
   
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