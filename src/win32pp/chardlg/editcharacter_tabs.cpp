#include "../shared_functions.h"
#include "../../model/gamecharacter.h"
#include "editcharacter_tabviews.h"
#include "../../adventuregamer_constants.h"

using namespace EditCharacterDialogConstants;

//=============================================================================
//
// Descriptions Tab
//
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message. See the Win32++ documentation
/// for more information
///----------------------------------------------------------------------------

BOOL EditCharacterDescriptionsTab::OnCommand(WPARAM wParam, LPARAM lParam) {

    if (lParam) {

        const WORD ctrlID = LOWORD(wParam);
        const WORD ctrlAction = HIWORD(wParam);

        if (ctrlID == DescriptionsTab::ControlIDs::BrowseIcon || 
            ctrlID == DescriptionsTab::ControlIDs::BrowseSound) {
            if (ctrlAction == BN_CLICKED) {

                const bool findIcon     = ctrlID == DescriptionsTab::ControlIDs::BrowseIcon ? true : false;
                const int whichControl  = findIcon ? 4 : 5;

                if (dlgOnBrowseForMedia(*this, txtDescriptions[whichControl], findIcon)) {
                    parentWindow->madeChange();
                }

                return TRUE;
            }
        } else if (ctrlID >= DescriptionsTab::ControlIDs::NameText && 
                   ctrlID <= DescriptionsTab::ControlIDs::OnDeathText) {
            if (ctrlAction == EN_CHANGE) {
                parentWindow->madeChange();
                return TRUE;
            }
        }

    }

    return FALSE;
}

///----------------------------------------------------------------------------
/// insertData - Takes the data given by the user, and inputs it into the
/// builder object.
///----------------------------------------------------------------------------

void EditCharacterDescriptionsTab::insertData(GameCharacter::Builder& builder) {

    CString wideDesc = txtDescriptions[0].GetWindowText().Left(GameCharacterConstants::MaxCharacterName);
    builder.description(TtoA(wideDesc).c_str(), 0);

    wideDesc = txtDescriptions[1].GetWindowText().Left(GameCharacterConstants::MaxDescriptionLength);
    builder.description(TtoA(wideDesc).c_str(), 1);

    wideDesc = txtDescriptions[2].GetWindowText().Left(GameCharacterConstants::MaxFightText);
    builder.description(TtoA(wideDesc).c_str(), 2);

    wideDesc = txtDescriptions[3].GetWindowText().Left(GameCharacterConstants::MaxDeathText);
    builder.description(TtoA(wideDesc).c_str(), 3);

    wideDesc = txtDescriptions[4].GetWindowText();
    builder.description(TtoA(wideDesc).c_str(), 4);

    wideDesc = txtDescriptions[5].GetWindowText();
    builder.description(TtoA(wideDesc).c_str(), 5);

}

///----------------------------------------------------------------------------
/// populateFields - Fill the tab page with the relevant data from the
/// Game Character being worked on.
/// @param a constant reference to the Game Character containing the data to be
/// used to fill out the fields.
///----------------------------------------------------------------------------

void EditCharacterDescriptionsTab::populateFields(const GameCharacter& gameCharacter) {

    CString caption;

    for (int i = 0; i < GameCharacterDescriptions::NumAllDescriptions; ++i) {
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

    if (txtDescriptions[4].GetTextLength() != 0) {
        if (!iconDescValidator.validate()) {
            return &iconDescValidator;
        }
    }

    if (txtDescriptions[5].GetTextLength() != 0) {
        if (!soundDescValidator.validate()) {
            return &soundDescValidator;
        }
    }

    return NULL;
}

//=============================================================================
//
// Qualities Tab
//
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Process the WM_COMMAND message. Refer to to the Win32++
/// documentation for more details.
///----------------------------------------------------------------------------

BOOL EditCharacterQualitiesTab::OnCommand(WPARAM wParam, LPARAM lParam) {

    if (!lParam) {
        return FALSE;
    }

    const WORD ctrlID = LOWORD(wParam);
    const WORD ctrlAction = HIWORD(wParam);

    if (ctrlID == QualitiesTab::ControlIDs::MoneyOnHand) {

        if (ctrlAction == EN_KILLFOCUS) {

            // If the user enters a bogus value, we'll just reset the value
            // to the minimum amount of money a character can have.
            int newValue = AdventureGamerConstants::MinAmountOfMoney;

            try {
                newValue = std::stoi(TtoA(txtMoney.GetWindowText()).c_str());
            } catch (const std::invalid_argument&) {
                int newValue = AdventureGamerConstants::MinAmountOfMoney;
            } catch (const std::out_of_range&) {
                int newValue = AdventureGamerConstants::MinAmountOfMoney;
            }

            spnMoney.SetPos(newValue);
        } else if (ctrlAction == EN_CHANGE) {
            parentWindow->madeChange();
        }

    } else if (ctrlID >= QualitiesTab::ControlIDs::FlagEnterDark && 
               ctrlID <= QualitiesTab::ControlIDs::FlagStalker) {

        if (ctrlAction == BN_CLICKED) {
            parentWindow->madeChange();
        }

    } else if (ctrlID == QualitiesTab::ControlIDs::CharType) {

        if (ctrlAction == CBN_SELCHANGE) {
            parentWindow->madeChange();
        }

    } else {

        return FALSE;

    }

    return TRUE;

}

///----------------------------------------------------------------------------
/// insertData - Takes the data given by the user, and inputs it into the
/// builder object.
///----------------------------------------------------------------------------

void EditCharacterQualitiesTab::insertData(GameCharacter::Builder& builder) {

    uint8_t newFlags = 0;

    for (int i = 0; i < GameCharacterFlags::NumFlags; ++i) {

        if (btnFlags[i].GetCheck() == BST_CHECKED) {
            newFlags += (1 << i);
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

    for (int i = 0; i < GameCharacterFlags::NumFlags; ++i) {

        if (flags & (1 << i)) {
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

    if (!moneyValidator.validate()) {
        return &moneyValidator;
    }

    return NULL;

}

//=============================================================================
//
// Attributes Tab
//
//=============================================================================


///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message. See the Win32++ documentation
/// for more information
///----------------------------------------------------------------------------

BOOL EditCharacterAttributesTab::OnCommand(WPARAM wParam, LPARAM lParam) {

    if (lParam) {

        const WORD ctrlID = LOWORD(wParam);
        const WORD notifyCode = HIWORD(wParam);

        if (ctrlID >= AttributesTab::ControlIDs::Energy &&
            ctrlID <= AttributesTab::ControlIDs::Luck) {

            const WORD index = ctrlID - AttributesTab::ControlIDs::Energy;

            if (notifyCode == EN_KILLFOCUS) {

                // If the user inputs a bogus value, we will reset the field to the
                // minimum attribute value.

                int newValue = AdventureGamerConstants::MinAttributeValue;

                try {
                    newValue = std::stoi(TtoA(txtAttribType[index].GetWindowText()).c_str());
                } catch (const std::invalid_argument&) {
                    newValue = AdventureGamerConstants::MinAttributeValue;
                } catch (const std::out_of_range&) {
                    newValue = AdventureGamerConstants::MinAttributeValue;
                }

                spnAttribType[index].SetPos(newValue);
                parentWindow->madeChange();

            } else if (notifyCode == EN_CHANGE) {
                parentWindow->madeChange();
            }

        } else if (ctrlID == AttributesTab::ControlIDs::SightType) {
            if (notifyCode == CBN_SELCHANGE) {
                parentWindow->madeChange();
            }
        }
    } else {
        return FALSE;
    }

    return TRUE;
}

///----------------------------------------------------------------------------
/// insertData - Takes the data given by the user, and inputs it into the
/// builder object.
///----------------------------------------------------------------------------

void EditCharacterAttributesTab::insertData(GameCharacter::Builder& builder) {

    for (int i = 0; i < AttributeTypes::NumTypesForCharacters; ++i) {

        int amount = 0;

#ifdef _DEBUG
        // The data was previously validated, so unless a programming error occurred
        // this should not fail.
        try {
#endif // _DEBUG
            amount = std::stoi(TtoA(txtAttribType[i].GetWindowText()).c_str());
#ifdef _DEBUG
        } catch (const std::invalid_argument&) {
            assert(0);
        } catch (const std::out_of_range&) {
            assert(0);
        }
#endif // _DEBUG 

        builder.attribute(amount, i);

    }

    builder.sight(cbxSight.GetCurSel() + 1);
}

///----------------------------------------------------------------------------
/// populateFields - Copy the data from the Game Character into the fields
/// on this tab page. 
///----------------------------------------------------------------------------

void EditCharacterAttributesTab::populateFields(const GameCharacter& gameCharacter) {

    for (int i = 0; i < AttributeTypes::NumTypesForCharacters; ++i) {
        spnAttribType[i].SetPos(gameCharacter.getAttribute(i));
    }

    cbxSight.SetCurSel(gameCharacter.getSight() - 1);

}

///----------------------------------------------------------------------------
/// validateFields - Ensures that the data given by the user is valid, and if
/// is not, gives the user a chance to correct it.
/// @return NULL if no errors occurred, or a pointer to an input validator
/// if something was wrong
///----------------------------------------------------------------------------

InputValidator* EditCharacterAttributesTab::validateFields() {

    for (int i = 0; i < AttributeTypes::NumTypesForCharacters; ++i) {
        if (!attributeValidator[i].validate()) {
            return &attributeValidator[i];
        }
    }

    return NULL;
}

//=============================================================================
//
// Misc Tab
//
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message. See the Win32++ documentation
/// for more information
///----------------------------------------------------------------------------

BOOL EditCharacterMiscTab::OnCommand(WPARAM wParam, LPARAM lParam) {

    if (lParam) {

        const WORD ctrlID = LOWORD(wParam);
        const WORD notifyCode = HIWORD(wParam);

        if (ctrlID >= MiscTab::ControlIDs::XTextCoord && 
            ctrlID <= MiscTab::ControlIDs::YTextCoord) {
            if (notifyCode == EN_CHANGE) {
                parentWindow->madeChange();
            }
        }
    } else {
        return FALSE;
    }

    return TRUE;
}

///----------------------------------------------------------------------------
/// insertData - Takes the data given by the user, and inputs it into the
/// builder object.
///----------------------------------------------------------------------------

void EditCharacterMiscTab::insertData(GameCharacter::Builder& builder) {

    int groundX = 0;
    int groundY = 0;

#ifdef _DEBUG
    // The data was previously validated, so unless a programming error occurred
    // this should not fail.
    try {
#endif // _DEBUG
        groundX = std::stoi(TtoA(txtCoords[0].GetWindowText()).c_str());
        groundY = std::stoi(TtoA(txtCoords[1].GetWindowText()).c_str());
#ifdef _DEBUG
    } catch (const std::invalid_argument&) {
        assert(0);
    } catch (const std::out_of_range&) {
        assert(0);
    }
#endif // _DEBUG 

    builder.location(groundX, groundY);

}

///----------------------------------------------------------------------------
/// populateFields - Copy the data from the Game Character into the fields
/// on this tab page.
///----------------------------------------------------------------------------

void EditCharacterMiscTab::populateFields(const GameCharacter& gameCharacter) {

    CString caption;
    SetWindowTextFromStr(std::to_string(gameCharacter.getX()), txtCoords[0], caption);
    SetWindowTextFromStr(std::to_string(gameCharacter.getY()), txtCoords[1], caption);

    std::vector<size_t> objectIndices = gameMap->getCharacterInventory(gameCharacter.getID());
    const std::vector<GameObject>& gameObjects = gameMap->getGameObjects();
    const size_t oiSize = objectIndices.size();

    for (size_t i = 0; i < oiSize; ++i) {
        lsbInventory.AddString(AtoT(gameObjects[objectIndices[i]].getName().c_str()));
    }

}

///----------------------------------------------------------------------------
/// validateFields - Ensures that the data given by the user is valid, and if
/// is not, gives the user a chance to correct it.
/// @return NULL if no errors occurred, or a pointer to an input validator
/// if something was wrong
///----------------------------------------------------------------------------

InputValidator* EditCharacterMiscTab::validateFields() {

    for (int i = 0; i < 2; ++i) {
        if (!coordValidator[i].validate()) {
            return &coordValidator[i];
        }
    }

    return NULL;
}