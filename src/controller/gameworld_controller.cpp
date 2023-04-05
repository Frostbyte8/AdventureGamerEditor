#include "gameworld_controller.h"
#include <fstream>
#include <stdexcept>

bool GameWorldController::LoadWorld(const std::string& filePath, const std::string& fileName) {
    
    std::string fileNameTemp = filePath + fileName;
	std::ifstream ifs;
	ifs.open(fileNameTemp.c_str(), std::ifstream::in | std::ios::binary);
    
    //std::ofstream ofs;
    //std::string tempName2 = filePath;
    //tempName2.append("DEBUG.SG0");
    //ofs.open(tempName2.c_str(), std::ofstream::out | std::ios::binary);

    if(ifs) {

        try {
			GameMap* newMap = new GameMap();
            newMap->readMap(ifs, filePath, fileName);
			
			if(gameMap) {
				delete gameMap;
				gameMap = NULL;
			}

            gameMap = newMap;
        }
        catch (const std::runtime_error& e) {

            mainWindow->DisplayErrorMessage(e.what(), "Error reading file");
            return false;

        }
    }
    else {

        mainWindow->DisplayErrorMessage("Unable to open file.", "File not found");
        return false;

    }

	

    return true;
}
