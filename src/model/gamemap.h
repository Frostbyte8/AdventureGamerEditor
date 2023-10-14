#ifndef __GAMEMAP_H__
#define __GAMEMAP_H__

#include <fstream>
#include <vector>
#include <map>
#include "gametile.h"
#include "gameobject.h"
#include "gamecharacter.h"
#include "gameinfo.h"
#include "connection_point.h"
#include "../compat/stdint_compat.h"

//-----------------------------------------------------------------------------
// GameMapConstants
//-----------------------------------------------------------------------------

namespace GameMapConstants {

    // TODO: Test these values. Rows for sure cannot go beyond 100,
    // but the limit might be sooner than that.
    const unsigned int MaxCols              = 100;
    const unsigned int MaxRows              = 100;

    const unsigned int MaxTileName          = 192;
    const unsigned int MaxTileDescription   = 1024;
    const unsigned int MaxObjects           = 100;
    const unsigned int MaxCharacters        = 30;

    const unsigned int MaxStoryText         = 2048;
    const unsigned int MaxSummaryText       = 8192;
}

class GameMap {

    public:

        // This makes it so only certain classes
        // can use some of the public functions.
        
        class GMKey {
            friend class GameWorldController;
            GMKey() {};
            GMKey(GMKey &t) {};
        };

		GameMap() {};
		GameMap(const int& numRows, const int& numCols);
    
        // Accessors
        const int& getWidth() const;
        const int& getHeight() const;
        const int& getLastCharacterID() const;
        const int& getLastObjectID() const;
        const int getFirstUnusedObjectID() const;
        const int getFirstUnusedCharacterID() const;
		inline const int getNumTiles() const;
        const std::string& getStory() const { return story; }
        const std::string& getSummary() const { return summary; }
        inline const GameTile& getTile(const int& index) const { return tiles[index]; }
        const GameInfo& getGameInfo() const { return gameInfo; }

        // Collection Accessors
        const std::vector<size_t> getCharacterInventory(const size_t& charID) const;
        const std::vector<size_t> getReliantObjectsFromID(const size_t& objectID) const;
		const std::vector<GameObject>& getGameObjects() const;
        const std::vector<GameObject> getGameObjectsAtRowCol(const int& row, const int& col) const;
        const std::vector<GameCharacter> getGameCharactersAtRowCol(const int& row, const int& col) const;
        const std::vector<GameCharacter>& getGameCharacters() const;
        const std::vector<GameTile::DrawInfo> getTileDrawData() const;
        const std::vector<GameTile>& getTiles() const;

        // Mutators
        void addCharacter(GMKey, GameCharacter& gameCharacter);
        void addObject(GMKey, GameObject& gameObject);
        void deleteCharacter(GMKey, const size_t& index);
        void deleteObject(GMKey, const size_t& index);
        void replaceCharacter(GMKey, const size_t& index, const GameCharacter& gameChar);
        void replaceObject(GMKey, const size_t& index, const GameObject& gameObject);

        // TODO: Story and Summary should be part of INFO not Map
        void setStory(GMKey, const std::string& inStory);
        void setSummary(GMKey, const std::string& inSummary);

        void updateTile(GMKey, const size_t& index, const int& type, const int& flags) {

            GameTile::Builder bd = GameTile::Builder(tiles[index]);

            bd.sprite(type);
            bd.flags(flags);

            tiles[index] = bd.build();
        }
        void updateTileDescription(GMKey, const size_t& index, const std::string& tileName, const std::string& tileDescription);
        void updateGameInfo(GMKey, const GameInfo& newInfo);

        // Public Functions
        const size_t characterIndexFromID(const int& charID) const;
        const size_t objectIndexFromID(const int& objectID) const;
        const int& characterIDFromIndex(const size_t& charIndex) const;
        const int& objectIDFromIndex(const size_t& objectIndex) const;
		const unsigned int indexFromRowCol(const int& row, const int& col) const;
        bool isRowColInMapBounds(const int& row, const int& col) const;

        void readMap(std::ifstream& mapFile, const std::string& filePath, const std::string& fileName);
        void writeMap(std::ofstream& mapFile, const std::string& filePath, const std::string& fileName);

        const SimplePoint* findJumpPoint(const int& row, const int& col) const;
        const SimplePoint* findSwitchPoint(const int& row, const int& col) const;

        bool removeJumpPoint(const SimplePoint& point1, const SimplePoint& point2);
        bool removeSwitch(const SimplePoint& point1, const SimplePoint& point2);
        bool removeFeature(GMKey, const int& row, const int& col);
        bool updateTileFlags(GMKey, const int& row, const int& col, const uint8_t& newFlags);

    private:

        const bool ifConnectionExists(const std::vector<ConnectionPoint>& connections, const ConnectionPoint& connectionPoint) const;
        const SimplePoint* findMatchingPoint(const int& row, const int& col, const std::vector<ConnectionPoint>& connections) const;
        
        std::map<unsigned int, std::string> readRowDescriptions(const std::string& rowFileName);

        void readCharacters(std::ifstream& mapFile);
        void readJumps(std::ifstream& mapFile);
        void readObjects(std::ifstream& mapFile);
        void readStory(const std::string& storyFileName);
        void readSwitches(std::ifstream& mapFile);        

        void writeStory(const std::string& storyFileName);
        void writeJumps(std::ofstream& mapFile);
        void writeSwitches(std::ofstream& mapFile);
        void writeObjects(std::ofstream& mapFile);

        GameInfo gameInfo;
        
        int numCols;
        int numRows;
        int lastObjectID;
        int lastCharacterID;
        int lastUnusedCharacterID;

        std::string summary;
        std::string story;
        std::vector<GameTile> tiles;
        std::vector<ConnectionPoint> jumpPoints;
        std::vector<ConnectionPoint> switchConnections;
        std::vector<GameObject> gameObjects;
        std::vector<GameCharacter> gameCharacters;

        GameInfo::Key key;

};

#endif // __GAMEMAP_H__
