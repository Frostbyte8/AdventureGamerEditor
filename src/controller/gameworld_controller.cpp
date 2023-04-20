#include "gameworld_controller.h"
#include <fstream>
#include <stdexcept>

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
            mainWindow->DisplayErrorMessage(e.what(), "Error reading file");
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


bool GameWorldController::doesCharacterExist(const int& charID) const {
    const std::vector<GameCharacter>& gameCharacters = gameMap->getGameCharacters();

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

    return false;
}