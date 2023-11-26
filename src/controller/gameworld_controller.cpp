#include "gameworld_controller.h"
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include "../compat/std_extras_compat.h"
#include "../util/languagemapper.h"

//=============================================================================
// Constructors / Destructor
//=============================================================================

GameWorldController::GameWorldController(MainWindowInterface* inMainWindow) : mainWindow (inMainWindow),
changedSinceLastSave(false) {
    gameMap = new GameMap(EditorConstants::DefaultRows, EditorConstants::DefaultCols);
    worldFilePath = "";
    worldFileName = "";
    drawingTileIndex = 0;
    selectedTileIndex = 0;
    selectedRow = 0;
    selectedCol = 0;
    firstJumpConnection = SimplePoint(-1, -1);
    secondJumpConnection = SimplePoint(-1, -1);
    firstSwitchConnection = SimplePoint(-1, -1);
    secondJumpConnection = SimplePoint(-1, -1);
}

GameWorldController::~GameWorldController() {
	if(gameMap) {
		delete gameMap;
		gameMap = NULL;
	}
}

//=============================================================================
// Accessors
//=============================================================================

///----------------------------------------------------------------------------
/// hasUnsavedChanges
/// @returns true if there are unsaved changes, and false if there is not.
///----------------------------------------------------------------------------

bool GameWorldController::hasUnsavedChanges() const {
    return changedSinceLastSave;
}

///----------------------------------------------------------------------------
/// hasFirstJumpConnectionBeenSet
/// @returns true if the first connection was set, false if it was not
///----------------------------------------------------------------------------

bool GameWorldController::hasFirstJumpConnectionBeenSet() const {
    
    if(firstJumpConnection.getX() != -1) {
        return true;
    }

    return false;
}

///----------------------------------------------------------------------------
/// hasFirstSwitchConnectionBeenSet
/// @returns true if the first connection was set, false if it was not
///----------------------------------------------------------------------------

bool GameWorldController::hasFirstSwitchConnectionBeenSet() const {

    if (firstSwitchConnection.getX() != -1) {
        return true;
    }

    return false;
}


//=============================================================================
// Public Functions
//=============================================================================

//-----------------------------------------------------------------------------
// Objects and Characters
//-----------------------------------------------------------------------------

///----------------------------------------------------------------------------
/// canAddObject - Checks to see if the limit on objects has been reached
/// @returns true if an object can still be added, false if it cannot.
///----------------------------------------------------------------------------

bool GameWorldController::canAddObject() const {

    if(gameMap->getGameObjects().size() < AdventureGamerConstants::MaxNumObjects) {
        return true;
    }
    
    return false;
    
}

///----------------------------------------------------------------------------
/// canAddCharacter - Checks to see if the limit on characters has been reached
/// @returns true if a character can still be added, false if it cannot.
///----------------------------------------------------------------------------

bool GameWorldController::canAddCharacter() const {

    if(gameMap->getGameCharacters().size() < AdventureGamerConstants::MaxNumCharacters) {
        return true;
    }
    
    return false;
}

///----------------------------------------------------------------------------
/// tryAlterObject - Alters an object. How it does that depends on the
/// alter type.
/// @param Add and Edit open a Dialog box to alter the object. Place updates
/// where an existing object is, and delete removes it.
/// do not have dialog boxes.
/// @param index of the object being altered. Ignored if alter type is Add.
/// @return true if the window opened the dialog box for the object to be
/// edited. If false, the operation failed.
///----------------------------------------------------------------------------

bool GameWorldController::tryAlterObject(const int& alterType, const int& index) {

    assert(alterType >= AlterType::Add && alterType <= AlterType::Delete);

    const std::vector<GameObject>& gameObjects = gameMap->getGameObjects();
    LanguageMapper& langMap = LanguageMapper::getInstance();

    if (alterType == AlterType::Edit || alterType == AlterType::Place || alterType == AlterType::Delete) {

        // Verify the Index

        if(!vecIndexInRange(gameObjects, index)) {
            mainWindow->displayErrorMessage(langMap.get("ErrInvalidObjIndexText"),
                                            langMap.get("ErrInvalidObjIndexTitle"));
            return false;
        }

        // Place and delete do not need a dialog (at least at this time), so deal with them here.

        if(alterType == AlterType::Place) {

            GameObject::Builder objectBuilder = GameObject::Builder(gameObjects[index]);
            objectBuilder.location(selectedCol, selectedRow);
            return tryReplaceObject(objectBuilder);

        }
        else if(alterType == AlterType::Delete) {
            return tryDeleteObject(gameObjects[index].getID());
        }

    }
    else {

        // Verify if we can add an object

        if (!canAddObject()) {
            mainWindow->displayErrorMessage(langMap.get("ErrObjLimitReachedText"),
                                            langMap.get("ErrObjLimitReachedTitle"));
            return false;
        }

    }

    // If we need to open a dialog box, we can now do that here.

    if (!mainWindow->canCreateDialog(EditorDialogTypes::AlterObject)) {

        mainWindow->displayErrorMessage(langMap.get("ErrCreatingDialogText"),
                                        langMap.get("ErrCreatingDialogTItle"));
        return false;
    }

    bool wasDialogCreated = false;

    if (alterType == AlterType::Add) {
        GameObject::Builder objectBuilder = GameObject::Builder();
        wasDialogCreated = mainWindow->startEditObjectDialog(objectBuilder, false);
    } 
    else if (alterType == AlterType::Edit) {
        // Obtain the Object we will be editing
        GameObject::Builder objectBuilder = GameObject::Builder(gameObjects[index]);
        wasDialogCreated = mainWindow->startEditObjectDialog(objectBuilder, true);
    }

    if (!wasDialogCreated) {
        mainWindow->onDialogEnd(EditorDialogTypes::AlterObject);
        mainWindow->displayErrorMessage(langMap.get("ErrCreatingDialogText"),
                                        langMap.get("ErrCreatingDialogTitle"));
        return false;
    }

    return true;

}

///----------------------------------------------------------------------------
/// tryAddObject - Attempt to add a new object to the game world.
/// @param a reference to a GameObject::Builder containing the object that will
/// be verified and added to the world.
/// @return true if it was able to add the object, false it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryAddObject(GameObject::Builder& objectBuilder) {

    LanguageMapper& langMap = LanguageMapper::getInstance();

    sanitizeObjectStrings(objectBuilder);
    objectBuilder.ID(gameMap->getFirstUnusedObjectID());
    
    try {
        
        gameMap->addObject(gmKey, objectBuilder.build());
    } 
    catch (const std::bad_alloc&) {

        mainWindow->displayErrorMessage(langMap.get("ErrAddObjOutOfMemoryText"),
                                        langMap.get("ErrAddObjOutOfMemoryTitle"));
        return false;

    }

    const bool updateHereList = (objectBuilder.getCol() == selectedCol && 
                                 objectBuilder.getRow() == selectedRow) ? true : false;

    changedSinceLastSave = true;
    mainWindow->onEntitiesChanged(true, updateHereList, false, false);

    return true;

}

///----------------------------------------------------------------------------
/// tryReplaceObject - Attempt to replace an object that already exists in the
/// game world.
/// @param Object Builder containing the data to replace the object with.
/// @param if true, send the notification to the main window that the object list
/// has been updated, if false, don't send it. Default is true.
/// @return true if the Object was replaced successfully, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryReplaceObject(GameObject::Builder& objectBuilder, const bool shouldNotify) {

    LanguageMapper& langMap = LanguageMapper::getInstance();

    if (objectBuilder.getID() == GameObjectConstants::NoID) {

        mainWindow->displayErrorMessage(langMap.get("ErrObjectNoIDText"),
                                        langMap.get("ErrObjectNoIDTitle"));
        return false;

    }

    // Located the object we are replacing
    const size_t index = gameMap->objectIndexFromID(objectBuilder.getID());
    bool updateHereList = false;

    if (index == (size_t)-1) {

        mainWindow->displayErrorMessage(langMap.get("ErrReplaceObjIDNotFoundText"),
                                        langMap.get("ErrReplaceObjIDNotFoundTitle"));
        return false;

    }
    else {

        const GameObject& originalObject = gameMap->getGameObjects()[index];

        if(selectedCol == objectBuilder.getCol() && selectedRow == objectBuilder.getRow()) {
            updateHereList = true;
        }
        else if(selectedCol == originalObject.getX() && selectedRow == originalObject.getY()) {
            updateHereList = true;
        }

        sanitizeObjectStrings(objectBuilder);
        gameMap->replaceObject(gmKey, index, objectBuilder.build());
    }
    
    changedSinceLastSave = true;


    // In some cases, replacing does not need the list to update IE: Moving
    // an object's position on the map.
    mainWindow->onEntitiesChanged(shouldNotify, updateHereList, false, false);
    
    return true;

}

///----------------------------------------------------------------------------
/// tryDeleteObject - Attempts to delete an object. If it finds that this
/// object is required by another object, it will ask the user if it wants
/// to update that object before continuing.
/// @param the ID (not index) of the Object to remove
/// @return true if the object was removed, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryDeleteObject(const int& objectID) {

    const size_t objectIndex = gameMap->objectIndexFromID(objectID);

    if (objectIndex != (size_t)-1) {
        const std::vector<size_t> objectIndices = gameMap->getReliantObjectsFromID(objectID);

        const GameObject originalObject = gameMap->getGameObjects()[objectIndex];

        if (!objectIndices.empty()) {

            const std::vector<GameObject>& gameObjects = gameMap->getGameObjects();
            const size_t objectIndicesSize = objectIndices.size();

            LanguageMapper& langMap = LanguageMapper::getInstance();
            std::string message = langMap.get("ReliantObjectsTextStart");
            message.append("\n");

            for (size_t i = 0; i < objectIndicesSize; ++i) {
                message.append("\n" + gameObjects[objectIndices[i]].getName());
            }

            message.append("\n\n");
            message.append(langMap.get("ReliantObjectsTextEnd"));

            std::string messageTitle = langMap.get("ReliantObjectsTitle");

            if (mainWindow->askYesNoQuestion(message, messageTitle, true) != GenericInterfaceResponses::Yes) {
                return false;
            }

            for (size_t k = 0; k < objectIndicesSize; ++k) {

                GameObject::Builder updatedObject(gameObjects[objectIndices[k]]);
                updatedObject.usedWithID(GameObjectConstants::UsedAlone);
                gameMap->replaceObject(gmKey, objectIndices[k], updatedObject.build());

            }

        }

        const bool updateHereList = (selectedCol == originalObject.getX() && 
                                     selectedRow == originalObject.getY()) ? true : false;

        gameMap->deleteObject(gmKey, objectIndex);
        changedSinceLastSave = true;

        mainWindow->onEntitiesChanged(true, updateHereList, false, false);
        return true;
    }

    return false;

}

///----------------------------------------------------------------------------
/// tryAlterCharacter - Alters an Character. How it does that depends on the
/// alter type.
/// @param Add and Edit open a Dialog box to alter the object. Place updates
/// where an existing character is, and delete removes it.
/// do not have dialog boxes.
/// @param index of the character being altered. Ignored if alter type is Add.
/// @return true if the window opened the dialog box for the character to be
/// edited. If false, the operation failed.
///----------------------------------------------------------------------------

bool GameWorldController::tryAlterCharacter(const int& alterType, const int& index) {

    assert(alterType >= AlterType::Add && alterType <= AlterType::Delete);

    const std::vector<GameCharacter>& gameCharacters = gameMap->getGameCharacters();
    LanguageMapper& langMap = LanguageMapper::getInstance();

    if (alterType == AlterType::Edit || alterType == AlterType::Place || alterType == AlterType::Delete) {

        // Verify the Index

        if (!vecIndexInRange(gameCharacters, index)) {
            mainWindow->displayErrorMessage(langMap.get("ErrInvalidCharIndexText"),
                                            langMap.get("ErrInvalidCharIndexTitle"));
            return false;
        }

        // Place and delete do not need a dialog (at least at this time), so deal with them here.

        if (alterType == AlterType::Place) {

            GameCharacter::Builder characterBuilder = GameCharacter::Builder(gameCharacters[index]);
            characterBuilder.location(selectedCol, selectedRow);
            return tryReplaceCharacter(characterBuilder, false);

        }
        else if (alterType == AlterType::Delete) {
            return tryDeleteCharacter(gameCharacters[index].getID());
        }

    }
    else {

        // Verify if we can add a character

        if (!canAddCharacter()) {
            mainWindow->displayErrorMessage(langMap.get("ErrCharLimitReachedText"),
                                            langMap.get("ErrCharLimitReachedTitle"));
            return false;
        }

    }

    // If we need to open a dialog box, we can now do that here.

    if (!mainWindow->canCreateDialog(EditorDialogTypes::AlterCharacter)) {

        mainWindow->displayErrorMessage(langMap.get("ErrCreatingDialogText"),
                                        langMap.get("ErrCreatingDialogTItle"));
        return false;
    }

    bool wasDialogCreated = false;

    if (alterType == AlterType::Add) {
        GameCharacter::Builder characterBuilder = GameCharacter::Builder();
        wasDialogCreated = mainWindow->startEditCharacterDialog(characterBuilder, false);
    }
    else if (alterType == AlterType::Edit) {
        // Obtain the Object we will be editing
        GameCharacter::Builder characterBuilder = GameCharacter::Builder(gameCharacters[index]);
        wasDialogCreated = mainWindow->startEditCharacterDialog(characterBuilder, true);
    }

    if (!wasDialogCreated) {
        mainWindow->onDialogEnd(EditorDialogTypes::AlterCharacter);
        mainWindow->displayErrorMessage(langMap.get("ErrCreatingDialogText"),
                                        langMap.get("ErrCreatingDialogTitle"));
        return false;
    }

    return true;

}

///----------------------------------------------------------------------------
/// tryAddCharacter - Attempt to add a new character to the game world.
/// @param a reference to a GameCharacter::Builder containing the character 
/// that will be verified and added to the world.
/// @return true if it was able to add the character, false it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryAddCharacter(GameCharacter::Builder& characterBuilder) {

    LanguageMapper& langMap = LanguageMapper::getInstance();

    sanitizeCharacterStrings(characterBuilder);
    characterBuilder.ID(gameMap->getFirstUnusedCharacterID());

    try {
        gameMap->addCharacter(gmKey, characterBuilder.build());
    }
    catch (const std::bad_alloc&) {

        mainWindow->displayErrorMessage(langMap.get("ErrAddCharOutOfMemoryText"),
                                        langMap.get("ErrAdaCharOutOfMemoryTitle"));
        return false;

    }

    const bool updateHereList = (characterBuilder.getCol() == selectedCol && 
                                 characterBuilder.getRow() == selectedRow) ? true : false;

    changedSinceLastSave = true;
    mainWindow->onEntitiesChanged(false, false, true, updateHereList);
    return true;

}

///----------------------------------------------------------------------------
/// tryReplaceCharacter - Attempt to replace an character that already exists
/// in the game world.
/// @param Character Builder containing the data to replace the character with.
/// @param if true, send the notification to the main window that the character
/// list has been updated, if false, don't send it. Default is true.
/// @return true if the character was replaced successfully, false if it was
/// not.
///----------------------------------------------------------------------------

bool GameWorldController::tryReplaceCharacter(GameCharacter::Builder& characterBuilder, const bool shouldNotify) {

    LanguageMapper& langMap = LanguageMapper::getInstance();

    if (characterBuilder.getID() == GameCharacterConstants::NoID) {

        mainWindow->displayErrorMessage(langMap.get("ErrCharNoIDText"),
                                        langMap.get("ErrCharNoIDTitle"));
        return false;

    }

    // Located the character we are replacing
    const size_t index = gameMap->characterIndexFromID(characterBuilder.getID());

    if (index == (size_t)-1) {

        mainWindow->displayErrorMessage(langMap.get("ErrReplaceCharIDNotFoundText"),
                                        langMap.get("ErrReplaceCharIDNotFoundTitle"));
        return false;

    }
    else {
        sanitizeCharacterStrings(characterBuilder);
        gameMap->replaceCharacter(gmKey, index, characterBuilder.build());
    }

    changedSinceLastSave = true;

    const bool updateHereList = (characterBuilder.getCol() == selectedCol &&
                                 characterBuilder.getRow() == selectedRow) ? true : false;

    mainWindow->onEntitiesChanged(false, false, shouldNotify, updateHereList);

    return true;

}

///----------------------------------------------------------------------------
/// tryDeleteCharacter - Attempts to delete a character. If it finds that this
/// character is holding an item, it will inform the user that the item will be
/// moved to 0,0 on the map before continuing.
/// @param the ID (not index) of the character to remove
/// @return true if the character was removed, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryDeleteCharacter(const int& charID) {

    const size_t charIndex = gameMap->characterIndexFromID(charID);

    if (charIndex != (size_t)-1) {

        const std::vector<size_t> objectIndices = gameMap->getCharacterInventory(charID);

        if (!objectIndices.empty()) {

            const std::vector<GameObject>& gameObjects = gameMap->getGameObjects();
            const size_t objectIndicesSize = objectIndices.size();


            LanguageMapper& langMap = LanguageMapper::getInstance();
            std::string message = langMap.get("MoveCharHeldObjectsTextStart");

            message.append("\n");

            for (size_t i = 0; i < objectIndicesSize; ++i) {
                message.append("\n" + gameObjects[objectIndices[i]].getName());
            }

            message.append("\n\n");
            message.append(langMap.get("MoveCharHeldObjectsTextEnd"));

            std::string messageTitle = langMap.get("MoveCharHeldObjectsTitle");

            if (mainWindow->askYesNoQuestion(message, messageTitle, true) != GenericInterfaceResponses::Yes) {
                return false;
            }

            for (size_t k = 0; k < objectIndicesSize; ++k) {

                GameObject::Builder movedObject(gameObjects[objectIndices[k]]);
                movedObject.location(0, 0);
                gameMap->replaceObject(gmKey, objectIndices[k], movedObject.build());

            }

        }

        const GameCharacter& originalCharacter = gameMap->getGameCharacters()[charIndex];

        const bool updateObjectsHereList = (selectedCol == 0 && selectedRow == 0) ? true : false;
        const bool updateHereList = (originalCharacter.getX() == selectedCol &&
                                     originalCharacter.getY() == selectedRow) ? true : false;

        gameMap->deleteCharacter(gmKey, charIndex);
        changedSinceLastSave = true;

        mainWindow->onEntitiesChanged(false, updateObjectsHereList, true, updateHereList);

        return true;
    }

    return false;

}

//-----------------------------------------------------------------------------
// Tiles
//-----------------------------------------------------------------------------

///----------------------------------------------------------------------------
/// tryAddFeatureToSelectedTile - Tries to add a feature to the tile that is
/// currently selected
/// @return true if the feature was added, or there was nothing to change,
/// false if the feature could not be added.
///----------------------------------------------------------------------------

bool GameWorldController::tryAddFeatureToSelectedTile(const int& featureType) {

    assert(featureType < TileModifiers::ALLMODS + 1);

    LanguageMapper& langMap = LanguageMapper::getInstance();

    // First, if the feature is already set, we won't do anything.

    const GameTile& currentTile = gameMap->getTile(selectedTileIndex);

    if(currentTile.getSpriteModifier() == featureType) {
        return true; // Nothing to do!
    }

    // Next we'll check to see if we can indeed add the specific modType to
    // the tile

    GameTile::Builder updatedTile(currentTile);
    updatedTile.sprite(updatedTile.calculateSprite(currentTile.getSpriteIndex(), featureType));

    if(!updatedTile.isModiferValid()) {
        mainWindow->displayErrorMessage(langMap.get("ErrTriedInvalidFeatureText"),
                                        langMap.get("ErrTriedInvalidFeatureTitle"));
        return false;
    }

    // Okay, so this tile is okay, now we need to check if it has any
    // connections to other tiles that may be affected by this tile being
    // changed.

    if (!findAndRemoveConnection(currentTile)) {
        return false;
    }

    // Check if this is a start feature, if so, ask the user if they want to move
    // the player start here.

    if(currentTile.isStraightaway() && featureType == TileModifiers::Start) {

        const int response = mainWindow->askYesNoQuestion(langMap.get("MovePlayerStartCoordText"), 
                                                          langMap.get("MovePlayerStartCoordTitle"), false);

        if(response == GenericInterfaceResponses::Yes) {
            gameMap->setPlayerCoordinates(selectedRow, selectedCol);
        }

    }

    // Finally, we can update the tile.

    changedSinceLastSave = true;

    gameMap->updateTile(gmKey, selectedTileIndex, updatedTile.build());
    mainWindow->onTileUpdated(selectedTileIndex, EditorTileUpdateFlags::Type);

    return true;

}

///----------------------------------------------------------------------------
/// tryToggleTileDarkness - Attempts to toggle the darkness flag of a tile.
/// @return true if the darkness was toggled successfully, false if it was not
/// or the user tried to apply darkness to a gate tile.
///----------------------------------------------------------------------------

bool GameWorldController::tryToggleTileDarkness() {

    LanguageMapper& langMap = LanguageMapper::getInstance();
    const GameTile& currentTile = gameMap->getTile(selectedTileIndex);
    
    if(currentTile.hasGate()) {
        mainWindow->displayErrorMessage(langMap.get("ErrGateCantBeDarkText"),
                                        langMap.get("ErrGateCantBeDarkTitle"));
        return false;
    }

    if (!findAndRemoveConnection(currentTile)) {
        return false;
    }


    GameTile::Builder updatedTile(currentTile);
    updatedTile.flags(currentTile.getFlags() ^ TileFlags::Dark);

    changedSinceLastSave = true;
    gameMap->updateTile(gmKey, selectedTileIndex, updatedTile.build());
    mainWindow->onTileUpdated(selectedTileIndex, EditorTileUpdateFlags::Type);

    return true;
}

///----------------------------------------------------------------------------
/// trySelectNewTile - Tries to change the tile that is selected on the game
/// map. It will notify the main window if tile has changed.
/// @param new row to be selected
/// @param new column to be selected
/// @return true if a new tile was selected, false if it does not.
/// False may indicate an error, that is up for the caller to decide.
///----------------------------------------------------------------------------

bool GameWorldController::trySelectNewTile(const int& row, const int& col) {

    if(!updateSelectionIfValid(row, col)) {
        // Could be many reasons: Clicked out side the map bounds, some other
        // function screwed up, so if an error happens, that will be the
        // the caller's problem.
        return false;
    }

    mainWindow->onSelectedTileChanged();

    return true;
}

///----------------------------------------------------------------------------
/// trySelectNewTile - Tries to change the tile that is selected on the game
/// map. It will notify the main window if tile has changed.
/// @param new tile index to be selected
/// @return true if a new tile was selected, false if it does not.
///----------------------------------------------------------------------------

bool GameWorldController::trySelectNewTile(const int& index) {

    if (updateSelectionIfValid(-1, -1, index)) {
        // Could be many reasons: Clicked out side the map bounds, some other
        // function screwed up, so if an error happens, that will be the
        // the caller's problem.
        return false;
    }

    mainWindow->onSelectedTileChanged();

    return true;
}

///----------------------------------------------------------------------------
/// trySetDrawingTile - Set which tile is being drawn with. If the index is
/// invalid, change nothing.
/// @param an integer specifying what tile to draw with. Must be between 0 and
/// 31. 0-15 For grassy tiles, 16-31 for dirt tiles.
/// @return true if the tile being drawn with has changed, false if it was not.
/// False may indicate an error, that is up for the caller to decide.
///----------------------------------------------------------------------------

bool GameWorldController::trySetDrawingTile(const int& newDrawTileIndex) {

    if(newDrawTileIndex < 0 || newDrawTileIndex > 31) {
        // Could be many reasons: Click out side the map bounds, some other
        // function screwed up, so if an error happens, that will be the
        // the caller's problem.
        return false;
    }

    drawingTileIndex = newDrawTileIndex;

    uint8_t newSpriteIndex = newDrawTileIndex;

    // Tiles 16 and up are just the dirt tiles, so since that flag is at 128
    // we can simply add 112 to reach that.

    if (newDrawTileIndex > 15) {
        newSpriteIndex += 112;
    }

    drawingTile.sprite(newSpriteIndex);
    assert(drawingTile.isModiferValid());

    mainWindow->onDrawingTileChanged();

    return true;

}

///----------------------------------------------------------------------------
/// tryDrawOnSelectedTile - Attempts to draw on the selected tile with the tile
/// selected in the road palette.
/// @return true if the tile was drawn to, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryDrawOnSelectedTile() {
    
    if(!findAndRemoveConnection(gameMap->getTile(selectedTileIndex))) {
        return false;
    }
    
    if(firstJumpConnection.getColumn() == selectedCol && 
            firstJumpConnection.getRow() == selectedRow) {

        firstJumpConnection = SimplePoint(-1, -1);
        // mainWindow->onConnectionUpdated(1);

    }
    else if(firstSwitchConnection.getColumn() == selectedCol &&
            firstSwitchConnection.getRow() == selectedRow) {

        firstSwitchConnection = SimplePoint(-1, -1);
        // mainWindow->onConnectionUpdated(2);

    }
   
    gameMap->updateTile(gmKey, selectedTileIndex, drawingTile.build());

    int updateFlags = EditorTileUpdateFlags::Type;

    if(!gameMap->getTile(selectedTileIndex).getDescription().empty()) {
        updateFlags |= EditorTileUpdateFlags::Description;
    }

    changedSinceLastSave = true;
    mainWindow->onTileUpdated(selectedTileIndex, updateFlags);

    return true;
}

///----------------------------------------------------------------------------
/// tryEditTileDescription - Attempts to edit a tile's name and description.
/// It does this by requesting the main window open a dialog box. By default
/// it assumes the selected tile is the target.
/// @param row of the tile to be edited. TODO: NOT IMPLEMENTED.
/// @param col of the tile to be edited. TODO: NOT IMPLEMENTED.
/// @return true if the dialog was started. false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryEditTileDescription(const int& row, const int& col) {
    
    LanguageMapper& langMap = LanguageMapper::getInstance();
    int index = 0;

    /*
    if (wasRowColSpecified(row, col)) {
        if (validRequestedTileRowCol(row, col)) {
            index = gameMap->indexFromRowCol(row, col);
        }
        else {
            return false;
        }
    }
    else {
        assert(gameMap->isIndexInMapBounds(selectedTileIndex));
        index = selectedTileIndex;
    }
    */

    // TODO: Ignore the above for now.
    assert(gameMap->isIndexInMapBounds(selectedTileIndex));
    index = selectedTileIndex;

    const GameTile& gameTile = gameMap->getTile(index);

    if (!gameTile.isAnyRoadTile() && !gameTile.isDirtRoad()) {
        mainWindow->displayErrorMessage(langMap.get("ErrGrassCantHaveDescText"),
                                        langMap.get("ErrGrassCantHaveDescTitle"));
        return false;
    }

    

    if(!mainWindow->canCreateDialog(EditorDialogTypes::EditTileDescription)) {
        mainWindow->displayErrorMessage(langMap.get("ErrCreatingDialogText"),
                                        langMap.get("ErrCreatingDialogTItle"));
        return false;
    }

    if(!mainWindow->startEditTileDescriptionDialog(gameTile.getName(), gameTile.getDescription())) {
        mainWindow->onDialogEnd(EditorDialogTypes::EditTileDescription);
        mainWindow->displayErrorMessage(langMap.get("ErrCreatingDialogText"),
                                        langMap.get("ErrCreatingDialogTitle"));
        
        return false;
    }

    return true;
}

///----------------------------------------------------------------------------
/// tryUpdateTileDescription - Attempts to update a tile description. If the
/// row and col are not both specified, the tile that is currently selected
/// is the target.
/// @param a string containing the new name for the tile.
/// @param a string containing the new description for the tile.
/// @param the row of the tile to be updated. Col must also be set or this is
/// ignored. By default this is ignored.
/// @param the col of the tile to be updated. Row must also be set or this is
/// ignored. By default, this is ignored.
/// @return true if the tile was updated, false if it was not. Note that using
/// the selected tile will always return true.
///----------------------------------------------------------------------------

bool GameWorldController::tryUpdateTileDescription(std::string tileName, 
std::string tileDescription, const int& row, const int& col) {
    
    LanguageMapper& langMap = LanguageMapper::getInstance();
    int index = 0;

    if(wasRowColSpecified(row, col)) {
        if(validRequestedTileRowCol(row, col)) {
            index = gameMap->indexFromRowCol(row, col);
        }
        else {
            return false;
        }
    }
    else {
        assert(gameMap->isIndexInMapBounds(selectedTileIndex));
        index = selectedTileIndex;
    }

    tileName.erase(std::remove(tileName.begin(), tileName.end(), '\"'), tileName.end());
    tileDescription.erase(std::remove(tileDescription.begin(), tileDescription.end(), '\"'), tileDescription.end());

    // TODO: For now, we will assume the selected tile was the target.
    // In the future, we will only send this message if the selected tile is
    // changing.
    index = selectedTileIndex;
    
    gameMap->updateTileDescription(gmKey, index, tileName, tileDescription);
    changedSinceLastSave = true;

    mainWindow->onTileUpdated(index, EditorTileUpdateFlags::Description);

    return true;
}

//-----------------------------------------------------------------------------
// Story/Summary, World Properties, and Resize
//-----------------------------------------------------------------------------

///----------------------------------------------------------------------------
/// tryEditSummaryAndStory - Attempts to edit the story and summary of the
/// game world. It does this by requesting the main window open a dialog box.
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryEditSummaryAndStory() {

    LanguageMapper& langMap = LanguageMapper::getInstance();

    if (!mainWindow->canCreateDialog(EditorDialogTypes::EditStoryAndSummary)) {
        mainWindow->displayErrorMessage(langMap.get("ErrCreatingDialogText"),
                                        langMap.get("ErrCreatingDialogTItle"));
        return false;
    }

    if (!mainWindow->startEditStoryAndSummaryDialog(gameMap->getStory(), gameMap->getSummary())) {
        mainWindow->onDialogEnd(EditorDialogTypes::EditStoryAndSummary);
        mainWindow->displayErrorMessage(langMap.get("ErrCreatingDialogText"),
                                        langMap.get("ErrCreatingDialogTitle"));

        return false;
    }

    return true;

}

///----------------------------------------------------------------------------
/// tryUpdateStoryAndSummary - Attempts to update the story/summary, and then
/// notifies the window that a change occurred. 
/// @param A string of the new Story text to use for the story
/// @param A string of the new Summary text to use for the summary
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryUpdateStoryAndSummary(std::string inStory, std::string inSummary) {

    inStory.erase(std::remove(inStory.begin(), inStory.end(), '\"'), inStory.end());
    inSummary.erase(std::remove(inSummary.begin(), inSummary.end(), '\"'), inSummary.end());

    gameMap->setStory(gmKey, inStory);
    gameMap->setSummary(gmKey, inSummary);

    changedSinceLastSave = true;
    mainWindow->onStoryAndSummaryUpdated();

    return true;

}

///----------------------------------------------------------------------------
/// tryEditWorldInfo - Attempts to edit the properties of the game world. It
/// does this by requesting the main window open a dialog box.
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryEditWorldInfo() {

    LanguageMapper& langMap = LanguageMapper::getInstance();

    if (!mainWindow->canCreateDialog(EditorDialogTypes::EditWorldInfo)) {
        mainWindow->displayErrorMessage(langMap.get("ErrCreatingDialogText"),
                                        langMap.get("ErrCreatingDialogTItle"));
        return false;
    }

    if (!mainWindow->startEditWorldInfoDialog(gameMap->getGameInfo())) {
        mainWindow->onDialogEnd(EditorDialogTypes::EditWorldInfo);
        mainWindow->displayErrorMessage(langMap.get("ErrCreatingDialogText"),
                                        langMap.get("ErrCreatingDialogTitle"));

        return false;
    }

    return true;

}

///----------------------------------------------------------------------------
/// tryUpdateWorldInfo - Attempts to update the game info for the world.
/// @param A reference to a GameInfo object that contains the new info
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryUpdateWorldInfo(const GameInfo& newInfo) {
    gameMap->updateGameInfo(gmKey, newInfo);
    changedSinceLastSave = true;
    mainWindow->onWorldInfoUpdated();
    return true;
}

///----------------------------------------------------------------------------
/// tryEditWorldSize - Attempts to edit the size of the game world. It does
/// this by requesting the main window open a dialog box.
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryEditWorldSize() {

    LanguageMapper& langMap = LanguageMapper::getInstance();

    if (!mainWindow->canCreateDialog(EditorDialogTypes::ResizeWorld)) {
        mainWindow->displayErrorMessage(langMap.get("ErrCreatingDialogText"),
                                        langMap.get("ErrCreatingDialogTItle"));
        return false;
    }

    // TODO: Change this from getHeight to getRows.

    if (!mainWindow->startResizeWorldDialog(gameMap->getHeight(), gameMap->getWidth())) {
        mainWindow->onDialogEnd(EditorDialogTypes::ResizeWorld);
        mainWindow->displayErrorMessage(langMap.get("ErrCreatingDialogText"),
                                        langMap.get("ErrCreatingDialogTitle"));

        return false;
    }

    return true;

}

///----------------------------------------------------------------------------
/// tryResizeWorld - Attempts to resize the game world.
/// @param the new number of rows the game world will have.
/// @param the new number of columns the game world will have,.
/// @return true if the operation was successful and the world size has 
/// changed. False is it failed and the size did not change.
///----------------------------------------------------------------------------

bool GameWorldController::tryResizeWorld(const int& numRows, const int& numCols) {

    LanguageMapper& langMap = LanguageMapper::getInstance();

    // Sanitize input

    if(numCols > AdventureGamerConstants::MaxWorldWidth || 
       numRows > AdventureGamerConstants::MaxWorldHeight) {

        mainWindow->displayErrorMessage(langMap.get("ErrNewSizeTooBigText"),
                                        langMap.get("ErrNewSizeTooBigTitle"));

        return false;
    }
    
    if(numCols < AdventureGamerConstants::MinWorldWidth ||
       numRows < AdventureGamerConstants::MinWorldHeight) {
        
        mainWindow->displayErrorMessage(langMap.get("ErrNewSizeTooSmallText"),
                                        langMap.get("ErrNewSizeTooSmallTitle"));
        return false;
    }


    if(!gameMap->resizeMap(numRows, numCols)) {

        mainWindow->displayErrorMessage(langMap.get("ErrResizingMapText"),
                                        langMap.get("ErrResizingMapTitle"));
        return false;
    }

    // Move the cursor if it's out of bounds

    if(!gameMap->isIndexInMapBounds(selectedTileIndex)) {
        trySelectNewTile(0);
    }

    // Also reset the jump and switch connections if necessary

    int whichConnections = 0;

    if(firstJumpConnection.getColumn() >= numCols || 
       firstJumpConnection.getRow() >= numRows) {

        firstJumpConnection = SimplePoint(-1, -1);
        whichConnections = 1;

    }

    if(firstSwitchConnection.getColumn() >= numCols || 
       firstSwitchConnection.getRow() >= numRows) {

        firstSwitchConnection = SimplePoint(-1, -1);
        whichConnections += 2;

    }

    // mainWindow->onConnectionUpdated(whichConnections);

    changedSinceLastSave = true;
    mainWindow->onWorldResized();

    return true;
}

//-----------------------------------------------------------------------------
// Switch / Jump Pad connection
//-----------------------------------------------------------------------------

///----------------------------------------------------------------------------
/// tryCreateJumpConnection - Attempts to connect two jump pads together. The
/// second tile does not need to be a valid jump pad, only the first.
/// @return true if the operation was valid and the first point was made, or
/// true if a jump pad connection was added. False if any error occurs.
///----------------------------------------------------------------------------

bool GameWorldController::tryCreateJumpConnection() {

    LanguageMapper& langMap = LanguageMapper::getInstance();

    const bool isSecondPoint = firstJumpConnection.getX() != -1 ? true : false;
    const GameTile& currentTile = gameMap->getTile(selectedTileIndex);

    if(!currentTile.hasJumpPad()) {

        if(!isSecondPoint) {
            mainWindow->displayErrorMessage(langMap.get("ErrNoJumppadOnTileText"),
                                            langMap.get("ErrNoJumppadOnTileTitle"));
            return false;
        }
        else {
            const int response = mainWindow->askYesNoQuestion(langMap.get("SecondJumppadText"),
                                                              langMap.get("SecondJumppadTitle"),
                                                              true);
            if(response != GenericInterfaceResponses::Yes) {
                return false;
            }
        }
    }

    if(isSecondPoint) {
        if(selectedCol == firstJumpConnection.getColumn() && selectedRow == firstJumpConnection.getRow()) {
            mainWindow->displayErrorMessage(langMap.get("ErrCannotJumpToSelfText"),
                                            langMap.get("ErrCannotJumpToSelfTitle"));
            return false;
        }
    }

    const SimplePoint* alreadyExists = gameMap->findJumpPoint(selectedRow, selectedCol);

    if(alreadyExists) {
        mainWindow->displayErrorMessage(langMap.get("ErrJumpAlreadyExistsText"),
                                        langMap.get("ErrJumpAlreadyExistsTitle"));
        return false;
    }

    // Okay, no errors, so time to add the jump.

    if(!isSecondPoint) {
        firstJumpConnection = SimplePoint(selectedCol, selectedRow);
    }
    else {

        secondJumpConnection = SimplePoint(selectedCol, selectedRow);

        gameMap->addJump(gmKey, firstJumpConnection, secondJumpConnection);

        std::string messageText = langMap.get("JumppadAddedText");
        const std::string messageTitle = langMap.get("JumppadAddedTitle");
        formatConnectionString(messageText, firstJumpConnection, secondJumpConnection);

        mainWindow->displayMessage(messageText, messageTitle, GenericInterfaceMessageTypes::Information);

        firstJumpConnection = SimplePoint(-1, -1);
        secondJumpConnection = SimplePoint(-1, -1);        

    }

    // mainWindow->onConnectionUpdated(1);
   
    return true;
}

///----------------------------------------------------------------------------
/// tryCreateSwitchConnection - Attempts to connect a Switch to a tile.
/// 
///----------------------------------------------------------------------------

bool GameWorldController::tryCreateSwitchConnection() {

    LanguageMapper& langMap = LanguageMapper::getInstance();

    // Unlike Jumps, switches need to carefully added as they can crash the
    // game if they are invalid.

    const bool isSecondPoint = firstSwitchConnection.getX() != -1 ? true : false;
    const GameTile& currentTile = gameMap->getTile(selectedTileIndex);

    if(!isSecondPoint) {

        if(!currentTile.hasSwitch()) {
            mainWindow->displayErrorMessage(langMap.get("ErrNoSwitchOnTileText"),
                                            langMap.get("ErrNoSwitchOnTileTitle"));
            return false;
        }

    }
    else {

        if(!currentTile.hasGate() && !currentTile.isDark()) {
            mainWindow->displayErrorMessage(langMap.get("ErrMustBeDarkOrGateText"),
                                            langMap.get("ErrMustBeDarkOrGateTitle"));
            return false;
        }

    }

    const SimplePoint* alreadyExists = gameMap->findSwitchPoint(selectedRow, selectedCol);

    if (alreadyExists) {
        mainWindow->displayErrorMessage(langMap.get("ErrSwitchAlreadyExistsText"),
                                        langMap.get("ErrSwitchAlreadyExistsTitle"));
        return false;
    }

    if (!isSecondPoint) {
        firstSwitchConnection = SimplePoint(selectedCol, selectedRow);
    }
    else {

        secondSwitchConnection = SimplePoint(selectedCol, selectedRow);

        gameMap->addSwitch(gmKey, firstSwitchConnection, secondSwitchConnection);

        std::string messageText = langMap.get("SwitchAddedText");
        const std::string messageTitle = langMap.get("SwitchAddedTitle");
        formatConnectionString(messageText, firstSwitchConnection, secondSwitchConnection);

        mainWindow->displayMessage(messageText, messageTitle, GenericInterfaceMessageTypes::Information);

        firstSwitchConnection = SimplePoint(-1, -1);
        secondSwitchConnection = SimplePoint(-1, -1);

    }


    // mainWindow->onConnectionUpdated(2);

    return true;
}

//-----------------------------------------------------------------------------
// Code that is being rewritten or cleaned still
//-----------------------------------------------------------------------------

///----------------------------------------------------------------------------
/// loadWorld - Attempt to load a new Adventure Gamer World. If it cannot load
/// the file, it will avoid erasing the currently loaded world.
/// @param path to the Adventure Gamer World File
/// @param the name of the Adventure Gamer File
/// @return true if the operation completed successfully, false if it could not
///----------------------------------------------------------------------------

bool GameWorldController::loadWorld(const std::string& filePath,
                                    const std::string& fileName) {
    
    std::string fileNameTemp = filePath + fileName;
	std::ifstream ifs;
	ifs.open(fileNameTemp.c_str(), std::ifstream::in | std::ios::binary);
    
    GameMap* newMap = NULL;

    bool loadSuccessful = false;

    if(ifs) {

        try {

			GameMap* newMap = new GameMap();
            newMap->readMap(ifs, filePath, fileName);
			
			if(gameMap) {
				delete gameMap;
				gameMap = NULL;
			}

            gameMap = newMap;
            newMap = NULL;

            loadSuccessful = true;

        }
        catch (const std::runtime_error& e) {

            std::string fileReadError = LanguageMapper::getInstance().get("FileMenu") + " ";
            fileReadError.append(e.what());

            mainWindow->displayErrorMessage(fileReadError.c_str(), LanguageMapper::getInstance().get("FileMenu"));
        }
    }
    else {

        mainWindow->displayErrorMessage("Unable to open file.", "File not found");

    }

    if(!loadSuccessful) {
        if(newMap) {
            delete newMap;
            newMap = NULL;
        }
    }

    worldFilePath = filePath;
    worldFileName = fileName;

    drawingTileIndex = 0;
    trySelectNewTile(0);
    changedSinceLastSave = false;

    mainWindow->onEntitiesChanged(true, true, true, true);

    mainWindow->onWorldResized();
    mainWindow->onWorldInfoUpdated();

    return loadSuccessful;
}

///----------------------------------------------------------------------------
/// newWorld - Attempt to create a new Adventure Gamer World. If it cannot, it
/// will avoid erasing the currently loaded world.
/// @return true if the operation completed successfully, false if it could not
///----------------------------------------------------------------------------

bool GameWorldController::newWorld() {

    GameMap* newMap = NULL;

    bool wasWorldCreated = false;

    try {
		
        newMap = new GameMap(EditorConstants::DefaultRows, EditorConstants::DefaultCols);		

		if(gameMap) {
			delete gameMap;
			gameMap = NULL;
		}

        gameMap = newMap;
        newMap = NULL;
        wasWorldCreated = true;
    }
    catch (const std::runtime_error& e) {
        std::string errMessage = "Unable to create a new Adventure Gamer world: ";
        errMessage.append(e.what());
        mainWindow->displayErrorMessage(errMessage, "Error creating new world.");
    }
    catch (const std::bad_alloc& e) {
        mainWindow->displayErrorMessage(e.what(), "Out of Memory?");
    }

    if(!wasWorldCreated) {
        delete newMap;
        newMap = NULL;
    }

    worldFilePath = "";
    worldFileName = "";

    trySelectNewTile(0);
    drawingTileIndex = 0;
    changedSinceLastSave = false;
    
    mainWindow->onEntitiesChanged(true, true, true, true);
    mainWindow->onWorldResized();
    mainWindow->onWorldInfoUpdated();

    return wasWorldCreated;
}

///----------------------------------------------------------------------------
/// saveWorld - Attempt to save the game world. If not, prompt the main window
/// for more information, or prompt it regardless if the user requested to
/// save the file as something else.
/// @param if true, save the file as something else, otherwise attempt to save
/// the file.
/// @return true if the operation completed successfully, false if it could not
///----------------------------------------------------------------------------

bool GameWorldController::saveWorld(bool saveAs) {

    if (saveAs || (worldFilePath.empty() || worldFileName.empty())) {
        
        std::string filePath;
        std::string fileName;

        const int retVal = mainWindow->onSaveFileDialog(filePath, fileName);

        if (retVal != GenericInterfaceResponses::Ok) {
            // Abort saving.
            return false;
        }

        worldFilePath = filePath;
        worldFileName = fileName;
        
    }

    std::ofstream ofs;
    std::string fullPathName = worldFilePath + worldFileName;
    ofs.open(fullPathName.c_str(), std::ofstream::out | std::ios::binary);

    if (ofs) {
        gameMap->writeMap(ofs, worldFilePath, worldFileName);
    }
    else {
        mainWindow->displayErrorMessage("Unable to write file.", "File Write Error");
        return false;
    }

    changedSinceLastSave = true;

    return true;
}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// askAndUpdateSisterTile - For connected tiles, prompt the user about those
/// tiles being updated, and if they say yes, update them.
/// @param the messageID from the langmap that should be displayed
/// @param the titleID from the langmap that should be displayed
/// @param the x-coordinate of the sister tile
/// @param the y-coordinate of the sister tile
/// @return true if the other tile was updated, false if it was not.
///----------------------------------------------------------------------------

inline bool GameWorldController::askAndUpdateSisterTile(const std::string& messageId, const std::string& titleID,
                                                        const int& x, const int& y) {

    LanguageMapper& langMap = LanguageMapper::getInstance();

    std::string messageText = langMap.get(messageId);
    const std::string messageTitle = langMap.get(titleID);

    formatCoordinateString(messageText, x, y);

    const int response = mainWindow->askYesNoQuestion(messageText, messageTitle, true);

    if (response != GenericInterfaceResponses::Yes) {
        return false;
    }

    gameMap->clearTileFeature(gameMap->indexFromRowCol(y, x));

    return true;

}

///----------------------------------------------------------------------------
/// findAndRemoveTile - Check if the tile has any connections to other tiles,
/// if it does, update those tiles and remove jump or switch point.
/// @param a constant reference to the game tile that may have connections
/// @return true if the other tile was updated or there was no connection,
/// false if the connection was not removed.
///----------------------------------------------------------------------------

inline bool GameWorldController::findAndRemoveConnection(const GameTile& tile) {
    
    if (tile.hasJumpPad()) {
        if (!tryRemoveSisterJumppad()) {
            return false;
        }
    }
    else if (tile.hasSwitch()) {
        if (!tryRemoveSwitchSisterTile()) {
            return false;
        }
    }
    else if (tile.isDark() || tile.hasGate()) {
        if (!tryRemoveSwitch()) {
            return false;
        }
    }

    return true;
}

///----------------------------------------------------------------------------
/// formatCoordinateString - Attempts to replace two %d's with coordinates
/// in a given string.
/// @param string to be modified
/// @param the first coordinate to insert
/// @param the second coordinate to insert
///----------------------------------------------------------------------------

inline void GameWorldController::formatCoordinateString(std::string& str, const int& coord1, const int& coord2) {
    
    std::size_t pos = str.find("%d");
    if (pos != std::string::npos) {
        std::string strVal = std::to_string(coord1);
        str.replace(pos, 2, strVal);
    }

    pos = str.find("%d");
    if (pos != std::string::npos) {
        std::string strVal = std::to_string(coord2);
        str.replace(pos, 2, strVal);
    }

}

///----------------------------------------------------------------------------
/// formatConnectionString - Attempts to replace four %d's with coordinates
/// in a given string
/// @param string to be modified
/// @param A SimplePoint containing the first set of coordinates
/// @param A SimplePoint containing  the second  set of coordinates
///----------------------------------------------------------------------------

inline void GameWorldController::formatConnectionString(std::string& str, const SimplePoint& coord1, const SimplePoint& coord2) {

    std::size_t pos = 0;
    
    for(int i = 0; i < 2; ++i) {
        pos = str.find("%d", pos);
        if (pos != std::string::npos) {
            std::string strVal = std::to_string((i == 0 ? coord1.getX() : coord1.getY()));
            str.replace(pos, 2, strVal);
        }
        else {
            return;
        }
    }

    for(int i = 0; i < 2; ++i) {
        pos = str.find("%d", pos);
        if (pos != std::string::npos) {
            std::string strVal = std::to_string((i == 0 ? coord2.getX() : coord2.getY()));
            str.replace(pos, 2, strVal);
        }
        else {
            return;
        }
    }

}

///----------------------------------------------------------------------------
/// tryRemoveSisterJumppad - Attempts to remove the Jump pad that a jump pad is
/// connected to, if one exists. It does this by updating the tile to remove
/// the jump feature and then updating the jump pad list.
/// @return true if a Jump pad was removed, or if none existed, false if it
/// could not be removed.
///----------------------------------------------------------------------------

bool GameWorldController::tryRemoveSisterJumppad() {

    // TODO: Make sure jump pads can only be connected once.
    // Possibly make a feature where jump pads can jump to any tile that does
    // not contain a jump pad. Complex, but that would be cool.

    const SimplePoint* otherJumpPad = gameMap->findJumpPoint(selectedRow, selectedCol);

    if (otherJumpPad) {

        if (!askAndUpdateSisterTile("RemoveOtherJumppadText", "RemoveOtherJumppadTitle", 
                                    otherJumpPad->getX(), otherJumpPad->getY())) {
            return false;
        }

        gameMap->removeJumpPoint(SimplePoint(selectedCol, selectedRow), *otherJumpPad);

    }

    return true; 

}

///----------------------------------------------------------------------------
/// tryRemoveSwitch - Attempts to remove a switch that is connected to another
/// tile. It does this by removing the switch feature from the the tile, and
/// then removing that switch from the switch list.
/// @return true if a switched was removed, or if none existed, false if it
/// could not be removed.
///----------------------------------------------------------------------------

bool GameWorldController::tryRemoveSwitch() {

    const SimplePoint* connectedTilePoint = gameMap->findSwitchPoint(selectedRow, selectedCol);

    if (connectedTilePoint) {

        if(!askAndUpdateSisterTile("RemoveSwitchText", "RemoveSwitchTitle",
                                   connectedTilePoint->getX(), connectedTilePoint->getY())) {
            return false;
        }

        gameMap->removeSwitch(SimplePoint(selectedCol, selectedRow), *connectedTilePoint);

    }
    
    return true;
}

///----------------------------------------------------------------------------
/// tryRemoveSwitchSisterTile - Remove the other tile that is connected to the
/// switch tile (Gate or Dark Tile).
/// @return true if a gate or dark tile was removed, or none existed, false if
/// was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryRemoveSwitchSisterTile() {

    const SimplePoint* connectedTilePoint = gameMap->findSwitchPoint(selectedRow, selectedCol);

    if (connectedTilePoint) {

        const GameTile& sisterTile = gameMap->getTile(gameMap->indexFromRowCol(connectedTilePoint->getRow(),
                                                                               connectedTilePoint->getColumn()));

        std::string title;
        std::string ID;

        if(sisterTile.hasGate()) {
            title = "RemoveGateText";
            ID = "RemoveGateTitle";
        }
        else {
            title = "RemoveDarknessText";
            ID = "RemoveDarknessTitle";
        }

        if (!askAndUpdateSisterTile(title, ID,
                                    connectedTilePoint->getX(), connectedTilePoint->getY())) {
            return false;
        }

        gameMap->removeSwitch(SimplePoint(selectedCol, selectedRow), *connectedTilePoint);

    }

    return true;
}

///----------------------------------------------------------------------------
/// sanitizeCharacterStrings - Strip out Double quote characters from any
/// strings attached the character.
/// @param A reference to a Character Builder to sanitize. This will be
/// modified.
///----------------------------------------------------------------------------

void GameWorldController::sanitizeCharacterStrings(GameCharacter::Builder& characterBuilder) {

    for(int i = 0; i < GameCharacterDescriptions::NumAllDescriptions; ++i) {
        std::string desc = characterBuilder.base.description[i];
        desc.erase(std::remove(desc.begin(), desc.end(), '\"'), desc.end());
        characterBuilder.description(desc, i);
    }

}

///----------------------------------------------------------------------------
/// sanitizeObjectStrings - Strip out Double quote characters from any
/// strings attached an object.
/// @param A reference to a Object Builder to sanitize. This will be modified.
///----------------------------------------------------------------------------

void GameWorldController::sanitizeObjectStrings(GameObject::Builder& objectBuilder) {

    for (int i = 0; i < GameObjectDescriptions::NumAllDescriptions; ++i) {
        std::string desc = objectBuilder.base.description[i];
        desc.erase(std::remove(desc.begin(), desc.end(), '\"'), desc.end());
        objectBuilder.description(desc, i);
    }

}

///----------------------------------------------------------------------------
/// validRequestedTileRowCol - Checks if the Row/Col given is valid, if not,
/// displays an error message about it.
/// @param const reference to the row to check
/// @param const reference to the column to check
/// @return true if the Row and Col are valid, false if not.
///----------------------------------------------------------------------------

bool GameWorldController::validRequestedTileRowCol(const int& row, const int& col) const {

    if (gameMap->isRowColInMapBounds(row, col)) {
        return true;
    }

    LanguageMapper& langMap = LanguageMapper::getInstance();
    mainWindow->displayErrorMessage(langMap.get("ErrRowColNotInBoundsText"),
                                    langMap.get("ErrRowColNotInBoundsTitle"));

    return false;

}

///----------------------------------------------------------------------------
/// vecIndexInRange - Tests if the index given is in range in the vector given.
/// @param Vector to test the range on
/// @param index to be checked
/// @return true if the index is in range, false if it is not.
///----------------------------------------------------------------------------

template <typename T>
bool GameWorldController::vecIndexInRange(const T& vec, const size_t& index) const {
    if (!vec.empty()) {
        if (index >= 0 && index < vec.size()) {
            return true;
        }
    }
    return false;
}

///----------------------------------------------------------------------------
/// wasRowColSpecified - Given a Row and Col, check if those arguments were
/// specified and not -1.
/// @param const reference to the row to check
/// @param const reference to the column to check
/// @return true if row and col were both specified, false if not.
///----------------------------------------------------------------------------

bool GameWorldController::wasRowColSpecified(const int& row, const int& col) const {

    if (row != EditorConstants::IGNORE_ROW && col != EditorConstants::IGNORE_COL) {
        return true;
    }

    return false;
}

///----------------------------------------------------------------------------
/// updateSelectionIfValid - Updates what tile is selected IF the selection
/// is valid. Index is used if specified. If it isn't, then it tries to use
/// the row and column specified.
/// @param row to be selected
/// @param col to be selected
/// @param the index to be selected
/// @return true if the selection is valid and updated, false if it is invalid
/// an remains unchanged
///----------------------------------------------------------------------------

bool GameWorldController::updateSelectionIfValid(const int& row, const int& col, const int& index) {

    LanguageMapper& langMap = LanguageMapper::getInstance();

    if(index >= 0) {

        // Index was specified, so we'll use that.

        if(!gameMap->isIndexInMapBounds(index)) {
            //mainWindow->displayErrorMessage(langMap.get("ErrNewTileSelectionBoundsText"),
            //                                langMap.get("ErrNewTileSelectionBoundsTitle"));
            return false;
        }

        selectedTileIndex = index;
        gameMap->rowColFromIndex(selectedRow, selectedCol, index);

    }
    else if(row >= 0 && col >= 0) {
        
        if(!gameMap->isRowColInMapBounds(row, col)) {
            //mainWindow->displayErrorMessage(langMap.get("ErrNewTileSelectionBoundsText"),
            //                                langMap.get("ErrNewTileSelectionBoundsTitle"));
            return false;
        }

        selectedTileIndex = gameMap->indexFromRowCol(row, col);
        selectedRow = row;
        selectedCol = col;

    }
    else {
        //mainWindow->displayErrorMessage(langMap.get("ErrNewTileSelectionBoundsText"),
                                        //langMap.get("ErrNewTileSelectionBoundsTitle"));
        return false;
    }

    return true;
}