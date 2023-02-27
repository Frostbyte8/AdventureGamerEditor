#include "gameworld_controller.h"
#include <fstream>
#include <stdexcept>

bool GameWorldController::LoadWorld(const std::string& filePath, const std::string& fileName) {
    
    std::string fileNameTemp = filePath + fileName;
	std::ifstream ifs;
	ifs.open(fileNameTemp.c_str(), std::ifstream::in | std::ios::binary);

    if(ifs) {

        try {
            gameInfo.readHeader(ifs);
            gameMap.readMap(ifs, filePath, fileName);
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
