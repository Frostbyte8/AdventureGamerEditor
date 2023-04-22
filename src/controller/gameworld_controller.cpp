#include "gameworld_controller.h"
#include <fstream>
#include <stdexcept>
#include "../compat/std_extras_compat.h"
#include "../util/languagemapper.h"

//=============================================================================
// Constructors / Destructors
//=============================================================================

///----------------------------------------------------------------------------
/// Constructors
///----------------------------------------------------------------------------

GameWorldController::GameWorldController(MainWindowInterface* inMainWindow) : mainWindow (inMainWindow) {
    gameMap = new GameMap(AdventureGamerConstants::DefaultRows, AdventureGamerConstants::DefaultCols);
}

///----------------------------------------------------------------------------
/// Destructor
///----------------------------------------------------------------------------

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
// Collection Accessors
//=============================================================================

const std::vector<GameObject>& GameWorldController::getGameObjects() const {
     return gameMap->getGameObjects();
}

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

            readCharacters(ifs);
            loadSuccessful = true;

        }
        catch (const std::runtime_error& e) {

            std::string fileReadError = LanguageMapper::getInstance().get(LanguageConstants::FileReadError) + " ";
            fileReadError.append(e.what());

            mainWindow->DisplayErrorMessage(fileReadError.c_str(), LanguageMapper::getInstance().get(LanguageConstants::FileReadErrorCaption));
        }
    }
    else {

        mainWindow->DisplayErrorMessage("Unable to open file.", "File not found");

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
        gameCharacters.clear();
        wasWorldCreated = true;
    }
    catch (const std::runtime_error& e) {
        std::string errMessage = "Unable to create a new Adventure Gamer world: ";
        errMessage.append(e.what());
        mainWindow->DisplayErrorMessage(errMessage, "Error creating new world.");
    }
    catch (const std::bad_alloc& e) {
        mainWindow->DisplayErrorMessage(e.what(), "Out of Memory?");
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
        mainWindow->DisplayErrorMessage("Unable to write file.", "File Write Error");
        return false;
    }

    return true;
}

///----------------------------------------------------------------------------
/// doesCharacterExist - Checks if a character exists via it's ID.
/// @param ID of the character to search for
/// @returns true if the character exists and false if it does not
///----------------------------------------------------------------------------

bool GameWorldController::doesCharacterExist(const int& charID) const {

    //const std::vector<GameCharacter>& gameCharacters = gameMap->getGameCharacters();

    for(std::vector<GameCharacter>::const_iterator it = gameCharacters.begin();
        it != gameCharacters.end(); ++it) {
            if( it->getID() == charID) {
                return true;
            }
    }

    return false;
}

///----------------------------------------------------------------------------
/// tryGetTileCopy - tries to get a copy of the tile at the specified row/col.
/// @param row of the tile get
/// @param column of the tile to get
/// @returns true if it was able to get a copy, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryGetTileCopy(const int& row, const int& col, GameTile& outTile) const {

    if(gameMap->isTileIndexInMapBounds(row, col)) {
        const unsigned int index = gameMap->indexFromRowCol(row, col);
        GameTile::Builder builder(gameMap->getTile(index));
        outTile = builder.build();
        return true;
    }
    else {
        mainWindow->DisplayErrorMessage(LanguageMapper::getInstance().get(LanguageConstants::TileGetError),
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
    
    if(gameMap->isTileIndexInMapBounds(row, col)) {

        const size_t charIndex = getCharacterIndexFromID(charID);

        if(charIndex != (size_t)-1) {

            GameCharacter::Builder charBuilder(gameCharacters[charIndex]);
            std::string newLocation = std::to_string(col) + "," + std::to_string(row);
            charBuilder.location(newLocation);
            replaceCharacter(charIndex, charBuilder.build());
            return true;

        }

    }

    return false;
}

///----------------------------------------------------------------------------
/// tryRemoveCharacter - Attempts to remove a character. It also alters the
/// location property of objects that are attached to it by setting their
/// location to "0,0".
/// @param the ID (not index) of the Character to remove
/// @return true if the operation was successful, false if it was not.
///----------------------------------------------------------------------------

bool GameWorldController::tryRemoveCharacter(const int& charID) {

    //const std::vector<GameCharacter>& gameCharacters = gameMap->getGameCharacters();

    // So first, we'll loop through and find the character we are looking for.
    // TODO: make this a seperate function

    const size_t charIndex = getCharacterIndexFromID(charID);

    if(charIndex != (size_t)-1) {

        // Check if any of the objects indicate that this Character/Creature
        // is in possession of them, then cache their index, and name.

        const std::vector<GameObject>& gameObjects = gameMap->getGameObjects();
        std::vector<size_t> objectIndices;
        std::vector<std::string> objectNames;
        objectIndices.reserve(4);
        objectNames.reserve(4);

        for(size_t k = 0; k < gameObjects.size(); ++k) {

            if(gameObjects[k].getCreatureID() == charID) {
                objectIndices.push_back(gameObjects[k].getID());
                objectNames.push_back(gameObjects[k].getName());
            }
        }

        // If any objects were found, we need to ask the user if they
        // still wish to remove the object.

        if(objectIndices.size()) {
            
            std::string message = "The following objects will be placed at 0,0 if this object is deleted:\n";
            
            for(size_t j = 0; j < objectNames.size(); ++j) {
                message.append("\n" + objectNames[j]);
            }

            message.append("\n\nDo you still wish to delete this Character?");

            if(mainWindow->AskYesNoQuestion(message, "Remove Character?", true) != MainWindowInterfaceResponses::Yes) {
                return false;
            }

            for(size_t m = 0; m < objectIndices.size(); ++m) {

                GameObject::Builder movedObject(gameObjects[objectIndices[m]]);
                movedObject.location(GameObjectConstants::DefaultLocation);
                gameMap->replaceObject(objectIndices[m], movedObject.build());

            }

        }

        // Now to finally delete the Character

        deleteCharacter(charIndex);
        return true;


    }

    return false;
}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// readCharacters - Reads the "{cretr" section of the map file
/// @param mapFile an ifstream of the map file to be read from already at the
/// "{cretr" section
/// @throws runtime_error if there is a problem reading the file.
///----------------------------------------------------------------------------

void GameWorldController::readCharacters(std::ifstream& mapFile) {

    std::string line;
    std::string errorMsg = "Error reading characters: ";
    Frost::getLineWindows(mapFile, line);

    if(AdventureGamerHeadings::Characters.compare(line)) {
        errorMsg.append("Expected \"" + AdventureGamerHeadings::Characters + "\", but got \"" + line + "\".");
        throw std::runtime_error(errorMsg);
    }

    std::getline(mapFile, line);
    const int numChars = std::stoi(line);
    gameCharacters.clear();
    gameCharacters.reserve(numChars);

    for(int i = 0; i < numChars; i++) {
        GameCharacter::Builder characterBuilder;
        characterBuilder.readCharacter(mapFile);
        GameCharacter gameCharacter = characterBuilder.build();
        gameCharacters.push_back(gameCharacter);
    }

}