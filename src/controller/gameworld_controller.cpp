#include "gameworld_controller.h"
#include <fstream>
#include <stdexcept>
#include "../compat/std_extras_compat.h"
#include "../util/languagemapper.h"

//=============================================================================
// Constructors / Destructors
//=============================================================================

GameWorldController::GameWorldController(MainWindowInterface* inMainWindow) : mainWindow (inMainWindow) {
    gameMap = new GameMap(AdventureGamerConstants::DefaultRows, AdventureGamerConstants::DefaultCols);
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

            std::string fileReadError = LanguageMapper::getInstance().get(LanguageConstants::FileReadError) + " ";
            fileReadError.append(e.what());

            mainWindow->displayErrorMessage(fileReadError.c_str(), LanguageMapper::getInstance().get(LanguageConstants::FileReadErrorCaption));
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
		
        newMap = new GameMap(AdventureGamerConstants::DefaultRows, AdventureGamerConstants::DefaultCols);		

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
        mainWindow->displayErrorMessage(LanguageMapper::getInstance().get(LanguageConstants::TileGetError),
                                        LanguageMapper::getInstance().get(LanguageConstants::TileGetErrorCaption));
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

                if(!mainWindow->askYesNoQuestion("The object is currently held by " + characterName + ". Do you still want to place it on the ground?", "Place Object?", true) == MainWindowInterfaceResponses::Yes) {
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

bool GameWorldController::tryAddCharacter(GameCharacter::Builder& gameCharacter) {

    // Find out if Adventuer Gamer has a hard limit on the number of Characters.
    // TODO: Reuse unused IDs first.
    const int nextID = gameMap->getLastCharacterID() + 1;
    gameCharacter.ID(nextID);
    try {
        gameMap->addCharacter(gmKey, gameCharacter.build());
    }
    catch (const std::bad_alloc&) {
        mainWindow->displayErrorMessage("Could not add character: Out of memory.", "Out of Memory");
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

    // Find out if Adventuer Gamer has a hard limit on the number of Objects.
    // TODO: Reuse unused IDs first.
    // TODO: Check if the user wanted to add or replace, and catch that if
    // necessary.

    if(gameObject.getID() == GameObjectConstants::NoID) {

        const int nextID = gameMap->getLastObjectID() + 1;
        gameObject.ID(nextID);

        try {
            gameMap->addObject(gmKey, gameObject.build());
        }
        catch (const std::bad_alloc&) {
            mainWindow->displayErrorMessage("Could not add object: Out of memory.", "Out of Memory");
            return false;
        }
    }
    else {
        const size_t index = gameMap->objectIndexFromID(gameObject.getID());
        gameMap->replaceObject(gmKey, index, gameObject.build());
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

            if(mainWindow->askYesNoQuestion(message, "Remove Character?", true) != MainWindowInterfaceResponses::Yes) {
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

            if(mainWindow->askYesNoQuestion(message, "Remove Object?", true) != MainWindowInterfaceResponses::Yes) {
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
