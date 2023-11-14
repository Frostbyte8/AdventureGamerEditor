#include "editcharacter_tabviews.h"
#include "../../model/gamecharacter.h"

#include "../shared_functions.h"

using namespace EditCharacterDialogConstants::MiscTab;

//=============================================================================
// Win32++ Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Processes the WM_COMMAND message. See the Win32++ documentation
/// for more information
///----------------------------------------------------------------------------

BOOL EditCharacterMiscTab::OnCommand(WPARAM wParam, LPARAM lParam) {

    if(lParam) {

        const WORD ctrlID = LOWORD(wParam);
        const WORD notifyCode = HIWORD(wParam);

        if(ctrlID >= ControlIDs::XTextCoord && ctrlID <= ControlIDs::YTextCoord) {
            if(notifyCode == EN_CHANGE) {
                parentWindow->madeChange();
            }
        }
    }
    else {
        return FALSE;
    }

    return TRUE;
}

//=============================================================================
// Public Functions
//=============================================================================

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
        groundX = std::stoi(WtoA(txtCoords[0].GetWindowText()).c_str());
        groundY = std::stoi(WtoA(txtCoords[1].GetWindowText()).c_str());
#ifdef _DEBUG
    }
    catch(const std::invalid_argument&) {
        assert(0);
    }
    catch(const std::out_of_range&) {
        assert(0);
    }
#endif // _DEBUG 

    builder.location(groundX, groundY);

}

///----------------------------------------------------------------------------
/// populateFields - 
///----------------------------------------------------------------------------

void EditCharacterMiscTab::populateFields(const GameCharacter& gameCharacter) {

    CString caption;
    SetWindowTextFromStr(std::to_string(gameCharacter.getX()), txtCoords[0], caption);
    SetWindowTextFromStr(std::to_string(gameCharacter.getY()), txtCoords[1], caption);

    std::vector<size_t> objectIndices = gameMap->getCharacterInventory(gameCharacter.getID());
    const std::vector<GameObject>& gameObjects = gameMap->getGameObjects();
    const size_t oiSize = objectIndices.size();

    for(size_t i = 0; i < oiSize; ++i) {
        lsbInventory.AddString(AtoW(gameObjects[objectIndices[i]].getName().c_str()));
    }

}

///----------------------------------------------------------------------------
/// validateFields - Ensures that the data given by the user is valid, and if
/// is not, gives the user a chance to correct it.
/// @return NULL if no errors occurred, or a pointer to an input validator
/// if something was wrong
///----------------------------------------------------------------------------

InputValidator* EditCharacterMiscTab::validateFields() {

    for(int i = 0; i < 2; ++i) {
        if(!coordValidator[i].validate()) {
            return &coordValidator[i];
        }
    }

    return NULL;
}
