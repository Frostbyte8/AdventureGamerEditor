#include "editcharacter_tabviews.h"
#include "../../model/gamecharacter.h"

#include "../shared_functions.h"

using namespace EditCharacterDialogConstants::QualitiesTab;

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Process the WM_COMMAND message. Refer to to the Win32++
/// documentation for more details.
///----------------------------------------------------------------------------

BOOL EditCharacterQualitiesTab::OnCommand(WPARAM wParam, LPARAM lParam) {

    if(!lParam) {
        return FALSE;
    }

    const WORD ctrlID = LOWORD(wParam);
    const WORD ctrlAction = HIWORD(wParam);

    if(ctrlID == ControlIDs::MoneyOnHand) {

        if(ctrlAction == EN_KILLFOCUS) {

            // If the user enters a bogus value, we'll just reset the value
            // to the minimum amount of money a character can have.
            int newValue = AdventureGamerConstants::MinAmountOfMoney;

            try {
                newValue = std::stoi(WtoA(txtMoney.GetWindowText()).c_str());
            }
            catch(const std::invalid_argument&) {
                int newValue = AdventureGamerConstants::MinAmountOfMoney;
            }
            catch(const std::out_of_range&) {
                int newValue = AdventureGamerConstants::MinAmountOfMoney;
            }

            spnMoney.SetPos(newValue);
        }
        else if(ctrlAction == EN_CHANGE) {
            parentWindow->madeChange();
        }

    }
    else if(ctrlID >= ControlIDs::FlagEnterDark && ctrlID <= 
            ControlIDs::FlagStalker) {

        if(ctrlAction == BN_CLICKED) {
            parentWindow->madeChange();
        }

    }
    else if(ctrlID == ControlIDs::CharType) {

        if(ctrlAction == CBN_SELCHANGE) {
            parentWindow->madeChange();
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

void EditCharacterQualitiesTab::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditCharacterQualitiesTab";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

//=============================================================================
// Public Functions
//=============================================================================

///----------------------------------------------------------------------------
/// insertData - Takes the data given by the user, and inputs it into the
/// builder object.
///----------------------------------------------------------------------------

void EditCharacterQualitiesTab::insertData(GameCharacter::Builder& builder) {

    uint8_t newFlags = 0;

    for(int i = 0; i < GameCharacterFlags::NumFlags; ++i) {

        if(btnFlags[i].GetCheck() == BST_CHECKED) {
            newFlags += (1<<i);
        }

    }

    builder.flags(newFlags);
    builder.money(spnMoney.GetPos());
    builder.type(cbxType.GetCurSel() + 1);

}

///----------------------------------------------------------------------------
/// populateFields - Copy the data from the Game Character into the fields
/// on this tab page. 
///----------------------------------------------------------------------------

void EditCharacterQualitiesTab::populateFields(const GameCharacter& gameCharacter) {

    const uint8_t flags = gameCharacter.getFlags();

    for(int i = 0; i < GameCharacterFlags::NumFlags; ++i) {

        if(flags & (1<<i)) {
            btnFlags[i].SetCheck(BST_CHECKED);
        }

    }

    spnMoney.SetPos(gameCharacter.getMoney());
    cbxType.SetCurSel(gameCharacter.getType() - 1);

}
 
///----------------------------------------------------------------------------
/// validateFields - Ensures that the data given by the user is valid, and if
/// is not, gives the user a chance to correct it.
/// @return NULL if no errors occurred, or a pointer to an input validator
/// if something was wrong
///----------------------------------------------------------------------------

InputValidator* EditCharacterQualitiesTab::validateFields() {

    // TODO: Validate that Character type is also a valid value.

    if(!moneyValidator.validate()) {
        return &moneyValidator;
    }

    return NULL;

}
