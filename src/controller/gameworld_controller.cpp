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

bool GameWorldController::loadWorld(const std::string& filePath, const std::string& fileName) {
    
    std::string fileNameTemp = filePath + fileName;
	std::ifstream ifs;
	ifs.open(fileNameTemp.c_str(), std::ifstream::in | std::ios::binary);
    
    //std::ofstream ofs;
    //std::string tempName2 = filePath;
    //tempName2.append("DEBUG.SG0");
    //ofs.open(tempName2.c_str(), std::ofstream::out | std::ios::binary);
    
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
    catch (const std::runtime_error&) {
        mainWindow->DisplayErrorMessage("Unable to create new world.", "New World Error");
    }

    if(!wasWorldCreated) {
        delete newMap;
        newMap = NULL;
    }

    return wasWorldCreated;
}

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