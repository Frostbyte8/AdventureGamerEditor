#ifndef __GAMEMAP_H__
#define __GAMEMAP_H__

#include <fstream>
#include <vector>
#include <map>
#include "gametile.h"
#include "connection_point.h"
#include "../compat/stdint_compat.h"

class GameMap {

    public:
    
        void readMap(std::ifstream& mapFile, const std::string& filePath, const std::string& fileName);
        const int& getWidth() const {return numCols;}
        const int& getHeight() const {return numRows;}
        const std::vector<GameTile>& getTiles() const {return tiles;}
        
        const bool ifConnectionExists(const std::vector<ConnectionPoint>& connections, const ConnectionPoint& connectionPoint) const;
        const unsigned int indexFromRowCol(const int& row, const int& col) const;

    private:

        std::map<unsigned int, std::string> readRowDescriptions(const std::string& rowFileName);
        GameTile readTile(std::ifstream& mapFile, const std::string& description);
        void readJumps(std::ifstream& mapFile);
        void readSwitches(std::ifstream& mapFile);

        std::vector<ConnectionPoint> jumpPoints;
        std::vector<ConnectionPoint> switchConnections;
        std::vector<GameTile> tiles;
        int numCols;
        int numRows;

        

};

#endif // __GAMEMAP_H__
