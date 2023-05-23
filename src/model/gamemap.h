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
		inline const int getNumTiles() const;
        inline const GameTile& getTile(const int& index) const { return tiles[index]; }

        // Collection Accessors
        const std::vector<size_t> getCharacterInventory(const size_t& charID) const;
        const std::vector<size_t> getReliantObjectsFromID(const size_t& objectID) const;
		const std::vector<GameObject>& getGameObjects() const;
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

        // Public Functions
        const size_t characterIndexFromID(const int& charID) const;
        const size_t objectIndexFromID(const int& objectID) const;
        const int& characterIDFromIndex(const size_t& charIndex) const;
        const int& objectIDFromIndex(const size_t& objectIndex) const;
		const unsigned int indexFromRowCol(const int& row, const int& col) const;
        bool isRowColInMapBounds(const int& row, const int& col) const;

        void readMap(std::ifstream& mapFile, const std::string& filePath, const std::string& fileName);
        void writeMap(std::ofstream& mapFile);

    private:

        const bool ifConnectionExists(const std::vector<ConnectionPoint>& connections, const ConnectionPoint& connectionPoint) const;
        std::map<unsigned int, std::string> readRowDescriptions(const std::string& rowFileName);

        void readCharacters(std::ifstream& mapFile);
        void readJumps(std::ifstream& mapFile);
        void readObjects(std::ifstream& mapFile);
        void readStory(const std::string& storyFileName);
        void readSwitches(std::ifstream& mapFile);

        GameInfo gameInfo;
        
        int numCols;
        int numRows;
        int lastObjectID;
        int lastCharacterID;
        int lastUnusedObjectID;
        int lastUnusedCharacterID;

        std::string story;
        std::vector<GameTile> tiles;
        std::vector<ConnectionPoint> jumpPoints;
        std::vector<ConnectionPoint> switchConnections;
        std::vector<GameObject> gameObjects;
        std::vector<GameCharacter> gameCharacters;

        GameInfo::Key key;

};

#endif // __GAMEMAP_H__
