#include "gamemap.h"
#include "../util/frost.h"
#include "../compat/std_extras_compat.h"

//=============================================================================
// Public Functions
//=============================================================================

void GameMap::readMap(std::ifstream& mapFile, const std::string& fileName,
                      const std::string& filePath) {

    std::string line;

    // Whatever the value read is, it is always one more than it says.

    std::getline(mapFile, line);
    numCols = std::stoi(line) + 1;

    std::getline(mapFile, line);
    numRows = std::stoi(line) + 1;

    // TODO: out of range errors need to be caught to

    for(int row = 0; row < numRows; row++) {

        std::string rowID = "{row " + std::to_string(row);

        std::getline(mapFile, line);
        line = Frost::rtrim(line, 13);

        if(rowID.compare(line) != 0) {
            throw std::runtime_error("Row identifier not found. Expected \"" + rowID + "\", but got \"" + line + "\".");
        }

        std::string rowFilePath = filePath + fileName + ".T"; 
        
        if(row < 10) {
            rowFilePath += "0";
        }

        rowFilePath += std::to_string(row);

        const std::map<unsigned int, std::string> rowDescriptions = readRowDescriptions(rowFilePath);

        for(int col = 0; col < numCols; col++) {
            GameTile gt = readTile(mapFile, "");
            tiles.push_back(gt);
        }
    }

}

///----------------------------------------------------------------------------
/// readRowDescriptions - Reads the descriptions from the file given, if it
/// exists. If it does not, it is possible that there are no descriptions for
/// this row.
/// @param a string indicating the full path to the row file.
/// @return a map containing the strings, with they key being the column index
/// @throws invalid_argument, runtime_error, 
///----------------------------------------------------------------------------

std::map<unsigned int, std::string> GameMap::readRowDescriptions(const std::string& rowFileName) {

    std::ifstream ifs;
    std::string line;
    std::map<unsigned int, std::string> descriptionMap;

    ifs.open(rowFileName.c_str(), std::ifstream::in | std::ios::binary);
    
    if(ifs) {

        std::getline(ifs, line);
        const unsigned long numDescriptions = std::stoul(line);

        for(unsigned long i = 0; i < numDescriptions; i++) {
            std::getline(ifs, line);

            if(line.empty()) {
                break; // Nothing left.
            }
            
            const unsigned int colID = std::stoi(line);
            std::string description;

            std::getline(ifs, line);
            while(!Frost::endsWith(line, "\"")) {
                description += line;
                std::getline(ifs, line);
            }

            description += line;

            descriptionMap[colID] = description;
        }
    }

    return descriptionMap;
}

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// readTile - Reads a single tile from the map file, and it will also give
/// the tile a long description if it's specified.
/// @param mapFile an ifstream of the map file to be read from
/// @param string containing the long description of the tile.
/// @throws runtime_error if any of the numbers read are invalid
/// @throws runtime_error if the builder tries to build an invalid tile.
/// @returns a GameTile object
///----------------------------------------------------------------------------

GameTile GameMap::readTile(std::ifstream& mapFile, const std::string& description) {

    std::string line;
    GameTile::Builder tileBuilder;

    try {

        std::getline(mapFile, line);
        const int sprite = std::stoi(line);
        tileBuilder.sprite(sprite);

        std::getline(mapFile, line);
        tileBuilder.flags(std::stoi(line));

        if(sprite != 0) {
            std::getline(mapFile, line);
            tileBuilder.name(line);
        }

    }
    catch (const std::invalid_argument& e) {
        throw std::runtime_error("Tried to read a sprite number, but did not get a valid integer");
    }

    if(!description.empty()) {
        tileBuilder.description(description);
    }

    try {
        return tileBuilder.build();
    }
    catch (const std::invalid_argument& e) {
        throw std::runtime_error(e.what());
    }
}
