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
        const uint8_t& getWidth() const {return numCols;}
        const uint8_t& getHeight() const {return numRows;}
        const std::vector<GameTile> getTiles() const {return tiles;}

    private:

        std::map<unsigned int, std::string> readRowDescriptions(const std::string& rowFileName);
        GameTile readTile(std::ifstream& mapFile, const std::string& description);

        std::vector<GameTile> tiles;
        uint8_t numCols;
        uint8_t numRows;

};

#endif // __GAMEMAP_H__
