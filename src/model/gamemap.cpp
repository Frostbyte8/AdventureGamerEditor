#include "gamemap.h"
#include "../util/frost.h"
#include "../compat/std_extras_compat.h"
#include "../editor_constants.h"
#include <algorithm>

//=============================================================================
// Constructors/Destructor
//=============================================================================

GameMap::GameMap() : numCols(AdventureGamerConstants::DefaultCols), numRows(AdventureGamerConstants::DefaultRows) {
	GameTile::Builder builder;
	GameTile gt = builder.build();
	tiles.insert(tiles.begin(), getNumTiles(), gt);
}

//=============================================================================
// Accessors
//=============================================================================

const int GameMap::getNumTiles() const {
	return numCols * numRows;
}

///----------------------------------------------------------------------------
/// getHeight - Returns the height of the map
/// @return an interger indicating the height of the map
///----------------------------------------------------------------------------

const int& GameMap::getHeight() const {
    return numRows;
}

///----------------------------------------------------------------------------
/// getTileDrawData - Return tile information relevant to drawing tiles.
/// @return a vector filled with tile drawing data.
///----------------------------------------------------------------------------

const std::vector<GameTile::DrawInfo> GameMap::getTileDrawData() {

    std::vector<GameTile::DrawInfo> drawDataVec;
    drawDataVec.reserve(tiles.size());

    for(std::vector<GameTile>::iterator it = tiles.begin(); it != tiles.end(); ++it) {
        drawDataVec.push_back(it->getDrawInfo());
    }

    return drawDataVec;
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

///----------------------------------------------------------------------------
/// getGameObjects - Return a vector containing Game Objects within the current
/// map.
/// @return a vector containing the Game Objects.
///----------------------------------------------------------------------------

const std::vector<GameObject>& GameMap::getGameObjects() const {
	return gameObjects;
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

    const std::string storyFilePath = filePath + fileName.substr(0, fileName.length() - 4) + ".STY";

    readStory(storyFilePath);

    // TODO: Throwing an error right now causes the object to be in an undefined state.
    // We also need to do some better error checking.
    // TODO: Verify correct extentions.
	// TODO: Reserve tiles

    gameInfo.readHeader(key, mapFile);

    std::string line;

    // Whatever the value read is, it is always one more than it says.

    std::getline(mapFile, line);
    numCols = std::stoi(line) + 1;

    std::getline(mapFile, line);
    numRows = std::stoi(line) + 1;

    for(int row = 0; row < numRows; row++) {

        const std::string rowID = AdventureGamerHeadings::Row + std::to_string(row);

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

            GameTile::Builder tileBuilder;
            tileBuilder.readTile(mapFile, tileDescription);
            tiles.push_back(tileBuilder.build());
        }

    }

    readJumps(mapFile);
    readSwitches(mapFile);    
    gameInfo.readPlayerAttributes(key, mapFile);
    readObjects(mapFile);
    readCharacters(mapFile);
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
/// readCharacters - Reads the "{cretr" section of the map file
/// @param mapFile an ifstream of the map file to be read from already at the
/// "{cretr" section
/// @throws runtime_error if there is a problem reading the file.
///----------------------------------------------------------------------------

void GameMap::readCharacters(std::ifstream& mapFile) {

    std::string line;
    std::getline(mapFile, line);
    std::string errorMsg = "Error reading characters: ";
    line = Frost::rtrim(line, 13);

    if(AdventureGamerHeadings::Characters.compare(line)) {
        errorMsg.append("Expected \"" + AdventureGamerHeadings::Characters + "\", but got \"" + line + "\".");
        throw std::runtime_error(errorMsg);
    }

    std::getline(mapFile, line);
    const int numChars = std::stoi(line);
    gameCharacters.reserve(numChars);

    for(int i = 0; i < numChars; i++) {
        GameCharacter::Builder characterBuilder;
        characterBuilder.readCharacter(mapFile);
        GameCharacter gameCharacter = characterBuilder.build();
        gameCharacters.push_back(gameCharacter);
    }
}

///----------------------------------------------------------------------------
/// readJumps - Read the "{jumps" section of the file.
/// @param mapFile an ifstream of the map file to be read from, already at the
/// "{jumps" section.
/// @throws runtime_error if there are any problems reading the file.
///----------------------------------------------------------------------------

void GameMap::readJumps(std::ifstream& mapFile) {

    std::string line;
    std::getline(mapFile, line);
    std::string errorMsg = "Error reading Jumps: ";
    line = Frost::rtrim(line, 13);

    // Read the header

    if(AdventureGamerHeadings::Jumps.compare(line)) {
        errorMsg.append("Expected \"" + AdventureGamerHeadings::Jumps + "\", but got \"" + line + "\".");
        throw std::runtime_error(errorMsg);
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
                errorMsg.append("Jump Pad position was read, but the coordinates given were not that of a Jump Pad tile.");
                throw std::runtime_error(errorMsg);
            }

            std::getline(mapFile, line);
            x = std::stoi(line);
            std::getline(mapFile, line);
            y = std::stoi(line);
            SimplePoint jumpB(x, y);

            tileIndex = indexFromRowCol(y, x);
            if(!(tiles.at(tileIndex).hasJumpPad())) {
                errorMsg.append("Jump Pad position was read, but the coordinates given were not that of a Jump Pad tile.");
                throw std::runtime_error(errorMsg);
            }

            ConnectionPoint jumpConnection(jumpA, jumpB);
            
            if(!ifConnectionExists(jumpPoints, jumpConnection)) {
                jumpPoints.push_back(jumpConnection);
            }
            else {
                errorMsg.append("Duplicate Jump Point was read.");
                throw std::runtime_error(errorMsg);
            }
        }
    }
    catch (const std::invalid_argument& e) {
        errorMsg.append(e.what());
        throw std::runtime_error(errorMsg);
    }
    catch (const std::out_of_range& e) {
        errorMsg.append(e.what());
        throw std::runtime_error(errorMsg);
    }
}

///----------------------------------------------------------------------------
/// readRowDescriptions - Reads the descriptions from the file given, if it
/// exists. If it does not, it is possible that there are no descriptions for
/// this row.
/// @param a string indicating the full path to the row file.
/// @return a map containing the strings, with they key being the column index
/// @throws runtime_error if there is a problem reading the file.
///----------------------------------------------------------------------------

std::map<unsigned int, std::string> GameMap::readRowDescriptions(const std::string& rowFileName) {

    std::ifstream ifs;
    std::string line;
    std::string errorMsg = "Error reading row descriptions: ";
    std::map<unsigned int, std::string> descriptionMap;

    // TODO: check if the file exists, then check if we can open it, and error out
    // if necessary.

    ifs.open(rowFileName.c_str(), std::ifstream::in | std::ios::binary);
    
    if(ifs) {

        try {
            std::getline(ifs, line);
            const unsigned long numDescriptions = std::stoul(line);

            for(unsigned long i = 0; i < numDescriptions; i++) {
                std::getline(ifs, line);

                if(line.empty()) {
                    break; // Nothing left.
                }
                
                const unsigned int colID = std::stoi(line);

                if(colID >= static_cast<unsigned int>(numCols)) {
                    errorMsg.append("The column indicated is outside the boundries of the map.");
                    throw std::runtime_error(errorMsg);
                }

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
        catch (const std::invalid_argument& e) {
            errorMsg.append(e.what());            
            throw std::runtime_error(errorMsg);
        }
        catch (const std::out_of_range& e) {
            errorMsg.append(e.what());
            throw std::runtime_error(errorMsg);
        }
    }

    return descriptionMap;
}

///----------------------------------------------------------------------------
/// readObjects - Reads the "{objct" section of the map file
/// @param mapFile an ifstream of the map file to be read from already at the
/// "{objct" section
///----------------------------------------------------------------------------

void GameMap::readObjects(std::ifstream& mapFile) {

    std::string line;
    std::getline(mapFile, line);
    std::string errorMsg = "Error reading objects: ";
    line = Frost::rtrim(line, 13);

    if(AdventureGamerHeadings::Objects.compare(line)) {
        errorMsg.append("Expected \"" + AdventureGamerHeadings::Objects + "\", but got \"" + line + "\".");
        throw std::runtime_error(errorMsg); 
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
/// readStory - Reads the STY file.
/// @param a string indicating the full path to the story file.
///----------------------------------------------------------------------------

void GameMap::readStory(const std::string& storyFilePath) {
	
    std::ifstream ifs;
	ifs.open(storyFilePath.c_str(), std::ifstream::in | std::ios::binary);

	if(ifs) {
		ifs.seekg(0, std::ios::end);
		story.resize(ifs.tellg());
		ifs.seekg(0, std::ios::beg);
		ifs.read(&story[0], story.size());
		ifs.close();
    }
    
    // Story files aren't mandatory, so if they're not found, there is no
    // error.
    story = Frost::rtrim(Frost::ltrim(story, '"'), "\"\x0D\x0A");

}

///----------------------------------------------------------------------------
/// readSwitches - Read the "{swtchs" section of the file.
/// @param mapFile an ifstream of the map file to be read from already at the
/// "{swtchs" section.
/// @throws runtime_error if there are any problems reading the file
///----------------------------------------------------------------------------

void GameMap::readSwitches(std::ifstream& mapFile) {
    
    std::string line;
    std::getline(mapFile, line);
    std::string errorMsg = "Error reading switches: ";
    line = Frost::rtrim(line, 13);

    // Read the header

    if(AdventureGamerHeadings::Switches.compare(line)) {
        errorMsg.append("Expected \"" + AdventureGamerHeadings::Switches + "\", but got \"" + line + "\".");
        throw std::runtime_error(errorMsg);
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
                errorMsg.append("Read switch, but no switch was found at the coordinates read.");
                throw std::runtime_error(errorMsg);
            }
            
            // Get the tile effected
            std::getline(mapFile, line);
            x = std::stoi(line);
            std::getline(mapFile, line);
            y = std::stoi(line);
            SimplePoint connectionB(x, y);

            
            tileIndex = indexFromRowCol(y, x);
            if(! (tiles.at(tileIndex).hasGate() || tiles.at(tileIndex).isDark()) ) {
                errorMsg.append("Read switch, but the tile it effects is not a gate or dark space.");
                throw std::runtime_error(errorMsg);
            }

        }
    }
    catch (const std::invalid_argument& e) {
        errorMsg.append(e.what());
        throw std::runtime_error(errorMsg);
    }
    catch (const std::out_of_range& e) {
        errorMsg.append(e.what());
        throw std::out_of_range(errorMsg);
    }

}



void GameMap::writeMap(std::ofstream& mapFile) {
    gameInfo.writeHeader(key, mapFile);
}