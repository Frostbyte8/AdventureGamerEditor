#ifndef __GAMEMAP_H__
#define __GAMEMAP_H__

#include <fstream>
#include <vector>
#include <map>
#include "gametile.h"
#include "gameobject.h"
#include "gamecharacter.h"
#include "connection_point.h"
#include "../compat/stdint_compat.h"


class GameMap {

    public:
    
        const int& getWidth() const;
        const std::vector<GameTile>& getTiles() const;
        const int& getHeight() const;
        const unsigned int indexFromRowCol(const int& row, const int& col) const;

        void readMap(std::ifstream& mapFile, const std::string& filePath, const std::string& fileName);
        void readObjects(std::ifstream& mapFile);

    private:

        const bool ifConnectionExists(const std::vector<ConnectionPoint>& connections, const ConnectionPoint& connectionPoint) const;
        std::map<unsigned int, std::string> readRowDescriptions(const std::string& rowFileName);
        GameTile readTile(std::ifstream& mapFile, const std::string& description);
        void readJumps(std::ifstream& mapFile);
        void readSwitches(std::ifstream& mapFile);

        std::vector<ConnectionPoint> jumpPoints;
        std::vector<ConnectionPoint> switchConnections;
        std::vector<GameTile> tiles;
        std::vector<GameObject> gameObjects;
        int numCols;
        int numRows;

        

};

#endif // __GAMEMAP_H__
