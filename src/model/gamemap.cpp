#include "gamemap.h"
#include "../util/frost.h"
#include "../compat/std_extras_compat.h"
#include "../editor_constants.h"
#include <algorithm>

//=============================================================================
// Accessors
//=============================================================================

///----------------------------------------------------------------------------
/// getHeight - Returns the height of the map
/// @return an interger indicating the height of the map
///----------------------------------------------------------------------------

const int& GameMap::getHeight() const {
    return numRows;
}

///----------------------------------------------------------------------------
/// getTiles - Return a constant reference to a vector containing the tiles
/// data on the map.
/// @return a constant reference to a vector filled with GameTiles
///----------------------------------------------------------------------------

const std::vector<GameTile>& GameMap::getTiles() const {
    return tiles;
}

///----------------------------------------------------------------------------
/// getWidth - Returns the width of the map
/// @return an interger indicating the width of the map
///----------------------------------------------------------------------------

const int& GameMap::getWidth() const {
    return numCols;
}

//=============================================================================
// Public Functions
//=============================================================================

///----------------------------------------------------------------------------
/// indexFromRowCol - Converts the given row and column to an index for the
/// current map being edited.
/// @param integer specifying the row the tile is on
/// @param integer specifying the column the tile is on
/// @throws out_of_range if the row or column is outside the boundries of the
/// map
/// @return the index of the tile specified
///----------------------------------------------------------------------------

const unsigned int GameMap::indexFromRowCol(const int& row, const int& col) const {
    if(row > numRows || col > numCols || col < 0 || row < 0) {
        throw std::out_of_range("Index of of range.");
    }
    return (row * numCols) + col;
}

///----------------------------------------------------------------------------
/// readMap - Reads the SG0 and TXX file of the map name given, and then uses
/// it to construct the game map.
/// @param an ifstream if the map file
/// @param a string to the path where the mapfile is located
/// @param a string indicating the file's name
/// @throws runtime_error
///----------------------------------------------------------------------------

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
        // In addition, we need to catch the other errors and state that they are simply runtime
        // errors

    }

    readJumps(mapFile);
    readSwitches(mapFile);

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
/// ifConnectionExists - Checks to see if a connection exists in the specified
/// connection vector, at the given coordinates given.
/// @param vector containing the connection points
/// @param ConnectionPoint to check for
/// @return true if the connection was found, false if it was not.
///----------------------------------------------------------------------------

const bool GameMap::ifConnectionExists(const std::vector<ConnectionPoint>& connections,
                                       const ConnectionPoint& connectionPoint) const {
    
    if(!connections.size()) {
        return false;
    }
    
    std::vector<ConnectionPoint>::const_iterator it = find(connections.begin(), connections.end(),
                                                           connectionPoint);
    return !(it == connections.end());
}

///----------------------------------------------------------------------------
/// readObjects - 
///----------------------------------------------------------------------------

void GameMap::readObjects(std::ifstream& mapFile) {

    std::string line;
    std::getline(mapFile, line);
    line = Frost::rtrim(line, 13);

    if(AdventureGamerHeadings::Objects.compare(line)) {
        throw std::runtime_error("Error reading file. Expected \"" + AdventureGamerHeadings::Objects + "\", but got \"" + line + "\".");
    }

    std::getline(mapFile, line);
    const int numObjects = std::stoi(line);
    gameObjects.reserve(numObjects);

    for(int i = 0; i < numObjects; i++) {
        GameObject::Builder objectBuilder;
        objectBuilder.readObject(mapFile);
        GameObject gameObject = objectBuilder.build();
        gameObjects.push_back(gameObject);
    }
}

///----------------------------------------------------------------------------
/// readTile - Reads a single tile from the map file, and it will also give
/// the tile a long description if it's specified.
/// @param mapFile an ifstream of the map file to be read from
/// @param string containing the long description of the tile.
/// @throws runtime_error if any of the numbers read are invalid
/// @throws runtime_error if the builder tries to build an invalid tile.
/// @returns a GameTile object
///----------------------------------------------------------------------------

// TODO: readTile should be part of the gametile model.

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
    catch (const std::invalid_argument&) {
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
/// readJumps - Read the "{jumps" section of the file.
/// @param mapFile an ifstream of the map file to be read from
/// @throws runtime_error if any of the numbers read are invalid
/// @throws runtime_error if any of the tiles are 
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

            unsigned int tileIndex = indexFromRowCol(y, x);
            if(!(tiles.at(tileIndex).hasJumpPad())) {
                throw std::runtime_error("Error reading file: Jump Pad position was read, but the coordinates given were not that of a Jump Pad tile");
            }

            std::getline(mapFile, line);
            x = std::stoi(line);
            std::getline(mapFile, line);
            y = std::stoi(line);
            SimplePoint jumpB(x, y);

            tileIndex = indexFromRowCol(y, x);
            if(!(tiles.at(tileIndex).hasJumpPad())) {
                throw std::runtime_error("Error reading file: Jump Pad position was read, but the coordinates given were not that of a Jump Pad tile");
            }

            ConnectionPoint jumpConnection(jumpA, jumpB);
            
            if(!ifConnectionExists(jumpPoints, jumpConnection)) {
                jumpPoints.push_back(jumpConnection);
            }
            else {
                throw std::runtime_error("Error reading file: Duplicate Jump Point was read");
            }
        }
    }
    catch (const std::invalid_argument&) {
        throw std::runtime_error("Tried to read a number, but did not get a valid integer");
    }
    catch (const std::out_of_range&) {
        throw std::out_of_range("Tried to access a jump pad tile, but the index given was out of the valid range");
    }
}

///----------------------------------------------------------------------------
/// readSwitches - Read the "{swtchs" section of the file.
/// @param mapFile an ifstream of the map file to be read from
/// @throws runtime_error if any of the numbers read are invalid
/// @throws runtime_error if any of the tiles are 
///----------------------------------------------------------------------------

void GameMap::readSwitches(std::ifstream& mapFile) {
    std::string line;
    std::getline(mapFile, line);
    line = Frost::rtrim(line, 13);

    // Read the header

    if(AdventureGamerHeadings::Switches.compare(line)) {
        throw std::runtime_error("Error reading file. Expected \"" + AdventureGamerHeadings::Switches + "\", but got \"" + line + "\".");
    }

    try {

        std::getline(mapFile, line);
        const int numSwitches = std::stoi(line);
        switchConnections.reserve(numSwitches);

        for(int i = 0; i < numSwitches; i++) {

            // Get the tile with the switch on it
            std::getline(mapFile, line);
            int x = std::stoi(line);
            std::getline(mapFile, line);
            int y = std::stoi(line);
            SimplePoint connectionA(x, y);

            
            unsigned int tileIndex = indexFromRowCol(y, x);
            if(!(tiles.at(tileIndex).hasSwitch())) {
                throw std::runtime_error("Error reading file: Read switch, but no switch was found at the coordinates read.");
            }
            

            // Get the tile effected
            std::getline(mapFile, line);
            x = std::stoi(line);
            std::getline(mapFile, line);
            y = std::stoi(line);
            SimplePoint connectionB(x, y);

            
            tileIndex = indexFromRowCol(y, x);
            if(! (tiles.at(tileIndex).hasGate() || tiles.at(tileIndex).isDark()) ) {
                throw std::runtime_error("Error reading file: Read switch, but the tile it effects is not a gate or dark space.");
            }

            /*
            ConnectionPoint jumpConnection(jumpA, jumpB);
            
            if(!ifConnectionExists(jumpPoints, jumpConnection)) {
                jumpPoints.push_back(jumpConnection);
            }
            else {
                throw std::runtime_error("Error reading file: Duplicate Jump Point was read");
            }
            */
        }
    }
    catch (const std::invalid_argument&) {
        throw std::runtime_error("Tried to read a number, but did not get a valid integer");
    }
    catch (const std::out_of_range&) {
        throw std::out_of_range("Tried to access a jump pad tile, but the index given was out of the valid range");
    }

}