#include "gameworld_controller.h"
#include <fstream>
#include <stdexcept>
#include "../compat/std_extras_compat.h"
#include "../util/languagemapper.h"

//=============================================================================
// Constructors / Destructors
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

    mainWindow->onTileUpdated(selectedTileIndex);

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
/// tryPlaceCharacterAtTile - Attempts to move a character to the given row
/// and column
/// @param integer specifying the row
/// @param integer specifying the column
/// @param the ID (not index) of the Character
/// @return true if the operation was successful, false if it was not
///----------------------------------------------------------------------------

bool GameWorldController::tryPlaceCharacterAtTile(const int& row, const int& col, const int& charID) {
    
    if(gameMap->isRowColInMapBounds(row, col)) {

        const size_t charIndex = gameMap->characterIndexFromID(charID);

        if(charIndex != (size_t)-1) {
            
            const std::vector<GameCharacter>& gameCharacters = gameMap->getGameCharacters();
            GameCharacter::Builder charBuilder(gameCharacters[charIndex]);
            charBuilder.location(col, row);
            gameMap->replaceCharacter(gmKey, charIndex, charBuilder.build());
            return true;

        }

    }

    return false;
}

///----------------------------------------------------------------------------
/// tryPlaceObjectAtTile - Attempts to move an object to the given row and
/// column
/// @param integer specifying the row
/// @param integer specifying the column
/// @param the ID (not index) of the Object to move
/// @return true if the operation was successful, false if it was not
///----------------------------------------------------------------------------

bool GameWorldController::tryPlaceObjectAtTile(const int& row, const int& col, const int& objectID) {
    
    if(gameMap->isRowColInMapBounds(row, col)) {

        const size_t objectIndex = gameMap->objectIndexFromID(objectID);

        if(objectIndex != (size_t)-1) {
           
            const std::vector<GameObject>& gameObjects = gameMap->getGameObjects();

            if(gameObjects[objectIndex].getIsLocated() == GameObjectConstants::LocatedOnCharacter) {
                
                const std::vector<GameCharacter>& gameCharacters = gameMap->getGameCharacters();
                const size_t charIndex = gameMap->characterIndexFromID(gameObjects[objectIndex].getCreatureID());
                const std::string characterName = gameCharacters[charIndex].getName();

                if(!mainWindow->askYesNoQuestion("The object is currently held by " + characterName + ". Do you still want to place it on the ground?", "Place Object?", true) == GenericInterfaceResponses::Yes) {
                    return false;
                }

            }

            GameObject::Builder objectBuilder(gameObjects[objectIndex]);          
            objectBuilder.location(col, row);
            gameMap->replaceObject(gmKey, objectIndex, objectBuilder.build());

            return true;

        }

    }

    return false;
}

///----------------------------------------------------------------------------
/// tryAddCharacter - Attempts to add a Character.
/// @param A reference to a Character Builder object that will get finalized
/// before being added.
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryAddCharacter(GameCharacter::Builder& characterBuilder) {

    // TODO: At some future point, have tryCharacter access the main window's
    // function to spawn the add dialog

    if(!canAddCharacter()) {
        showErrorMessage("ErrCharLimitReachedText", "ErrCharLimitReachedTitle");
        return false;
    }

    const int nextID = gameMap->getFirstUnusedCharacterID();

    characterBuilder.ID(nextID);

    try {
        gameMap->addCharacter(gmKey, characterBuilder.build());
    }
    catch (const std::bad_alloc&) {
        showErrorMessage("ErrAddCharOutOfMemoryText", "ErrAddCharOutOfMemoryTitle");
        return false;
    }
    return true;
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
/// tryAlterObject - Attempts to alter an object, either by adding a new one,
/// editing an existing one, or deleting it.
/// @param an integer specifying which mode the object is to be altered with
/// @return true if the object was altered in some way, false if it was not
///----------------------------------------------------------------------------

bool GameWorldController::tryAlterObject(const int& alterType, const int& index) {
    
    LanguageMapper& langMap = LanguageMapper::getInstance();

    if (!mainWindow->canCreateDialog(EditorDialogTypes::AlterObject)) {

        mainWindow->displayErrorMessage(langMap.get("ErrCreatingDialogText"),
                                        langMap.get("ErrCreatingDialogTItle"));
        return false;
    }


    const std::vector<GameObject>& gameObjects = getGameMap()->getGameObjects();
    
    bool wasDialogCreated = false;

    if (alterType == AlterType::Add) {
        if (gameObjects.size() >= AdventureGamerConstants::MaxNumObjects) {
            mainWindow->displayErrorMessage(langMap.get("ErrObjLimitReachedText"),
                                            langMap.get("ErrObjLimitReachedTitle"));
            return false;
        }
        else {
            GameObject::Builder objectBuilder = GameObject::Builder();
            wasDialogCreated = mainWindow->onAlterObject(objectBuilder, false);
        }
    }
    else if (alterType == AlterType::Edit) {
        if (gameMap->getGameObjects().empty() || index > gameObjects.size() - 1) {
            mainWindow->displayErrorMessage(langMap.get("ErrInvalidObjIndexText"),
                                            langMap.get("ErrInvalidObjIndexTitle"));
            return false;
        }
        else {
            // Obtain the Object we will be editing
            GameObject::Builder objectBuilder = GameObject::Builder(getGameMap()->getGameObjects()[index]);
            wasDialogCreated = mainWindow->onAlterObject(objectBuilder, true);
        }
    }

    if (!wasDialogCreated) {
        mainWindow->displayErrorMessage(langMap.get("ErrCreatingDialogText"),
                                        langMap.get("ErrCreatingDialogTitle"));
        mainWindow->onDialogEnd(EditorDialogTypes::AlterObject);
        return false;
    }

    return true;
}

///----------------------------------------------------------------------------
/// <!> DEPERCATED <!>
/// tryAddObject - Attempts to add an Object.
/// @param A reference to a Object Builder object that will get finalized
/// before being added.
/// @return true if the operation was successful, false if it was not.
/// <!> DEPERCATED <!>
///----------------------------------------------------------------------------

bool GameWorldController::tryAddObject(GameObject::Builder& gameObject) {

    // TODO: At some future point, have tryCharacter access the main window's
    // function to spawn the add dialog

    if(!canAddObject()) {
        showErrorMessage("ErrObjLimitReachedText", "ErrObjLimitReachedTitle");
        return false;
    }

    if(gameObject.getID() == GameObjectConstants::NoID) {

        const int nextID = gameMap->getFirstUnusedObjectID();
        gameObject.ID(nextID);

        try {
            gameMap->addObject(gmKey, gameObject.build());
        }
        catch (const std::bad_alloc&) {
            showErrorMessage("ErrAddObjOutOfMemoryText", "ErrAddObjOutOfMemoryTitle");
            return false;
        }
    }
    else {
        showErrorMessage("ErrObjNoIDText", "ErrObjNoIDTitle");
        return false;
    }

    return true;
}

///----------------------------------------------------------------------------
/// tryReplaceCharacter - Attempts to replace a Character
/// @param A reference to a Character Builder object. Must have a valid ID of an
/// already existing character.
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryReplaceCharacter(GameCharacter::Builder& characterBuilder) {

    // Find out if Adventure Gamer has a hard limit on the number of Characters.

    if(characterBuilder.getID() != GameCharacterConstants::NoID) {
 
        const size_t index = gameMap->characterIndexFromID(characterBuilder.getID());

        if(index == (size_t)-1) {
            mainWindow->displayErrorMessage("Could not find character to replace: ID not found.", "ID not found");
            return false;
        }

        gameMap->replaceCharacter(gmKey, index, characterBuilder.build());

    }
    else {
        mainWindow->displayErrorMessage("Invalid Character ID given.", "Invalid ID");
        return false;
    }

    return true;
}

///----------------------------------------------------------------------------
/// tryReplaceObject - Attempts to replace an Object.
/// @param A reference to a Object Builder object. Must have a valid ID of an
/// already existing object.
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryReplaceObject(GameObject::Builder& objectBuilder) {

    // Find out if Adventure Gamer has a hard limit on the number of Objects.

    if(objectBuilder.getID() != GameObjectConstants::NoID) {
 
        const size_t index = gameMap->objectIndexFromID(objectBuilder.getID());

        if(index == (size_t)-1) {
            mainWindow->displayErrorMessage("Could not find object to replace: ID not found.", "ID not found");
            return false;
        }

        gameMap->replaceObject(gmKey, index, objectBuilder.build());

    }
    else {
        mainWindow->displayErrorMessage("Invalid Object ID given.", "Invalid ID");
        return false;
    }

    return true;
}

///----------------------------------------------------------------------------
/// tryRemoveCharacter - Attempts to remove a character. It also alters the
/// location property of objects that are attached to it by setting their
/// location to "0,0".
/// @param the ID (not index) of the Character to remove
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryRemoveCharacter(const int& charID) {

    const size_t charIndex = gameMap->characterIndexFromID(charID);

    if(charIndex != (size_t)-1) {

        const std::vector<size_t> objectIndices = gameMap->getCharacterInventory(charID);
        
        if(!objectIndices.empty()) {

            const std::vector<GameObject>& gameObjects = gameMap->getGameObjects();

            const size_t objectIndicesSize = objectIndices.size(); 

            std::string message = "The following objects will be placed at 0,0 if this object is deleted:\n";
            
            for(size_t i = 0; i < objectIndicesSize; ++i) {
                message.append("\n" + gameObjects[objectIndices[i]].getName());
            }

            message.append("\n\nDo you still wish to delete this Character?");

            if(mainWindow->askYesNoQuestion(message, "Remove Character?", true) != GenericInterfaceResponses::Yes) {
                return false;
            }

            for(size_t k = 0; k < objectIndicesSize; ++k) {

                GameObject::Builder movedObject(gameObjects[objectIndices[k]]);
                movedObject.location(0, 0);
                gameMap->replaceObject(gmKey, objectIndices[k], movedObject.build());

            }

        }

        gameMap->deleteCharacter(gmKey, charIndex);
        return true;
    }  

    return false;
}

///----------------------------------------------------------------------------
/// tryRemoveObject - Attempts to remove an object. If it finds that this
/// object is required by another object, it will ask the user if it wants
/// to update that object before continuing.
/// @param the ID (not index) of the Object to remove
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryRemoveObject(const int& objectID) {

    const size_t objectIndex = gameMap->objectIndexFromID(objectID);

    if(objectIndex != (size_t)-1) {
        const std::vector<size_t> objectIndices = gameMap->getReliantObjectsFromID(objectID);

        if(!objectIndices.empty()) {

            const std::vector<GameObject>& gameObjects = gameMap->getGameObjects();
            const size_t objectIndicesSize = objectIndices.size(); 

            std::string message = "The following objects rely on this object, and will be updated to no longer require it:\n";
            
            for(size_t i = 0; i < objectIndicesSize; ++i) {
                message.append("\n" + gameObjects[objectIndices[i]].getName());
            }

            message.append("\n\nDo you still wish to delete this Object?");

            if(mainWindow->askYesNoQuestion(message, "Remove Object?", true) != GenericInterfaceResponses::Yes) {
                return false;
            }

            for(size_t k = 0; k < objectIndicesSize; ++k) {

                GameObject::Builder updatedObject(gameObjects[objectIndices[k]]);
                updatedObject.usedWithID(GameObjectConstants::UsedAlone);
                gameMap->replaceObject(gmKey, objectIndices[k], updatedObject.build());

            }

        }

        gameMap->deleteObject(gmKey, objectIndex);
    }

    return false;
}

///----------------------------------------------------------------------------
/// tryUpdateGameInfo - Attempts to update the game info for the world.
/// @param A reference to a GameInfo object that contains the new info
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryUpdateGameInfo(const GameInfo& newInfo) {

    gameMap->updateGameInfo(gmKey, newInfo);
    //mainWindow->GameInfoUpdated(newInfo);
    return true;
}

///----------------------------------------------------------------------------
/// tryUpdateTileDescription - Attempts to update the tiles name and
/// description. 
/// @param row of the tile being changed
/// @param col of the tile being changed
/// @param Name of the tile to be used
/// @param Long Description of the tile
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryUpdateTileDescription(const int& row, const int& col, const std::string& inName, const std::string& inDescription) {

    if(!gameMap->isRowColInMapBounds(row, col)) {
        // TODO: Handle error
        return false;
    }

    gameMap->updateTileDescription(gmKey, gameMap->indexFromRowCol(row, col), inName, inDescription);

    return true;
}

bool GameWorldController::tryUpdateTileDescription(const int& index, const std::string& inName, const std::string& inDescription) {
    // TODO: index in bounds
    gameMap->updateTileDescription(gmKey, index, inName, inDescription);
    return true;
}

///----------------------------------------------------------------------------
/// tryUpdateStoryAndSummary - Attempts to update the story/summary.
/// @param Story text to use for the story
/// @param Summary text to use for the summary
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryUpdateStoryAndSummary(const std::string& inStory, const std::string& inSummary) {

    gameMap->setStory(gmKey, inStory);
    gameMap->setSummary(gmKey, inSummary);

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
    mainWindow->onTileUpdated(otherTileIndex);

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