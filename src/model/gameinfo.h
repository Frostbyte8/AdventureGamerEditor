#ifndef __GAMEINFO_H__
#define __GAMEINFO_H__

#include <string>
#include <fstream>
#include "../compat/stdint_compat.h"
#include "../editor_constants.h"

class GameInfo {

    public:
        GameInfo();
        bool readHeader(std::ifstream& mapFile);
        //void readPlayerAttributes(std::ifstream& mapFile);

	private:
		std::string         gameName;
		std::string         unusedHeaderString;
		std::string         currencyName;
        uint8_t	            baseAttributes[AdventureGamerConstants::NumAttributes];
		uint8_t	            randomAttributes[AdventureGamerConstants::NumAttributes];
		int8_t	            playerDefaultHearing;
        int8_t              playerDefaultSight;
        uint8_t             playerStartX;
        uint8_t             playerStartY;
};

#endif // __GAMEINFO_H__