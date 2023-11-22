#include "gameworld_controller.h"
#include <fstream>
#include <stdexcept>
#include "../compat/std_extras_compat.h"
#include "../util/languagemapper.h"

//=============================================================================
// Constructors / Destructor
//=============================================================================

GameWorldController::GameWorldController(MainWindowInterface* inMainWindow) : mainWindow (inMainWindow) {
    gameMap = new GameMap(EditorConstants::DefaultRows, EditorConstants::DefaultCols);
    worldFilePath = "";
    worldFileName = "";
    drawingTileIndex = 0;
    selectedTileIndex = 0;
    selectedRow = 0;
    selectedCol = 0;
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
            return tryReplaceObject(objectBuilder, false);

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

    objectBuilder.ID(gameMap->getFirstUnusedObjectID());

    try {
        gameMap->addObject(gmKey, objectBuilder.build());
    } 
    catch (const std::bad_alloc&) {

        mainWindow->displayErrorMessage(langMap.get("ErrAddObjOutOfMemoryText"),
                                        langMap.get("ErrAddObjOutOfMemoryTitle"));
        return false;

    }

    mainWindow->onGameObjectsChanged();

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

    if (index == (size_t)-1) {

        mainWindow->displayErrorMessage(langMap.get("ErrReplaceObjIDNotFoundText"),
                                        langMap.get("ErrReplaceObjIDNotFoundTitle"));
        return false;

    }
    else {
        gameMap->replaceObject(gmKey, index, objectBuilder.build());
    }
    
    if (shouldNotify) {
        mainWindow->onGameObjectsChanged();
    }

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

        gameMap->deleteObject(gmKey, objectIndex);

        mainWindow->onGameObjectsChanged();

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

    characterBuilder.ID(gameMap->getFirstUnusedCharacterID());

    try {
        gameMap->addCharacter(gmKey, characterBuilder.build());
    }
    catch (const std::bad_alloc&) {

        mainWindow->displayErrorMessage(langMap.get("ErrAddCharOutOfMemoryText"),
                                        langMap.get("ErrAdaCharOutOfMemoryTitle"));
        return false;

    }

    mainWindow->onGameCharactersChanged();

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
        gameMap->replaceCharacter(gmKey, index, characterBuilder.build());
    }

    if (shouldNotify) {
        mainWindow->onGameCharactersChanged();
    }

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

        gameMap->deleteCharacter(gmKey, charIndex);
        mainWindow->onGameCharactersChanged();
        return true;
    }

    return false;

}



//-----------------------------------------------------------------------------
// Tiles
//-----------------------------------------------------------------------------

///----------------------------------------------------------------------------
/// tryEditTileDescription - Attempts to edit a tile's name and description.
/// It does this by requesting the main window open a dialog box. By default
/// it assumes the selected tile is the target.
/// @param row of the tile to be edited. TODO: NOT IMPLEMENTED.
/// @param col of the tile to be edited. TODO: NOT IMPLEMENTED.
/// @return true if the dialog was started. false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryEditTileDescription(const int& row, const int& col) {
    
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

    LanguageMapper& langMap = LanguageMapper::getInstance();

    if(!mainWindow->canCreateDialog(EditorDialogTypes::EditTileDescription)) {
        mainWindow->displayErrorMessage(langMap.get("ErrCreatingDialogText"),
                                        langMap.get("ErrCreatingDialogTItle"));
        return false;
    }

    const GameTile& gameTile = gameMap->getTile(index);

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

bool GameWorldController::tryUpdateTileDescription(const std::string& inName, 
const std::string& inDescription, const int& row, const int& col) {
    
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

    // TODO: For now, we will assume the selected tile was the target.
    // In the future, we will only send this message if the selected tile is
    // changing.
    
    gameMap->updateTileDescription(gmKey, selectedTileIndex, inName, inDescription);

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
/// @param Story text to use for the story
/// @param Summary text to use for the summary
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryUpdateStoryAndSummary(const std::string& inStory, const std::string& inSummary) {

    gameMap->setStory(gmKey, inStory);
    gameMap->setSummary(gmKey, inSummary);

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
    mainWindow->onWorldInfoUpdated(newInfo);
    return true;
}

//-----------------------------------------------------------------------------
// Code that is being rewritten or cleaned still
//-----------------------------------------------------------------------------

///-----------------------------------------------------------------------------
/// loadWorld - Attempt to load a new Adventure Gamer World. If it cannot load
/// the file, it will avoid erasing the currently loaded world.
/// @param path to the Adventure Gamer World File
/// @param the name of the Adventure Gamer File
/// @return true if the operation completed successfully, false if it could not
///-----------------------------------------------------------------------------

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
    tryUpdateSelectedTile(0);

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

    tryUpdateSelectedTile(0);
    drawingTileIndex = 0;

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

    return true;
}

///----------------------------------------------------------------------------
/// tryChangeSelectedTile - Attempts to change the type of the tile selected
/// to the one the user is drawing with.
/// @returns true if the tile was changed successfully, false if it was not
///----------------------------------------------------------------------------

bool GameWorldController::tryChangeSelectedTile() {

    const GameTile& gameTile = gameMap->getTile(selectedTileIndex);

    // TODO: A way to properly preserve the darkness flag

    if (!tryUpdateConnectedTile(gameTile)) {
        return false;
    }

    // Get the information we need to update the tile with.

    const uint8_t roadType  = drawingTileIndex & 0xF;
    const uint8_t isDirt    = drawingTileIndex > 15 ? TileModifiers::DirtRoad :
                                                      TileModifiers::None;

    GameTile::Builder newTile(gameTile);

    uint8_t newSprite = roadType;
    
    newSprite += isDirt ? 128 : 0;
    newTile.flags(gameTile.getFlags() & ~(TileFlags::Dark)); // Clear darkness.
    newTile.sprite(GameTile::Builder::calculateSprite(roadType, isDirt));
    

    gameMap->updateTile(gmKey, selectedTileIndex, newTile.build());

    //mainWindow->onTileUpdated(selectedTileIndex);

    return true;

}

///----------------------------------------------------------------------------
/// tryGetTileCopy - tries to get a copy of the tile at the specified row/col.
/// @param row of the tile get
/// @param column of the tile to get
/// @returns true if it was able to get a copy, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryGetTileCopy(const int& row, const int& col, GameTile& outTile) const {

    if(gameMap->isRowColInMapBounds(row, col)) {
        const unsigned int index = gameMap->indexFromRowCol(row, col);
        GameTile::Builder builder(gameMap->getTile(index));
        outTile = builder.build();
        return true;
    }
    else {
        mainWindow->displayErrorMessage(LanguageMapper::getInstance().get("FileMenu"),
                                        LanguageMapper::getInstance().get("FileMenu"));
    }

    return false;
}

///----------------------------------------------------------------------------
/// tryAddFeatureToTile - Attempts to Add a feature to a tile.
/// @param What feature to try and set to the tile
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryAddFeatureToTile(const int& modType) {
    
    const GameTile& gameTile    = gameMap->getTile(selectedTileIndex);
    GameTile::Builder newTile(gameTile);

    // We'll have to calculate what the new combined sprite will be with the
    // feature set.

    const uint8_t spriteIndex = gameTile.getSpriteIndex();
    const uint8_t modifiers = modType | (gameTile.isDirtRoad() ?
                              TileModifiers::DirtRoad : TileModifiers::None);

    // TODO: Preserve darkness in some cases. The only time is should be cleared
    // is when a new feature is a gate.

    newTile.sprite(GameTile::Builder::calculateSprite(spriteIndex, modifiers));
    newTile.flags(gameTile.getFlags() & ~(TileFlags::Dark)); // Clear darkness.


    // And we can only continue if both the modifier is valid, and if there is
    // a tile connected to this one, that it can be updated too.

    if (!newTile.isModiferValid() || tryUpdateConnectedTile(gameTile) == false) {
        return false;
    }

    gameMap->updateTile(gmKey, selectedTileIndex, newTile.build());

    return true;

}

///----------------------------------------------------------------------------
/// tryUpdateSelectedTileIndex - Attempts to change which tile is selected. It
/// also caches the row and column values as well.
/// @param new index of the tile to be selected
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryUpdateSelectedTile(const int& newIndex) {

    if (!gameMap->isIndexInMapBounds(newIndex)) {
        return false;
    }

    selectedTileIndex = newIndex;
    gameMap->rowColFromIndex(selectedRow, selectedCol, selectedTileIndex);
    mainWindow->onSelectedTileChanged(selectedRow, selectedCol);

    return true;
}

///----------------------------------------------------------------------------
/// tryUpdateSelectedTileIndex - Attempts to change which tile is selected. It
/// also caches the row and column values as well.
/// @param new row of the tile being selected
/// @param new col of the tile being selected
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryUpdateSelectedTile(const int& newRow, const int& newCol) {

    if(!gameMap->isRowColInMapBounds(newRow, newCol)) {
        return false;
    }

    selectedRow = newRow;
    selectedCol = newCol;
    selectedTileIndex = gameMap->indexFromRowCol(newRow, newCol);
    mainWindow->onSelectedTileChanged(selectedRow, selectedCol);

    return true;
}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// showErrorMessage - Tells the main window to display the error message
/// with the parameters given. This is for more simplistic messages.
/// @param a string with the Language ID for the body of the text
/// @param a string with the Language ID for the title of the message
///----------------------------------------------------------------------------

void GameWorldController::showErrorMessage(const std::string& errTextID, const std::string& errTitleID) const {
    LanguageMapper& langMap = LanguageMapper::getInstance();
    mainWindow->displayErrorMessage(langMap.get(errTextID), langMap.get(errTitleID));
}

//=============================================================================
// New Functions to be moved after
//=============================================================================

///----------------------------------------------------------------------------
/// findConnectionPoint - Checks to see if the tile given has connection in
/// the Jump list or Switch list.
/// @param tile to check for.
/// @return NULL if no points are found, and a valid pointer to a SimplePoint
/// is one is found.
///----------------------------------------------------------------------------

inline const SimplePoint* GameWorldController::findConnectionPoint(const GameTile& tile) const {

    if (tile.hasJumpPad()) {
        return gameMap->findJumpPoint(selectedRow, selectedCol);
    }
    else if (tile.hasConnectionFeature()) {
        return gameMap->findSwitchPoint(selectedRow, selectedCol);
    }

    return NULL;
}

///----------------------------------------------------------------------------
/// tryUpdateConnectedTile - Checks if there is a connected tile, and if there
/// is, attempts to update that tile to remove the feature from it.
/// @param tile with the connection feature on it
/// @return true if there is nothing to remove, or if the remove was successful,
/// false if it could not be removed.
///----------------------------------------------------------------------------

bool GameWorldController::tryUpdateConnectedTile(const GameTile& firstTile) {
    
    const SimplePoint* connectedTo = findConnectionPoint(firstTile);

    if (!connectedTo) {
        return true; // Nothing to remove
    }

    // connectedTo will become invalidated, so we will copy it's data now.

    const int otherRow = connectedTo->getRow();
    const int otherCol = connectedTo->getColumn();

    const int otherTileIndex = gameMap->indexFromRowCol(otherRow, otherCol);
    const GameTile& otherTile = gameMap->getTile(otherTileIndex);
    GameTile::Builder otherTileBuilder(otherTile);


    std::string messageText = "";
    std::string messageTitle = "";

    if (mainWindow->askYesNoQuestion("Altering this tile will update the tile at X,Y. Continue?", "Remove jump pads?", true) != GenericInterfaceResponses::Yes) {
        return false; // Okay, leave it alone
    }

    bool wasSuccessful = false;

    if (firstTile.hasJumpPad()) {
        wasSuccessful = gameMap->removeJumpPoint(SimplePoint(selectedCol, selectedRow), SimplePoint(otherCol, otherRow));
    }
    else if (firstTile.hasGate()) {
        wasSuccessful = gameMap->removeSwitch(SimplePoint(selectedCol, selectedRow), SimplePoint(otherCol, otherRow));
    }
    else if (firstTile.isDark()) {
        wasSuccessful = gameMap->removeSwitch(SimplePoint(selectedCol, selectedRow), SimplePoint(otherCol, otherRow));
    }
    else if (firstTile.hasSwitch()) {

        wasSuccessful = gameMap->removeSwitch(SimplePoint(selectedCol, selectedRow), SimplePoint(otherCol, otherRow));

        if (otherTile.isDark()) {
            // Darkness is an exception to the rule, it has to be done here.
            wasSuccessful = false; // We won't need to run the other builder code now.
            otherTileBuilder.flags(otherTile.getFlags() & ~(TileFlags::Dark));
            gameMap->updateTileFlags(gmKey, otherRow, otherCol, otherTile.getFlags() & ~(TileFlags::Dark));
            gameMap->updateTile(gmKey, otherTileIndex, otherTileBuilder.build());
        }
    }

    if (wasSuccessful) {
        const uint8_t otherTileNewSprite = GameTile::Builder::calculateSprite(otherTile.getSpriteIndex(), otherTile.getSpriteModifier() & TileModifiers::DirtRoad);
        otherTileBuilder.sprite(otherTileNewSprite);
        gameMap->updateTile(gmKey, otherTileIndex, otherTileBuilder.build());
    }

    // Inform the main window this tile was updated, incase it needs to do anything
    // with it.
    //mainWindow->onTileUpdated(otherTileIndex);

    return true; // Removal was successful

}

// For now, get this working and refactor it after

bool GameWorldController::tryAddFirstJumpConnection() {

    const GameTile& currentTile = gameMap->getTile(selectedTileIndex);

    if (findConnectionPoint(currentTile) != NULL) {
        return false;
    }

    if (currentTile.isDeadend() && currentTile.hasJumpPad()) {

        firstJumpConnection = SimplePoint(selectedCol, selectedRow);
        return true;
    }

    return false;

}

bool GameWorldController::tryAddSecondJumpConnection() {

    const GameTile& currentTile = gameMap->getTile(selectedTileIndex);

    // TODO: Make sure everything is in bounds

    if (findConnectionPoint(currentTile) != NULL) {
        return false;
    }

    if (currentTile.isDeadend() && currentTile.hasJumpPad()) {
        secondJumpConnection = SimplePoint(selectedCol, selectedRow);
    }
    else {
        return false;
    }

    if (firstJumpConnection == secondJumpConnection) {
        return false; // Can't jump to itself!
    }

    gameMap->addJump(gmKey, firstJumpConnection, secondJumpConnection);

    return true;

}

bool GameWorldController::tryStartSwitchConnection() {

    const GameTile& currentTile = gameMap->getTile(selectedTileIndex);

    if (findConnectionPoint(currentTile) != NULL) {
        return false;
    }

    if (currentTile.isCorner() && currentTile.hasSwitch()) {

        firstSwitchConnection = SimplePoint(selectedCol, selectedRow);
        return true;
    }

    return false;
}

bool GameWorldController::tryEndSwitchConnection() {
    
    const GameTile& currentTile = gameMap->getTile(selectedTileIndex);

    if (findConnectionPoint(currentTile) != NULL) {
        return false;
    }

    // TODO: Dark spaces

    if ((currentTile.isStraightaway() && currentTile.hasGate()) || currentTile.isDark()) {
        secondSwitchConnection = SimplePoint(selectedCol, selectedRow);
    }
    else {
        return false;
    }

    if (firstSwitchConnection == secondSwitchConnection) {
        return false; // Can't connect a switch to itself
    }

    const int firstIndex = gameMap->indexFromRowCol(firstSwitchConnection.getRow(), firstSwitchConnection.getColumn());
    const GameTile& firstTile = gameMap->getTile(firstIndex);

    if (!firstTile.hasSwitch()) {
        // TODO: Move this to the tile change code instead
        return false; // The tile switched before we could complete the task. 
    }

    gameMap->addSwitch(gmKey, firstSwitchConnection, secondSwitchConnection);

    return true;

}

///----------------------------------------------------------------------------
/// tryMakeTileDark - Attempts to make a tile dark.
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryMakeTileDark() {

    const GameTile& gameTile = gameMap->getTile(selectedTileIndex);

    if (gameTile.hasGate()) {
        return false;
    }

    GameTile::Builder newTile(gameTile);

    newTile.flags(gameTile.getFlags() | TileFlags::Dark);

    gameMap->updateTile(gmKey, selectedTileIndex, newTile.build());

    return true;
}

//=============================================================================
// Private Functions
//=============================================================================

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

bool GameWorldController::validRequestedTileRowCol(const int& row, const int& col) const {

    if (gameMap->isRowColInMapBounds(row, col)) {
        return true;
    }

    LanguageMapper& langMap = LanguageMapper::getInstance();
    mainWindow->displayErrorMessage(langMap.get("ErrRowColNotInBoundsText"),
                                    langMap.get("ErrRowColNotInBoundsTitle"));

    return false;

}

bool GameWorldController::wasRowColSpecified(const int& row, const int& col) const {

    if (row != EditorConstants::IGNORE_ROW && col != EditorConstants::IGNORE_COL) {
        return true;
    }

    return false;
}