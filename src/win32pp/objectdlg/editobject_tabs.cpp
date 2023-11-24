#include "editobject_tabviews.h"
#include "../shared_functions.h"
#include <algorithm>

using namespace EditObjectDialogConstants;

//=============================================================================
//
// Descriptions Tab
//
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message. See the Win32++ documentation
/// for more information
///----------------------------------------------------------------------------

BOOL EditObjectDescriptionsTab::OnCommand(WPARAM wParam, LPARAM lParam) {

    if (lParam) {

        const WORD ctrlID = LOWORD(wParam);
        const WORD ctrlAction = HIWORD(wParam);

        if (ctrlID >= DescriptionsTab::ControlIDs::Name && ctrlID <= DescriptionsTab::ControlIDs::OnLastUse) {
            if (ctrlAction == EN_CHANGE) {
                parentWindow->madeChange();

            }
            return TRUE;
        } else if (ctrlID == DescriptionsTab::ControlIDs::BrowseIcon || ctrlID == DescriptionsTab::ControlIDs::BrowseSound) {
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

///----------------------------------------------------------------------------
/// insertData - Takes the data given by the user, and inputs it into the
/// builder object.
///----------------------------------------------------------------------------

void EditObjectDescriptionsTab::insertData(GameObject::Builder& builder) {

    // TODO: For the controller version of this, send the text and let the controller
    // do the string capping.

    CString wideDesc = txtDescriptions[0].GetWindowText().Left(GameObjectConstants::MaxNameLength);
    builder.description(WtoA(wideDesc).c_str(), 0);

    for (int i = 1; i < 6; ++i) {
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

    for (int i = 0; i < GameObjectDescriptions::NumAllDescriptions; ++i) {

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

BOOL EditObjectQualitiesTab::OnCommand(WPARAM wParam, LPARAM lParam) {

    if (!lParam) {
        return FALSE;
    }

    const WORD ctrlID = LOWORD(wParam);
    const WORD ctrlAction = HIWORD(wParam);

    if (ctrlID >= QualitiesTab::ControlIDs::MasterKey && ctrlID <= QualitiesTab::ControlIDs::Money) {
        flagsChanged(ctrlID, ctrlAction);
        parentWindow->madeChange();
    } else if (ctrlID == QualitiesTab::ControlIDs::UsesBox || ctrlID == QualitiesTab::ControlIDs::MoneyBox) {
        if (ctrlAction == EN_KILLFOCUS) {
            updatePropertyValue(ctrlID);
        } else if (ctrlAction == EN_CHANGE) {
            parentWindow->madeChange();
        }
    } else if (ctrlID == QualitiesTab::ControlIDs::UsedWith) {
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

void EditObjectQualitiesTab::insertData(GameObject::Builder& builder) {

    // Figure out if the object is in a fixed location or is money, and then
    // handle those special cases, otherwise, just find out what flags need
    // to be set.

    uint8_t newFlags = 0;

    if (btnFlags[6].IsWindowEnabled() &&
        btnFlags[6].GetCheck() == BST_CHECKED) {

        newFlags = GameObjectFlags1::FixedLocation;

    } else if (btnFlags[7].IsWindowEnabled() &&
               btnFlags[7].GetCheck() == BST_CHECKED) {

        newFlags = GameObjectFlags1::Money;

        if (btnFlags[1].GetCheck() == BST_CHECKED) {
            newFlags |= GameObjectFlags1::Invisible;
        }

    } else {

        for (int i = 0; i < 6; ++i) {
            if (btnFlags[i].GetCheck() == BST_CHECKED) {
                newFlags += (1 << i);
            }
        }

    }

    builder.flags1(newFlags);
    builder.monetaryWorth(spnProperties[0].GetPos());
    builder.uses(spnProperties[1].GetPos());

    const int currentID = builder.getID();
    size_t usedWithIndex = cbxUsedWith.GetCurSel();


    if (usedWithIndex == 0) {
        // Nothing selected
        builder.usedWithID(0);
    } else if (currentID == GameObjectConstants::NoID) {
        // Selected something, but it's also a new object
        builder.usedWithID(gameMap->objectIDFromIndex(usedWithIndex - 1));
    } else {
        // Find the proper Index

        size_t currentIndex = gameMap->objectIndexFromID(currentID);

        if (currentIndex < usedWithIndex) {
            usedWithIndex++;
        }

        builder.usedWithID(gameMap->objectIDFromIndex(usedWithIndex - 1));

    }

}

///----------------------------------------------------------------------------
/// populateFields - Take the data from the GameObject and fill in the fields
/// on the tab page.
/// @param a constant reference to GameObject object
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::populateFields(const GameObject &gameObject, const GameMap& gameMap) {

    const uint8_t flags = gameObject.getFlags1();

    for (int i = 0; i < GameObjectFlags1::NumFlags; ++i) {
        if (flags & (1 << i)) {
            btnFlags[i].SetCheck(BST_CHECKED);
            if ((flags & (1 << i)) == GameObjectFlags1::Money ||
                (flags & (1 << i)) == GameObjectFlags1::FixedLocation) {
                flagsChanged(QualitiesTab::ControlIDs::MasterKey + i, BN_CLICKED);
            }
        }
    }

    CString windowText = AtoW(std::to_string(
        gameObject.getMonetaryWorth()).c_str());

    txtProperties[0].SetWindowText(windowText);

    windowText = AtoW(std::to_string(gameObject.getUses()).c_str());
    txtProperties[1].SetWindowText(windowText);

    size_t usedWithIndex = gameMap.objectIndexFromID(gameObject.getUsedWithID());
    const size_t thisIndex = gameMap.objectIndexFromID(gameObject.getID());

    // Remove the current object from the list.

    if (gameObject.getID() != GameObjectConstants::NoID) {
        cbxUsedWith.DeleteString(thisIndex + 1);
    }

    if (usedWithIndex == ((size_t)-1)) {
        cbxUsedWith.SetCurSel(0);
    } else {

        if (thisIndex > usedWithIndex) {
            usedWithIndex++;
        }

        cbxUsedWith.SetCurSel(usedWithIndex);
    }
}

//============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// flagsChanged - Updates the check boxes when one of the flags is selected,
/// as well as toggling the two textboxes if necessary.
/// @param an integer containing the control ID of the control being processed
/// @param an integer indicating the notification code the control sent
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::flagsChanged(const WORD& ctrlID, const WORD& ctrlAction) {

    const int which = ctrlID - QualitiesTab::ControlIDs::MasterKey;
    const bool isChecked = btnFlags[which].GetCheck() == BST_CHECKED ? TRUE : FALSE;

    // If an object is at a fixed location, it can't have any other flag
    // set. You also can't sell it.
    // TODO: It also cannot be in a character's inventory.

    // If an object is Money, the only other thing it can be is invisible, it
    // also cannot be used, but does have monetary value, and it cannot be used
    // with another object.

    if (ctrlID == QualitiesTab::ControlIDs::Fixed) {

        for (int i = 0; i < 8; ++i) {
            if (i != which) {
                btnFlags[i].EnableWindow(!isChecked);
            }
        }

        txtProperties[0].EnableWindow(!isChecked);

    } 
    else if (ctrlID == QualitiesTab::ControlIDs::Money) {

        for (int i = 0; i < 8; ++i) {
            if (i != which &&
                i != QualitiesTab::ControlIDs::Invisible - QualitiesTab::ControlIDs::MasterKey) {

                btnFlags[i].EnableWindow(!isChecked);

            }
        }

        txtProperties[1].EnableWindow(!isChecked);
        cbxUsedWith.EnableWindow(!isChecked);

    }

}

///----------------------------------------------------------------------------
/// validateFields - Ensures that the data given by the user is valid, and if
/// is not, gives the user a chance to correct it.
/// @return NULL if no errors occurred, or a pointer to an input validator
/// if something was wrong
///----------------------------------------------------------------------------

InputValidator* EditObjectQualitiesTab::validateFields() {

    // if the object is not in a fixed position, we need to validate it's worth
    if (btnFlags[6].GetCheck() != BST_CHECKED) {

        if (!moneyValidator.validate()) {
            return &moneyValidator;
        }

    }

    // If the object is not money, we need to validate the number of uses
    if (btnFlags[7].GetCheck() != BST_CHECKED) {
        if (!usesValidator.validate()) {
            return &usesValidator;
        }
    }

    return NULL;

}

///----------------------------------------------------------------------------
/// updatePropertyValue - Caps the uses and monetary worth text boxes.
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::updatePropertyValue(const WORD& ctrlID) {

    const int ctrlIndex = ctrlID - QualitiesTab::ControlIDs::UsesBox;

    // If the value can't be determined, reset it to 0.
    int newValue = 0;

    try {
        newValue = std::stoi(WtoA(txtProperties[ctrlIndex].GetWindowText()).c_str());
    } catch (const std::invalid_argument&) {} catch (const std::out_of_range&) {}

    spnProperties[ctrlIndex].SetPos(newValue);

}

//=============================================================================
//
// Effects Tab
//
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message. See the Win32++ documentation
/// for more information
///----------------------------------------------------------------------------

BOOL EditObjectEffectsTab::OnCommand(WPARAM wParam, LPARAM lParam) {

    if (lParam) {

        const WORD ctrlID = LOWORD(wParam);
        const WORD notifyCode = HIWORD(wParam);

        if (ctrlID >= EffectsTab::ControlIDs::EnergyBase &&
            ctrlID <= EffectsTab::ControlIDs::TorchLifeRandom) {

            if (notifyCode == EN_KILLFOCUS) {
                updateAttributeValue(ctrlID);
                return TRUE;
            } else if (notifyCode == EN_CHANGE) {
                parentWindow->madeChange();
            }

        } else if (ctrlID >= EffectsTab::ControlIDs::EffectsConsume && ctrlID <= EffectsTab::ControlIDs::EffectsTemp) {
            if (notifyCode == BN_CLICKED) {
                parentWindow->madeChange();
            }
        } else if (ctrlID >= EffectsTab::ControlIDs::MakesSight && ctrlID <= EffectsTab::ControlIDs::MakesHearing) {
            if (notifyCode == CBN_SELCHANGE) {
                parentWindow->madeChange();
            }
        } else if (ctrlID >= EffectsTab::ControlIDs::EnergyPosBTN && ctrlID <= EffectsTab::ControlIDs::TorchNegBTN) {
            if (notifyCode == BN_CLICKED) {
                parentWindow->madeChange();
            }
        }
    }

    return FALSE;
}

///----------------------------------------------------------------------------
/// insertData - Takes the data given by the user, and inputs it into the
/// builder object.
///----------------------------------------------------------------------------

void EditObjectEffectsTab::insertData(GameObject::Builder& builder) {

    if (btnEffect[1].GetCheck() == BST_CHECKED) {
        builder.flags2(builder.getFlags2() | GameObjectFlags2::EffectsTemporary);
    }

    int baseAmount = 0;
    int randAmount = 0;

    for (int i = 0; i < 5; ++i) {

#ifdef _DEBUG
        // The data was previously validated, so unless a programming error occurred
        // this should not fail.
        try {
#endif // _DEBUG

            baseAmount = std::stoi(WtoA(txtAttribAmount[i * 2].GetWindowText()).c_str());
            randAmount = std::stoi(WtoA(txtAttribAmount[(i * 2) + 1].GetWindowText()).c_str());

#ifdef _DEBUG
        } catch (const std::invalid_argument&) {
            assert(0);
        } catch (const std::out_of_range&) {
            assert(0);
        }
#endif // _DEBUG 

        if (btnAttribPolarity[(i * 2) + 1].GetCheck() == BST_CHECKED) {
            baseAmount *= -1;
            randAmount *= -1;
        }

        builder.attributeBase(baseAmount, i);
        builder.attributeRandom(randAmount, i);

    }

    builder.makesSight(cbxSenses[0].GetCurSel());
    builder.makesHearing(cbxSenses[1].GetCurSel());

}

///----------------------------------------------------------------------------
/// populateFields - Take the data from the GameObject and fill in the fields
/// on the tab page.
/// @param a constant reference to GameObject object
///----------------------------------------------------------------------------

void EditObjectEffectsTab::populateFields(const GameObject& gameObject, const GameMap& gameMap) {

    if (gameObject.getFlags2() & GameObjectFlags2::EffectsTemporary) {
        btnEffect[1].SetCheck(BST_CHECKED);
    } else {
        btnEffect[0].SetCheck(BST_CHECKED);
    }

    for (int i = 0; i < 5; ++i) {

        int baseAmount      = gameObject.getAttributeBase(i);
        int randomAmount    = gameObject.getAttributeRandom(i);
        int polarAmount     = (baseAmount < 0 ? 1 : 0);

        btnAttribPolarity[(i * 2) + polarAmount].SetCheck(BST_CHECKED);

        CString caption = AtoW(std::to_string(abs(baseAmount)).c_str());
        txtAttribAmount[(i * 2)].SetWindowText(caption);
        caption = AtoW(std::to_string(abs(randomAmount)).c_str());
        txtAttribAmount[(i * 2) + 1].SetWindowText(caption);

    }

    cbxSenses[0].SetCurSel(gameObject.getMakesSight());
    cbxSenses[1].SetCurSel(gameObject.getMakesHearing());
}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// updateAttributeValue - Update the spinner control to match the value in
/// the textbox. Note that the spinner control will auto-cap the value if it
/// exceeds the valid range.
/// @param ID of the textbox control that is associated with the spinner
/// control to be updated
///----------------------------------------------------------------------------

void EditObjectEffectsTab::updateAttributeValue(const WORD& ctrlID) {

    const int ctrlIndex = ctrlID - EffectsTab::ControlIDs::EnergyBase;

    // If the user some how inputs an invalid number, we will reset
    // the value back to the minimum attribute value.

    int newValue = AdventureGamerConstants::MinAttributeValue;

    try {
        newValue = std::stoi(WtoA(txtAttribAmount[ctrlIndex].GetWindowText()).c_str());
    } catch (const std::invalid_argument&) {} catch (const std::out_of_range&) {}

    spnAttribAmount[ctrlIndex].SetPos(newValue);

}

//=============================================================================
//
// Locations Tab
//
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Process the WM_COMMAND message. Refer to to the Win32++
/// documentation for more details.
///----------------------------------------------------------------------------

BOOL EditObjectLocationsTab::OnCommand(WPARAM wParam, LPARAM lParam) {

    if (!lParam) {
        return FALSE;
    }

    const WORD ctrlID = LOWORD(wParam);
    const WORD ctrlAction = HIWORD(wParam);

    if (ctrlID >= LocationsTab::ControlIDs::OnGround && ctrlID <= LocationsTab::ControlIDs::OnCharacter) {
        locatedAtChanged(ctrlID, ctrlAction);
        parentWindow->madeChange();
    } else if (ctrlID == LocationsTab::ControlIDs::UnlocksDoor) {
        toggleUnlocksDoor(btnUnlocksDoor.GetCheck() == BST_CHECKED ? TRUE : FALSE);
        parentWindow->madeChange();
    } else if (ctrlID == LocationsTab::ControlIDs::XGroundText || ctrlID == LocationsTab::ControlIDs::YGroundText ||
               ctrlID == LocationsTab::ControlIDs::XDoorText || ctrlID == LocationsTab::ControlIDs::YDoorText) {
        if (ctrlAction == EN_CHANGE) {
            parentWindow->madeChange();
        }
    } else if (ctrlID == LocationsTab::ControlIDs::WhichCharacter) {
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

void EditObjectLocationsTab::insertData(GameObject::Builder& builder) {

    if (btnLocatedAt[0].GetCheck() == BST_CHECKED) {

#ifdef _DEBUG
        // The data was previously validated, so unless a programming error occurred
        // this should not fail.
        try {
#endif // _DEBUG

            int groundX = std::stoi(WtoA(txtGroundCoord[0].GetWindowText()).c_str());
            int groundY = std::stoi(WtoA(txtGroundCoord[1].GetWindowText()).c_str());
            builder.location(groundX, groundY);

#ifdef _DEBUG
        } catch (const std::invalid_argument&) {
            assert(0);
        } catch (const std::out_of_range&) {
            assert(0);
        }
#endif // _DEBUG

    } else if (btnLocatedAt[1].GetCheck() == BST_CHECKED) {

        builder.location();

    } else {

        const int charIndex = cbxWhichCharacter.GetCurSel();

        if (charIndex == 0) {
            // Default to on the ground at 0, 0
            builder.location(0, 0);
        } else {
            // Map Index to ID
            builder.location(gameMap->characterIDFromIndex(charIndex - 1));
        }
    }

    if (btnUnlocksDoor.GetCheck() == BST_CHECKED) {

        int doorX = 0;
        int doorY = 0;
#ifdef _DEBUG
        // The data was previously validated, so unless a programming error occurred
        // this should not fail.
        try {
#endif // _DEBUG

            doorX = std::stoi(WtoA(txtDoorCoord[0].GetWindowText()).c_str());
            doorY = std::stoi(WtoA(txtDoorCoord[1].GetWindowText()).c_str());

#ifdef _DEBUG
        } catch (const std::invalid_argument&) {
            assert(0);
        } catch (const std::out_of_range&) {
            assert(0);
        }
#endif // _DEBUG

        // Only set this if the value is valid
        if ((doorX > -1 && doorX <= gameMap->getWidth()) &&
            (doorY > -1 && doorY <= gameMap->getHeight())) {

            builder.flags2(builder.getFlags2() | GameObjectFlags2::Key);
            builder.doorColumn(doorX);
            builder.doorRow(doorY);

        }

    }

}

///----------------------------------------------------------------------------
/// populateFields - Take the data from the GameObject and fill in the fields
/// on the tab page.
/// @param a constant reference to GameObject object
///----------------------------------------------------------------------------

void EditObjectLocationsTab::populateFields(const GameObject& gameObject, const GameMap& gameMap) {

    const int isLocated = gameObject.getIsLocated();

    btnLocatedAt[isLocated].SetCheck(BST_CHECKED);
    locatedAtChanged(isLocated + LocationsTab::ControlIDs::OnGround, BN_CLICKED);

    txtGroundCoord[0].SetWindowText(AtoW(std::to_string(
        gameObject.getX()).c_str()));

    txtGroundCoord[1].SetWindowText(AtoW(std::to_string(
        gameObject.getY()).c_str()));

    txtDoorCoord[0].SetWindowText(AtoW(std::to_string(
        gameObject.getDoorColumn()).c_str()));

    txtDoorCoord[1].SetWindowText(AtoW(std::to_string(
        gameObject.getDoorRow()).c_str()));

    const BOOL unlocksDoor = gameObject.getFlags2() & GameObjectFlags2::Key ? TRUE : FALSE;

    toggleUnlocksDoor(unlocksDoor);
    btnUnlocksDoor.SetCheck(unlocksDoor);

    const size_t whichChar = gameMap.characterIndexFromID(gameObject.getCreatureID());

    if (whichChar == ((size_t)-1)) {
        cbxWhichCharacter.SetCurSel(0);
    } else {
        cbxWhichCharacter.SetCurSel(whichChar + 1);
    }

}

///----------------------------------------------------------------------------
/// validateFields - Ensures that the data given by the user is valid, and if
/// is not, gives the user a chance to correct it.
/// @return NULL if no errors occurred, or a pointer to an input validator
/// if something was wrong
///----------------------------------------------------------------------------

InputValidator* EditObjectLocationsTab::validateFields() {

    // TODO: Prevent validation the item is held by a character and the
    // fixed flag is set.

    if (btnLocatedAt[0].GetCheck() == BST_CHECKED) {
        for (int i = 0; i < 2; ++i) {
            if (!groundCoordValidator[i].validate()) {
                return &groundCoordValidator[i];
            }
        }
    }

    if (btnUnlocksDoor.GetCheck() == BST_CHECKED) {
        for (int i = 0; i < 2; ++i) {
            if (!doorCoordVaildator[i].validate()) {
                return &doorCoordVaildator[i];
            }
        }
    }

    return NULL;
}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// locatedAtChanged - Change what is able to be set after the location of the
/// object has been changed.
/// @param an integer referring to the control ID of the option that was chosen.
/// @param an integer that indicated the control's notification code
///----------------------------------------------------------------------------

void EditObjectLocationsTab::locatedAtChanged(const WORD& ctrlID, const WORD& ctrlAction) {

    const int which = ctrlID - LocationsTab::ControlIDs::OnGround;
    const bool isChecked = btnLocatedAt[which].GetCheck() == BST_CHECKED ? true : false;

    if (ctrlID == LocationsTab::ControlIDs::OnGround && isChecked) {
        txtGroundCoord[0].EnableWindow(TRUE);
        txtGroundCoord[1].EnableWindow(TRUE);
    } 
    else {
        txtGroundCoord[0].EnableWindow(FALSE);
        txtGroundCoord[1].EnableWindow(FALSE);
    }

    if (ctrlID == LocationsTab::ControlIDs::OnCharacter && isChecked) {
        cbxWhichCharacter.EnableWindow(TRUE);
    } 
    else {
        cbxWhichCharacter.EnableWindow(FALSE);
    }

}

///----------------------------------------------------------------------------
/// toggleUnlockDoor - Change whether the unlock door textboxes are enabled
/// or not based on the state of the unlocks door checkbox
/// @param a bool indicating whether or not the object unlocks a door.
///----------------------------------------------------------------------------

void EditObjectLocationsTab::toggleUnlocksDoor(const BOOL& doesUnlock) {

    txtDoorCoord[0].EnableWindow(doesUnlock);
    txtDoorCoord[1].EnableWindow(doesUnlock);

}

///----------------------------------------------------------------------------
/// canBeHeldByEntities - Checks to see if the object can be held by an
/// entity. TODO: This may be incomplete, if the Fixed Location flag is set
/// this shouldn't be possible to set.
///----------------------------------------------------------------------------

void EditObjectLocationsTab::canBeHeldByEntities(const BOOL canHold) {

    btnLocatedAt[1].EnableWindow(canHold);
    btnLocatedAt[2].EnableWindow(canHold);

    if (!canHold) {
        locatedAtChanged(LocationsTab::ControlIDs::OnGround, BN_CLICKED);
        btnLocatedAt[0].SetCheck(BST_CHECKED);
        btnLocatedAt[1].SetCheck(BST_UNCHECKED);
        btnLocatedAt[2].SetCheck(BST_UNCHECKED);
    }
    else {
        if(btnLocatedAt[1].GetCheck() == BST_CHECKED) {
            cbxWhichCharacter.EnableWindow(true);
        }
    }

}