#include "gamemap.h"
#include "../util/frost.h"
#include "../compat/std_extras_compat.h"
#include "../editor_constants.h"
#include <algorithm>

//=============================================================================
// Public Functions
//=============================================================================

void GameMap::readMap(std::ifstream& mapFile, const std::string& filePath,
                      const std::string& fileName) {

    std::string line;

    // Whatever the value read is, it is always one more than it says.

    std::getline(mapFile, line);
    numCols = std::stoi(line) + 1;

    std::getline(mapFile, line);
    numRows = std::stoi(line) + 1;

    // TODO: out of range errors need to be caught to
    // Further more, read rows needs to be in it's own function.

    for(int row = 0; row < numRows; row++) {

        std::string rowID = AdventureGamerHeadings::Row + std::to_string(row);

        std::getline(mapFile, line);
        line = Frost::rtrim(line, 13);

        // This is only true if they're not equal.
        if(rowID.compare(line)) {
            throw std::runtime_error("Row identifier not found. Expected \"" + rowID + "\", but got \"" + line + "\".");
        }

        std::string rowFilePath = filePath + fileName.substr(0, fileName.length() - 4) + ".T"; 
        
        if(row < 10) {
            rowFilePath += "0";
        }

        rowFilePath += std::to_string(row);

        const std::map<unsigned int, std::string> rowDescriptions = readRowDescriptions(rowFilePath);

        for(int col = 0; col < numCols; col++) {

            // Before we can read a tile, we'll need to get the description for it,
            // if one exists.

            std::string tileDescription;

            std::map<unsigned int, std::string>::const_iterator it;
			it = rowDescriptions.find(col);

            if(it != rowDescriptions.end()) {
                tileDescription = it->second;
            }

            GameTile gt = readTile(mapFile, tileDescription);
            tiles.push_back(gt);
        }

        // TODO: Throwing an error right now causes the object to be in an undefined state, to test
        // this, throw an error here and find a way to stop it from being incomplete. Probably
        // a builder pattern, but maybe not.

    }

    readJumps(mapFile);

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

            do {
                if(ifs.eof()) {
                    break;
                }
                std::getline(ifs, line);

                line = Frost::rtrim(line, '\r');

                if(Frost::endsWith(line, "\"")) {
                    break;
                }

                description += line;

            } while(true);

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

///----------------------------------------------------------------------------
/// readJumps - 
///----------------------------------------------------------------------------

void GameMap::readJumps(std::ifstream& mapFile) {

    std::string line;
    std::getline(mapFile, line);
    line = Frost::rtrim(line, 13);

    // Read the header

    if(AdventureGamerHeadings::Jumps.compare(line)) {
        throw std::runtime_error("Error reading file. Expected \"" + AdventureGamerHeadings::Jumps + "\", but got \"" + line + "\".");
    }

    try {
        std::getline(mapFile, line);
        const int numJumps = std::stoi(line);
        jumpPoints.reserve(numJumps);

        for(int i = 0; i < numJumps; i++) {

            std::getline(mapFile, line);
            int x = std::stoi(line);
            std::getline(mapFile, line);
            int y = std::stoi(line);
            SimplePoint jumpA(x, y);

            std::getline(mapFile, line);
            x = std::stoi(line);
            std::getline(mapFile, line);
            y = std::stoi(line);
            SimplePoint jumpB(x, y);

            ConnectionPoint jumpConnection(jumpA, jumpB);

            
            if(!ifConnectionExists(jumpPoints, jumpConnection)) {
                jumpPoints.push_back(jumpConnection);
            }
            else {
                throw std::runtime_error("Error reading file: Duplicate Jump Point was read");
            }

        }
    }
    catch (const std::invalid_argument& e) {
        throw std::runtime_error("Tried to read a number, but did not get a valid integer");
    }
}

const inline bool GameMap::ifConnectionExists(const std::vector<ConnectionPoint>& connections, const ConnectionPoint& connectionPoint) const {
    
    if(!connections.size()) {
        return false;
    }
    
    std::vector<ConnectionPoint>::const_iterator it = find(connections.begin(), connections.end(), connectionPoint);
    return !(it == connections.end());
}