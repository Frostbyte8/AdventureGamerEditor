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

    return loadSuccessful;
}

///-----------------------------------------------------------------------------
/// newWorld - Attempt to create a new Adventure Gamer World. If it cannot, it
/// will avoid erasing the currently loaded world.
/// @return true if the operation completed successfully, false if it could not
///-----------------------------------------------------------------------------

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

    return wasWorldCreated;
}

///-----------------------------------------------------------------------------
/// saveWorld - Attempts to save the currently loaded world to the file path
/// and name given.
/// @param Path to the file being written.
/// @param name of the file being written to.
/// @return true if the operation completed successfully, false if it could not
///-----------------------------------------------------------------------------

bool GameWorldController::saveWorld(const std::string& filePath, const std::string& fileName) {
    std::ofstream ofs;
    std::string fileNameTemp = filePath;
    fileNameTemp.append("DEBUG.SG0");
    ofs.open(fileNameTemp.c_str(), std::ofstream::out | std::ios::binary);

    if(ofs) {

    }
    else {
        mainWindow->displayErrorMessage("Unable to write file.", "File Write Error");
        return false;
    }

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
/// tryAddObject - Attempts to add an Object.
/// @param A reference to a Object Builder object that will get finalized
/// before being added.
/// @return true if the operation was successful, false if it was not.
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

    // Find out if Adventuer Gamer has a hard limit on the number of Characters.

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

    // Find out if Adventuer Gamer has a hard limit on the number of Objects.

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

///----------------------------------------------------------------------------
/// tryUpdateTileType - Update the tile type used on the tile given. If the
/// tile is solid grass, it will remove the tile name and description.
/// @param row of the tile being changed
/// @param col of the tile being changed
/// @param type to replace the tile with
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryUpdateTileType(const int& row, const int& col, const int& type) {

    // TODO: Type constants
    if (type < 15 && type >= 0) {

        if (!gameMap->isRowColInMapBounds(row, col)) {
            return false;
        }

        const size_t tileIndex = gameMap->indexFromRowCol(row, col);
        const GameTile& gameTile = gameMap->getTile(tileIndex);

        // First we need to figure out if this tile is connected to anything else

        // Jump pads are an interesting case, they can both have a Jump feature,
        // and a dark switch feature. 

        if (gameTile.hasJumpPad()) {

            const SimplePoint* jumpPadDestination = gameMap->findJumpPoint(row, col);

            // TODO: it's possible a jump pad may not have a connection if it is not yet connected.
            if (jumpPadDestination != NULL) {

                // TODO: Make sure darkness is preserved.
                gameMap->updateTile(gmKey, gameMap->indexFromRowCol(row, col), type, 0);
                gameMap->removeFeature(gmKey, jumpPadDestination->getRow(), jumpPadDestination->getColumn());
                gameMap->removeJumpPoint(*jumpPadDestination, SimplePoint(col, row));

            }

        }

        if (gameTile.hasConnectionFeature()) {
            
            // Now check to see if this tile has a gate or dark space on it. Note that
            // Tiles with gates on them can either be a gate, or a door, but not both.
            if (gameTile.isDark()) {
                // TODO: See if a tile can be dark without a switch.
                const SimplePoint* matchingPoint = gameMap->findSwitchPoint(row, col);
            }
            
            
        }
        else {
            // Since it does not, we can easily clear it without any problems.
            if (type == 0) {
                //gameMap->updateTileDescription(gmKey, gameMap->indexFromRowCol(row, col), "", "");
            }
        }

        gameMap->updateTile(gmKey, gameMap->indexFromRowCol(row, col), type, 0);

    }
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