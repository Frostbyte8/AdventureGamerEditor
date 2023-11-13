#include "editcharacter_tabviews.h"
#include "../../model/gamecharacter.h"

#include "../shared_functions.h"
#include "../../adventuregamer_constants.h"


using namespace EditCharacterDialogConstants::AttributesTab;

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message. See the Win32++ documentation
/// for more information
///----------------------------------------------------------------------------

BOOL EditCharacterAttributesTab::OnCommand(WPARAM wParam, LPARAM lParam) {

    if(lParam) {

        const WORD ctrlID = LOWORD(wParam);
        const WORD notifyCode = HIWORD(wParam);

        if(ctrlID >= ControlIDs::Energy &&
           ctrlID <= ControlIDs::Luck) {

            const WORD index = ctrlID - ControlIDs::Energy;
            
            if(notifyCode == EN_KILLFOCUS) {

                // If the user inputs a bogus value, we will reset the field to the
                // minimum attribute value.

                int newValue = AdventureGamerConstants::MinAttributeValue;

                try {
                    newValue = std::stoi(WtoA(txtAttribType[index].GetWindowText()).c_str());
                }
                catch(const std::invalid_argument&) {
                    newValue = AdventureGamerConstants::MinAttributeValue;
                }
                catch(const std::out_of_range&) {
                    newValue = AdventureGamerConstants::MinAttributeValue;
                }
               
                spnAttribType[index].SetPos(newValue);
                parentWindow->madeChange();

            }
            else if(notifyCode == EN_CHANGE) {
                parentWindow->madeChange();
            }

        }
        else if(ctrlID == ControlIDs::SightType) {
            if(notifyCode == CBN_SELCHANGE) {
                parentWindow->madeChange();
            }
        }
    }
    else {
        return FALSE;
    }

    return TRUE;
}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for dialog
///----------------------------------------------------------------------------

void EditCharacterAttributesTab::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditCharacterAttributesTab";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

//=============================================================================
// Public Functions
//=============================================================================

///----------------------------------------------------------------------------
/// insertData - Takes the data given by the user, and inputs it into the
/// builder object.
///----------------------------------------------------------------------------

void EditCharacterAttributesTab::insertData(GameCharacter::Builder& builder) {

    for(int i = 0; i < AttributeTypes::NumTypesForCharacters; ++i) {
        
        int amount = 0;

#ifdef _DEBUG
        // The data was previously validated, so unless a programming error occured
        // this should not fail.
        try {
#endif // _DEBUG
            amount = std::stoi(WtoA(txtAttribType[i].GetWindowText()).c_str());
#ifdef _DEBUG
        }
        catch(const std::invalid_argument&) {
            assert(0);
        }
        catch(const std::out_of_range&) {
            assert(0);
        }
#endif // _DEBUG 

        builder.attribute(amount, i);

    }

    builder.sight(cbxSight.GetCurSel() + 1);
}

///----------------------------------------------------------------------------
/// populateFields - 
///----------------------------------------------------------------------------

void EditCharacterAttributesTab::populateFields(const GameCharacter& gameCharacter) {

    for(int i = 0; i < AttributeTypes::NumTypesForCharacters; ++i) {
        spnAttribType[i].SetPos(gameCharacter.getAttribute(i));
    }

    cbxSight.SetCurSel(gameCharacter.getSight() - 1);

}

//=============================================================================
// Protected Functions
//=============================================================================

///----------------------------------------------------------------------------
/// validateFields - Ensures that the data given by the user is valid, and if
/// is not, gives the user a chance to correct it.
/// @return NULL if no errors occurred, or a pointer to an input validator
/// if something was wrong
///----------------------------------------------------------------------------

InputValidator* EditCharacterAttributesTab::validateFields() {

    for(int i = 0; i < AttributeTypes::NumTypesForCharacters; ++i) {
        if(!attributeValidator[i].validate()) {
            return &attributeValidator[i];
        }
    }

    return NULL;
}
