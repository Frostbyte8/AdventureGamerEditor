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

		GameMap() {};
		GameMap(const int& numRows, const int& numCols);
    
        const int& getWidth() const;
        const std::vector<GameTile>& getTiles() const;
        const int& getHeight() const;
		inline const int getNumTiles() const;
		const std::vector<GameObject>& getGameObjects() const;

		const unsigned int indexFromRowCol(const int& row, const int& col) const;
        
        GameTile getTile(const int& row, const int& col) const;

        void readMap(std::ifstream& mapFile, const std::string& filePath, const std::string& fileName);
        void writeMap(std::ofstream& mapFile);
        const std::vector<GameTile::DrawInfo> getTileDrawData();

    private:

        const bool ifConnectionExists(const std::vector<ConnectionPoint>& connections, const ConnectionPoint& connectionPoint) const;
        std::map<unsigned int, std::string> readRowDescriptions(const std::string& rowFileName);

        //void reaadTiles(std::ifstream& mapFile);
        void readCharacters(std::ifstream& mapFile);
        void readJumps(std::ifstream& mapFile);
        void readObjects(std::ifstream& mapFile);
        void readStory(const std::string& storyFileName);
        void readSwitches(std::ifstream& mapFile);

        GameInfo gameInfo;
        int numCols;
        int numRows;
        std::string story;
        std::vector<GameTile> tiles;
        std::vector<ConnectionPoint> jumpPoints;
        std::vector<ConnectionPoint> switchConnections;
        std::vector<GameObject> gameObjects;
        std::vector<GameCharacter> gameCharacters;

        GameInfo::Key key;

};

#endif // __GAMEMAP_H__
