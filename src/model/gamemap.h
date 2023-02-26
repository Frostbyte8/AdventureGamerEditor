#ifndef __GAMEMAP_H__
#define __GAMEMAP_H__

#include <fstream>
#include "gametile.h"

class GameMap {

    public:
    
    private:
        void readTile(std::ifstream& mapFile, const std::string& description);
        unsigned int    numCols;
        unsigned int    numRows;

};

#endif // __GAMEMAP_H__
