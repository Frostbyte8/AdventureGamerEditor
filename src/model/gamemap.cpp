#include "gamemap.h"
#include "../util/frost.h"
#include "../compat/std_extras_compat.h"
#include "../editor_constants.h"
#include <algorithm>

//=============================================================================
// Constructors/Destructor
//=============================================================================

GameMap::GameMap(const int& numRows, const int& numCols) {
	this->numCols = numCols;
	this->numRows = numRows;
	GameTile::Builder builder;
	GameTile gt = builder.build();
	tiles.insert(tiles.begin(), getNumTiles(), gt);
    lastCharacterID = 0;
    lastObjectID = 0;
}

//=============================================================================
// Accessors
//=============================================================================

///----------------------------------------------------------------------------
/// getCharacterInventory - Fills a vector with each of the objects in the
/// character's inventory, given the character's ID.
/// @param ID of the Character
/// @return a vector containing indices of each object.
///----------------------------------------------------------------------------

const std::vector<size_t> GameMap::getCharacterInventory(const size_t& charID) const {
    
    std::vector<size_t> objectIndices;
    objectIndices.reserve(4);

    const size_t goSize = gameObjects.size();

    for(size_t i = 0; i < goSize; ++i) {

        if(gameObjects[i].getCreatureID() == charID) {
            objectIndices.push_back(i);
        }
    }

    return objectIndices;
}

///----------------------------------------------------------------------------
/// getFirstUnusedObjectID - Get the first ID not used by any object
/// @return an integer specifying the ID
///----------------------------------------------------------------------------

const int GameMap::getFirstUnusedObjectID() const {

    const size_t goSize = gameObjects.size();

    for(size_t i = 0; i < goSize; ++i) {

        if(gameObjects[i].getID() != static_cast<int>(i+1)) {
            return i+1;            
        }
    }

    return goSize+1;
}

///----------------------------------------------------------------------------
/// getFirstUnusedCharacterID - Get the first ID not used by any character
/// @return an integer specifying the ID
///----------------------------------------------------------------------------

const int GameMap::getFirstUnusedCharacterID() const {

    const size_t gcSize = gameCharacters.size();

    for(size_t i = 0; i < gcSize; ++i) {

        if(gameCharacters[i].getID() != static_cast<int>(i+1)) {
            return i+1;            
        }
    }

    return gcSize+1;
}

///----------------------------------------------------------------------------
/// getGameCharacters - Return a vector containing Game Characters within the
/// current map.
/// @return a vector containing the Game Objects.
///----------------------------------------------------------------------------

const std::vector<GameCharacter>& GameMap::getGameCharacters() const {
	return gameCharacters;
}

///----------------------------------------------------------------------------
/// getGameObjects - Return a vector containing Game Objects within the current
/// map.
/// @return a vector containing the Game Objects.
///----------------------------------------------------------------------------

const std::vector<GameObject>& GameMap::getGameObjects() const {
	return gameObjects;
}

///----------------------------------------------------------------------------
/// getGameObjectsAtRowCol - Return a vector containing Game Objects within the
/// current map, and on the current tile.
/// @param an integer indicating the row to search for
/// @param an integer indicating the column to search for
/// @return a vector containing the Game Objects that match the given criteria
///----------------------------------------------------------------------------

const std::vector<GameObject> GameMap::getGameObjectsAtRowCol(const int& row, const int& col) const {
        
    std::vector<GameObject> objects;

    if (!isRowColInMapBounds(row, col)) {
        return objects;
    }

    const size_t numObjects = gameObjects.size();
    
    for (size_t i = 0; i < numObjects; ++i) {  
        const GameObject& curObject = gameObjects[i];


        if (curObject.getIsLocated() == GameObjectConstants::LocatedOnGround &&
           (curObject.getX() == col && curObject.getY() == row)) {
            objects.push_back(curObject);
        }
    }

    return objects;

}

///----------------------------------------------------------------------------
/// getGameCharactersAtRowCol - Return a vector containing Game Objects within
/// the current map, and at the given row and column
/// @param an integer indicating the row to search for
/// @param an integer indicating the column to search for
/// @return a vector containing the Game Objects that match the given criteria
///----------------------------------------------------------------------------

const std::vector<GameCharacter> GameMap::getGameCharactersAtRowCol(const int& row, const int& col) const {

    std::vector<GameCharacter> chars;

    if (!isRowColInMapBounds(row, col)) {
        return chars;
    }

    const size_t numChars = gameCharacters.size();

    for (size_t i = 0; i < numChars; ++i) {
        const GameCharacter& curChar = gameCharacters[i];

        if(curChar.getX() == col && curChar.getY() == row) {
            chars.push_back(curChar);
        }
    }

    return chars;

}


///----------------------------------------------------------------------------
/// getHeight - Returns the height of the map
/// @return an integer indicating the height of the map
///----------------------------------------------------------------------------

const int& GameMap::getHeight() const {
    return numRows;
}

///----------------------------------------------------------------------------
/// getLastCharacterID - Returns the last ID used on a character
/// @return an integer indicating the the last ID used on a character
///----------------------------------------------------------------------------

const int& GameMap::getLastCharacterID() const {
    return lastCharacterID;
}

///----------------------------------------------------------------------------
/// getLastObjectID - Returns the last ID used on an object
/// @return an integer indicating the the last ID used on an object
///----------------------------------------------------------------------------

const int& GameMap::getLastObjectID() const {
    return lastObjectID;
}

///----------------------------------------------------------------------------
/// getReliantObjectsFromID - Fills a vector with each the indices of the
/// objects that rely on the objectID to be used.
/// @param ID of the Object
/// @return a vector containing indices of objects
///----------------------------------------------------------------------------

const std::vector<size_t> GameMap::getReliantObjectsFromID(const size_t& objectID) const {
    
    std::vector<size_t> objectIndices;
    objectIndices.reserve(4);

    const size_t goSize = gameObjects.size();

    for(size_t i = 0; i < goSize; ++i) {

        if(gameObjects[i].getUsedWithID() == objectID) {
            objectIndices.push_back(i);
        }
    }

    return objectIndices;
}

///----------------------------------------------------------------------------
/// getTileDrawData - Return tile information relevant to drawing tiles.
/// @return a vector filled with tile drawing data.
///----------------------------------------------------------------------------

const std::vector<GameTile::DrawInfo> GameMap::getTileDrawData() const {

    std::vector<GameTile::DrawInfo> drawDataVec;
    drawDataVec.reserve(tiles.size());

    for(std::vector<GameTile>::const_iterator it = tiles.begin();
        it != tiles.end(); ++it) {
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
/// getHeight - Returns the total number of tiles on the map
/// @return an integer indicating the total number of tiles on the map.
///----------------------------------------------------------------------------

const int GameMap::getNumTiles() const {
	return numCols * numRows;
}


///----------------------------------------------------------------------------
/// getWidth - Returns the width of the map
/// @return an interger indicating the width of the map
///----------------------------------------------------------------------------

const int& GameMap::getWidth() const {
    return numCols;
}

//=============================================================================
// Mutators
//=============================================================================

///----------------------------------------------------------------------------
/// addCharacter - Adds a character to the game world.
/// @param GMKey used to restrict access of this function.
/// @param GameCharacter to add
///----------------------------------------------------------------------------

void GameMap::addCharacter(GMKey, GameCharacter& gameCharacter) {

    if(gameCharacter.getID() < static_cast<int>(gameCharacters.size())) {
        gameCharacters.insert(gameCharacters.begin()+gameCharacter.getID() - 1,
                             1, gameCharacter);
    }
    else {
        gameCharacters.push_back(gameCharacter);
    }

}

///----------------------------------------------------------------------------
/// addJump - Adds a jump connection to the jump list
/// @param GMKey used to restrict access of this function.
/// @param Simple Point of the first tile in the jump
/// @param Simple Point of the second tile in the jump
///----------------------------------------------------------------------------

void GameMap::addJump(GMKey, SimplePoint& firstJump, SimplePoint& secondJump) {
    ConnectionPoint newJump = ConnectionPoint(firstJump, secondJump);
    jumpPoints.push_back(newJump);
}

///----------------------------------------------------------------------------
/// addSwitch - Adds a switch connection to the switch list
/// @param GMKey used to restrict access of this function.
/// @param Simple Point of the first tile in the switch
/// @param Simple Point of the second tile in the switch
///----------------------------------------------------------------------------

void GameMap::addSwitch(GMKey, SimplePoint& firstConnection, SimplePoint& secondConnection) {
    ConnectionPoint newSwitch = ConnectionPoint(firstConnection, secondConnection);
    switchConnections.push_back(newSwitch);
}

///----------------------------------------------------------------------------
/// addObject - Adds a object to the game world.
/// @param GMKey used to restrict access of this function.
/// @param GameObject to add
///----------------------------------------------------------------------------

void GameMap::addObject(GMKey, GameObject& gameObject) {

    if(gameObject.getID() < static_cast<int>(gameObjects.size())) {
        gameObjects.insert(gameObjects.begin()+gameObject.getID() - 1, 1,
                           gameObject); 
    }
    else {
        gameObjects.push_back(gameObject);
    }
}

///----------------------------------------------------------------------------
/// deleteCharacter - Removes the character from the game world.
/// @param GMKey used to restrict access of this function.
/// @param index of the character to delete.
///----------------------------------------------------------------------------

void GameMap::deleteCharacter(GMKey, const size_t& index) {   
    gameCharacters.erase(gameCharacters.begin() + index);
}

///----------------------------------------------------------------------------
/// deleteObject - Removes the object from the game world.
/// @param GMKey used to restrict access of this function.
/// @param index of the object to delete.
///----------------------------------------------------------------------------

void GameMap::deleteObject(GMKey, const size_t& index) {
    gameObjects.erase(gameObjects.begin() + index);
}

///----------------------------------------------------------------------------
/// replaceCharacter - Replace a character at the given index.
/// @param GMKey used to restrict access of this function.
/// @param index of the character being replaced,
/// @param new character data to be replaced with.
///----------------------------------------------------------------------------

void GameMap::replaceCharacter(GMKey, const size_t& index, const GameCharacter& gameChar) {
    gameCharacters[index] = gameChar;
}

///----------------------------------------------------------------------------
/// replaceObject - Replace a character at the given index.
/// @param GMKey used to restrict access of this function.
/// @param index of the object being replaced,
/// @param new object data to be replaced with.
///----------------------------------------------------------------------------

void GameMap::replaceObject(GMKey, const size_t& index, const GameObject& gameObject) {
    gameObjects[index] = gameObject;
}

///----------------------------------------------------------------------------
/// setStory - Set the story of the Game World
/// @param GMKey used to restrict access of this function.
/// @param Story text to be used for the story
///----------------------------------------------------------------------------

void GameMap::setStory(GMKey, const std::string& inStory) {
    story = inStory;
}

///----------------------------------------------------------------------------
/// setSummary - Set the Summary of the Game World
/// @param GMKey used to restrict access of this function.
/// @param Summary text to be used for the summary
///----------------------------------------------------------------------------

void GameMap::setSummary(GMKey, const std::string& inSummary) {
    summary = inSummary;
}

///----------------------------------------------------------------------------
/// updateTileDescription - Update the tile's name and description.
/// @param GMKey used to restrict access of this function.
/// @param index of the tile being updated.
/// @param new name of the tile.
/// @param new description of the tile.
///----------------------------------------------------------------------------

void GameMap::updateTileDescription(GMKey, const size_t& index,
                                    const std::string& tileName,
                                    const std::string& tileDescription) {

    GameTile::Builder bd = GameTile::Builder(tiles[index]);

    bd.name(tileName);
    bd.description(tileDescription);

    tiles[index] = bd.build();
   
}

///----------------------------------------------------------------------------
/// updateGameInfo - Update the GameInfo information
/// @param GMKey used to restrict access of this function.
/// @param GameInfo object containing new information to be used.
///----------------------------------------------------------------------------

void GameMap::updateGameInfo(GMKey, const GameInfo& newInfo) {    
    gameInfo = newInfo;
}

//=============================================================================
// Public Functions
//=============================================================================

///----------------------------------------------------------------------------
/// characterIndexFromID - Search for a character via it's ID, then return
/// its index in the gameCharacter vector if it is found.
/// @param character ID to search for.
/// @return the index in the game character vector of the character if found,
/// (size_t)-1 if it was not.
///----------------------------------------------------------------------------

const size_t GameMap::characterIndexFromID(const int& charID) const {

    const size_t gcSize = gameCharacters.size();

    for(size_t i = 0; i < gcSize; ++i) {
        
        if(gameCharacters[i].getID() == charID) {
            return i;
        }
    }

    return (size_t)-1;

}

///----------------------------------------------------------------------------
/// objectIndexFromID - Search for an object via it's ID, then return its index
/// in the gameCharacter vector if it is found.
/// @param object ID to search for.
/// @return the index in the game object vector of the object if found,
/// (size_t)-1 if it was not.
///----------------------------------------------------------------------------

const size_t GameMap::objectIndexFromID(const int& objectID) const {

    const size_t goSize = gameObjects.size();

    for(size_t i = 0; i < goSize; ++i) {
        
        if(gameObjects[i].getID() == objectID) {
            return i;
        }
    }

    return (size_t)-1;

}

///----------------------------------------------------------------------------
/// characterIDFromIndex - If found, returns the ID of the object with the
/// given index
/// @param character index to search for.
/// @return the ID of the object if found, GameCharacterConstants::NoID if
/// not.
///----------------------------------------------------------------------------

const int& GameMap::characterIDFromIndex(const size_t& charIndex) const {

    const size_t gcSize = gameCharacters.size();

    if(charIndex > gcSize || gameCharacters.empty()) {
        return GameCharacterConstants::NoID;
    }

    return gameCharacters[charIndex].getID();

}

///----------------------------------------------------------------------------
/// objectIDFromIndex - If found, returns the ID of the object with the given
/// index
/// @param object index to search for.
/// @return the ID of the object if found, GameObjectConstants::NoID if not.
///----------------------------------------------------------------------------

const int& GameMap::objectIDFromIndex(const size_t& objectIndex) const {

    const size_t goSize = gameObjects.size();

    if(objectIndex > goSize || gameObjects.empty()) {
        return GameObjectConstants::NoID;
    }

    return gameObjects[objectIndex].getID();

}

///----------------------------------------------------------------------------
/// indexFromRowCol - Converts the given row and column to an index for the
/// current map being edited. It assumes that that the row and column given
/// are valid.
/// @param integer specifying the row the tile is on
/// @param integer specifying the column the tile is on
/// @return the index of the tile specified
///----------------------------------------------------------------------------

const unsigned int GameMap::indexFromRowCol(const int& row, 
                                            const int& col) const {
    return (row * numCols) + col;
}

///----------------------------------------------------------------------------
/// rowColFromIndex - Converts an index into a row and column value. Note that
/// the function assumes the index is in a valid range.
/// @param (out) integer to write the row to
/// @param (out) integer to write the column to
/// @param integer specifying the index.
///----------------------------------------------------------------------------

const void GameMap::rowColFromIndex(int& row, int& col,
                                    const int& index) const {
    row = index / numCols;
    col = index - (row * numCols);

}


///----------------------------------------------------------------------------
/// isRowColInMapBounds - Check if a given row and column are with in the
/// boundaries of the map.
/// @param row to check
/// @param col to check
/// @return true if it is with map boundaries, false if it is not.
///----------------------------------------------------------------------------

bool GameMap::isRowColInMapBounds(const int& row, const int& col) const {
    
    if(row > numRows - 1 || col > numCols - 1 || col < 0 || row < 0 || 
        indexFromRowCol(row, col) >= tiles.size()) {
        return false;
    }

    return true;

}

///----------------------------------------------------------------------------
/// isIndexMapBounds - Check if a given index is within the bounds of the map
/// boundaries of the map.
/// @param index to check
/// @return true if it is with map boundaries, false if it is not.
///----------------------------------------------------------------------------

bool GameMap::isIndexInMapBounds(const int& index) const {

    if (index >= tiles.size()) {
        return false;
    }

    return true;

}

///----------------------------------------------------------------------------
/// findSwitchPoint - 
///----------------------------------------------------------------------------

const SimplePoint* GameMap::findSwitchPoint(const int& row, const int& col) const {
    return findMatchingPoint(row, col, switchConnections);
}

///----------------------------------------------------------------------------
/// findJumpPoint - 
///----------------------------------------------------------------------------

const SimplePoint* GameMap::findJumpPoint(const int& row, const int& col) const {
    return findMatchingPoint(row, col, jumpPoints);
}

///----------------------------------------------------------------------------
/// readMap - Reads the SG0 and TXX file of the map name given, and then uses
/// it to construct the game map.
/// @param an ifstream of the map file
/// @param a string to the path where the map file is located
/// @param a string indicating the file's name
/// @throws runtime_error
///----------------------------------------------------------------------------

void GameMap::readMap(std::ifstream& mapFile, const std::string& filePath,
                      const std::string& fileName) {

    lastCharacterID = 0;
    lastObjectID = 0;

    const std::string storyFilePath = filePath + fileName.substr(0, fileName.length() - 4) + ".STY";

    readStory(storyFilePath);

    // TODO: Throwing an error right now causes the object to be in an undefined state.
    // We also need to do some better error checking.
    // TODO: Verify correct extensions.

    gameInfo.readHeader(key, mapFile);

    std::string line;

    // Whatever the value read is, it is always one more than it says.

    try {

        std::getline(mapFile, line);
        numCols = std::stoi(line) + 1;

        std::getline(mapFile, line);
        numRows = std::stoi(line) + 1;

    }
    catch (const std::invalid_argument& e) {
        throw std::runtime_error(e.what());
    }
    catch (const std::out_of_range& e) {
        throw std::runtime_error(e.what());
    }

    tiles.reserve((numCols * numRows));

    for (int row = 0; row < numRows; ++row) {

        const std::string rowID = AdventureGamerHeadings::Row + std::to_string(row);

        Frost::getLineWindows(mapFile, line);

        // This is only true if they're not equal.
        if (rowID.compare(line)) {
            throw std::runtime_error("Row identifier not found. Expected \"" + rowID + "\", but got \"" + line + "\".");
        }

        std::string rowFilePath = filePath + fileName.substr(0, fileName.length() - 4) + ".T";

        if (row < 10) {
            rowFilePath += "0";
        }

        rowFilePath += std::to_string(row);

        const std::map<unsigned int, std::string> rowDescriptions = readRowDescriptions(rowFilePath);

        for (int col = 0; col < numCols; ++col) {

            // Before we can read a tile, we'll need to get the description for it,
            // if one exists.

            std::string tileDescription;
            std::map<unsigned int, std::string>::const_iterator it;
            it = rowDescriptions.find(col);

            if (it != rowDescriptions.end()) {
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

///----------------------------------------------------------------------------
/// removeJumpPoint - Removes a Jump Point, if found.
/// @param First point in the connection
/// @param Second Point in the connection.
/// @return true if the point was removed from the Jump vector, false if not.
///----------------------------------------------------------------------------

bool GameMap::removeJumpPoint(const SimplePoint& point1, const SimplePoint& point2) {

    const size_t jmpSize = jumpPoints.size();
    const ConnectionPoint conPoint(point1, point2);

    for (size_t i = 0; i < jmpSize; ++i) {
        if (jumpPoints[i] == conPoint) {
            jumpPoints.erase(jumpPoints.begin() + i);
            return true;
        }
    }
    
    return false;
}

///----------------------------------------------------------------------------
/// removeSwitch - Removes a Switch connection from the switch vector
/// @param First point in the connection
/// @param Second Point in the connection.
/// @return true if the point was removed from the switch vector, false if not.
///----------------------------------------------------------------------------

bool GameMap::removeSwitch(const SimplePoint& point1, const SimplePoint& point2) {

    const size_t switchSize = switchConnections.size();
    const ConnectionPoint conPoint(point1, point2);

    for (size_t i = 0; i < switchSize; ++i) {
        if (switchConnections[i] == conPoint) {
            switchConnections.erase(switchConnections.begin() + i);
            return true;
        }
    }

    return false;
}

///----------------------------------------------------------------------------
/// removeFeature - Removes a feature from a tile
/// @param GMKey
/// @param row of the tile to remove the feature from
/// @param column of the tile to remove the feature from.
/// @return true if the feature was removed, false if it was not
///----------------------------------------------------------------------------

bool GameMap::removeFeature(GMKey, const int& row, const int& col) {

    const size_t index = indexFromRowCol(row, col);

    GameTile::Builder tb(tiles[index]);

    // When removing a feature, preserve the dirt road modifier.

    tb.sprite(GameTile::Builder::calculateSprite(tiles[index].getSpriteIndex(), tiles[index].getSpriteModifier() & TileModifiers::DirtRoad));

    tiles[index] = tb.build();

    return true;
}

///----------------------------------------------------------------------------
/// updateTileFlags - Remove darkness from a tile
/// @param GMKey
/// @param row of the tile to remove the feature from
/// @param column of the tile to remove the feature from.
/// @return true if the feature was removed, false if it was not
///----------------------------------------------------------------------------

bool GameMap::updateTileFlags(GMKey, const int& row, const int& col, const uint8_t& newFlags) {

    const size_t index = indexFromRowCol(row, col);

    GameTile::Builder tb(tiles[index]);

    // When removing a feature, preserve the dirt road modifier.

    tb.flags(newFlags);

    tiles[index] = tb.build();

    return true;

}

///----------------------------------------------------------------------------
/// writeMap - Writes the SG0 and TXX files of the map given.
/// @param an ofstream of the map file
/// @param a string to the path where the map file is located
/// @param a string indicating the file's name
/// @throws runtime_error
///----------------------------------------------------------------------------

void GameMap::writeMap(std::ofstream& mapFile, const std::string& filePath,
                       const std::string& fileName) {

    const std::string storyFilePath = filePath + fileName.substr(0, fileName.length() - 4) + ".STY";
    writeStory(storyFilePath);
    
    gameInfo.writeHeader(key, mapFile);
    Frost::writeVBInteger(mapFile, numCols - 1);
    Frost::writeVBInteger(mapFile, numRows - 1);

    for (int row = 0; row < numRows; ++row) {

        const std::string rowID = AdventureGamerHeadings::Row + std::to_string(row);
        Frost::writeVBLine(mapFile, rowID);

        std::string rowFilePath = filePath + fileName.substr(0, fileName.length() - 4) + ".T";

        if (row < 10) {
            rowFilePath += "0";
        }

        rowFilePath += std::to_string(row);
        std::map<int, std::string> rowDescriptions;

        for (int col = 0; col < numCols; ++col) {

            size_t currentTile = indexFromRowCol(row, col);
            
            if (!tiles[currentTile].getDescription().empty()) {
                rowDescriptions[col] = tiles[currentTile].getDescription();
            }

            tiles[currentTile].write(mapFile);

        }

        if (!rowDescriptions.empty()) {
            std::ofstream rowDescFile(rowFilePath.c_str(), std::ofstream::out |
                                                           std::ios::binary);

            Frost::writeVBInteger(rowDescFile, rowDescriptions.size());

            for (std::map<int, std::string>::iterator it = rowDescriptions.begin();
                 it != rowDescriptions.end(); ++it) {
                Frost::writeVBInteger(rowDescFile, it->first);
                Frost::writeVBString(rowDescFile, it->second);
            }

        }
    }

    writeJumps(mapFile);
    writeSwitches(mapFile);
    gameInfo.writePlayerAttributes(key, mapFile);
    writeObjects(mapFile);
    writeCharacters(mapFile);

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
/// findMatchingPoint - Give the row and column, find the matching point if one
/// exists.
/// @param row of the connection point to search for
/// @param column of the connection point to search for
/// @return value NULL if nothing was found, a valid connection point if it was
///----------------------------------------------------------------------------
const SimplePoint* GameMap::findMatchingPoint(const int& row, const int& col, const std::vector<ConnectionPoint>& connections) const {

    const size_t conSize = connections.size();
    SimplePoint* matchingPoint = NULL;

    for (size_t i = 0; i < conSize; ++i) {

        const SimplePoint& conPoint1 = connections[i].getConnectPoint1();
        const SimplePoint& conPoint2 = connections[i].getConnectPoint2();

        if (conPoint1.getColumn() == col && conPoint1.getRow() == row) {
            return &conPoint2;
        }
        else if (conPoint2.getColumn() == col && conPoint2.getRow() == row) {
            return &conPoint1;
        }
    }

    return NULL;
}

///----------------------------------------------------------------------------
/// readCharacters - Reads the "{cretr" section of the map file
/// @param mapFile an ifstream of the map file to be read from already at the
/// "{cretr" section
/// @throws runtime_error if there is a problem reading the file.
///----------------------------------------------------------------------------

void GameMap::readCharacters(std::ifstream& mapFile) {

    std::string line;
    std::string errorMsg = "Error reading characters: ";
    Frost::getLineWindows(mapFile, line);

    if(AdventureGamerHeadings::Characters.compare(line)) {
        errorMsg.append("Expected \"" + AdventureGamerHeadings::Characters + "\", but got \"" + line + "\".");
        throw std::runtime_error(errorMsg);
    }

    std::getline(mapFile, line);
    const int numChars = std::stoi(line);
    gameCharacters.reserve(numChars);

    for(int i = 0; i < numChars; i++) {
        GameCharacter::Builder characterBuilder;

        try {
            characterBuilder.readCharacter(mapFile);
        }
        catch (const std::invalid_argument e) {
            errorMsg.append(e.what());
            throw std::runtime_error(errorMsg);
        }

        GameCharacter gameCharacter = characterBuilder.build();
        gameCharacters.push_back(gameCharacter);

        if(gameCharacter.getID() > lastCharacterID) {
            lastCharacterID = gameCharacter.getID();
        }

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
    std::string errorMsg = "Error reading Jumps: ";
    Frost::getLineWindows(mapFile, line);

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

            if(!isRowColInMapBounds(y, x)) {
                errorMsg.append("Tile index was out of bounds");
                throw std::out_of_range(errorMsg);
            }

            unsigned int tileIndex = indexFromRowCol(y, x);
            if(!(tiles[tileIndex].hasJumpPad())) {
                // TODO: Jump pad features don't actually have to be in the jump list.
                errorMsg.append("Jump Pad position was read, but the coordinates given were not that of a Jump Pad tile.");
                //throw std::runtime_error(errorMsg);
            }

            std::getline(mapFile, line);
            x = std::stoi(line);
            std::getline(mapFile, line);
            y = std::stoi(line);
            SimplePoint jumpB(x, y);

            if(!isRowColInMapBounds(y, x)) {
                errorMsg.append("Tile index was out of bounds");
                throw std::out_of_range(errorMsg);
            }

            tileIndex = indexFromRowCol(y, x);


            if(!(tiles[tileIndex].hasJumpPad())) {
                // TODO: Jumppads can be one way, that is jump to a non-jumppad tile.
                errorMsg.append("Jump Pad position was read, but the coordinates given were not that of a Jump Pad tile.");
                //throw std::runtime_error(errorMsg);
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
                    errorMsg.append("The column indicated is outside the boundaries of the map.");
                    throw std::runtime_error(errorMsg);
                }

                descriptionMap[colID] = Frost::readVBString(ifs);
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
    else {
        if(Frost::doesFileExist(rowFileName.c_str())) {
            errorMsg.append("could not open " + rowFileName + " for reading.");
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
    std::string errorMsg = "Error reading objects: ";
    Frost::getLineWindows(mapFile, line);

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
        
        if(gameObject.getID() > lastCharacterID) {
            lastCharacterID = gameObject.getID();
        }

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
		story.resize(static_cast<size_t>(ifs.tellg()));
		ifs.seekg(0, std::ios::beg);
		ifs.read(&story[0], story.size());
		ifs.close();
    }

    const size_t summarySize = story.find("\"\r\n"); 

    summary = story.substr(0, summarySize);
    story = story.substr(summarySize + 3, std::string::npos);
    
    // Story files aren't mandatory, so if they're not found, there is no
    // error.

    story = Frost::rtrim(Frost::ltrim(story, "\""), "\"\n\r");
    summary =  Frost::rtrim(Frost::ltrim(summary, "\""), "\n\r");

}

///----------------------------------------------------------------------------
/// readSwitches - Read the "{swtchs" section of the file.
/// @param mapFile an ifstream of the map file to be read from already at the
/// "{swtchs" section.
/// @throws runtime_error if there are any problems reading the file
///----------------------------------------------------------------------------

void GameMap::readSwitches(std::ifstream& mapFile) {
    
    std::string line;
    std::string errorMsg = "Error reading switches: ";
    Frost::getLineWindows(mapFile, line);

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

            if(!isRowColInMapBounds(y, x)) {
                errorMsg.append("Tile index was out of bounds");
                throw std::out_of_range(errorMsg);
            }

            unsigned int tileIndex = indexFromRowCol(y, x);

            if(!(tiles[tileIndex].hasSwitch())) {
                errorMsg.append("Read switch, but no switch was found at the coordinates read.");
                throw std::runtime_error(errorMsg);
            }
            
            // Get the tile effected
            std::getline(mapFile, line);
            x = std::stoi(line);
            std::getline(mapFile, line);
            y = std::stoi(line);
            SimplePoint connectionB(x, y);

            if(!isRowColInMapBounds(y, x)) {
                errorMsg.append("Tile index was out of bounds");
                throw std::out_of_range(errorMsg);
            }

            tileIndex = indexFromRowCol(y, x);

            if(! (tiles[tileIndex].hasGate() || tiles[tileIndex].isDark()) ) {
                errorMsg.append("Read switch, but the tile it effects is not a gate or dark space.");
                throw std::runtime_error(errorMsg);
            }

            ConnectionPoint switchConnection(connectionA, connectionB);

            if (!ifConnectionExists(switchConnections, switchConnection)) {
                switchConnections.push_back(switchConnection);
            }
            else {
                errorMsg.append("Duplicate Switch Connection was read.");
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

///----------------------------------------------------------------------------
/// writeStory - Writes the STY file.
/// @param a string indicating the full path to the story file.
///----------------------------------------------------------------------------

void GameMap::writeStory(const std::string& storyFilePath) {
    
    std::ofstream ofs;
    ofs.open(storyFilePath.c_str(), std::ofstream::out | std::ios::binary);

    if (ofs) {
        ofs.put('\"');
        ofs.write(&summary[0], summary.size());
        ofs.put('\"');
        ofs.put('\r');
        ofs.put('\n');
        

        ofs.put('\"');
        ofs.write(&story[0], story.size());
        ofs.put('\"');
        ofs.put('\r');
        ofs.put('\n');
        
    }

}

///----------------------------------------------------------------------------
/// writeJumps - Write the Jump points to the map file
/// @param ofstream of the map file to be written to.
///----------------------------------------------------------------------------

void GameMap::writeJumps(std::ofstream& mapFile) {
    
    Frost::writeVBLine(mapFile, AdventureGamerHeadings::Jumps);

    const size_t numJumps = jumpPoints.size();

    Frost::writeVBInteger(mapFile, numJumps);

    for (size_t i = 0; i < numJumps; ++i) {
        const SimplePoint& firstPoint = jumpPoints[i].getConnectPoint1();
        const SimplePoint& secondPoint = jumpPoints[i].getConnectPoint2();
        Frost::writeVBInteger(mapFile, firstPoint.getX());
        Frost::writeVBInteger(mapFile, firstPoint.getY());
        Frost::writeVBInteger(mapFile, secondPoint.getX());
        Frost::writeVBInteger(mapFile, secondPoint.getY());
    }
}

///----------------------------------------------------------------------------
/// writeSwitches - Write the Switch Connections to the map file
/// @param ofstream of the map file to be written to.
///----------------------------------------------------------------------------

void GameMap::writeSwitches(std::ofstream& mapFile) {
    
    Frost::writeVBLine(mapFile, AdventureGamerHeadings::Switches);
    const size_t numSwitches = switchConnections.size();

    Frost::writeVBInteger(mapFile, numSwitches);

    for (size_t i = 0; i < numSwitches; ++i) {
        const SimplePoint& firstPoint = switchConnections[i].getConnectPoint1();
        const SimplePoint& secondPoint = switchConnections[i].getConnectPoint2();
        Frost::writeVBInteger(mapFile, firstPoint.getX());
        Frost::writeVBInteger(mapFile, firstPoint.getY());
        Frost::writeVBInteger(mapFile, secondPoint.getX());
        Frost::writeVBInteger(mapFile, secondPoint.getY());
    }

}

///----------------------------------------------------------------------------
/// writeObjects - Write the objects section to the map file given.
/// @param ofstream of the map file to be written to.
///----------------------------------------------------------------------------

void GameMap::writeObjects(std::ofstream& mapFile) {
    
    Frost::writeVBLine(mapFile, AdventureGamerHeadings::Objects);
    
    const size_t numObjects = gameObjects.size();

    Frost::writeVBInteger(mapFile, numObjects);

    for (size_t i = 0; i < numObjects; ++i) {
        gameObjects[i].writeObject(mapFile);
    }
}

///----------------------------------------------------------------------------
/// writeCharacters - Write the characters section to the map file given.
/// @param ofstream of the map file to be written to.
///----------------------------------------------------------------------------

void GameMap::writeCharacters(std::ofstream& mapFile) {

    Frost::writeVBLine(mapFile, AdventureGamerHeadings::Characters);
    
    const size_t numChars = gameCharacters.size();

    Frost::writeVBInteger(mapFile, numChars);

    for (size_t i = 0; i < numChars; ++i) {
        gameCharacters[i].writeCharacter(mapFile);
    }
}

//=============================================================================
// New Functions to be moved after
//=============================================================================

void GameMap::updateTile(GMKey, const size_t& index, GameTile& gameTile) {
    tiles[index] = gameTile;
}