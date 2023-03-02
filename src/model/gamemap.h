#ifndef __GAMEMAP_H__
#define __GAMEMAP_H__

#include <fstream>
#include <vector>
#include <map>
#include "gametile.h"
#include "../compat/stdint_compat.h"

class GameMap {

    public:
    
        void readMap(std::ifstream& mapFile, const std::string& filePath, const std::string& fileName);
        const int& getWidth() const {return numCols;}
        const int& getHeight() const {return numRows;}
        const std::vector<GameTile>& getTiles() const {return tiles;}

    private:

        std::map<unsigned int, std::string> readRowDescriptions(const std::string& rowFileName);
        GameTile readTile(std::ifstream& mapFile, const std::string& description);

        std::vector<GameTile> tiles;
        int numCols;
        int numRows;

};

#endif // __GAMEMAP_H__
